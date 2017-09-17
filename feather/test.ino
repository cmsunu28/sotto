/*********************************************************************
 This is an example for our nRF51822 based Bluefruit LE modules

 Pick one up today in the adafruit shop!

 Adafruit invests time and resources providing this open source code,
 please support Adafruit and open-source hardware by purchasing
 products from Adafruit!

 MIT license, check LICENSE for more information
 All text above, and the splash screen below must be included in
 any redistribution
*********************************************************************/

/*
  Highly edited HID keyboard code
*/

#include <Arduino.h>
#include <SPI.h>
#if not defined (_VARIANT_ARDUINO_DUE_X_) && not defined(ARDUINO_ARCH_SAMD)
  #include <SoftwareSerial.h>
#endif

#include "Adafruit_BLE.h"
#include "Adafruit_BluefruitLE_SPI.h"
#include "Adafruit_BluefruitLE_UART.h"

#include "BluefruitConfig.h"

/*=========================================================================
    APPLICATION SETTINGS

    FACTORYRESET_ENABLE       Perform a factory reset when running this sketch
   
                              Enabling this will put your Bluefruit LE module
                              in a 'known good' state and clear any config
                              data set in previous sketches or projects, so
                              running this at least once is a good idea.
   
                              When deploying your project, however, you will
                              want to disable factory reset by setting this
                              value to 0.  If you are making changes to your
                              Bluefruit LE device via AT commands, and those
                              changes aren't persisting across resets, this
                              is the reason why.  Factory reset will erase
                              the non-volatile memory where config data is
                              stored, setting it back to factory default
                              values.
       
                              Some sketches that require you to bond to a
                              central device (HID mouse, keyboard, etc.)
                              won't work at all with this feature enabled
                              since the factory reset will clear all of the
                              bonding data stored on the chip, meaning the
                              central device won't be able to reconnect.
    MINIMUM_FIRMWARE_VERSION  Minimum firmware version to have some new features
    -----------------------------------------------------------------------*/
    #define FACTORYRESET_ENABLE         0
    #define MINIMUM_FIRMWARE_VERSION    "0.6.6"
/*=========================================================================*/

// Create the bluefruit object, either software serial...uncomment these lines

/* Hardware SPI, using SCK/MOSI/MISO hardware SPI pins and then user selected CS/IRQ/RST */
Adafruit_BluefruitLE_SPI ble(BLUEFRUIT_SPI_CS, BLUEFRUIT_SPI_IRQ, BLUEFRUIT_SPI_RST);

// A small helper
void error(const __FlashStringHelper*err) {
  Serial.println(err);
  while (1);
}

// sotto pins

//fingers
int f0=A0;
int f1=A1;
int f2=A2;
int f3=A3;
int f4=A4;

// motors
int m0=3;
int m1=5;
int m2=9;
int m3=10;
int m4=11;

// sotto states
int state=0;
// 0: waiting for input
// 1: input started
// 2: all released
// 3: recording input

int mode=1;
// 1: letters
// 2: capitals
// 3: punctuation
// 4: numbers
// 5: backspace

// reads
String lastRead="00000";
String thisRead="00000";

// threshold
int threshold=200;

// mode timers
int modeTimer=1000;
int modeStart;


/**************************************************************************/
/*!
    @brief  Sets up the HW an the BLE module (this function is called
            automatically on startup)
*/
/**************************************************************************/
void setup(void)
{

  // pins
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
  
  // adafruit stuff
  while (!Serial);  // required for Flora & Micro
  delay(500);

  Serial.begin(115200);
  
  /* Initialise the module */
  Serial.print(F("Initialising the Bluefruit LE module: "));

  if ( !ble.begin(VERBOSE_MODE) )
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

  /* Change the device name to make it easier to find */
  Serial.println(F("Setting device name to 'Bluefruit Keyboard': "));
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

  Serial.println();
  Serial.println(F("Go to your phone's Bluetooth settings to pair your device"));
  Serial.println(F("then open an application that accepts keyboard input"));

  Serial.println();
  Serial.println(F("Enter the character(s) to send:"));
//  Serial.println(F("- \\r for Enter"));
//  Serial.println(F("- \\n for newline"));
//  Serial.println(F("- \\t for tab"));
//  Serial.println(F("- \\b for backspace"));

  Serial.println();
}

/**************************************************************************/
/*!
    @brief  Constantly poll for new command or response data
*/
/**************************************************************************/
void loop() {
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
      // wait for unkey
      if (thisRead=="00000") {
        // input finished
        state=2;
      }
    }

    else if (state==2) {
      // translate mode
      setMode(lastRead);
      modeStart=millis()-1001;
      state=0;
    }
  }

  // if there is input, send it

  if (state==0) {
    // wait for input
    if (thisRead!="00000") {
      // input!
      state=1;
    }
  }

  else if (state==1) {
    // wait for unkey
    if (thisRead=="00000") {
      // input finished!
      state=2;
    }
  }

  else if (state==2) {
    // check for mode
    if (lastRead=="11100") {
      // change mode in the next second, otherwise revert
      modeStart=millis();
    }
    else {
      String val;
      // if it is in letter mode (currently the only mode)
      if (mode==1) {
        val = letter(lastRead);
      }
      else if (mode==2) {
        // capital letters
        val = letter(lastRead);
        val.toUpperCase();
      }
      else if (mode==3) {
        // punctuation
        val = punctuation(lastRead);
      }
      else if (mode==4) {
        char input[12];
        lastRead.toCharArray(input,12);
        long decval = strtol(input, NULL, 2);
        val = String(decval, DEC);
      }
      
      Serial.print("\nSending ");
      Serial.println(val);
    
      ble.print("AT+BleKeyboard=");
      ble.println(val);
    
      if( ble.waitForOK() ) {
        Serial.println( F("OK!") );
        state=0;
      }
      else {
        Serial.println( F("FAILED!") );
        // remain in this state
      }
    }
  }
  
}

void readKeys() {
  lastRead=thisRead;
  String r=isOn(f0);
  r=r+isOn(f1);
  r=r+isOn(f2);
  r=r+isOn(f3);
  r=r+isOn(f4);
  thisRead=r;
}

String isOn(int pin) {
  if (analogRead(pin)>threshold) {return "1";}
  else {return "0";}
}

void setMode(String bin) {
  if (bin=="00001") {mode=1;}
  else if (bin=="00010") {mode=2;}
  else if (bin=="00100") {mode=3;}
  else if (bin=="01000") {mode=4;}
  else if (bin=="10000") {mode=5;}
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
}
