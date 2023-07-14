import os
import m5
from m5.core import setOutputDir

from gem5.components.processors.cpu_types import CPUTypes

def addCPUTypeArgument(parser, default):
    parser.add_argument(
        "--cpu-type",
        type = lambda name: CPUTypes.__members__.get(name),
        default = default,
        help = "cpu type for simulation",
        choices = [type for name, type in CPUTypes.__members__.items()],
    )

def setOutDir(outdir):
    os.makedirs(outdir)
    setOutputDir(outdir)
    m5.options.outdir = outdir

def handle_checkpoint():
    m5.checkpoint(m5.options.outdir)
    yield True