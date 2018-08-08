#include <SimpleModbusSlave.h> // https://github.com/angeloc/simplemodbusng/tree/master/SimpleModbusSlave

#define SLAVE_ID          1
#define DEVICE_VER_CONST  100
#define LEDPIN            13 // onboard led
byte SendPin = 10;

//////////////// registers of your slave ///////////////////
enum 
{  
  A,  
  B,
  C,
  D,
  LED,  
  F,   
  G,        
  H,
  I,
  
  HOLDING_REGS_SIZE // leave this one
};
unsigned int holdingRegs[HOLDING_REGS_SIZE]; // function 3 and 16 register array
//////////////// registers of your slave ///////////////////

void setup () {
  holdingRegs[A] = DEVICE_VER_CONST;
  modbus_configure(9600, SLAVE_ID, SendPin, HOLDING_REGS_SIZE, 0);

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(LEDPIN, LOW);

  holdingRegs[A]    = 10; 
  holdingRegs[B]    = 20;
  holdingRegs[C]    = 50;
  holdingRegs[D]    = 100;
  holdingRegs[LED]  = 150;
  holdingRegs[F]    = 254;
  holdingRegs[G]    = 257;    
  holdingRegs[H]    = 65500;
  holdingRegs[I]    = 66777;
}

void loop () {   // nikde nepouzivat zadny delay/sleep!
  modbus_update(holdingRegs);

  if (holdingRegs[I] == 1) { // Mam merit?
    holdingRegs[I] = 0; // vynulovat, aby to nemerilo porad dokola
    // tady by se neco delalo...
    serialFlushBuffer(); // vyprazdime buffer - to bude delat bordel, pokud budou aktualne prichazet data...
  }

  digitalWrite(LEDPIN, holdingRegs[LED]);
}

void serialFlushBuffer() {
  while (Serial.read() >= 0) { /* do nothing */ }
}
