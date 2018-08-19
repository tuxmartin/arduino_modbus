#include <SimpleModbusSlave.h> // https://github.com/angeloc/simplemodbusng/tree/master/SimpleModbusSlave

// serial port baud rate detection
unsigned long previousMillis_serial = 0;
const long serial_speed_test_interval = 2000;  // [s]
const long serial_speed_list[] = {9600, 19200, 38400, 57600, 115200};
byte serial_speed_last = 0;
// serial port baud rate detection

#define SLAVE_ID          1
#define DEVICE_TYPE_CONST 2
#define DEVICE_VER_CONST  100
#define LEDPIN            13 // onboard led
byte SendPin = 10;

// BME 280
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>
#define SEALEVELPRESSURE_HPA (1013.25)
Adafruit_BME280 bme; // I2C
unsigned long previousMillis_bme280 = 0;
const long bme280_interval = 5000;
// BME 280

//////////////// registers of your slave ///////////////////
enum 
{  
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
  
  HOLDING_REGS_SIZE // leave this one
};
unsigned int holdingRegs[HOLDING_REGS_SIZE]; // function 3 and 16 register array
//////////////// registers of your slave ///////////////////

void setup () {
  holdingRegs[SERIAL_SPEED_DETECT_NUMBER] = 0; // serial port baud rate detection
  
  holdingRegs[DEVICE_TYPE] = DEVICE_TYPE_CONST;
  holdingRegs[DEVICE_FW_VERSION] = DEVICE_VER_CONST;

  // BME 280
  long sea_level_pressure = SEALEVELPRESSURE_HPA * 100;
  holdingRegs[BME280_SEALEVELPRESSURE_HPA_H] = sea_level_pressure / 32767;
  holdingRegs[BME280_SEALEVELPRESSURE_HPA_L] = sea_level_pressure - (holdingRegs[BME280_SEALEVELPRESSURE_HPA_H] * 32767);
  // BME 280

  //modbus_configure(9600, SLAVE_ID, SendPin, HOLDING_REGS_SIZE, 0);
  modbus_configure(serial_speed_list[serial_speed_last], SLAVE_ID, SendPin, HOLDING_REGS_SIZE, 0); //9600

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);

  // BME 280
  bool status;
  status = bme.begin(0x76);  
  if (!status) {
    holdingRegs[BME280_ERROR] = 1;    
    while (1);
  } else {
    holdingRegs[BME280_ERROR] = 0;
  }
  // BME 280
}

void loop () {   // nikde nepouzivat zadny delay/sleep!
  modbus_update(holdingRegs);
  unsigned long currentMillis = millis();
  serial_port_baud_rate_detection(currentMillis); 

  // BME 280
  if (currentMillis - previousMillis_bme280 >= bme280_interval) {
    // save the last time you blinked the LED
    previousMillis_bme280 = currentMillis;  
    read_from_sensor();
  }
  // BME 280

  /*
  if (holdingRegs[I] == 1) { // Mam merit?
    holdingRegs[I] = 0; // vynulovat, aby to nemerilo porad dokola
    // tady by se neco delalo...
    serialFlushBuffer(); // vyprazdime buffer - to bude delat bordel, pokud budou aktualne prichazet data...
  }
  */

  digitalWrite(LEDPIN, holdingRegs[LED]);
}

void serialFlushBuffer() {
  while (Serial.read() >= 0) { /* do nothing */ }
}

// BME 280
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
// BME 280

void serial_port_baud_rate_detection(unsigned long currentMillis) {
  if (currentMillis - previousMillis_serial >= serial_speed_test_interval) {
    previousMillis_serial = currentMillis;  
    if (holdingRegs[SERIAL_SPEED_DETECT_NUMBER] != 12345) {      
      Serial.end();
      modbus_configure(serial_speed_list[serial_speed_last++], SLAVE_ID, SendPin, HOLDING_REGS_SIZE, 0); //9600      
    }
    if (serial_speed_last >= 5 /*sizeof(serial_speed_list)/sizeof(long)*/ ) {
      serial_speed_last = 0;      
    }
    holdingRegs[SERIAL_SPEED_DETECT_NUMBER] = 0;
  }   
}
