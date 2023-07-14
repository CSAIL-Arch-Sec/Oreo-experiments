import argparse
import json
import os
import sys
from uuid import uuid4

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
from gem5.resources.resource import *
from gem5.simulate.simulator import Simulator
from gem5.simulate.exit_event import ExitEvent

from checkpoint.common import *

parser = argparse.ArgumentParser(
    description = "configuration script for checkpoint restore"
)

addCPUTypeArgument(parser, default = CPUTypes.O3)

parser.add_argument(
    "--script",
    type = str,
    default = "/root/experiments/command-scripts/exit-immediate.rcS",
    help = "path to script to run ??"
)

# stuff to help us restore

parser.add_argument(
    "--checkpoint-dir",
    type = str,
    default = None,
    help = "directory of checkpoint to restore from"
)

parser.add_argument(
    "--checkpoint-id",
    type = str,
    default = None,
    help = "id of checkpoint to restore from"
)

args = parser.parse_args()

# things for reading generated checkpoint config

if not (args.checkpoint_dir or args.checkpoint_id):
    parser.error('No checkpoint specified, please provide one of --checkpoint-dir or --checkpoint-id, thanks :D')
elif args.checkpoint_dir and args.checkpoint_id:
    parser.error('Please specify only one of --checkpoint-dir or --checkpoint-id, thanks :D')

checkpoint_dir = args.checkpoint_dir or \
    f"/root/experiments/m5outs/{args.checkpoint_id}/m5out-gen-cpt"
    # f"/Users/jzha/protect-kaslr/experiments/m5outs/{args.checkpoint_id}/m5out-gen-cpt"

with open(f"{checkpoint_dir}/config.json") as f:
    config = json.load(f)

cpu_cores = len(config.get("board").get("processor").get("cores"))
print(f'num_cores: {cpu_cores}')

kernel_path = config.get("board").get("workload").get("object_file")
print(f'kernel path: {kernel_path}')

disk_image_paths = [disk.get("image").get("child").get("image_file") for disk in \
    config.get("board").get("pc").get("south_bridge").get("ide").get("disks")]
if len(disk_image_paths) != 1:
    sys.exit("for now we are only dealing with single disk image ;-;")
disk_image_path = disk_image_paths[0]
print(f'disk path: {disk_image_path}')

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
    num_cores = cpu_cores,
)

# Here we setup the board. The X86Board allows for Full-System X86 simulations

board = X86Board(
    clk_freq = "3GHz",
    processor = processor,
    memory = memory,
    cache_hierarchy = cache_hierarchy,
)

board.set_kernel_disk_workload(
    kernel = CustomResource(
        local_path = kernel_path
    ),
    disk_image=CustomDiskImageResource(
        local_path = disk_image_path,
        disk_root_partition = "1"
    ),
    readfile = args.script,
)

parent_dir, _ = os.path.split(checkpoint_dir)
output_dir = f'{parent_dir}/m5out-{uuid4()}'
setOutDir(output_dir)

def handle_checkpoint():
    m5.checkpoint(m5.options.outdir)
    yield True

simulator = Simulator(
    board=board,
    on_exit_event={
        ExitEvent.CHECKPOINT: handle_checkpoint(),
    },
    checkpoint_path = checkpoint_dir,
)

print("Running the simulation")

simulator.run()

print("Done with the simulation")
