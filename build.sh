#!/bin/bash

cd $(dirname $0)

# sudo apt install pipx


which yolk || pipx install yolk3k

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

cp esphome/platformio.ini .
# patch < platformio.ini.patch
cp esphome/pylintrc .
cp esphome/sdkconfig.defaults .
cp esphome/script/clang-tidy scripts/clang-tidy
cp esphome/.vscode/tasks.json .vscode



docker rmi piotech:latest

docker build -t piotech:latest -f docker/Dockerfile .


