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

uint8_t colorresult;
uint16_t redvalue = 0, greenvalue = 0, bluevalue = 0, colorvalue = 0;
uint8_t grayscale = 0;
long systime = 0, colorcode = 0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  colorsensor.SensorInit();  //
  systime = millis();
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(WGM12);
  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);
  attachInterrupt(Motor_1.getIntNum(), isr_process_encoder1, RISING);
  attachInterrupt(Motor_2.getIntNum(), isr_process_encoder2, RISING);
  int stop = 0;
  while (true) {
    buzzer.setpin(45);
    if (millis() - systime > 200) {//alle 200 millisekunden Farbsensor abfragen
      systime = millis();
      colorresult = colorsensor.Returnresult();
      redvalue   = colorsensor.ReturnRedData();
      greenvalue = colorsensor.ReturnGreenData();
      bluevalue  = colorsensor.ReturnBlueData();
      colorvalue = colorsensor.ReturnColorData();
      colorcode = colorsensor.ReturnColorCode();
      grayscale  = colorsensor.ReturnGrayscale();
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
      if(colorvalue <100 &&colorvalue >70){
        Serial.println(" SILBER");
        buzzer.tone(500, 1000);
      }else{
        if(colorvalue <20 &&colorvalue >0){
        Serial.println(" SCHWARZ");
        move(1, -40);
        buzzer.tone(700, 1000);
      }else{
        if(colorvalue <47 &&colorvalue >25){
        Serial.println(" ROT");
      }else{
        Serial.println(" ");
      }
      }
      }if(usr.distanceCm()< 20 && usv.distanceCm()< 10){ //vorne u rechts block
        turnForSeconds(3, 40, 0.4);
      }else if(usr.distanceCm()> 20){ //rechts lücke
      turnForSeconds(1, 40, 0.7);
      turnForSeconds(4, 40, 0.4);
      turnForSeconds(1, 40, 0.7);
    }else if(usr.distanceCm()> 9&&usr.distanceCm()< 15){ // zwischen 9 u 11 rechts Korrigieren
      move(4, 40);
    }else if(usr.distanceCm()> 5&&usr.distanceCm()< 7){ //zwischen 5 u 7 rechts
      move(3, 40);
    }else{
      move(1,40);
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
  } else if (direction == 5) {  //links
    leftSpeed = -result;
    rightSpeed = -result/1.5;
  } else if (direction == 6) {  //rechts
    leftSpeed = result/1.5;
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
  move(1, 0);
}
void loop() {
  _loop();
}
