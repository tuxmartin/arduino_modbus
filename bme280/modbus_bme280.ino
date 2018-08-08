#include <SimpleModbusSlave.h> // https://github.com/angeloc/simplemodbusng/tree/master/SimpleModbusSlave

#define SLAVE_ID          1
#define DEVICE_TYPE_CONST 2
#define DEVICE_VER_CONST  100
#define LEDPIN            13 // onboard led
byte SendPin = 10;

#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C
unsigned long previousMillis = 0;
const long bme280_interval = 5000;

//////////////// registers of your slave ///////////////////
enum 
{  
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
  
  HOLDING_REGS_SIZE // leave this one
};
unsigned int holdingRegs[HOLDING_REGS_SIZE]; // function 3 and 16 register array
//////////////// registers of your slave ///////////////////

void setup () {
  holdingRegs[DEVICE_TYPE] = DEVICE_TYPE_CONST;
  holdingRegs[DEVICE_FW_VERSION] = DEVICE_VER_CONST;

  long sea_level_pressure = SEALEVELPRESSURE_HPA * 100;
  holdingRegs[BME280_SEALEVELPRESSURE_HPA_H] = sea_level_pressure / 32767;
  holdingRegs[BME280_SEALEVELPRESSURE_HPA_L] = sea_level_pressure - (holdingRegs[BME280_SEALEVELPRESSURE_HPA_H] * 32767);

  modbus_configure(9600, SLAVE_ID, SendPin, HOLDING_REGS_SIZE, 0);

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);

  bool status;
  status = bme.begin(0x76);  
  if (!status) {
    holdingRegs[BME280_ERROR] = 1;    
    while (1);
  } else {
    holdingRegs[BME280_ERROR] = 0;
  }

}

void loop () {   // nikde nepouzivat zadny delay/sleep!
  modbus_update(holdingRegs);

  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= bme280_interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;  
    read_from_sensor();
  }

  digitalWrite(LEDPIN, holdingRegs[LED]);
}

void serialFlushBuffer() {
  while (Serial.read() >= 0) { /* do nothing */ }
}

void read_from_sensor() {
  int H;
  int L;
  long ValueLong = 0;
  ValueLong = bme.readPressure();
  H = ValueLong / 32767;
  L = ValueLong - (H * 32767); 
  // rebuildLong = ( HighINT * 32767) + LowInt

  holdingRegs[BME280_PRESSURE_H]  = H;
  holdingRegs[BME280_PRESSURE_L]  = L;

  float sea_leel_pressure = (holdingRegs[BME280_SEALEVELPRESSURE_HPA_H] * 32767) + holdingRegs[BME280_SEALEVELPRESSURE_HPA_L];

  holdingRegs[BME280_TEMPERATURE] = bme.readTemperature() * 100;
  holdingRegs[BME280_ALTITUDE]    = bme.readAltitude(sea_leel_pressure) * 100;
  holdingRegs[BME280_HUMIDITY]    = bme.readHumidity() * 100;
}
