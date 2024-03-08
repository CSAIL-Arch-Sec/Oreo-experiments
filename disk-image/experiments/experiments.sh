# Copyright (c) 2020 The Regents of the University of California.
# SPDX-License-Identifier: BSD 3-Clause

echo 'Experiments Installation Started'

sudo -S chown gem5 -R /home/gem5/experiments/
sudo -S chgrp gem5 -R /home/gem5/experiments/
echo "installing experiments ..."
cd /home/gem5/experiments && ./install.sh
sudo -S chown gem5 -R /home/gem5/experiments/
sudo -S chgrp gem5 -R /home/gem5/experiments/

sudo -S chown gem5 -R /home/gem5/LEBench-Sim/
sudo -S chgrp gem5 -R /home/gem5/LEBench-Sim/
echo "installing LEBench-Sim ..."
cd /home/gem5/LEBench-Sim && ./install.sh
sudo -S chown gem5 -R /home/gem5/LEBench-Sim/
sudo -S chgrp gem5 -R /home/gem5/LEBench-Sim/

sudo chown -R gem5:gem5 /home/gem5/experiments/
sudo chown -R gem5:gem5 /home/gem5/LEBench-Sim/

echo 'Experiments Installation Done'
