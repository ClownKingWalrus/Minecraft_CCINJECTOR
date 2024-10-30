#include <IRremote.h>
#include <Stepper.h>

const int RECV_PIN = 10; //ir receiver pin
IRrecv irrecv(RECV_PIN); //initlize the library with the pin
decode_results results; //old depricated way of telling retuned we will use it since it works
int clearComCount = 0; //make sure we actually recieve the correct values instead of garbage
uint32_t testCase = 0xBA45FF00; //our current test case to ensure we are not getting garbage values
uint32_t clearCase = 0xBA55FF00;
const int IR_Send_Pin = 13; //IR Sender Pin
IRsend irsend(IR_Send_Pin); //initlize the ir library to the IR sender pin

//millis()
unsigned long time; // millis() is a unsinged long value

//step motor turn amount and pins
const int stepsPerRevolution = 200; //current turn amount for motor about 10-15%
Stepper myStepper(stepsPerRevolution, 2, 3, 4, 5); //initlize the motor which is bipolar to the unipolar library :skull:

//recieve and return hex value from ir reciever this funciton resumes recieving as of now
uint32_t IrReceieve() {
  if (IrReceiver.decode()) {
  uint32_t hexValue = (IrReceiver.decodedIRData.decodedRawData, HEX);// think i need to push 0x infront
  Serial.print("HEX: ");
  Serial.println(hexValue, HEX);
  irrecv.resume(); //resume reading for 
  return (hexValue);
  }
}

//ability to send ir signals in a function
void IrSend(uint32_t hexValue) {
  irsend.sendNECRaw(hexValue, 32);
}

//this function is to get a clear signal from the other tower
void ConfirmSignal() {
  if (clearComCount > 3) {
    return;
  }

  if (millis() - time > 1000) {
    Serial.println("Sending TestCase");
    IrSend(testCase);
    time = millis();
    while (millis() - time < 5000) {
      Serial.println("WAITING");
      if (IrReceieve() == testCase) {
        clearComCount++;
        Serial.println("We confimed IR signal adding com count");
        time = millis(); //reset time so we can check again if it fails the millis wont get reset indefentily
      }
    }

    if (clearComCount >= 3) {
      for (int i = 0; i < 4; i++) {
        IrSend(clearCase);
        Serial.println("Sending Clear Case");
      }
      time = millis();
      return; // return so we dont accidently rotate on final iteration causing possible loss of signal
    }
    myStepper.step(stepsPerRevolution); //after this function actually does its checks then we can check the next area
    time = millis();
  }
}

void setup() {
  //MOTOR INITLIZING
  myStepper.setSpeed(60); //motor turn speed

  //SERIAL INITLIZING
  Serial.begin(9600); //data transfer through com port

  //IR INITLIZING
  irrecv.enableIRIn(); //this just is a alias for the start() function 
  // irsend.begin(IR_Send_Pin); //initlizes the pin with blinking led on board to show its working
  // irrecv.blink13(true); //see if ir receive gets anything it blinks on board

  time = millis(); //set time to whatever the current time is
}

void loop() {
  // ConfirmSignal(); //will only go until it receives 3 valid signals
  IrReceieve();
  delay(200);
}





// if (IrReceiver.decode()) {
//     Serial.println(IrReceiver.decodedIRData.decodedRawData, HEX);
//     if (IrReceiver.decodedIRData.decodedRawData == testcase) {
//       Serial.println("We confirmed adding to count");
//       clearComCount++;
//     }
//     irrecv.resume();
//   }
//   if (clearComCount < 3 && millis() - time >= 500) {
//   Serial.println("Stepping Forward");
//   myStepper.step(stepsPerRevolution);
//   time = millis();