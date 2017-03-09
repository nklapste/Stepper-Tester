#include "Arduino.h"
#include <AccelStepper.h>
#include <LiquidCrystal.h>
#define HALFSTEP 8

// Motor pin definitions
#define motorPin1  2      // IN1 on the ULN2003 driver 1
#define motorPin2  10     // IN2 on the ULN2003 driver 1
#define motorPin3  11     // IN3 on the ULN2003 driver 1
#define motorPin4  12     // IN4 on the ULN2003 driver 1
unsigned char Joy_x_pin = A3;

int joy_x_center;
int joy_x_pos;
int pos = 0;
int multiplier = 1;

// Initialize with pin sequence IN1-IN3-IN2-IN4 for using the AccelStepper with 28BYJ-48
AccelStepper stepper1(HALFSTEP, motorPin1, motorPin3, motorPin2, motorPin4);

LiquidCrystal lcd(3,4,6,7,8,9);

void calibrate_joyx(){
	delay(500);
	Serial.println("Calibrating Joystick...");
	joy_x_center=analogRead(Joy_x_pin);
}

int read_joystick(){
	int horz=analogRead(Joy_x_pin);
	if(abs(joy_x_center+abs(horz-joy_x_center))>(joy_x_center+65)){
	 joy_x_pos = (joy_x_center-horz)/100;
	}
	else{
		joy_x_pos = 0;
	}
return joy_x_pos;
}

void grab_input(){
	int k = 0;
    char input[100];

    if(Serial.available()!=0){ // take in serial input
    	while(input[k-1]!='e'){
    		while(Serial.available()!=0){
    			input[k] = Serial.read();
    			++k;
    		}

    	}

    	lcd_clear_row(1);

    	if(input[k-2]=='m'){ //input position muliplier

    		int val = atoi(input);

    		Serial.print("Inputed Multiplier: ");
    		Serial.println(val);

    		multiplier = val;

    		lcd.setCursor(0,1);
    	    lcd.print("Mult: ");
    		lcd.print(multiplier);
    	}
    	else if(input[k-2]=='s'){ // input speed

    		int val = atoi(input);

    		Serial.print("Inputed Speed: ");
    		Serial.println(val);

    		float speed = val;

    		if(speed>1000){
    			speed=1000;
    		}

    		lcd.setCursor(0,1);
    		lcd.print("Spd: ");
    		lcd.print(speed);

    		stepper1.setSpeed(speed);
    	}
    	else if(input[k-2]=='a'){ //input acceleration

    		int val = atoi(input);

    		Serial.print("Inputed Acceleration: ");
    		Serial.println(val);

    		float acceleration = val;

    		if(acceleration>1000){
    			acceleration=1000;
    		}

    		lcd.setCursor(0,1);
    	    lcd.print("Acc: ");
    		lcd.print(acceleration);

    		stepper1.setAcceleration(acceleration);
    	}
    	else if(input[k-2]=='M'){ //input position

        		int val = atoi(input);

        		Serial.print("Moving To: ");
        		Serial.println(val);

        		int moveval = val;

        		lcd.setCursor(0,1);
        	    lcd.print("Moving To: ");
        		lcd.print(moveval);

        		while(stepper1.currentPosition()!= moveval){
        			stepper1.moveTo(moveval);
        		    stepper1.run();
        		}

    	}
    }
}

void lcd_clear_row(int row){
	  lcd.setCursor(0,row);
	  lcd.print("                      ");
	  lcd.setCursor(0,row);
}

void setup() {
  // setup stepper
  stepper1.setMaxSpeed(1000.0);
  stepper1.setAcceleration(400.0);
  stepper1.setSpeed(1000);

  //setup serial mon
  Serial.begin(9600);
  Serial.println("Starting...");

  //setup joystick
  digitalWrite(Joy_x_pin, INPUT);
  calibrate_joyx();

  //LCD setup
  lcd.begin(16,2);
  lcd.setCursor(0,0);

}//--(end setup )---

void loop() {
	read_joystick();
	stepper1.move(joy_x_pos*multiplier);
    stepper1.run();

    if(stepper1.currentPosition()!= pos){
    pos = stepper1.currentPosition();
    lcd_clear_row(0);
    lcd.print("Position: ");
    lcd.print(pos);
    }

    grab_input();
}
