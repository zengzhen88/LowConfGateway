#!/bin/sh

case "$1" in
    start)
        /root/jgd215/start.sh
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
