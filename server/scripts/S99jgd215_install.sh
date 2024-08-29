#!/bin/sh

set -x

case "$1" in
    start)
        SHELL_FILE=$(cd "$(dirname "$0")";pwd)/$(basename $0)

        echo "$SHELL_FILE"

        if [ -e /root/jgd215_arm_*.ipk ] ;
        then
            opkg install /root/jgd215_arm_*.ipk
        fi

        rm -rf /etc/init.d/S99jgd215_install.sh
        ;;
    stop)
        ;;
    restart|reload)
        ;;
    *)
        echo "Usage: $0 {start|stop|restart}"
        exit 1
esac

exit $?
