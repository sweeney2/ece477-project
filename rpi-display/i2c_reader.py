#!/usr/bin/python3

import fileinput
import os
import re
import select
import subprocess
import sys
import quick2wire.i2c as i2c

ADDRESS = 0x0C

GESTURES = (
        'left',
        'right',
        'up',
        'down',
        )


while True:

    # Pass stdin -> stdout
    if select.select([sys.stdin,],[],[],0.0)[0]:
        message = next(sys.stdin).strip()
        print(message)
        sys.stdout.flush()

    # Generate messages based on I2C
    with i2c.I2CMaster() as bus:
        result = True
        read_items = []
        while result:
            result =  int(bus.transaction(i2c.reading(ADDRESS, 1))[0][0])
            read_items.append(result)
        read_items = read_items[:-1]
        if read_items:
            if read_items[1:]:
                message = ''.join(chr(c) for c in read_items)
                print('m:{}'.format(message))
            elif read_items[0] in (1, 2, 3, 4,):
                print('g:{}'.format(GESTURES[read_items[0] - 1]))
            sys.stdout.flush()

