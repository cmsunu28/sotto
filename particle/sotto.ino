#include <I2Cdev.h>
#include <MPU6050.h>

// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;

int led=D7;

int g0=D2;
int g1=D3;
int g2=D4;
int g3=D5;
int g4=D6;

int goingDown = -3000;
int goingUp = 200;
int newValue=0;

int pin[5]={D2,D3,D4,D5,D6};
int raw[5]={0,0,0,0,0};
int rawAvg[5]={0,0,0,0,0};

// values for each finger are:
// 0: currently unpressed
// 1: going down
// 2: pressed
// 3: going up
int value[5]={0,0,0,0,0};

int now=0;

int releaseTimerOn=0;
Timer releaseTimer(200, parseValues, true);

void setup() {
    Wire.begin();

    Serial.begin(9600);

    pinMode(led, OUTPUT);
    pinMode(pin[0],OUTPUT);
    pinMode(pin[1],OUTPUT);
    pinMode(pin[2],OUTPUT);
    pinMode(pin[3],OUTPUT);
    pinMode(pin[4],OUTPUT);

    digitalWrite(pin[0],HIGH);
    digitalWrite(pin[1],HIGH);
    digitalWrite(pin[2],HIGH);
    digitalWrite(pin[3],HIGH);
    digitalWrite(pin[4],HIGH);
}

void loop() {
    // read raw accel/gyro measurements from device
    for (int x=0;x<5;x++) {
        read(x);
        if (value[x]==1) {
          // new value
          // set as pressed
          value[x]=2;
          Serial.println("Pressed "+String(x));
        }
        else if (value[x]==3) {
          // new value
          Serial.println("got a value");
        }
    }

}


void read(int g) {
    digitalWrite(pin[0],HIGH);
    digitalWrite(pin[1],HIGH);
    digitalWrite(pin[2],HIGH);
    digitalWrite(pin[3],HIGH);
    digitalWrite(pin[4],HIGH);
    digitalWrite(pin[g],LOW);
    accelgyro.initialize();
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    raw[g]=gy;
    getValue(g);
}

void getValue(int g) {
    // for now, if it passes threshold send it to the values array
    if (raw[g]<goingDown) {
        if (value[g]==0) {
            // typing down for the first time
            value[g]=1;
        }
    }
    else if (raw[g]>goingUp) {
        if (value[g]==2) {
            value[g]=3;
            // set a timer to look for the other releases
            releaseTimer.start();
            releaseTimerOn=1;
            // also set flag
        }
    }
}

void parseValues() {
    // parses all the values of the just-released ones (state 3), and then resets them to 0.
    String bin="";
    for (int x=0;x<5;x++) {
        // check for 3s
        if (value[x]==3) {
            bin="1"+bin;
            value[x]=0;
        }
        else {
            bin="0"+bin;
        }
    }

    int number=binToInt(bin);
    // and then parse the number itself
    String letter=intToLetter(number);
    Serial.println("Typed: "+letter);
    releaseTimerOn=0;
}

int binToInt(String bin) {
   // convert binary string to integer
   char *p;
   int r = strtol(bin, & p, 2);
   return r;
}

String intToLetter(int number) {
    // gets letter assigned to this one
    if (number==1) {
		return "A";
	}
	else if (number==2) {
		return "B";
	}
	else if (number==3) {
		return "C";
	}
	else if (number==4) {
		return "D";
	}
	else if (number==5) {
		return "E";
	}
	else if (number==6) {
		return "F";
	}
	else if (number==7) {
		return "G";
	}
	else if (number==8) {
		return "H";
	}
	else if (number==9) {
		return "I";
	}
	else if (number==10) {
		return "J";
	}
	else if (number==11) {
		return "K";
	}
	else if (number==12) {
		return "L";
	}
	else if (number==13) {
		return "M";
	}
	else if (number==14) {
		return "N";
	}
	else if (number==15) {
		return "O";
	}
	else if (number==16) {
		return "P";
	}
	else if (number==17) {
		return "Q";
	}
	else if (number==18) {
		return "R";
	}
	else if (number==19) {
		return "S";
	}
	else if (number==20) {
		return "T";
	}
	else if (number==21) {
		return "U";
	}
	else if (number==22) {
		return "V";
	}
	else if (number==23) {
		return "W";
	}
	else if (number==24) {
		return "X";
	}
	else if (number==25) {
		return "Y";
	}
	else if (number==26) {
		return "Z";
	}
	else if (number==31) {
		return " ";
	}
}
