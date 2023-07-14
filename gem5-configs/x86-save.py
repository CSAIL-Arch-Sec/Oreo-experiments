import argparse
from uuid import uuid4

from gem5.utils.requires import requires
from gem5.components.boards.x86_board import X86Board
from gem5.components.memory import DualChannelDDR4_2400
from gem5.components.processors.simple_processor import (
    SimpleProcessor,
)
from gem5.components.processors.cpu_types import CPUTypes
from gem5.isas import ISA
from gem5.resources.resource import *
from gem5.simulate.simulator import Simulator
from gem5.simulate.exit_event import ExitEvent

from utils.common import *

parser = argparse.ArgumentParser(
    description = "configuration script for checkpoint generation"
)

addCPUTypeArgument(parser, default = CPUTypes.KVM)

# cpu cores

# def positive_int_arg_filter(value):
#     int_value = int(value)
#     if int_value <= 0:
#         raise argparse.ArgumentTypeError(f"{int_value} not positive int")
#     return int_value

parser.add_argument(
    "--cpu-cores",
    # type = positive_int_arg_filter,
    type = int,
    default = 2,
    help = "number of cpu cores for run"
)

# kernel and disk image

parser.add_argument(
    "--kernel",
    type = lambda path: CustomResource(str(path)),
    default = Resource("x86-linux-kernel-4.19.83"),
    help = "kernel to use for run"
)

parser.add_argument(
    "--disk-image-path",
    type = str,
    default = "/root/experiments/disk-image/syscall/syscall-image/syscall",
    help = "disk image path to use for run"
)

# maybe more sensible "default" would be Resource("x86-ubuntu-18.04-img") ??

parser.add_argument(
    "--disk-root-partition",
    type = str,
    default = "1",
    help = "root partiton of disk image"
)

# checkpoint shenanigans

# uhh enforce no slash at end ??

parser.add_argument(
    "--outputs-dir",
    type = str,
    default = "/root/experiments/m5outs"
)

# parse args blah

args = parser.parse_args()

# We check for the required gem5 build.

requires(
    isa_required = ISA.X86,
    kvm_required = args.cpu_type == CPUTypes.KVM,
)

from gem5.components.cachehierarchies.classic.no_cache import NoCache

# Memory: Dual Channel DDR4 2400 DRAM device.
# The X86 board only supports 3 GB of main memory.

memory = DualChannelDDR4_2400(size="3GB")

processor = SimpleProcessor(
    cpu_type = args.cpu_type,
    isa = ISA.X86,
    num_cores = args.cpu_cores,
)

# Here we setup the board. The X86Board allows for Full-System X86 simulations

board = X86Board(
    clk_freq = "3GHz",
    processor = processor,
    memory = memory,
    cache_hierarchy = NoCache(),
)

board.set_kernel_disk_workload(
    # The x86 linux kernel will be automatically downloaded to the
    # `~/.cache/gem5` directory if not already present.
    kernel = Resource("x86-linux-kernel-4.19.83"),
    disk_image = CustomDiskImageResource(
        local_path = args.disk_image_path,
        disk_root_partition = args.disk_root_partition
    ),
)

output_dir = f"{args.outputs_dir}/{uuid4()}/m5out-gen-cpt"
setOutDir(output_dir)

simulator = Simulator(
    board = board,
    on_exit_event = {
        ExitEvent.CHECKPOINT: handle_checkpoint(),
    },
)

print("Starting simulation")

simulator.run()

print("Done with the simulation")
