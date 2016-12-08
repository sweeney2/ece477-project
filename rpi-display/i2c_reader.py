#!/usr/bin/python3

import fileinput
import os
import re
import subprocess
import sys


while True:

    # Pass stdin -> stdout
    for message in sys.stdin:
        print(message.strip())
        sys.stdout.flush()

    # Generate messages based on I2C
    pass

