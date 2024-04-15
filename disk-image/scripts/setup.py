from pathlib import Path
from tenacity import retry
import click
import subprocess


script_dir = Path(__file__).resolve().parent
disk_dir = script_dir.parent
root_dir = disk_dir.parent.parent


def start_vm(vm_path: Path, ssh_dest: str, ssh_password: str):
    @retry()
    def is_started():
        p = subprocess.run(
            f"sshpass -p {ssh_password} ssh {ssh_dest} -t 'uname -a'",
            shell=True, stdout=subprocess.PIPE
        )
        if p.returncode:
            raise Exception

    p = subprocess.run(
        f"sshpass -p {ssh_password} ssh {ssh_dest} -t 'uname -a'",
        shell=True, stdout=subprocess.PIPE
    )
    if not p.returncode:
        print("VM already started")
        return

    subprocess.run(
        f"vmrun -T ws start {vm_path}",
        shell=True, stdout=subprocess.PIPE
    )
    is_started()
    print("VM started")


def stop_vm(vm_path: Path):
    subprocess.run(
        f"vmrun stop {vm_path}",
        shell=True, stdout=subprocess.PIPE
    )
    print("VM stopped")


def copy_files(src_list: list[Path], dest_path: Path, ssh_dest: str, ssh_password: str):
    for src in src_list:
        print("Copy from", src, "to", dest_path)
        cmd = f"sshpass -p {ssh_password} scp -r {src} {ssh_dest}:{dest_path}"
        print(cmd)
        subprocess.run(
            cmd,
            shell=True
        )


def base_setup(ssh_dest: str, ssh_password: str, gem5_suffix: str, linux_suffix: str):
    dest_path = Path("/home/gem5")
    # src_list = [
    #     root_dir / f"gem5{gem5_suffix}/util/m5/build/x86/out/m5",
    #     disk_dir / "shared/serial-getty@.service",
    #     disk_dir / "experiments/runscript.sh",
    #     root_dir / f"linux{linux_suffix}",
    #     root_dir / f"linux{linux_suffix}/vmlinux_gem5",
    #     disk_dir / "experiments/hack_back_ckpt.rcS",
    #     ]
    # copy_files(src_list, dest_path, ssh_dest, ssh_password)
    #
    # # TODO: Also need to check which branch contains the target linux and switch to the correct branch!!!
    # if linux_suffix != "":
    #     cmd = f"sshpass -p {ssh_password} ssh {ssh_dest} -t 'mv {dest_path}/linux{linux_suffix} {dest_path}/linux'"
    #     print(cmd)
    #     subprocess.run(
    #         cmd,
    #         shell=True, stdout=subprocess.PIPE
    #     )

    # linux_include_path = "arch/x86/include/asm"
    # copy_files([root_dir / f"linux{linux_suffix}" / linux_include_path / "*"],
    #            dest_path / "linux" / linux_include_path,
    #            ssh_dest, ssh_password)

    script = disk_dir / "experiments/base-experiments.sh"
    cmd = f"sshpass -p {ssh_password} ssh {ssh_dest} -t 'bash -s' < {script}"
    print(cmd)
    subprocess.run(
        cmd,
        shell=True
    )


def experiments_setup(ssh_dest: str, ssh_password: str):
    dest_path = Path("/home/gem5")
    src_list = [
        disk_dir / "experiments/experiments",
        # disk_dir / "experiments/LEBench-Sim",
        ]
    copy_files(src_list, dest_path, ssh_dest, ssh_password)

    script = disk_dir / "experiments/experiments.sh"
    cmd = f"sshpass -p {ssh_password} ssh {ssh_dest} -t 'bash -s' < {script}"
    print(cmd)
    subprocess.run(
        cmd,
        shell=True
    )


def generate_img(vm_path: Path):
    img_dir = vm_path.parent
    img_name = vm_path.stem
    # TODO: Figure out why this suffix is added to the name!!!
    img_path = img_dir / f"{img_name}-000001.vmdk"
    output_path = disk_dir / f"experiments.img"
    cmd = f"qemu-img convert -f vmdk -O raw {img_path} {output_path}"
    print(cmd)
    p = subprocess.run(
        cmd,
        shell=True
    )
    if p.returncode:
        print("Failed at generating img!!!")


@click.command()
@click.option(
    "--vm-path",
    type=Path,
    default=Path("/home/shixinsong/vmware/ubuntu-jammy/ubuntu-jammy.vmx")
)
@click.option(
    "--ssh-dest",
    type=click.STRING,
    default="gem5@192.168.119.128"
)
@click.option(
    "--ssh-password",
    type=click.STRING,
    default="packerubuntu"
)
def main(vm_path: Path, ssh_dest: str, ssh_password: str):
    start_vm(vm_path, ssh_dest, ssh_password)
    # base_setup(ssh_dest, ssh_password, "-new", "-new")
    experiments_setup(ssh_dest, ssh_password)
    stop_vm(vm_path)
    generate_img(vm_path)

    # sudo touch /etc/cloud/cloud-init.disabled # disable cloud init
    # sudo systemctl disable systemd-networkd-wait-online.service


if __name__ == '__main__':
    main()
