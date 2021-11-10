'''
Oscup: Open Source Custom Uart Protocol
This Software was release under: GPL-3.0 License
Copyright � 2021 Daniel Rossi & Riccardo Salami
Version: ALPHA 1.1.0
'''

from pyOscup import Oscup
port = 'COM4'  # change COM port
baudrate = 115200  # change baudrate

oscup = Oscup(0x51, baudrate, port)
while True:
    print(oscup.read())
