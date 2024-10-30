#include <IRremote.h>

const int RECV_PIN = 10; //ir receiver pin
IRrecv irrecv(RECV_PIN); //initlize the library with the pin
decode_results results; //old depricated way of telling retuned we will use it since it works
bool clearAlienCase = false; //we need a specefic hex code sent to make this true
uint32_t testCase = 0xBA45FF00; //our current test case to ensure we are not getting garbage values
uint32_t clearCase = 0xBA55FF00;
const int IR_Send_Pin = 13; //IR Sender Pin
IRsend irsend(IR_Send_Pin); //initlize the ir library to the IR sender pin

//millis()
unsigned long time; // millis() is a unsinged long value

//recieve and return hex value from ir reciever this funciton resumes recieving as of now
uint32_t IrReceieve() {
  if (IrReceiver.decode()) {
  uint32_t hexValue = (IrReceiver.decodedIRData.decodedRawData, HEX); // store hexValue
  irrecv.resume(); //resume reading for 
  return hexValue;
  }
}

//ability to send ir signals in a function
void IrSend(uint32_t hexValue) {
  irsend.sendNECRaw(hexValue, 32);
}

//this function is to get a clear signal from the other tower
void AlienConfirmSignal() {
  if (!clearAlienCase) {
    if (IrReceieve() == testCase) {
      for (int i = 0; i < 4; i++) {
        IrSend(0xBA45FF00); //send 4 times to see if all 3 work
        Serial.println("Sending New Case");
      }
    }
    if (IrReceieve() == clearCase) {
      clearAlienCase = true;
      Serial.println("ClearedAlienCase");
    }
  }
}

void setup() {

  //SERIAL INITLIZING
  Serial.begin(9600); //data transfer through com port

  //IR INITLIZING
  irrecv.enableIRIn(); //this just is a alias for the start() function 
  // irsend.begin(IR_Send_Pin); //initlizes the pin with blinking led on board to show its working
  // irrecv.blink13(true); //see if ir receive gets anything it blinks on board

  time = millis(); //set time to whatever the current time is
}

void loop() {
  AlienConfirmSignal(); //only goes until it receives the clearCase
}