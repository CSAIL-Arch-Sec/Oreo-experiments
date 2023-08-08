
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

variable "image_name" {
  type    = string
  default = "base-ubuntu"
}

variable "image_size" {
  type    = string
  default = "24576"
}

variable "iso_checksum" {
  type    = string
  default = "34416ff83179728d54583bf3f18d42d2"
}

variable "iso_checksum_type" {
  type    = string
  default = "md5"
}

variable "iso_name" {
  type    = string
  default = "ubuntu-18.04.2-server-amd64.iso"
}

variable "iso_url" {
  type    = string
  default = "http://old-releases.ubuntu.com/releases/18.04.2/ubuntu-18.04.2-server-amd64.iso"
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

source "qemu" "base-ubuntu" {
  accelerator      = "kvm"
  boot_command     = ["${var.boot_command_prefix}", "debian-installer=${var.locale} auto locale=${var.locale} kbd-chooser/method=us ", "file=/floppy/${var.preseed} ", "fb=false debconf/frontend=noninteractive ", "hostname=${var.hostname} ", "/install/vmlinuz noapic ", "initrd=/install/initrd.gz ", "keyboard-configuration/modelcode=SKIP keyboard-configuration/layout=USA ", "keyboard-configuration/variant=USA console-setup/ask_detect=false ", "passwd/user-fullname=${var.ssh_fullname} ", "passwd/user-password=${var.ssh_password} ", "passwd/user-password-again=${var.ssh_password} ", "passwd/username=${var.ssh_username} ", "-- <enter>"]
  cpus             = "${var.vm_cpus}"
  disk_size        = "${var.image_size}"
  floppy_files     = ["shared/${var.preseed}"]
  format           = "raw"
  headless         = "${var.headless}"
  http_directory   = "shared/"
  iso_checksum     = "${var.iso_checksum_type}:${var.iso_checksum}"
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
  sources = ["source.qemu.base-ubuntu"]
  post-processor "checksum" {
    checksum_types = [ "md5" ]
    output = "experiments/${var.image_name}-image/packer_{{.BuildName}}_{{.ChecksumType}}.checksum"
  }
}
