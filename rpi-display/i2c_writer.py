#!/usr/bin/python3

import datetime
import fileinput
import os
import re
import subprocess
import sys
import quick2wire.i2c as i2c

ADDRESS = 0x0C
IMAGE_FOLDER = '/home/alarm/magic-mirror-images'


while True:

    # Pass stdin -> stdout
    for message in sys.stdin:
        
        if message.strip() == 'photo':
            today = datetime.datetime.now().strftime('%Y-%m-%d-')
            _, _, existing_files = next(os.walk(IMAGE_FOLDER))
            outfile = os.path.join(
                    IMAGE_FOLDER,
                    '{0}{1:02d}.jpg'.format(today, len([f for f in existing_files if f.startswith(today)])),
                    )
            subprocess.call(['raspistill', '-n', '-o', outfile, '-t', '1'])
            print('photo-taken')
        elif message.strip() == 'lights':
            with i2c.I2CMaster() as bus:
                bus.transaction(i2c.writing_bytes(ADDRESS, 0x01))
        else:
            print(message.strip())

    # Generate messages based on I2C
    pass

