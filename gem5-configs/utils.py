import os
import m5
import m5.core
import m5.options
import sys
import shutil
import m5.trace
from pretty_print import *

# https://stackoverflow.com/questions/185936/how-to-delete-the-contents-of-a-folder
def clear_dir(dir):
    for root, dirs, files in os.walk(dir):
        for f in files:
            os.unlink(os.path.join(root, f))
        for d in dirs:
            shutil.rmtree(os.path.join(root, d))

def set_outdir(outdir):
    pretty_print(f"Setting output directory to: {outdir}")
    os.makedirs(outdir, exist_ok=True)
    clear_dir(outdir)
    m5.core.setOutputDir(outdir)
    m5.options.outdir = outdir

# parts from m5.main
def handle_std_redirects(args, output_dir):
    if args.redirect_stderr:
        stderr_file = args.stderr_file or os.path.join(output_dir, "sim_stderr")
        pretty_print(f"Redirecting stderr to: {stderr_file}")
        redir_fd = os.open(stderr_file, os.O_WRONLY | os.O_CREAT | os.O_TRUNC)
        os.dup2(redir_fd, sys.stderr.fileno())
    if args.redirect_stdout:
        stderr_file = args.stderr_file or os.path.join(output_dir, "sim_stdout")
        pretty_print(f"Redirecting stdout to: {stderr_file}")
        redir_fd = os.open(stderr_file, os.O_WRONLY | os.O_CREAT | os.O_TRUNC)
        os.dup2(redir_fd, sys.stderr.fileno())

def set_debug_file(args, output_dir):
    debug_file_path = args.debug_file or os.path.join(output_dir, "sim_debug")
    pretty_print(f"Saving debug trace to: {debug_file_path}")
    m5.trace.output(debug_file_path)
    m5.options.debug_file = debug_file_path