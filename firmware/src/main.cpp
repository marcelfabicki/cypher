// ask for input type then angle/position then move each leg simultaneously to that value
#include <Arduino.h>
#include <ESP32Servo.h>

int angle;
int position;

Servo lowerLeg1;
Servo lowerLeg2;

Servo upperLeg1;
Servo upperLeg2;

void userInput() {
    Serial.print("Enter 'a' for angle input and 'p' for position input: ");
    while(true) {
            while (Serial.available() == 0) {}
            String input = Serial.readStringUntil('\n'); 
            input.trim();

            if (input == "a") {
                Serial.print("Enter angle: ");
                while (Serial.available() == 0) {}
                angle = Serial.parseInt();
                //servoAngle();
            }
            else if (input == "p") {
                Serial.print("Enter position: ");
                while (Serial.available() == 0) {}
                position = Serial.parseInt();
                break;
            }
            else {
                Serial.println("please enter a valid input: ");
            }

        }
    Serial.println(angle);
    Serial.println(position);
} 

void servoAngle() {
    for (int i = 0; i = 90; i++) {
        //myServo1.write(i);
        delay(200);
    }
}

void servoPosition () {

}

void setup() {
    Serial.begin(115200);
    delay(2000);

    lowerLeg1.attach(33);
    lowerLeg2.attach(19);

    upperLeg1.attach(32);
    upperLeg2.attach(18);

}

void loop() {
    //userInput();
	for (int pos = 0; pos <= 160; pos += 1) { // goes from 0 degrees to 180 degrees
		// in steps of 1 degree
		lowerLeg1.write(pos + 20); 
        lowerLeg2.write(180 - pos); // tell servo to go to position in variable 'pos'
        delay(15);             // waits 15ms for the servo to reach the position
	}
	for (int pos = 160; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
		lowerLeg1.write(pos + 20);
        lowerLeg2.write(180 - pos);   // tell servo to go to position in variable 'pos'
		delay(15);             // waits 15ms for the servo to reach the position
	}

    
}