// TODO: update limits

#include <Arduino.h>
#include <ESP32Servo.h>


int selectionStage = 1;

bool servoSelectionText = false;
int servoSelectionValue;

bool angleInputText = false;
int angleInputValue;

Servo lowerRightServo;
Servo upperRightServo;
Servo upperLeftServo;
Servo lowerLeftServo;

void writeServo (int servo, int angle) {
    Serial.print("Driving: ");
    Serial.println(servo);
    Serial.print("Angle: ");
    Serial.println(angle);

    if (servo == 1 || servo == 5) {
        upperLeftServo.write(constrain(180 - angle, 90 , 170));
    }
    if (servo == 3 || servo == 5) {
        upperRightServo.write(constrain(angle, 10, 90));
    }
    if (servo == 2 || servo == 6) {
        lowerLeftServo.write(constrain(180 - angle, 10 , 160));
    }
    if (servo == 4 || servo == 6) {
        lowerRightServo.write(constrain(angle, 30, 170));
    }
}   

void servoSelection() {
    servoSelectionText = true;
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();
        int number = input.toInt();

        if (number >= 1 && number <= 6) {
            servoSelectionValue = number;
            selectionStage = 2;
            servoSelectionText = false;
            return;
        }

        Serial.println("Please enter a number between 1-6");
    }
}

void angleInput() {
    angleInputText = true;
    if (Serial.available() > 0) {
        String input = Serial.readStringUntil('\n');
        input.trim();

        if (input == "q") {
            selectionStage = 1;
            angleInputText = false;
            return;   
        }

        int number = input.toInt();

        if (number >= 0 && number <= 180) {
            angleInputValue = number;
            writeServo(servoSelectionValue, angleInputValue);
            return;
        }
    
        Serial.println("Please enter an angle between 0-180, or q to quit");
    }
}

void setup() {
    Serial.begin(115200);
    delay(500);
    
	lowerLeftServo.attach(18);
    upperLeftServo.attach(19);

    lowerRightServo.attach(33);
    upperRightServo.attach(32);
}

void loop() {
    switch (selectionStage) {
        case 1:
            if (servoSelectionText == false) {
                Serial.println("Please select which servo(s) to drive 1-6:");
                Serial.println("1 - Upper Left");
                Serial.println("2 - Lower Left");
                Serial.println("3 - Upper Right");
                Serial.println("4 - Lower Right");
                Serial.println("5 - Both Upper");
                Serial.println("6 - Lower Both");
            }
            servoSelection();
            break;
        case 2:
            if (angleInputText == false) {
                Serial.println("Please enter an angle between 0-180, or q to quit");
            }
            angleInput();
            break;
    }

}