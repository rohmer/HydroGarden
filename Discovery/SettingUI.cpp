#include "SettingUI.h"

nlohmann::json SettingUI::ToJSON(Setting setting)
{
	switch (setting.SettingType())
	{
	case Setting::eSTR:
		return SettingUIString::ToJSON(setting);
	}
}