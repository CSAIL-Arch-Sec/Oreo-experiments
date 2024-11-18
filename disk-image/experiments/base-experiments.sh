# Copyright (c) 2020 The Regents of the University of California.
# SPDX-License-Identifier: BSD 3-Clause

echo 'Base Installation Started'

sudo apt-get update
sudo apt-get install -y debconf-utils build-essential gfortran

cd /home/gem5/linux && sudo make modules_install

# Allowing services to restart while updating some
# libraries.
sudo debconf-get-selections | grep restart-without-asking > /home/gem5/libs.txt
sed -i 's/false/true/g' /home/gem5/libs.txt
while read line; do echo $line | sudo debconf-set-selections; done < /home/gem5/libs.txt
sudo rm /home/gem5/libs.txt

sudo cp /home/gem5/serial-getty@.service /lib/systemd/system/
sudo cp /home/gem5/m5 /sbin
sudo rm /sbin/gem5
sudo ln -s /sbin/m5 /sbin/gem5
# copy and run outside (host) script after booting
cat "/home/gem5/hack_back_ckpt.rcS" | sudo tee -a /root/.bashrc

echo 'Base Installation Done'