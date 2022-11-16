import serial

with serial.Serial() as ser:
    ser.baudrate = 19200
    ser.port = 'COM6'
    ser.open()
    ser.write(b'hello')

