//Here's a very simple version

#include "math.h"
#include "ADXL345.h"

ADXL345 Accel;
double x;
double y;
double z;

int i=0;

void setup(){
  Serial.begin(9600);
  Wire.begin();
  Accel.set_bw(ADXL345_BW_12);
  Serial.print("BW_OK? ");
  Serial.println(Accel.status, DEC);
  delay(1000);
}

void loop(){
    double accData[3];
    Accel.get_Gxyz(accData);
    x=accData[0];
    y=accData[1];
    z=accData[2];

    // left hand orientation: relies on y, on 0 going to 245
    // left hand on: 0 255 0

    if (y>200) {
      // then you have depressed a key
      Serial.println("key pressed...");
    }
    else {
      Serial.println();
    }
}
