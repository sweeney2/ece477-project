#!/usr/bin/python3

import fileinput
import os
import re
import subprocess
import sys


DEVICE_NAMES = ('sda', 'sdb', 'sdc', 'sdd',)
IMAGE_DIR = '/home/alarm/magic-mirror-images'

for message in sys.stdin:

    message_split = re.split(r'\s+', message)
    if not len(message_split) == 6:
        continue

    (event, time, action, device, desc, _) = message_split

    device = device.split('/')[-1]
    desc = desc[1:-1]

    if desc != 'block':
        continue

    if action != 'add':
        continue

    print('usb-in')

    dev_dir = '/dev/{}'.format(device)
    mnt_dir = '/mnt/{}'.format(device)

    subprocess.call(['sudo', 'mkdir', '-p', mnt_dir])
    subprocess.call(['sudo', 'mount', dev_dir, mnt_dir])
    subprocess.call(['sudo', 'rsync', '-r', IMAGE_DIR, mnt_dir])
    subprocess.call(['sudo', 'umount', mnt_dir])
    subprocess.call(['sudo', 'rmdir', '/mnt/{}'.format(device)])

    print('usb-out')

