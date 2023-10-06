for module in "$@"
do
    insmod ./$module.ko
done