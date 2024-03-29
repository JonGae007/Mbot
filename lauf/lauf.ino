#include "MeAuriga.h"
#include "Wire.h"
#include <SoftwareSerial.h>
#include <Arduino.h>

MeUltrasonicSensor usv(7);
MeUltrasonicSensor usr(8);
MeEncoderOnBoard Motor_1(SLOT1);
MeEncoderOnBoard Motor_2(SLOT2);
MeColorSensor colorsensor(6);
MeBuzzer buzzer;
MeRGBLed rgbled_0(0, 12);
MeEncoderNew EmotorV2_7_1(0x11 + 7, 1);

uint8_t colorresult;
uint16_t redvalue = 0, greenvalue = 0, bluevalue = 0, colorvalue = 0;
uint8_t grayscale = 0;
long systime = 0, colorcode = 0;
int dreh = 1.2;
int speed1 = 50;  //WICHTIG

void setup() {
  rgbled_0.setpin(44);
  rgbled_0.fillPixelsBak(0, 2, 1);
  buzzer.setpin(45);
  Serial.begin(115200);
  EmotorV2_7_1.begin();
  colorsensor.SensorInit();
  systime = millis();
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(WGM12);
  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);
  attachInterrupt(Motor_1.getIntNum(), isr_process_encoder1, RISING);
  attachInterrupt(Motor_2.getIntNum(), isr_process_encoder2, RISING);
  int stop = 0;
  while (true) {
    if (millis() - systime > 200) {
      systime = millis();
      colorresult = colorsensor.Returnresult();
      redvalue = colorsensor.ReturnRedData();
      greenvalue = colorsensor.ReturnGreenData();
      bluevalue = colorsensor.ReturnBlueData();
      colorvalue = colorsensor.ReturnColorData();
      colorcode = colorsensor.ReturnColorCode();
      grayscale = colorsensor.ReturnGrayscale();
      Serial.print("R:");
      Serial.print(redvalue);
      Serial.print("\t");
      Serial.print("G:");
      Serial.print(greenvalue);
      Serial.print("\t");
      Serial.print("B:");
      Serial.print(bluevalue);
      Serial.print("\t");
      Serial.print("C:");
      Serial.print(colorvalue);
      Serial.print("\t");
      if (redvalue < 1300 && redvalue > 800 && greenvalue < 2200 && greenvalue > 1700 && bluevalue < 1300 && bluevalue > 700) {
        Serial.println(" SILBER");
        buzzer.tone(2000, 1000);
      } else {
        if (redvalue < 450 && redvalue > 0 && greenvalue < 1200 && greenvalue > 200 && bluevalue < 510 && bluevalue > 0) {
          Serial.println(" SCHWARZ");
          turnForSeconds(1, -40, 0.9);
          turnForSeconds(3, speed1, dreh);
          if (usv.distanceCm() > 15) {
            turnForSeconds(1, 40, 0.4);
          } else {
            turnForSeconds(3, speed1, dreh);
            move(1, 0);
          }
        } else {
          if (redvalue < 700 && redvalue > 600 && greenvalue < 1100 && greenvalue > 900 && bluevalue < 500 && bluevalue > 400) {
            Serial.println(" ROT");
            turnForSeconds(1,0,5);
            EmotorV2_7_1.move(30, 40);
            rgbled_0.setColor(0, 255, 255, 255);
            rgbled_0.show();
            delay(100);
            rgbled_0.setColor(0, 255, 0, 0);
            rgbled_0.show();
            delay(100);
            rgbled_0.setColor(0, 0, 0, 0);
            rgbled_0.show();
            if (usv.distanceCm() > 15){
              turnForSeconds(1, 40, 0.9);
            } else if (usr.distanceCm() > 15){
              turnForSeconds(4, speed1, dreh);
            }else {
              turnForSeconds(4, speed1, dreh);
              turnForSeconds(1, 40, 0.9);
            }

          } else {
            Serial.println(" ");
          }
        }
      }
      if (usr.distanceCm() < 20 && usv.distanceCm() < 15) {  //vorne u rechts block
        turnForSeconds(3, speed1, dreh);
      } else if (usr.distanceCm() > 16) {  //rechts lücke
        turnForSeconds(1, 40, 0.5);
        turnForSeconds(4, speed1, dreh);
        turnForSeconds(1, 40, 0.8);
      } else if (usr.distanceCm() > 7 && usr.distanceCm() < 15) {  // zwischen 9 u 11 rechts Korrigieren
        turnForSeconds(4, 30, 0.02);
        turnForSeconds(1, 40, 0.25);
      } else if (usr.distanceCm() > 1 && usr.distanceCm() < 5) {  //zwischen 5 u 7 rechts
        turnForSeconds(3, 30, 0.02);
        turnForSeconds(1, 40, 0.25);
      } else {
        move(1, 40);
      }
      _loop();


      _loop();
    }
  }
  _loop();
}
void isr_process_encoder1(void) {
  if (digitalRead(Motor_1.getPortB()) == 0) {
    Motor_1.pulsePosMinus();
  } else {
    Motor_1.pulsePosPlus();
  }
}
void isr_process_encoder2(void) {
  if (digitalRead(Motor_2.getPortB()) == 0) {
    Motor_2.pulsePosMinus();
  } else {
    Motor_2.pulsePosPlus();
  }
}
void move(int direction, int speed) {
  int leftSpeed = 0;
  int rightSpeed = 0;
  int result = speed / 100.0 * 255;
  if (direction == 1) {  //vorwärts
    leftSpeed = -result;
    rightSpeed = result;
  } else if (direction == 2) {  //rückwärts
    leftSpeed = result;
    rightSpeed = -result;
  } else if (direction == 3) {  //links
    leftSpeed = -result;
    rightSpeed = -result;
  } else if (direction == 4) {  //rechts
    leftSpeed = result;
    rightSpeed = result;
  }
  Motor_1.setTarPWM(leftSpeed);
  Motor_2.setTarPWM(rightSpeed);
}
void _delay(float seconds) {
  if (seconds < 0.0) {
    seconds = 0.0;
  }
  long endTime = millis() + seconds * 1000;
  while (millis() < endTime) {
    _loop();
  }
}
void _loop() {
  Motor_1.loop();
  Motor_2.loop();
}
void turnForSeconds(int direction, int speed, float seconds) {
  move(direction, speed);
  _delay(seconds);
  move(1, 0);  // Stoppen des mBot nach der festgelegten Zeit
}
void loop() {
  // put your main code here, to run repeatedly:
  _loop();
}
