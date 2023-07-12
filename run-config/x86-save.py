import argparse

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
from gem5.resources.resource import (
    Resource,
    CustomDiskImageResource,
)
from gem5.simulate.simulator import Simulator
from gem5.simulate.exit_event import ExitEvent

parser = argparse.ArgumentParser(
    description="config script for checkpoint saver"
)

# We check for the required gem5 build.

requires(
    isa_required=ISA.X86,
    kvm_required=True,
)

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
    kernel=Resource("x86-linux-kernel-4.19.83"),
    disk_image=CustomDiskImageResource(
        "/root/disk-image/syscall/syscall-image/syscall",
        disk_root_partition="1"
    ),
    readfile_contents=command,
)

def handle_checkpoint():
    m5.checkpoint(m5.options.outdir)
    yield True

simulator = Simulator(
    board=board,
    on_exit_event={
        ExitEvent.CHECKPOINT: handle_checkpoint(),
    },
)

print("Running the simulation")
print("Using KVM cpu")

simulator.run()

print("All simulation events were successful.")
print("Done with the simulation")