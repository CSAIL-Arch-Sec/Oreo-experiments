from gem5.components.processors.cpu_types import CPUTypes
from gem5.resources.resource import *

m5outs_default_dir = "/root/experiments/m5outs"

def add_std_redirect_arguments(parser):

    parser.add_argument(
        "--redirect-stderr",
        action = "store_true",
        default = True
    )
    parser.add_argument(
        "--no-redirect-stderr",
        dest = "redirect-stderr",
        action = "store_false"
    )
    parser.add_argument(
        "--stderr-file",
        type = str,
        default = None
    )
    parser.add_argument(
        "--redirect-stdout",
        action = "store_true",
        default = False
    )
    parser.add_argument(
        "--no-redirect-stdout",
        dest = "redirect-stdout",
        action = "store_false"
    )
    parser.add_argument(
        "--stdout-file",
        type = str,
        default = None
    )

def add_debug_arguments(parser):
    parser.add_argument(
        '--debug-file',
        type = str,
        default = None
    )


def add_checkpoint_restore_arguments(parser):
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
    parser.add_argument(
        "--checkpoint-tick",
        type = int,
        default = None,
        help = "for restoring tick checkpoint"
    )
    parser.add_argument(
        "--checkpoint-latest",
        action = "store_true",
        default = False,
        help = "restore from most recent checkpoint"
    )
    add_uuid_dir_argument(parser)

def add_checkpoint_save_arguments(parser):
    parser.add_argument(
        "--outputs-dir",
        type = str,
        default = m5outs_default_dir
    )
    parser.add_argument(
        '--checkpoint',
        type = str,
        nargs = '+',
        default = ""
    )
    add_uuid_dir_argument(parser)

def add_kernel_disk_arguments(parser):
    parser.add_argument(
        "--kernel",
        type = lambda path: CustomResource(str(path)),
        default = Resource("x86-linux-kernel-5.4.49"),
        help = "kernel to use for run"
    )
    parser.add_argument(
        "--disk-image-path",
        type = str,
        default = "/root/experiments/disk-image/experiments/experiments-image/experiments",
        help = "disk image path to use for run"
    )
    parser.add_argument(
        "--disk-root-partition",
        type = str,
        default = "1",
        help = "root partiton of disk image"
    )

def add_run_script_arguments(parser):
    parser.add_argument(
        "--script",
        type = str,
        default = "/root/experiments/command-scripts/exit_immediate.rcS",
        help = "path to script to run ??"
    )

def add_cpu_arguments(parser, default_type: CPUTypes):
    parser.add_argument(
        "--cpu-type",
        type = lambda name: CPUTypes.__members__.get(str(name)),
        default = default_type,
        help = "cpu type for simulation",
        choices = [type for _, type in CPUTypes.__members__.items()],
    )
    parser.add_argument(
        "--cpu-cores",
        type = int,
        default = 2,
        help = "number of cpu cores for run"
    )

def add_uuid_dir_argument(parser):
    parser.add_argument(
        "--uuid-dir",
        action = "store_true",
        default = False
    )

def add_kernel_phy_load_arguments(parser):
    val_to_int = lambda val: int(val, 0)

    parser.add_argument(
        "--load-addr-mask",
        type = val_to_int,
        default = 0xFFFFFFFFFFFFFFFF
    )

    parser.add_argument(
        "--load-addr-offset",
        type = val_to_int,
        default = 0
    )

    parser.add_argument(
        '--addr-check', 
        action = 'store_true',
        default = True
    )

    parser.add_argument(
        '--no-addr-check', 
        dest = 'addr_check', 
        action = 'store_false'
    )

def add_cache_arguments(parser):
    parser.add_argument(
        '--ruby-cache', 
        action = 'store_true',
        default = False
    )