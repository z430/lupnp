import math
from xbee import ZigBee
import serial
import struct

PORT = '/dev/ttyUSB0'
BAUD_RATE = 9600

def f(a,b):
    return int(math.pow(10,(int(math.log(b,10)) + 1)) * a + b)


def hex(bindata):
    return ''.join('%02x' % ord(byte) for byte in bindata)

# Open serial port
ser = serial.Serial(PORT, BAUD_RATE)

# Create API object
xbee = ZigBee(ser,escaped=True)
dht11 = 0
pir = 0
# Continuously read and print packets


response = xbee.wait_read_frame()
sa = hex(response['source_addr_long'][4:])
rf = hex(response['rf_data'])
datalength=len(rf)

#print sa,' ',rf
dht11 = int(rf[0:2], 16)
pir = int(rf[2:], 16)
z = int(str(dht11) + str(pir))

#z = int('%d%d' % (dht11, pir))
print type(dht11), type(pir), dht11, pir, rf, z

#print "humidity : ", int(dht11, 16), ' ', "pir : ",int(pir, 16), rf
#file = open('../Resource/SensorData.txt', 'w')
#file.write(z)
with open('Resource/SensorData.txt', 'w') as f:
    f.write('%d\n' % dht11)
    f.write('%d\n' % pir)

ser.close()