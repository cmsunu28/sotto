//Here's a very simple version

#include "math.h"
#include "ADXL345.h"

ADXL345 Accel;

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
    Serial.print("X: "+String(accData[0]));
    Serial.print("   ");
    Serial.print("Y: "+String(accData[1]));
    Serial.print("   ");
    Serial.println("Z: "+String(accData[2]));
}
