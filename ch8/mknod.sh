MODULE="ch8_mod_dev"
MAJOR=$(awk "\$1==\"$MODULE\" {print \$1}" /proc/devices)
mknod /dev/$MODULE c $MAJOR 0
