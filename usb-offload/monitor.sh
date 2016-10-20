#!/bin/bash

stdbuf -i0 -o0 udevadm monitor --udev | ./watch_usb.py
