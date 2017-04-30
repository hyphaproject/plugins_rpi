import sys
import time
import RPi.GPIO as GPIO
GPIO.setmode(GPIO.BCM)
HIGH = True
LOW = False

def readData(channel, SCLKPin, MOSIPin, MISOPin, CSPin):
        GPIO.output(CSPin, HIGH)
        GPIO.output(CSPin, LOW)
        GPIO.output(SCLKPin, LOW)

        sendcmd = channel
        sendcmd |= 0b00011000

        for i in range(5):
                if(sendcmd & 0x10):
                        GPIO.output(MOSIPin, HIGH)
                else:
                        GPIO.output(MOSIPin, LOW)

                GPIO.output(SCLKPin, HIGH)
                GPIO.output(SCLKPin, LOW)
                sendcmd <<=1

        adcvalue = 0
        for i in range(11):
                GPIO.output(SCLKPin, HIGH)
                GPIO.output(SCLKPin, LOW)
                adcvalue <<= 1
                if(GPIO.input(MISOPin)):
                        adcvalue |= 0x01
        return adcvalue

channel = int(sys.argv[1])
SCLK    = 18
MOSI    = 24
MISO    = 23
CS      = 25

GPIO.setwarnings(False)
GPIO.setup(SCLK, GPIO.OUT)
GPIO.setup(MOSI, GPIO.OUT)
GPIO.setup(MISO, GPIO.IN)
GPIO.setup(CS, GPIO.OUT)

print readData(channel, SCLK, MOSI, MISO, CS)
