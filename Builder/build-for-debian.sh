#!/bin/bash
python3 ./package_bemacs.py debian-source \
    --release=1 \
    --system-hunspell \
    --system-ucd \
    --colour
