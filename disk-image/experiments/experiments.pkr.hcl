
variable "desktop" {
  type    = string
  default = "false"
}

variable "headless" {
  type    = string
  default = "true"
}

variable "image_name" {
  type    = string
  default = "experiments"
}

variable "image_size" {
  type    = string
  default = "24576"
}

variable "iso_checksum" {
  type = string
  default = "file:experiments/base-ubuntu-image/base-ubuntu_md5.checksum"
}

variable "iso_url" {
  type    = string
  default = "experiments/base-ubuntu-image/base-ubuntu"
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

source "qemu" "experiments" {
  accelerator      = "kvm"
  cpus             = "${var.vm_cpus}"
  disk_image       = true
  disk_size        = "${var.image_size}"
  format           = "raw"
  headless         = "${var.headless}"
  http_directory   = "shared/"
  iso_checksum     = "${var.iso_checksum}"
  iso_urls         = ["${var.iso_url}"]
  memory           = "${var.vm_memory}"
  output_directory = "experiments/${var.image_name}-image"
  qemu_binary      = "/usr/bin/qemu-system-x86_64"
  qemuargs         = [["-cpu", "host"], ["-display", "none"]]
  shutdown_command = "echo '${var.ssh_password}'|sudo -S shutdown -P now"
  ssh_password     = "${var.ssh_password}"
  ssh_username     = "${var.ssh_username}"
  ssh_wait_timeout = "60m"
  vm_name          = "${var.image_name}"
}

build {
  sources = ["source.qemu.experiments"]

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

  provisioner "file" {
    destination = "/home/gem5/"
    source      = "experiments/experiments"
  }

  provisioner "file" {
    destination = "/home/gem5/"
    source      = "experiments/hack_back_ckpt.rcS"
  }

  provisioner "shell" {
    execute_command = "echo '${var.ssh_password}' | {{ .Vars }} sudo -E -S bash '{{ .Path }}'"
    scripts         = ["experiments/post-installation.sh", "experiments/experiments-install.sh"]
  }

}
