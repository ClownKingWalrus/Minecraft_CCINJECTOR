#include <IRremote.h>

const int RECV_PIN = 10; //ir receiver pin
IRrecv irrecv(RECV_PIN); //initlize the library with the pin
decode_results results; //old depricated way of telling retuned we will use it since it works
bool clearAlienCase = false; //we need a specefic hex code sent to make this true
uint32_t testCase = 0xBA45FF00; //our current test case to ensure we are not getting garbage values
uint32_t clearCase = 0xBA55FF00;
uint32_t clearMessageCase = 0xBA75FF00;
const int IR_Send_Pin = 13; //IR Sender Pin
IRsend irsend(IR_Send_Pin); //initlize the ir library to the IR sender pin
bool testClear = false;
bool clearMessage = false;
int globalIncrement = 0;
char recieved[5];

//millis()
unsigned long time; // millis() is a unsinged long value

//annot use in function
// if (IrReceiver.decode()) {
//       results.value = IrReceiver.decodedIRData.decodedRawData;
//       Serial.println(results.value);
//       irrecv.resume(); //resume reading for
//       }

//ability to send ir signals in a function
void IrSend(uint32_t hexValue) {
  irrecv.stop();
  irsend.sendNECRaw(hexValue, 32);
  delay(50);
  irrecv.enableIRIn();
}

//this function is to get a clear signal from the other tower
void AlienConfirmSignal() {
  if (!clearAlienCase) {
    if (IrReceiver.decode()) {
    results.value = IrReceiver.decodedIRData.decodedRawData;
    Serial.println(results.value);
    irrecv.resume(); //resume reading for
    }

    if (results.value == testCase && !testClear) {
      for (int i = 0; i < 5; i++) {
        IrSend(0xBA45FF00); //send 4 times to see if all 3 work
        Serial.println("Sending New Case");
      }
      testClear = true;
      results.value = 0;
    }

    if (results.value == clearCase) {
      clearAlienCase = true;
      Serial.println("ClearedAlienCase");
      for (int i = 0; i < 6; i++) {
        IrSend(clearCase);
        Serial.println("sending cleared confirmed");
      }
    }
    results.value = 0;
  }
}

int StringSize(String str) {
  int length = 0;
  while (str[length] != '\0') {
    length++;
  }
  return length;
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
      }
      globalIncrement = 0;
      clearMessage = true;
      return;
      //return recieved;
    }
  }
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
      clearMessage = false;
      return;
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
  if(!clearAlienCase) {
  AlienConfirmSignal(); //only goes until it receives the clearCase
  }
  else {
    if (!clearMessage) {
      MessageStored();
    }
    else if (clearMessage){
      SendMessage();
    }
  }
}




  // if (IrReceiver.decode()) {
  //   results.value = IrReceiver.decodedIRData.decodedRawData;
  //   Serial.println(char(results.value));
  //   irrecv.resume(); //resume reading for
  //   }