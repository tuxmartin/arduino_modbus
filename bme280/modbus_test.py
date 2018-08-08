#!/usr/bin/env python
# -*- coding: utf-8 -*-

# http://minimalmodbus.sourceforge.net/internalminimalmodbus.html

import minimalmodbus
from time import sleep
from time import strftime # vypis aktualniho data a casu
import serial

frekvenceMereni = 5 # [s]

MODBUS_3  = 3  # Read holding registers
MODBUS_16 = 16 # Write multiple registers
STUPEN    = u'\u00b0' # znak pro stupen

def main():
    print("---------------------------------------")
    print("Aktualni cas    =",  strftime("%Y-%m-%d %H:%M:%S"))

    try:
        dev1 = minimalmodbus.Instrument('/dev/ttyUSB1', 1) # port name, slave address (in decimal)
        dev1.serial.baudrate = 9600
        dev1.serial.bytesize = 8
        dev1.serial.stopbits = 1
        dev1.serial.parity   = serial.PARITY_NONE
    except Exception as e:
        print(e)

    """
            Slave 2 registers:
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

    try:
        data = dev1.read_registers(0, 12, MODBUS_3)

        if data[5] != 0:
            print("CHYBA BME280")
            return -1

        temperature = data[6] / 100.0
        sea_level_pressure = ((data[3] * 32767) + data[4]) / 100.0  # rebuildLong = (HighINT * 32767) + LowInt
        pressure = ((data[7] * 32767) + data[8]) / 100.0  # rebuildLong = (HighINT * 32767) + LowInt
        altitude = data[9] / 100.0
        humidity = data[10] / 100.0

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

while 1:
    main()
    sleep(frekvenceMereni)
