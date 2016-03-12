/*
 * Test script for moonbuggy motor control
 * 
 * *******FOR TESTING PURPOSES ONLY - NO OVER CURRENT PROTECTION - DO NOT RUN MOTORS WITH MECHANICAL LOAD*******
 */

const int throttle = A13;
const int field_PWM = 2;
const int field_phase = 38;
const int arm_PWM_L = 5;
const int arm_PWM_R = 4;

int sensorValue = 0;
int throttleValue = 0;



void setup() {
  pinMode(field_phase, OUTPUT); //phase pin
  pinMode(field_PWM, OUTPUT); //field PWM
  pinMode(arm_PWM_L, OUTPUT); //armature PWM left
  pinMode(arm_PWM_R, OUTPUT); //armature PWM right
  delay(1000);
}

void loop() {
  digitalWrite(field_phase, HIGH); //set PHASE pin HIGH FOR FWD LOW FOR REV
  analogWrite(field_PWM, 230); //set field voltage
  
  sensorValue = analogRead(throttle); //input of raw throttle value
  throttleValue = map(sensorValue, 180, 818, 0, 255); //map min/max of throttle to 8bit integer
  analogWrite(arm_PWM_L, throttleValue); //apply armature voltage
  analogWrite(arm_PWM_R, throttleValue); //apply armature voltage
  

}
