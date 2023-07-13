import argparse
import os

import m5

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
    CustomDiskImageResource,
)
from gem5.simulate.simulator import Simulator
from gem5.simulate.exit_event import ExitEvent

parser = argparse.ArgumentParser(
    description = "configuration script for checkpoint restore"
)

parser.add_argument(
    "--cpu-type",
    type = lambda name: CPUTypes.__members__.get(name),
    default = CPUTypes.KVM,
    help = "cpu type for checkpoint generation",
    choices = list(CPUTypes.__members__.values()),
)

parser.add_argument(
    "--script",
    type = str,
    default = "/root/experiments/command-scripts/exit-immediate.rcS",
    help = "path to script to run ??"
)

args = parser.parse_args()

# We check for the required gem5 build.

requires(
    isa_required = ISA.X86,
    coherence_protocol_required = CoherenceProtocol.MESI_TWO_LEVEL,
)

# Setting up all the fixed system parameters here
# Caches: MESI Two Level Cache Hierarchy

from gem5.components.cachehierarchies.ruby.mesi_two_level_cache_hierarchy import (
    MESITwoLevelCacheHierarchy,
)

cache_hierarchy = MESITwoLevelCacheHierarchy(
    l1d_size="32kB",
    l1d_assoc=8,
    l1i_size="32kB",
    l1i_assoc=8,
    l2_size="256kB",
    l2_assoc=16,
    num_l2_banks=2,
)

# Memory: Dual Channel DDR4 2400 DRAM device.
# The X86 board only supports 3 GB of main memory.

memory = DualChannelDDR4_2400(size="3GB")

processor = SimpleProcessor(
    cpu_type = args.cpu_type,
    isa = ISA.X86,
    num_cores = 2,
)

# Here we setup the board. The X86Board allows for Full-System X86 simulations

board = X86Board(
    clk_freq = "3GHz",
    processor = processor,
    memory = memory,
    cache_hierarchy = cache_hierarchy,
)

board.set_kernel_disk_workload(
    # The x86 linux kernel will be automatically downloaded to the
    # `~/.cache/gem5` directory if not already present.
    kernel=Resource("x86-linux-kernel-4.19.83"),
    disk_image=CustomDiskImageResource(
        "/root/experiments/disk-image/syscall/syscall-image/syscall",
        disk_root_partition="1"
    ),
    readfile = args.script,
)

def handle_checkpoint():
    m5.checkpoint(m5.options.outdir)
    yield True

simulator = Simulator(
    board=board,
    on_exit_event={
        ExitEvent.CHECKPOINT: handle_checkpoint(),
    },
    checkpoint_path=m5.options.outdir,
)

# We maintain the wall clock time.

print("Running the simulation")
print("Using KVM cpu")

# We start the simulation
simulator.run()

print("All simulation events were successful.")
print("Done with the simulation")