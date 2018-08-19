#!/usr/bin/env python
# -*- coding: utf-8 -*-

# http://minimalmodbus.sourceforge.net/internalminimalmodbus.html

import minimalmodbus
from time import sleep
from time import strftime # vypis aktualniho data a casu
import serial
import threading
import sys

frekvenceMereni = 1 # [s]

MODBUS_3  = 3  # Read holding registers
MODBUS_16 = 16 # Write multiple registers
STUPEN    = u'\u00b0' # znak pro stupen
SERIAL_SPEED_DETECT_NUMBER = 12345

serial_speed_list   = [9600, 19200, 38400, 57600, 115200]
#                      0   , 1,     2,     3,     4
serial_speed_list_I = 1

try:
    dev0 = minimalmodbus.Instrument('/dev/ttyUSB1', 1)  # port name, slave address (in decimal)
    dev0.serial.baudrate = serial_speed_list[serial_speed_list_I]
    dev0.serial.bytesize = 8
    dev0.serial.stopbits = 1
    dev0.serial.parity = serial.PARITY_NONE

    dev1 = minimalmodbus.Instrument('/dev/ttyUSB1', 1)  # port name, slave address (in decimal)
    dev1.serial.baudrate = serial_speed_list[serial_speed_list_I]
    dev1.serial.bytesize = 8
    dev1.serial.stopbits = 1
    dev1.serial.parity = serial.PARITY_NONE
except Exception as e:
    print(e)
    sys.exit(1)

lock = threading.Lock()

def serial_config_number():
    while True:
        lock.acquire()
        try:
            x = dev0.write_register(0, SERIAL_SPEED_DETECT_NUMBER, 0, MODBUS_16)
        except Exception as e:
            print(e)
        finally:
            lock.release()
        sleep(0.1)

def main():
    while True:
        print("SERIAL SPEED = " + str(serial_speed_list[serial_speed_list_I]))
        sleep(frekvenceMereni)
        print("---------------------------------------")
        print("Aktualni cas    =",  strftime("%Y-%m-%d %H:%M:%S"))

        """
                Slave 2 registers:
          SERIAL_SPEED_DETECT_NUMBER,
          DEVICE_TYPE,  
          DEVICE_SUBTYPE,
          DEVICE_FW_VERSION,
          BME280_SEALEVELPRESSURE_HPA_H,
          BME280_SEALEVELPRESSURE_HPA_L,
          BME280_ERROR,
          BME280_TEMPERATURE,  // C
          BME280_PRESSURE_H,     // hPa
          BME280_PRESSURE_L,     // hPa
          BME280_ALTITUDE,     // m   
          BME280_HUMIDITY,     // %
         LED,
        """

        lock.acquire()
        try:
            #dev1.write_register(1, zmerData, 0, MODBUS_16) # MEASURE - 1
            #x = dev1.write_register(0, 12345, 0, MODBUS_16)

            data = dev1.read_registers(0, 12, MODBUS_3)

            print(data[0])

            if data[6] != 0:
                print("CHYBA BME280")
                return -1

            temperature = data[7] / 100.0
            sea_level_pressure = ((data[4] * 32767) + data[5]) / 100.0  # rebuildLong = (HighINT * 32767) + LowInt
            pressure = ((data[8] * 32767) + data[9]) / 100.0  # rebuildLong = (HighINT * 32767) + LowInt
            altitude = data[1] / 100.0
            humidity = data[11] / 100.0

            print("RAW data        =",   data)
            print("Typ zarizeni    = " + str(data[0]))
            print("Verze firmware  = " + str(data[2]))
            print("Teplota         = " + str(temperature)        + " °C")
            print("Tlak            = " + str(pressure)           + " hPa")
            print("Tlak u hl. more = " + str(sea_level_pressure) + " hPa")
            print("Nadmorska vyska = " + str(altitude)           + "  m")
            print("Vlhkost         = " + str(humidity)           + "  %")

        except Exception as e:
            print(e)
        finally:
            lock.release()

#t = threading.Thread(target=serial_config_number, args=(i,))
t1 = threading.Thread(target=serial_config_number)
t2 = threading.Thread(target=main)
t1.start()
t2.start()

