
variable "boot_command_prefix" {
  type    = string
  default = "<enter><wait><f6><esc><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs><bs>"
}

variable "desktop" {
  type    = string
  default = "false"
}

variable "headless" {
  type    = string
  default = "true"
}

variable "hostname" {
  type    = string
  default = "gem5"
}

variable "image_size" {
  type    = string
  default = "24576"
}

variable "locale" {
  type    = string
  default = "en_US"
}

variable "preseed" {
  type    = string
  default = "preseed.cfg"
}

variable "ssh_fullname" {
  type    = string
  default = "gem5"
}

variable "ssh_password" {
  type    = string
  default = "12345"
}

variable "ssh_username" {
  type    = string
  default = "gem5"
}

variable "vm_cpus" {
  type    = string
  default = "16"
}

variable "vm_memory" {
  type    = string
  default = "8192"
}


# base-ubuntu build stage

variable "base_ubuntu_image_name" {
  type    = string
  default = "base-ubuntu"
}

variable "base_ubuntu_iso_checksum" {
  type    = string
  default = "34416ff83179728d54583bf3f18d42d2"
}

variable "base_ubuntu_iso_checksum_type" {
  type    = string
  default = "md5"
}

variable "base_ubuntu_iso_name" {
  type    = string
  default = "ubuntu-18.04.2-server-amd64.iso"
}

variable "base_ubuntu_iso_url" {
  type    = string
  default = "http://old-releases.ubuntu.com/releases/18.04.2/ubuntu-18.04.2-server-amd64.iso"
}

source "qemu" "base-ubuntu" {
  accelerator      = "kvm"
  boot_command     = ["${var.boot_command_prefix}", "debian-installer=${var.locale} auto locale=${var.locale} kbd-chooser/method=us ", "file=/floppy/${var.preseed} ", "fb=false debconf/frontend=noninteractive ", "hostname=${var.hostname} ", "/install/vmlinuz noapic ", "initrd=/install/initrd.gz ", "keyboard-configuration/modelcode=SKIP keyboard-configuration/layout=USA ", "keyboard-configuration/variant=USA console-setup/ask_detect=false ", "passwd/user-fullname=${var.ssh_fullname} ", "passwd/user-password=${var.ssh_password} ", "passwd/user-password-again=${var.ssh_password} ", "passwd/username=${var.ssh_username} ", "-- <enter>"]
  cpus             = "${var.vm_cpus}"
  disk_size        = "${var.image_size}"
  floppy_files     = ["shared/${var.preseed}"]
  format           = "raw"
  headless         = "${var.headless}"
  http_directory   = "shared/"
  iso_checksum     = "${var.base_ubuntu_iso_checksum_type}:${var.base_ubuntu_iso_checksum}"
  iso_urls         = ["${var.base_ubuntu_iso_url}"]
  memory           = "${var.vm_memory}"
  output_directory = "experiments/${var.base_ubuntu_image_name}-image"
  qemu_binary      = "/usr/bin/qemu-system-x86_64"
  qemuargs         = [["-cpu", "host"], ["-display", "none"]]
  shutdown_command = "echo '${var.ssh_password}'|sudo -S shutdown -P now"
  ssh_password     = "${var.ssh_password}"
  ssh_username     = "${var.ssh_username}"
  ssh_wait_timeout = "60m"
  vm_name          = "${var.base_ubuntu_image_name}"
}

build {
  sources = ["source.qemu.base-ubuntu"]
  post-processor "checksum" {
    checksum_types = [ "md5" ]
    output = "experiments/${var.base_ubuntu_image_name}-image/{{.BuildName}}_{{.ChecksumType}}.checksum"
  }
}


# base-experiments build stage

variable "base_experiments_image_name" {
  type    = string
  default = "base-experiments"
}

variable "base_experiments_iso_checksum" {
  type = string
  default = "file:experiments/base-ubuntu-image/base-ubuntu_md5.checksum"
}

variable "base_experiments_iso_url" {
  type    = string
  default = "experiments/base-ubuntu-image/base-ubuntu"
}

source "qemu" "base-experiments" {
  accelerator      = "kvm"
  cpus             = "${var.vm_cpus}"
  disk_image       = true
  disk_size        = "${var.image_size}"
  format           = "raw"
  headless         = "${var.headless}"
  http_directory   = "shared/"
  iso_checksum     = "${var.base_experiments_iso_checksum}"
  iso_urls         = ["${var.base_experiments_iso_url}"]
  memory           = "${var.vm_memory}"
  output_directory = "experiments/${var.base_experiments_image_name}-image"
  qemu_binary      = "/usr/bin/qemu-system-x86_64"
  qemuargs         = [["-cpu", "host"], ["-display", "none"]]
  shutdown_command = "echo '${var.ssh_password}'|sudo -S shutdown -P now"
  ssh_password     = "${var.ssh_password}"
  ssh_username     = "${var.ssh_username}"
  ssh_wait_timeout = "60m"
  vm_name          = "${var.base_experiments_image_name}"
}

build {
  sources = ["source.qemu.base-experiments"]

  provisioner "file" {
    destination = "/home/gem5/"
    source      = "/root/gem5/util/m5/build/x86/out/m5"
  }

  provisioner "file" {
    destination = "/home/gem5/"
    source      = "shared/serial-getty@.service"
  }

  provisioner "file" {
    destination = "/home/gem5/"
    source      = "experiments/runscript.sh"
  }

  # provisioner "file" {
  #   destination = "/home/gem5/"
  #   source      = "experiments/experiments"
  # }

  provisioner "file" {
    destination = "/home/gem5/"
    source      = "/root/linux"
  }

  provisioner "file" {
    destination = "/home/gem5/"
    source      = "/root/linux/vmlinux_gem5"
  }

  provisioner "file" {
    destination = "/home/gem5/"
    source      = "experiments/hack_back_ckpt.rcS"
  }

  provisioner "shell" {
    execute_command = "echo '${var.ssh_password}' | {{ .Vars }} sudo -E -S bash '{{ .Path }}'"
    scripts         = ["experiments/base-experiments-install.sh", "experiments/base-experiments-post-installation.sh"]
  }

  post-processor "checksum" {
    checksum_types = [ "md5" ]
    output = "experiments/${var.base_experiments_image_name}-image/{{.BuildName}}_{{.ChecksumType}}.checksum"
  }

}


# experiments build stage

variable "experiments_image_name" {
  type    = string
  default = "experiments"
}

variable "experiments_iso_checksum" {
  type = string
  default = "file:experiments/base-experiments-image/base-experiments_md5.checksum"
}

variable "experiments_iso_url" {
  type    = string
  default = "experiments/base-experiments-image/base-experiments"
}

source "qemu" "experiments" {
  accelerator      = "kvm"
  cpus             = "${var.vm_cpus}"
  disk_image       = true
  disk_size        = "${var.image_size}"
  format           = "raw"
  headless         = "${var.headless}"
  http_directory   = "shared/"
  iso_checksum     = "${var.experiments_iso_checksum}"
  iso_urls         = ["${var.experiments_iso_url}"]
  memory           = "${var.vm_memory}"
  output_directory = "experiments/${var.experiments_image_name}-image"
  qemu_binary      = "/usr/bin/qemu-system-x86_64"
  qemuargs         = [["-cpu", "host"], ["-display", "none"]]
  shutdown_command = "echo '${var.ssh_password}'|sudo -S shutdown -P now"
  ssh_password     = "${var.ssh_password}"
  ssh_username     = "${var.ssh_username}"
  ssh_wait_timeout = "60m"
  vm_name          = "${var.experiments_image_name}"
}

build {
  sources = ["source.qemu.experiments"]

  provisioner "file" {
    destination = "/home/gem5/"
    source      = "experiments/experiments"
  }

  provisioner "file" {
    destination = "/home/gem5/"
    source      = "experiments/LEBench-Sim"
  }

  provisioner "shell" {
    execute_command = "echo '${var.ssh_password}' | {{ .Vars }} sudo -E -S bash '{{ .Path }}'"
    scripts         = ["experiments/experiments-install.sh", "experiments/experiments-post-installation.sh"]
  }

}