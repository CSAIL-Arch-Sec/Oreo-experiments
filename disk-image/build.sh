PACKER_VERSION="1.9.1"

if [ ! -f ./packer ]; then
    wget https://releases.hashicorp.com/packer/${PACKER_VERSION}/packer_${PACKER_VERSION}_linux_amd64.zip;
    unzip packer_${PACKER_VERSION}_linux_amd64.zip;
    rm packer_${PACKER_VERSION}_linux_amd64.zip;
fi

while getopts ":s" option; do
  case $option in
    s)
      single_stage=true
      ;;
    *)
      echo "Usage: $0 [-s]"
      exit 1
      ;;
  esac
done

BUILD_STAGE_NAME=${1:-experiments}

if [ "$single_stage" = true ]; then
    echo "building $1-image ..."
    rm -rf experiments/$1-image
    ./packer validate -only=qemu.$1 experiments/experiments.pkr.hcl
    ./packer build  -only=qemu.$1 experiments/experiments.pkr.hcl
else
    if [ "$BUILD_STAGE_NAME" == "base-ubuntu" ]; then
        BUILD_STAGE_INDEX=0
    elif [ "$BUILD_STAGE_NAME" == "base-experiments" ]; then
        BUILD_STAGE_INDEX=1
    elif [ "$BUILD_STAGE_NAME" == "experiments" ]; then
        BUILD_STAGE_INDEX=2
    else
        echo "invalid stage -- options are { base-ubuntu, base-experiments, experiments }"
        exit 1
    fi

    if [ "$BUILD_STAGE_INDEX" -le "0" ]; then
        echo "building base-ubuntu-image ..."
        rm -r experiments/base-ubuntu-image
        ./packer validate -only=qemu.base-ubuntu experiments/experiments.pkr.hcl
        ./packer build  -only=qemu.base-ubuntu experiments/experiments.pkr.hcl
    fi
    if [ "$BUILD_STAGE_INDEX" -le "1" ]; then
        echo "building base-experiments-image ..."
        rm -r experiments/base-experiments-image
        ./packer validate -only=qemu.base-experiments experiments/experiments.pkr.hcl
        ./packer build  -only=qemu.base-experiments experiments/experiments.pkr.hcl
    fi
    if [ "$BUILD_STAGE_INDEX" -le "2" ]; then
        echo "building experiments-image ..."
        rm -r experiments/experiments-image
        ./packer validate -only=qemu.experiments experiments/experiments.pkr.hcl
        ./packer build  -only=qemu.experiments experiments/experiments.pkr.hcl
    fi
fi