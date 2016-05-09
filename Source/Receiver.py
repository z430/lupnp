from xbee import ZigBee
import serial
import struct

PORT = '/dev/ttyUSB0'
BAUD_RATE = 9600

def hex(bindata):
    return ''.join('%02x' % ord(byte) for byte in bindata)

# Open serial port
ser = serial.Serial(PORT, BAUD_RATE)

# Create API object
xbee = ZigBee(ser,escaped=True)

# Continuously read and print packets
while True:
    try:
        response = xbee.wait_read_frame()
        sa = hex(response['source_addr_long'][4:])
        rf = hex(response['rf_data'])
        datalength=len(rf)

        #print sa,' ',rf
        dht11 = rf[0:2]
        pir = rf[2:]
        print "humidity : ", int(dht11, 16), ' ', "pir : ",int(pir, 16)

    except KeyboardInterrupt:
        break

ser.close()