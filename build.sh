#!/bin/bash

cd $(dirname $0)

# sudo apt install pipx

docker rmi piotech:latest

docker build -t piotech:latest -t piotech:$(date +"%Y%m%d") -f docker/Dockerfile .

pipx install yolk3k

esphome_last_version=$(yolk -V esphome | awk '{print $2}')

if [ ! -d esphome ]; then
	wget -q https://github.com/esphome/esphome/archive/refs/tags/$esphome_last_version.tar.gz
    tar xzf $esphome_last_version.tar.gz
    mv esphome-$esphome_last_version esphome
	rm -f $esphome_last_version.tar.gz
fi

esphome_installed_version=$(grep __version__ esphome/esphome/const.py | awk -F= '{print $2}' | xargs)

if [ "$esphome_installed_version" != $esphome_last_version ]; then
	echo "a new version of esphome ($esphome_last_version) is different than the currently installed ($esphome_installed_version)"
	echo "'rm -fr esphome' to force new install"
	sleep 5
fi


