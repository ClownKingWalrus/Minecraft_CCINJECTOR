#include <IRremote.hpp>
#include <Stepper.h>

const int RECV_PIN = 10; //ir receiver pin
IRrecv irrecv(RECV_PIN); //initlize the library with the pin
decode_results results; //old depricated way of telling retuned we will use it since it works
bool clearComCount = false; //make sure we actually recieve the correct values instead of garbage
uint32_t testCase = 0xBA45FF00; //our current test case to ensure we are not getting garbage values
uint32_t clearCase = 0xBA55FF00;
uint32_t clearMessageCase = 0xBA75FF00;
const int IR_Send_Pin = 13; //IR Sender Pin
IRsend irsend(IR_Send_Pin); //initlize the ir library to the IR sender pin
bool testclear = false;
bool confirmed = false;
bool clearMessage = false;
int globalIncrement = 0;
char recieved[5];

//millis()
unsigned long time; // millis() is a unsinged long value

//step motor turn amount and pins
const int stepsPerRevolution = 200; //current turn amount for motor about 10-15%
Stepper myStepper(stepsPerRevolution, 2, 3, 4, 5); //initlize the motor which is bipolar to the unipolar library :skull:

//Cannot use this in a function for some reason it breaks the program if you try
// void IrReceieve() {
//   if (IrReceiver.decode()) {
//     results.value = IrReceiver.decodedIRData.decodedRawData;
//     irrecv.resume(); //resume reading for
//   }
// }

//ability to send ir signals in a function
void IrSend(uint32_t hexValue) {
  irrecv.stop();
  irsend.sendNECRaw(hexValue, 32);
  delay(50);
  irrecv.enableIRIn();
  delay(50);
}

//this function is to get a clear signal from the other tower
void ConfirmSignal() {
  if (clearComCount) {
    Serial.println("WE ARE IN HERE");
    delay(100);
    results.value = 0;
    if (IrReceiver.decode()) {
      results.value = IrReceiver.decodedIRData.decodedRawData;
      irrecv.resume(); //resume reading for
      delay(50);
      Serial.println(results.value);
    }

    if(results.value != clearCase) {
      for (int i = 0; i < 2; i++) {
        IrSend(clearCase);
        Serial.println("Sending Clear Case");
      }
    }
    else if (results.value == clearCase) {
      Serial.println("WE ARE TRUE");
      confirmed = true;
    }
    results.value = 0;
    return;
  }

  if (millis() - time > 1000) {
    Serial.println("Sending TestCase");
    IrSend(testCase);

    time = millis();
    while (millis() - time < 5000) {
      delay(300);
      Serial.println(results.value);
      Serial.println("WAITING");
      if (IrReceiver.decode()) {
      results.value = IrReceiver.decodedIRData.decodedRawData;
      Serial.println(results.value);
      irrecv.resume(); //resume reading for
      }

      Serial.println(results.value);
      if (results.value == testCase) {
        clearComCount = true;
        Serial.println("We confimed IR signal adding com count");
        time = millis();
        return;
      }
      results.value = 0;
    }

    myStepper.step(stepsPerRevolution); //after this function actually does its checks then we can check the next area
    myStepper.step(stepsPerRevolution); //after this function actually does its checks then we can check the next area
    time = millis();
  }
}

int StringSize(String str) {
  int length = 0;
  while (str[length] != '\0') {
    length++;
  }
  return length;
}

void SendMessage() {
  Serial.println("input message");
  String input;
  int length;
    while (Serial.available() == 0) {
  }
    input = Serial.readString();
    length = StringSize(input);

    if (length > 0) {
      for (int i = 0; i < length; i++) {
        Serial.print("Sending: ");
        Serial.println(input[i]);
        IrSend(input[i]);
      }
      IrSend(clearMessageCase);
      IrSend(clearMessageCase); //backup incase first fails
      clearMessage = true;
      return;
    }
}

void MessageStored() {
  if (IrReceiver.decode()) {
    results.value = IrReceiver.decodedIRData.decodedRawData;
    Serial.println("PULSE WAVE DETECTED");
    char x = char(results.value);
    recieved[globalIncrement] = x;
    globalIncrement++;
    irrecv.resume(); //resume reading for

    if (results.value == clearMessageCase) {
      int length = StringSize(recieved);
      for (int i = 0; i < globalIncrement; i++) {
        Serial.print(recieved[i]);
        Serial.println();
      }
      globalIncrement = 0;
      clearMessage = false;
      return;
      //return recieved;
    }
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
  results.value = 0;
}

void loop() {
  if (!confirmed) {
    ConfirmSignal();
  }
  else {
    if (!clearMessage)
    SendMessage();
    else if (clearMessage) {
      MessageStored();
    }
  }
  
  // myStepper.step(stepsPerRevolution); //after this function actually does its checks then we can check the next area
}