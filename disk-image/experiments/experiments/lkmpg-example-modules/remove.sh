for module in "$@"
do
    rmmod ./$module.ko
done