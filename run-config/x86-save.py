# Copyright (c) 2021 The Regents of the University of California.
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met: redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer;
# redistributions in binary form must reproduce the above copyright
# notice, this list of conditions and the following disclaimer in the
# documentation and/or other materials provided with the distribution;
# neither the name of the copyright holders nor the names of its
# contributors may be used to endorse or promote products derived from
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import argparse
import time
import sys

import m5
from m5.objects import Root

from gem5.utils.requires import requires
from gem5.components.boards.x86_board import X86Board
from gem5.components.memory import DualChannelDDR4_2400
from gem5.components.processors.simple_processor import (
    SimpleProcessor,
)
from gem5.components.processors.cpu_types import CPUTypes
from gem5.isas import ISA
from gem5.coherence_protocol import CoherenceProtocol
from gem5.resources.resource import (
    Resource,
    CustomResource,
    CustomDiskImageResource,
)
from gem5.simulate.simulator import Simulator
from gem5.simulate.exit_event import ExitEvent
from gem5.simulate.exit_event_generators import save_checkpoint_generator

# We check for the required gem5 build.

requires(
    isa_required=ISA.X86,
    coherence_protocol_required=CoherenceProtocol.MESI_TWO_LEVEL,
    kvm_required=True,
)

# Setting up all the fixed system parameters here
# Caches: MESI Two Level Cache Hierarchy

#from gem5.components.cachehierarchies.ruby.mesi_two_level_cache_hierarchy import (
#    MESITwoLevelCacheHierarchy,
#)
#
#cache_hierarchy = MESITwoLevelCacheHierarchy(
#    l1d_size="32kB",
#    l1d_assoc=8,
#    l1i_size="32kB",
#    l1i_assoc=8,
#    l2_size="256kB",
#    l2_assoc=16,
#    num_l2_banks=2,
#)

from gem5.components.cachehierarchies.classic.no_cache import NoCache

# Memory: Dual Channel DDR4 2400 DRAM device.
# The X86 board only supports 3 GB of main memory.

memory = DualChannelDDR4_2400(size="3GB")

processor = SimpleProcessor(
    cpu_type=CPUTypes.KVM,
    isa=ISA.X86,
    num_cores=2,
)

# Here we setup the board. The X86Board allows for Full-System X86 simulations

board = X86Board(
    clk_freq="3GHz",
    processor=processor,
    memory=memory,
    cache_hierarchy=NoCache(),
)


command = "m5 exit;"

board.set_kernel_disk_workload(
    # The x86 linux kernel will be automatically downloaded to the
    # `~/.cache/gem5` directory if not already present.
    # PARSEC benchamarks were tested with kernel version 4.19.83
    kernel=Resource("x86-linux-kernel-4.19.83"),
    # The x86-parsec image will be automatically downloaded to the
    # `~/.cache/gem5` directory if not already present.
    disk_image=CustomDiskImageResource(
        "disk-image/syscall/syscall-image/syscall",
        disk_root_partition="1"
    ),
    readfile_contents=command,
)

def handle_checkpoint():
    print("blah")
    simulator.save_checkpoint(m5.options.outdir)
    sys.exit(1)
    #m5.checkpoint(m5.options.outdir)
    print("checkpoint things")
    yield True
    #save_checkpoint_generator(m5.options.outdir)

simulator = Simulator(
    board=board,
    on_exit_event={
        ExitEvent.CHECKPOINT: handle_checkpoint(),
    },
)

# We maintain the wall clock time.

print("Running the simulation")
print("Using KVM cpu")

# We start the simulation
#m5.instantiate(m5.options.outdir)
simulator.run()

print("All simulation events were successful.")

# We print the final simulation statistics.

print("Done with the simulation")