#!/usr/bin/python3

import datetime
import fileinput
import os
import re
import subprocess
import sys


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
            print('PHOTO TAKEN')
        else:
            print(message.strip())

    # Generate messages based on I2C
    pass

