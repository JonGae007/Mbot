 #include "MeAuriga.h"
 #include "Wire.h"
 #include <SoftwareSerial.h>
 #include <Arduino.h>

MeUltrasonicSensor ultraschall_vorne(7);
MeUltrasonicSensor ultraschall_rechts(8);
MeEncoderOnBoard Motor_1(SLOT1);
MeEncoderOnBoard Motor_2(SLOT2);
MeColorSensor colorsensor(6);

uint8_t colorresult;
uint16_t redvalue=0,greenvalue=0,bluevalue=0,colorvalue=0;
uint8_t grayscale = 0;
long systime = 0,colorcode=0;
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  colorsensor.SensorInit();//
  systime = millis();
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(WGM12);
  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);
  attachInterrupt(Motor_1.getIntNum(), isr_process_encoder1, RISING);
  attachInterrupt(Motor_2.getIntNum(), isr_process_encoder2, RISING);
  int stop = 0;
  while (true) {
    if (ultraschall_vorne.distanceCm() > 10 && stop==0) {
      move(1, 40 / 100.0 * 255);  // mBot dreht sich vorwärts für 2 Sekunden
    }
    if(millis()-systime>200)
  {
    systime = millis();
    colorresult = colorsensor.Returnresult();
    redvalue   = colorsensor.ReturnRedData();
    greenvalue = colorsensor.ReturnGreenData();
    bluevalue  = colorsensor.ReturnBlueData();
    colorvalue = colorsensor.ReturnColorData();
    colorcode = colorsensor.ReturnColorCode();//RGB24code
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
    Serial.print("color:");
    
    switch(colorresult)
    {
       case BLACK:
      Serial.print("BLACK");
      break;
      case BLUE:
      Serial.print("BLUE");
      break;
      case YELLOW:
      Serial.print("YELLOW");
      break;
      case GREEN:
      Serial.print("GREEN");
      break;
      case RED:
      int stop = 1;
      break;
      case WHITE:
      Serial.print("WHITE");
      break;
      default:
      break;    
     }
    Serial.print("\t");
    Serial.print("code:");
    Serial.print(colorcode,HEX);
    Serial.print("\t");
    Serial.print("grayscale:");
    Serial.println(grayscale);
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
  if (direction == 1) {  //vorwärts
    leftSpeed = -speed;
    rightSpeed = speed;
  } else if (direction == 2) {  //rückwärts
    leftSpeed = speed;
    rightSpeed = -speed;
  } else if (direction == 3) {  //links
    leftSpeed = -speed;
    rightSpeed = -speed;
  } else if (direction == 4) {  //rechts
    leftSpeed = speed;
    rightSpeed = speed;
  }
  else if (direction == 5) {  //rechts
    leftSpeed = speed / 2;
    rightSpeed = speed;
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
