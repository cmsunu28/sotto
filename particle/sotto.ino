// This #include statement was automatically added by the Particle IDE.
#include <MPU6050.h>

// I2C device class (I2Cdev) demonstration Arduino sketch for MPU6050 class
// 10/7/2011 by Jeff Rowberg <jeff@rowberg.net>
// Updates should (hopefully) always be available at https://github.com/jrowberg/i2cdevlib
//
// Changelog:
//      2013-05-08 - added multiple output formats
//                 - added seamless Fastwire support
//      2011-10-07 - initial release

/* ============================================
I2Cdev device library code is placed under the MIT license
Copyright (c) 2011 Jeff Rowberg

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
===============================================
*/

// I2Cdev and MPU6050 must be installed as libraries, or else the .cpp/.h files
// for both classes must be in the include path of your project
#include <I2Cdev.h>
#include <MPU6050.h>

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

int16_t ax, ay, az;
int16_t gx, gy, gz;

int led=D7;

int g0=D2;
int g1=D3;
int g2=D4;
int g3=D5;
int g4=D6;

int now=0;

void setup() {
    Wire.begin();

    Serial.begin(9600);

    pinMode(led, OUTPUT);
    pinMode(g0,OUTPUT);
    pinMode(g1,OUTPUT);
    pinMode(g2,OUTPUT);
    pinMode(g3,OUTPUT);
    pinMode(g4,OUTPUT);

    digitalWrite(g0,HIGH);
    digitalWrite(g1,HIGH);
    digitalWrite(g2,HIGH);
    digitalWrite(g3,HIGH);
    digitalWrite(g4,HIGH);
}

void loop() {
    // read raw accel/gyro measurements from device
    if (now==0) {
        digitalWrite(led,LOW);
        digitalWrite(g0,LOW);
        digitalWrite(g1,HIGH);
        digitalWrite(g2,HIGH);
        digitalWrite(g3,HIGH);
        digitalWrite(g4,HIGH);
        accelgyro.initialize();
        accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        Serial.println(gy);
        now=1;
    }
    else if (now==1) {
        digitalWrite(led,HIGH);
        digitalWrite(g0,HIGH);
        digitalWrite(g1,LOW);
        digitalWrite(g2,HIGH);
        digitalWrite(g3,HIGH);
        digitalWrite(g4,HIGH);
        accelgyro.initialize();
        accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        Serial.print("      "); Serial.println(gy);
        now=2;
    }
    else if (now==2) {
        digitalWrite(led,HIGH);
        digitalWrite(g0,HIGH);
        digitalWrite(g1,HIGH);
        digitalWrite(g2,LOW);
        digitalWrite(g3,HIGH);
        digitalWrite(g4,HIGH);
        accelgyro.initialize();
        accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        Serial.print("            "); Serial.println(gy);
        now=3;
    }
    else if (now==3) {
        digitalWrite(led,HIGH);
        digitalWrite(g0,HIGH);
        digitalWrite(g1,HIGH);
        digitalWrite(g2,HIGH);
        digitalWrite(g3,LOW);
        digitalWrite(g4,HIGH);
        accelgyro.initialize();
        accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        Serial.print("                  "); Serial.println(gy);
        now=4;
    }
    else if (now==4) {
        digitalWrite(led,HIGH);
        digitalWrite(g0,HIGH);
        digitalWrite(g1,HIGH);
        digitalWrite(g2,HIGH);
        digitalWrite(g3,HIGH);
        digitalWrite(g4,LOW);
        accelgyro.initialize();
        accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
        Serial.print("                        "); Serial.println(gy);
        now=0;
    }
    delay(50);
}
