#!/bin/bash

sudo stdbuf -i0 -o0 udevadm monitor --udev | sudo ./usb_offload.py | sudo ./i2c_reader.py | sudo ./magic.py | sudo ./i2c_writer.py

