# Copyright (c) 2020 The Regents of the University of California.
# SPDX-License-Identifier: BSD 3-Clause

cd /home/gem5/
su gem5
echo "12345" | sudo -S apt update

# Allowing services to restart while updating some
# libraries.
sudo apt install -y debconf-utils
sudo debconf-get-selections | grep restart-without-asking > libs.txt
sed -i 's/false/true/g' libs.txt
while read line; do echo $line | sudo debconf-set-selections; done < libs.txt
sudo rm libs.txt
##

# Installing packages needed
sudo apt install -y build-essential

echo "12345" | sudo -S chown gem5 -R experiments/
echo "12345" | sudo -S chgrp gem5 -R experiments/
cd experiments
./install.sh
cd ..
echo "12345" | sudo -S chown gem5 -R experiments/
echo "12345" | sudo -S chgrp gem5 -R experiments/