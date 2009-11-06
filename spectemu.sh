#!/bin/sh --

if test "_$DISPLAY" != "_"; then
    type=x
else
    case "`tty`" in
        /dev/tty[1-9]|/dev/tty[1-6][0-9])
            type=vga;;
        *)
            echo 'No DISPLAY environ variable found and not running on Linux console.'
            echo 'Set the DISPLAY variable or make sure you are on a Linux console'
            echo 'where Midnight Commander is not running or uses its "-u" option.'
            exit 1;;
    esac
fi

if test $# = 0; then
    if test $type = x; then
        exec xterm -iconic -T "xterm - spectemu" -e xspect
    else
        exec vgaspect
    fi
    exit 1
fi

base="`basename "$1"`"
basez="`basename "$1" .z80`"

if test "_$base" = "_$basez"; then
    if test $type = x; then
        exec xterm -iconic -T "xterm - spectemu" -e xspect "$@"
    else
        exec vgaspect "$@"
    fi
    exit 1
fi

snap="$1"
shift
if test $type = x; then
    exec xterm -iconic -T "xterm - $basez" -e xspect "$@" "$snap"
else
    exec vgaspect "$@" "$snap"
fi
exit 1
