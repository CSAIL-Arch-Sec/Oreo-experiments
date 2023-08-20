# Copyright (c) 2020 The Regents of the University of California.
# SPDX-License-Identifier: BSD 3-Clause

cd /home/gem5/
su gem5

echo "12345" | sudo -S chown gem5 -R experiments/
echo "12345" | sudo -S chgrp gem5 -R experiments/
cd experiments
./install.sh
cd ..
echo "12345" | sudo -S chown gem5 -R experiments/
echo "12345" | sudo -S chgrp gem5 -R experiments/