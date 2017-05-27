#include "MPU6050.h"

int led=D7;

int maxVal=32767;
int minVal=-32768;

int tapState=0;

// MPU variables:
MPU6050 accelgyro;
int16_t ax, ay, az;
int16_t gx, gy, gz;


void setup() {

    Wire.begin();
    Serial.begin(9600);

}

void loop() {
    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    // the main one of interest here is gy, which tells us the angular momentum of the tapping finger

    if (gy==maxVal) {
      // downstroke happening
      if (tapState==0) {
        Serial.println("down");
        tapState=1;
        digitalWrite(led,HIGH);
      }
    }
    else if (gy==minVal) {
      // upstroke happening
      if (tapState==1) {
        Serial.println("up");
        tapState=0;
        digitalWrite(led,LOW);
      }
    }

    /*Serial.print("a/g:\t");
    Serial.print(ax); Serial.print("\t");
    Serial.print(ay); Serial.print("\t");
    Serial.print(az); Serial.print("\t");
    Serial.print(gx); Serial.print("\t");
    Serial.print(gy); Serial.print("\t");
    Serial.println(gz);*/

}
