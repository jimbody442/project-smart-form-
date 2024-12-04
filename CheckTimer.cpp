#include "CheckTimer.h"

CheckTimer::CheckTimer(unsigned long interval){
  this->interval = interval;
  this->previousMillis = millis();
}
CheckTimer::CheckTimer(unsigned int interval){
  this->interval = static_cast<unsigned long>(interval);
  this->previousMillis = millis();
}
void CheckTimer::update() {
  unsigned long currentMillis = millis();
  if (currentMillis - this->previousMillis >= this->interval){
    this->previousMillis = currentMillis;
  }
}
bool CheckTimer::isInverval() { // Ensure this is implemented
  return (millis() - previousMillis >= interval);
}

bool CheckTimer::isupdate() {
  unsigned long currentMillis = millis();
  if (currentMillis - this->previousMillis >= this->interval){
    this->previousMillis = currentMillis;
    return true;
  }
  else return false;
}

String CheckTimer::status(){
  return "This Class is MyTimer!!";
}


void CheckTimer::checkSensors() {
  /*
    // A0, A1, A2에서 센서 값 읽기
    int temperature = analogRead(A0);
    int illuminance = analogRead(A1);
    int waterLevel = analogRead(A2);

    // JsonParser 인스턴스에 값 설정
    jsonParser.setTemperature(temperature);
    jsonParser.setilluminance(illuminance);
    jsonParser.setWaterLevel(waterLevel);
    */
}











