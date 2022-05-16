import ntptime
import time
import dht 
import os
from time import sleep
from machine import Pin, SoftSPI
from sdcard import SDCard

#Pin assignment
#  MISO -> GPIO 13
#  MOSI -> GPIO 12
#  SCK  -> GPIO 14
#  CSD  -> GPIO 27
    
spisd = SoftSPI(-1,
                miso=Pin(13),
                mosi=Pin(12),
                sck=Pin(14))
sd = SDCard(spisd,Pin(27))

sensor = dht.DHT11(Pin(2))

print('Root directory: {}'.format(os.listdir()))
vfs = os.VfsFat(sd)
os.mount(vfs,'/sd')
print('Root directory: {}'.format(os.listdir()))
os.chdir('sd')
print('SD Card containst: {}'.format(os.listdir()))

while True:
  try:
    sleep(60)
    t = time.localtime()
    sensor.measure()
    temp = sensor.temperature()
    hum = sensor.humidity()
    
    timestamp = '{:04d}/{:02d}/{:02d} {:02d}:{:02d}:{:02d}'.format(t[0], t[1], t[2], t[3], t[4], t[5])
    directoryname ='{:04d}{:02d}{:02d}'.format(t[0], t[1], t[2])
    filename = '{:04d}{:02d}{:02d}_{:02d}.csv'.format(t[0], t[1], t[2], t[3])
    newRow = '{},{:3.1f}C,{:3.1f}%'.format(timestamp, temp,hum)
        
    dirlist = os.listdir()
    #print('dirlist=',dirlist)
    
    if directoryname not in dirlist:
        os.mkdir(directoryname)
        
    os.chdir(directoryname)
    #print('{}: {}'.format(directoryname,os.listdir()))
    filelist = os.listdir()
    if filename not in filelist:
        f = open(filename, 'w')
        f.write(newRow)
           f.close()
    else:
        f = open(filename, 'a')
        f.write('\n'+ newRow)
        f.close()
    
    os.chdir('..')
    
    #print('directoryname = ',directoryname)
    #print('newRow = ',newRow)
  except OSError as e:
    print('Failed to read sensor.')