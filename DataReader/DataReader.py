
from pyOscup import Oscup
from pyOscup import ErrorCodes
from struct import unpack
import csv
from datetime import datetime
from time import sleep
id = 0x1C
baudrate = 115200
port = "COM3"
oscup = Oscup(id, baudrate, port)

csv_header = ['temperature', 'humidity', 'CO2', 'TVOC', 'fire']
date = datetime.today().strftime('%Y_%m_%d_%H_%M_%S')

def cleaner(s):
    sr = s.replace(",","")
    sr = sr.replace("(","")
    sr = sr.replace(")","")
    return sr

with open('RAW_DATA_'+ date +".csv", 'w', newline='', encoding='UTF-8') as csvfile:
    writer = csv.writer(csvfile)
    writer.writerow(csv_header)

while True:
    error, packet = oscup.read()

    if error:
        if error != ErrorCodes.NO_DATA:
            print("Error: {}".format(error))
    else:
        id, command, length, payload, crc = packet.getParams()
        temperature = str(unpack('f', bytearray(payload[0:4])))
        humidity = str(unpack('f', bytearray(payload[4:8])))
        CO2 = str(unpack('i', bytearray(payload[8:12])))
        TVOC = str(unpack('i', bytearray(payload[12:16])))
        fire = str(unpack('i', bytearray(payload[16:20])))

        temperature = cleaner(temperature)
        humidity = cleaner(humidity)
        CO2 = cleaner(CO2)
        TVOC = cleaner(TVOC)
        fire = cleaner(fire)

        print("id: {} - command: {} - length: {}".format(hex(id), command, length))
        print("payload: " + " ".join([hex(byte) for byte in payload]))
        print("Temperature: {}".format(temperature))
        print("Humidity: {}".format(humidity))
        print("CO2: {}".format(CO2))
        print("TVOC: {}".format(TVOC))
        print("Button: {}".format(fire))
        print("crc: {}".format(crc))
        print("\n\n")
        
        data = list([temperature,humidity,CO2,TVOC,fire])
        with open('RAW_DATA_'+date+".csv", 'a', newline='', encoding='UTF-8') as csvfile:
            writer = csv.writer(csvfile)
            writer.writerow(data)
    
           

