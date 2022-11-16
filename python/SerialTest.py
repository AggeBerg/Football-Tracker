#!/bin/python3
import serial

ser = serial.Serial("/dev/ttyUSB0", 921600)

ser.write(b"x400y400\n")

ser.close()