#include <Adafruit_ATParser.h>
#include <Adafruit_BLE.h>
#include <Adafruit_BLEBattery.h>
#include <Adafruit_BLEEddystone.h>
#include <Adafruit_BLEGatt.h>
#include <Adafruit_BLEMIDI.h>
#include <Adafruit_BluefruitLE_SPI.h>
#include <Adafruit_BluefruitLE_UART.h>

#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined(ARDUINO_ARCH_SAMD)
  #include <SoftwareSerial.h>
#endif

#define FACTORYRESET_ENABLE         0
#define MINIMUM_FIRMWARE_VERSION    "0.6.6"
#define BUFSIZE 128

/* Hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(8, 7, 4);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

//fingers
int f0=A0;
int f1=A1;
int f2=A5;
int f3=A3;
int f4=A4;

// motors
int m0=11;
int m1=10;
int m2=9;
int m3=6;
int m4=5;

// read/write mode switcher
int rwPin=12;
int rw;
int lastRw=0;

// reads
String lastRead="00000";
String thisRead="00000";
String lastEntry;

String val;

// sotto states
int state=0;
// 0: waiting for input
// 1: input started
// 2: pending input
// 3: recording input

int mode=1;
// 1: letters
// 2: capitals
// 3: punctuation
// 4: numbers
// 5: backspace

// mode timers
int modeTimer=1000;
int modeStart;

void setup() {
  Serial.begin(115200);
  pinMode(f0,INPUT);
  pinMode(f1,INPUT);
  pinMode(f2,INPUT);
  pinMode(f3,INPUT);
  pinMode(f4,INPUT);
  pinMode(m0,OUTPUT);
  pinMode(m1,OUTPUT);
  pinMode(m2,OUTPUT);
  pinMode(m3,OUTPUT);
  pinMode(m4,OUTPUT);
  pinMode(rwPin,INPUT);

  // adafruit stuff
  while (!Serial);  // required for Flora & Micro
  delay(500);

  Serial.begin(115200);
  
  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(true) )
  {
    error(F("Couldn't find Bluefruit, make sure it's in command mode & check wiring?"));
  }
  Serial.println( F("OK!") );

  if ( FACTORYRESET_ENABLE )
  {
    /* Perform a factory reset to make sure everything is in a known state */
    Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  }

  /* Disable command echo from Bluefruit */
  ble.echo(false);

  Serial.println("Requesting Bluefruit info:");
  /* Print Bluefruit information */
  ble.info();

  // if single mode:
//  enableKeyboard();
//    enableMotors();

  // dual mode
    rw=1;
    switchRwMode();
    lastRw=rw;

  // play pattern to show that it booted up
  buzz(m0);
  buzz(m1);
  buzz(m2);
  buzz(m3);
  buzz(m4);
  buzz(m0);
  buzz(m1);
  buzz(m2);
  buzz(m3);
  buzz(m4);
  buzz(m0);
  buzz(m1);
  buzz(m2);
  buzz(m3);
  buzz(m4);
}

void loop() {

  rw=digitalRead(rwPin);
  if (rw!=lastRw) {
    switchRwMode();
    lastRw=rw;
  }

  if (rw) { checkKeyboard(); }
  else { checkMotors(); }

}

void switchRwMode() {
  if (rw) {
    // if it is on, it's write
    enableKeyboard();
  }
  else {
    // if it is off, then it's read mode
    enableMotors();
  }
}

void enableKeyboard() {
  if (! ble.sendCommandCheckOK(F( "AT+GAPDEVNAME=Sotto" )) ) {
    error(F("Could not set device name?"));
  }
  /* Enable HID Service */
  Serial.println(F("Enable HID Service (including Keyboard): "));
  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
  {
    if ( !ble.sendCommandCheckOK(F( "AT+BleHIDEn=On" ))) {
      error(F("Could not enable Keyboard"));
    }
  }else
  {
    if (! ble.sendCommandCheckOK(F( "AT+BleKeyboardEn=On"  ))) {
      error(F("Could not enable Keyboard"));
    }
  }

  /* Add or remove service requires a reset */
  Serial.println(F("Performing a SW reset (service changes require a reset): "));
  if (! ble.reset() ) {
    error(F("Couldn't reset??"));
  }
}


void checkKeyboard() {
    readKeys();

  // check for mode
  if (millis()<(modeStart+1000)) {
    if (state==0) {
      if (thisRead!="00000") {
        // input!
        state=1;
      }
    }

    else if (state==1) {
      // wait for any change
      if (thisRead!=lastRead && thisRead!="00000") {
        lastEntry=combin(lastEntry,lastRead);
      }
      else if (thisRead=="00000") {
        lastEntry=combin(lastEntry,lastRead);
        state=2;
      }
      
    }

    else if (state==2) {
      // translate mode
      setMode(lastEntry);
      modeStart=millis()-1001;
      lastEntry="";
      state=0;
    }
  }

  // if there is input, send it
  
//  Serial.println(thisRead);

  if (state==0) {
    // wait for input
    if (thisRead!="00000") {
      // input!
      state=1;
//      Serial.println("state 1");
    }
  }

  else if (state==1) {
    // wait for any change
    if (thisRead!=lastRead && thisRead!="00000") {
      lastEntry=combin(lastEntry,lastRead);
//      Serial.println("state 2");
    }
    else if (thisRead=="00000") {
      lastEntry=combin(lastEntry,lastRead);
      state=2;
//      Serial.println("state 2");
    }
    
  }

  else if (state==2) {
    // check for mode
    if (lastEntry=="11100") {
      // change mode in the next second, otherwise revert
      modeStart=millis();
    }
    else {
      // if it is in letter mode (currently the only mode)
      if (mode==1) {
        val = letter(lastEntry);
      }
      else if (mode==2) {
        // capital letters
        val = letter(lastEntry);
        val.toUpperCase();
      }
      else if (mode==3) {
        // punctuation
        val = punctuation(lastEntry);
      }
      else if (mode==4) {
        char input[12];
        lastEntry.toCharArray(input,12);
        long decval = strtol(input, NULL, 2);
        val = String(decval, DEC);
      }
    }
    
    Serial.println(val);
    //send it

    sendText(val);
    
    lastEntry="";
//    Serial.println("state 0");
    state=0;
  }
}


void sendText(String text) {
  Serial.print("\nSending ");
  Serial.println(text);
    
  ble.print("AT+BleKeyboard=");
  ble.println(text);
    
  if( ble.waitForOK() ) {
    Serial.println( F("OK!") );
    state=0;
  }
  else {
    Serial.println( F("FAILED!") );
    // remain in this state
  }
}

void enableMotors() {
  //reset
  Serial.println(F("Performing a factory reset: "));
    if ( ! ble.factoryReset() ){
      error(F("Couldn't factory reset"));
    }
  
  Serial.println(F("Please use Adafruit Bluefruit LE app to connect in UART mode"));
  Serial.println(F("Then Enter characters to send to Bluefruit"));
  Serial.println();

  ble.verbose(false);  // debug info is a little annoying after this point!

  /* Wait for connection */
  while (! ble.isConnected()) {
      delay(500);
  }

  Serial.println(F("******************************"));
//
//  // LED Activity command is only supported from 0.6.6
//  if ( ble.isVersionAtLeast(MINIMUM_FIRMWARE_VERSION) )
//  {
//    // Change Mode LED Activity
//    Serial.println(F("Change LED activity to " MODE_LED_BEHAVIOUR));
//    ble.sendCommandCheckOK("AT+HWModeLED=" MODE_LED_BEHAVIOUR);
//  }

  // Set module to DATA mode
  Serial.println( F("Switching to DATA mode!") );
  ble.setMode(BLUEFRUIT_MODE_DATA);

  Serial.println(F("******************************"));
}

void checkMotors() {
  // Check for user input
  char n, inputs[BUFSIZE+1];

  if (Serial.available())
  {
    n = Serial.readBytes(inputs, BUFSIZE);
    inputs[n] = 0;
    // Send characters to Bluefruit
    Serial.print("Sending: ");
    Serial.println(inputs);

    // Send input data to host via Bluefruit
    ble.print(inputs);
  }

  // Echo received data
  while ( ble.available() )
  {
    int c = ble.read();

    Serial.print((char)c);
    // convert
    String h=String((char)c);
    convert(h);

    // Hex output too, helps w/debugging!
//    Serial.print(" [0x");
//    if (c <= 0xF) Serial.print(F("0"));
//    Serial.print(c, HEX);
//    Serial.print("] ");
  }
}

void buzz(int motor) {
  digitalWrite(motor,HIGH);
  delay(100);
  digitalWrite(motor,LOW);
}

String combin(String bin1, String bin2) {
  // combines two binary string reads from keys
  String f="";
  for (int i=0;i<5;i++) {
    if (bin1.substring(i,i+1)=="1" || bin2.substring(i,i+1)=="1") {f+="1";}
    else {f+="0";}
  }
  Serial.println(f);
  return f;
}

void readKeys() {
  lastRead=thisRead;
  String r=String(digitalRead(f4))+String(digitalRead(f3))+String(digitalRead(f2))+String(digitalRead(f1))+String(digitalRead(f0));
  thisRead=r;
//  Serial.println(r);
}


void setMode(String bin) {
  if (bin=="00001") {mode=1;}
  else if (bin=="00010") {mode=2;}
  else if (bin=="00100") {mode=3;}
  else if (bin=="01000") {mode=4;}
  else if (bin=="10000") {mode=5;}
}

void convert(String l) {
  // letter l conver to buzzes
  l.toLowerCase();
  Serial.println(l);
  if (l=="a") {haptic("00001");}
  else if (l=="b") {haptic("00010");}
  else if (l=="c") {haptic("00011");}
  else if (l=="d") {haptic("00100");}
  else if (l=="e") {haptic("00101");}
  else if (l=="f") {haptic("00110");}
  else if (l=="g") {haptic("00111");}
  else if (l=="h") {haptic("01000");}
  else if (l=="i") {haptic("01001");}
  else if (l=="j") {haptic("01010");}
  else if (l=="k") {haptic("01011");}
  else if (l=="l") {haptic("01100");}
  else if (l=="m") {haptic("01101");}
  else if (l=="n") {haptic("01110");}
  else if (l=="o") {haptic("01111");}
  else if (l=="p") {haptic("10000");}
  else if (l=="q") {haptic("10001");}
  else if (l=="r") {haptic("10010");}
  else if (l=="s") {haptic("10011");}
  else if (l=="t") {haptic("10100");}
  else if (l=="u") {haptic("10101");}
  else if (l=="v") {haptic("10110");}
  else if (l=="w") {haptic("10111");}
  else if (l=="x") {haptic("11000");}
  else if (l=="y") {haptic("11001");}
  else if (l=="z") {haptic("11010");}
  else if (l==" ") {haptic("11111");}
}

void haptic(String bin) {
  Serial.println(bin);
  if (bin.substring(0,1)=="1") {digitalWrite(m4,HIGH);}
  if (bin.substring(1,2)=="1") {digitalWrite(m3,HIGH);}
  if (bin.substring(2,3)=="1") {digitalWrite(m2,HIGH);}
  if (bin.substring(3,4)=="1") {digitalWrite(m1,HIGH);}
  if (bin.substring(4,5)=="1") {digitalWrite(m0,HIGH);}
  delay(100);
  allOff();
}

void allOff() {
  digitalWrite(m0,LOW);
  digitalWrite(m1,LOW);
  digitalWrite(m2,LOW);
  digitalWrite(m3,LOW);
  digitalWrite(m4,LOW);
}

String letter(String bin) {
  if (bin=="00001") {return "a";}
  else if (bin=="00010") {return "b";}
  else if (bin=="00011") {return "c";}
  else if (bin=="00100") {return "d";}
  else if (bin=="00101") {return "e";}
  else if (bin=="00110") {return "f";}
  else if (bin=="00111") {return "g";}
  else if (bin=="01000") {return "h";}
  else if (bin=="01001") {return "i";}
  else if (bin=="01010") {return "j";}
  else if (bin=="01011") {return "k";}
  else if (bin=="01100") {return "l";}
  else if (bin=="01101") {return "m";}
  else if (bin=="01110") {return "n";}
  else if (bin=="01111") {return "o";}
  else if (bin=="10000") {return "p";}
  else if (bin=="10001") {return "q";}
  else if (bin=="10010") {return "r";}
  else if (bin=="10011") {return "s";}
  else if (bin=="10100") {return "t";}
  else if (bin=="10101") {return "u";}
  else if (bin=="10110") {return "v";}
  else if (bin=="10111") {return "w";}
  else if (bin=="11000") {return "x";}
  else if (bin=="11001") {return "y";}
  else if (bin=="11010") {return "z";}
  else {return " ";}
}

String punctuation(String bin) {
  if (bin=="00001") {return ".";}
  else if (bin=="00010") {return ",";}
  else if (bin=="00011") {return ";";}
  else if (bin=="00100") {return "'";}
  else if (bin=="00101") {return "\"";}
  else if (bin=="01000") {return "/";}
  else if (bin=="01001") {return "\\";}
  else if (bin=="10011") {return "$";}
  else if (bin=="11011") {return "#";}
  else if (bin=="10000") {return "-";}
  else if (bin=="11000") {return "=";}
  else if (bin=="11001") {return "+";}
  else if (bin=="10001") {return "()";}
  else {return " ";}
}