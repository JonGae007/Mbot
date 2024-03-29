// generated by mBlock5 for mBot Ranger
// codes make you happy

#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <MeAuriga.h>

MeLightSensor lightsensor_12(12);
MeGyro gyro_0(0, 0x69);
MeUltrasonicSensor usv(7);
MeUltrasonicSensor usr(8);
MeEncoderOnBoard Motor_1(SLOT1);
MeEncoderOnBoard Motor_2(SLOT2);
MeColorSensor colorsensor(6);
MeBuzzer buzzer;
MeRGBLed rgbled_0(0, 12);
uint8_t colorresult;
uint16_t redvalue = 0, greenvalue = 0, bluevalue = 0, colorvalue = 0;
uint8_t grayscale = 0;
long systime = 0, colorcode = 0;
int kurve = 500;

void _delay(float seconds) {
  if(seconds < 0.0){
    seconds = 0.0;
  }
  long endTime = millis() + seconds * 1000;
  while(millis() < endTime) _loop();
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
void setup() {
  rgbled_0.setpin(44);
  rgbled_0.fillPixelsBak(0, 2, 1);
  buzzer.setpin(45);
  gyro_0.begin();
  Serial.begin(115200);
  colorsensor.SensorInit();  //
  systime = millis();
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(WGM12);
  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);
  attachInterrupt(Motor_1.getIntNum(), isr_process_encoder1, RISING);
  attachInterrupt(Motor_2.getIntNum(), isr_process_encoder2, RISING);
  randomSeed((unsigned long)(lightsensor_12.read() * 123456));
  
  
  while(1) {
      Serial.println(gyro_0.getAngle(3));
      redvalue   = colorsensor.ReturnRedData();
      greenvalue = colorsensor.ReturnGreenData();
      bluevalue  = colorsensor.ReturnBlueData();
      if(redvalue <1300 &&redvalue >800&&greenvalue <2200&&greenvalue >1700&&bluevalue <1300 &&bluevalue >700){
        Serial.println(" SILBER");
        buzzer.tone(900, 1000);
      }
      if(redvalue <450 &&redvalue >0&&greenvalue <1200 &&greenvalue >200&&bluevalue <510 &&bluevalue >0){
        Serial.println(" SCHWARZ");
        move(2, 40);
        while()
        move(3, 40);
        delay(kurve);
        if(usv.distanceCm()< 20){
          move(3, 40);
          delay(kurve);
          move(1, 40);
        }
      }
      if(redvalue <700 &&redvalue >600&&greenvalue <1100 &&greenvalue >900&&bluevalue <500 &&bluevalue >400){
        Serial.println(" ROT");
        //Hier externer Encoder einfügen
      }
      if(usr.distanceCm()< 20 && usv.distanceCm()< 10){ //vorne u rechts block
        move(3, 40);
        delay(kurve);
      }
      if(usr.distanceCm()> 20){ //rechts lücke

      }
      _loop();
  }

}



void _loop() {
  gyro_0.update();
  Motor_1.loop();
  Motor_2.loop();
}

void loop() {
  _loop();
}