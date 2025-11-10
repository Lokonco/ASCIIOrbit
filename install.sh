#!/bin/bash
set -e

echo "Installing ASCIIOrbit..."
git clone https://github.com/Lokonco/ASCIIOrbit.git /tmp/asciiorbit
cd /tmp/asciiorbit
mkdir -p build && cd build
cmake ..
make
sudo make install
cd ~
rm -rf /tmp/asciiorbit
echo "ASCIIOrbit installed! Run with: asciiorbit"
