for module in "$@"
do
    insmod ./$module.ko
    chmod -R 777 /proc/$module
done