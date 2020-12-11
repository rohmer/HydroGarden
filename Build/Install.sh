#!/bin/bash

tar -xf HydroGarden.tar.gz
sudo systemctl stop HydroGarden.service
sudo cp HydroGarden.service /lib/systemd/system
sudo cp HydroGarden /usr/local/bin
sudo cp bcm2835.so /usr/local/bin
sudo systemctl start HydroGarden.service
sudo systemctl enable HydroGarden.service
sudo systemctl daemon-reload
