#include "Package.h"

sPackageStatus Package::CreatePackage(std::string packageDescription)
{
	sPackageStatus ret;
	std::string zipFile;
	try
	{
		InstallationDesc id = readDesc(packageDescription);
		if (id.GetName() == "")
		{
			ret.success = false;
			ret.msgs.push_back(sPackageMsg(false, "Failed to get package name from package descriptor"));
			return ret;
		}
	
		std::vector<sFile> files = id.GetFiles();
		zipFile = id.GetName();
		zipFile.erase(std::remove(zipFile.begin(), zipFile.end(), ' '), zipFile.end());
		struct archive *a;
		struct archive_entry *entry;
		struct stat st;
		char buff[8192];
		int len;
		int fd;
		// Add the packageDescriptor
		sFile pd;
		std::filesystem::path p(packageDescription);
		if(p.has_parent_path())
			pd.sourceLocation = p.parent_path();
		pd.fileName = p.filename();
		
		a = archive_write_new();
		archive_write_add_filter_gzip(a);
		archive_write_set_format_pax_restricted(a);
		archive_write_open_filename(a, zipFile.c_str());
		sFile archiveFile;
		archiveFile.fileName = "package.json";
		files.push_back(archiveFile);
		for (int i = 0; i < files.size(); i++)
		{
			std::filesystem::path ifp = files[i].sourceLocation;
			ifp /= files[i].fileName;
			
			stat(ifp.c_str(), &st);
			entry = archive_entry_new();  // Note 2
			std::filesystem::path ofp = files[i].destLocation;
			ofp /= files[i].fileName;
			std::string outfilePath = ofp;
			if (outfilePath.at(0) == '/')
				outfilePath = outfilePath.substr(1, outfilePath.length() - 1);
			archive_entry_set_pathname(entry, outfilePath.c_str());
			archive_entry_set_size(entry, st.st_size);  // Note 3
			archive_entry_set_filetype(entry, AE_IFREG);
			archive_entry_set_perm(entry, 0644);
			archive_write_header(a, entry);
			fd = open(ifp.c_str(), O_RDONLY);
			len = read(fd, buff, sizeof(buff));
			while (len > 0) {
				archive_write_data(a, buff, len);
				len = read(fd, buff, sizeof(buff));
			}
			close(fd);
			std::stringstream msg;
			msg << "Added: " << ifp << " to the archive";
			ret.msgs.push_back(sPackageMsg (true, msg.str()));
			archive_entry_free(entry);
		}
		
		archive_write_close(a);
		archive_write_free(a);
		std::stringstream ss;
		ss << "Created package: " << zipFile;
		ret.msgs.push_back(sPackageMsg(true, ss.str()));
	}
	catch (std::exception &e)
	{
		std::stringstream ss;
		ss << "Exception, procesing failed.  Msg: " << e.what();
		
		ret.success = false;
		ret.msgs.push_back(sPackageMsg(false, ss.str()));
		return ret;
	}
	std::stringstream ss;
	ss << "Created: " << zipFile << " successfully";
	ret.success = true;
	ret.msgs.push_back(sPackageMsg(true, ss.str()));
	return ret;
}

sPackageStatus  Package::InstallPackage(std::filesystem::path installPackage)
{
	sPackageStatus ret;
	ret.success = true;
	if (!std::filesystem::exists(installPackage))
	{
		std::stringstream ss;
		ss << "Install package: " << installPackage << " does not exist";
		ret.success = false;
		ret.msgs.push_back(sPackageMsg(false, ss.str()));
		return ret;
	}
	else
	{
		std::stringstream ss;	
		ss << "Install package: " << installPackage << " found";
		ret.msgs.push_back(sPackageMsg(true, ss.str()));
	}
	std::filesystem::path tmpDir = std::filesystem::temp_directory_path();
	
	tmpDir /= genUUID();
	
	if (!std::filesystem::create_directory(tmpDir))
	{
		std::stringstream ss;
		ss << "Could not create temp dir: " << tmpDir;
		ret.success = false;
		ret.msgs.push_back(sPackageMsg(false, ss.str()));
		return ret;
	}
	else
	{
		std::stringstream ss;	
		ss << "Temp dir created: " << tmpDir;
		ret.msgs.push_back(sPackageMsg(true, ss.str()));

	}
	
	ret =uncompressFile(installPackage, tmpDir, ret);
	if (!ret.success)
	{
		return ret;
	}
	else
	{
		std::stringstream ss;	
		ss << "Install package: " << installPackage << " uncompressed to: "<<tmpDir;
		ret.msgs.push_back(sPackageMsg(true, ss.str()));
	}
	// Load the package descriptor
	
	std::filesystem::path pdp = tmpDir;
	pdp /= "package.json";
	InstallationDesc installFile;
	try
	{
		installFile = readDesc(pdp);
		std::stringstream ss;	
		ss << "Package descriptor: " << pdp << " read";
		ret.msgs.push_back(sPackageMsg(true, ss.str()));
	}
	catch (const std::exception&e)
	{
		std::stringstream ss;
		ss << "Error reading install package descriptor: " << e.what();
		ret.success = false;
		ret.msgs.push_back(sPackageMsg(false, ss.str()));
		std::filesystem::remove_all(tmpDir);
		ret.msgs.push_back(sPackageMsg(true, "Removed temp directory"));
		return ret;	
	}
	std::vector<sFile> files = installFile.GetFiles();
	
	for(int i = 0 ; i < files.size() ; i++)
	{
		std::filesystem::path srcFile = tmpDir;
		srcFile /= files[i].destLocation.substr(1, files[i].destLocation.length() - 1);
		srcFile /= files[i].fileName;

		std::string sHash = sFile::GenerateHash(srcFile);
		if (sHash != files[i].fileHash)
		{
			std::stringstream ss;
			ss << "Signature mismatch on file: " << srcFile;
			ret.success = false;
			ret.msgs.push_back(sPackageMsg(false, ss.str()));
			std::filesystem::remove_all(tmpDir);
			return ret;
		}
		else
		{
			std::stringstream ss;
			ss << "Signatures match on file: " << srcFile;
			ret.msgs.push_back(sPackageMsg(true, ss.str()));
		}
	}
	
	std::vector<std::filesystem::path> copiedFiles;
	for (int i = 0; i < files.size(); i++)
	{
		std::filesystem::path destDir = files[i].destLocation;
		std::stringstream vFileName;
		vFileName << files[i].fileName;
		vFileName << "." << files[i].fileVersion.ToString();
		destDir /= vFileName.str();
		
		std::filesystem::path srcFile = tmpDir;
		srcFile /= files[i].destLocation.substr(1, files[i].destLocation.length() - 1);
		srcFile /= files[i].fileName;
		try
		{
			const auto copyOptions = std::filesystem::copy_options::overwrite_existing;
			if (!std::filesystem::copy_file(srcFile, destDir, copyOptions)) 
			{
				std::stringstream ss;
				ss << "Failed to copy: " << srcFile << " to: " << destDir;
				ret.success = false;
				ret.msgs.push_back(sPackageMsg(false, ss.str()));
				for (int j = 0; j < copiedFiles.size(); j++)
				{
					std::filesystem::remove(copiedFiles[i]);
				}
				std::filesystem::remove_all(tmpDir);
				return ret;
			}
			std::stringstream ss;
			ss << "Copied: " << srcFile << " to: " << destDir;
			ret.msgs.push_back(sPackageMsg(true, ss.str()));
		}
		catch (std::exception &e)
		{
			std::stringstream ss;
			ss << "Failed to copy: " << srcFile << " to: " << destDir << ", msg: " << e.what();
			ret.success = false;
			ret.msgs.push_back(sPackageMsg(false, ss.str()));
			for (int j = 0; j < copiedFiles.size(); j++)
			{
				std::filesystem::remove(copiedFiles[i]);
			}
			std::filesystem::remove_all(tmpDir);
			return ret;
			
		}
		copiedFiles.push_back(destDir);
		// Create link for file
		std::filesystem::path linkSrc;
		linkSrc /= files[i].destLocation;
		linkSrc /= files[i].fileName;
		
		try
		{
			std::filesystem::permissions(destDir,
				std::filesystem::perms::owner_all);
		}
		catch (std::exception &e)
		{
			std::stringstream ss;
			ss << "Failed to copy: " << srcFile << " to: " << destDir << ", msg: " << e.what();
			ret.success = false;
			ret.msgs.push_back(sPackageMsg(false, ss.str()));
			for (int j = 0; j < copiedFiles.size(); j++)
			{
				std::filesystem::remove(copiedFiles[i]);
			}
			std::filesystem::remove_all(tmpDir);
			return ret;
		}
		if (std::filesystem::exists(linkSrc))
			std::filesystem::remove(linkSrc);
		std::filesystem::create_symlink(destDir, linkSrc);		
		std::stringstream ss;
		ss << "Created symlink: " << linkSrc << " to: " << destDir;
		ret.msgs.push_back(sPackageMsg(true, ss.str()));
		
	}
	
	std::filesystem::remove_all(tmpDir);
	ret.msgs.push_back(sPackageMsg (true, "Removed temporary files"));
	return ret;
}

sPackageStatus Package::uncompressFile(std::filesystem::path archive, std::filesystem::path dest, sPackageStatus status)
{
	struct archive *a;
	struct archive *ext;
	struct archive_entry *entry;
	int flags;
	int r;

	/* Select which attributes we want to restore. */
	flags = ARCHIVE_EXTRACT_TIME;
	flags |= ARCHIVE_EXTRACT_PERM;
	flags |= ARCHIVE_EXTRACT_ACL;
	flags |= ARCHIVE_EXTRACT_FFLAGS;
	a = archive_read_new();
	archive_read_support_format_gnutar(a);
	archive_read_support_filter_gzip(a);
	ext = archive_write_disk_new();
	archive_write_disk_set_options(ext, flags);
	archive_write_disk_set_standard_lookup(ext);
	if ((r = archive_read_open_filename(a, archive.c_str(), 10240)))
	{
		std::stringstream ss;
		ss << "Failed to read package: " << archive;
		status.success = false;
		status.msgs.push_back(sPackageMsg(false, ss.str()));
		return status;
	}
	
	for (;;) 
	{
		r = archive_read_next_header(a, &entry);
		if (r == ARCHIVE_EOF)
			break;
		if (r < ARCHIVE_OK)	
		{
			std::stringstream ss;
			ss << "Error reading package: " << archive_error_string(a);
			status.success = false;
			status.msgs.push_back(sPackageMsg(false, ss.str()));
			return status;
		}
		if (r < ARCHIVE_WARN)
		{
			std::stringstream ss;
			ss << "Error reading package: " << archive_error_string(a);
			status.success = false;
			status.msgs.push_back(sPackageMsg(false, ss.str()));
			return status;
		}
		// SEt name to based in our tempdir;
		std::filesystem::path p = dest;
		p /= archive_entry_pathname(entry);
		archive_entry_set_pathname(entry, p.c_str());
		r = archive_write_header(ext, entry);
		if (r < ARCHIVE_OK)
		{
			std::stringstream ss;
			ss << "Error reading package: " << archive_error_string(a);
			status.success = false;
			status.msgs.push_back(sPackageMsg(false, ss.str()));
			return status;
		}
		else 
		{
			if (archive_entry_size(entry) > 0) 
			{
				r = copy_data(a, ext);
				if (r < ARCHIVE_OK)
				{
					std::stringstream ss;
					ss << "Error reading package: " << archive_error_string(a);
					status.success = false;
					status.msgs.push_back(sPackageMsg(false, ss.str()));
					return status;
				}
				if (r < ARCHIVE_WARN)
				{
					std::stringstream ss;
					ss << "Error reading package: " << archive_error_string(a);
					status.success = false;
					status.msgs.push_back(sPackageMsg(false, ss.str()));
					return status;
				}
				if (r >= ARCHIVE_OK)
				{
					std::stringstream ss;
				}
			}
		}
	}
	status.success = true;
	return status;
}
int Package::copy_data(struct archive *ar, struct archive *aw)
{
	int r;
	const void *buff;
	size_t size;
	la_int64_t offset;

	for (;;) {
		r = archive_read_data_block(ar, &buff, &size, &offset);
		if (r == ARCHIVE_EOF)
			return (ARCHIVE_OK);
		if (r < ARCHIVE_OK)
			return (r);
		r = archive_write_data_block(aw, buff, size, offset);
		if (r < ARCHIVE_OK) {
			return (r);
		}
	}
}

std::string Package::genUUID()
{
	uuid_t uuid;
	uuid_generate_random(uuid);
	char *cuuid = (char *)malloc(37);
	uuid_unparse_upper(uuid, cuuid);
	std::stringstream ss;
	ss << cuuid;
	free(cuuid);
	return ss.str();
}
	
InstallationDesc Package::readDesc(std::string packageDescription)
{
	try
	{
		std::ifstream infile(packageDescription);
		nlohmann::json j;
		infile >> j;
		infile.close();
		return (InstallationDesc::FromJson(j));
	}
	catch (std::exception)
	{
		return InstallationDesc();
	}
}