#!/bin/sh
cd /mnt/SDCARD/App/RomM/

if pgrep romm > /dev/null; then
    killall -9 romm
fi

/mnt/SDCARD/App/RomM/romm
