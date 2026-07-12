#!/bin/bash

python3 tools/wdpacker.py ./res -o game.wd
sudo mount /dev/sdb1 /mnt
sudo rm /mnt/game.wd
sudo cp ~/dev/goom/gayinvaders/game.wd  /mnt/
sudo umount /mnt
