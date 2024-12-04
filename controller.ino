#include <Arduino.h>
#include "CheckTimer.h"
#include "JsonParser.h"
#include "EepromManager.h"
//#define DEBUG
//#define FIRST 

const unsigned long interval_time = 1000;
CheckTimer checkTimer(interval_time); // 1초마다 체크
JsonParser jsonParser(Serial, 9600);
//#ifdef FIRST
//EepromManager eepromManager(true);
//#else
EepromManager eepromManager;
//#endif

const int pumpPin = 2;
const int ledPin = 3;
const int heaterPin = 4;

void setup() {
    jsonParser.begin();
    pinMode(pumpPin, OUTPUT);
    pinMode(ledPin, OUTPUT);
    pinMode(heaterPin, OUTPUT);
    pinMode(13, OUTPUT);
    
    eepromManager.readFromEeprom();
#ifdef DEBUG    
    Serial.println(strcmp(eepromManager.getPumpStatus(),"ON"));
    Serial.println(strcmp(eepromManager.getLedStatus(),"ON"));
    Serial.println(strcmp(eepromManager.getHeaterStatus(),"ON"));
#endif    
    // EEPROM에서 읽은 값을 핀에 출력
    digitalWrite(pumpPin, strncmp(eepromManager.getPumpStatus(),"ON",2) ? LOW : HIGH);
    digitalWrite(ledPin, strncmp(eepromManager.getLedStatus(),"ON", 2) ? LOW : HIGH);
    digitalWrite(heaterPin, strncmp(eepromManager.getHeaterStatus(),"ON",2) ? LOW : HIGH);
}

void loop() {
    char pumpOrder[5] = "OFF"; // 변수 선언 추가
    char ledOrder[5] =  "OFF"; // 변수 선언 추가
    char heaterOrder[5] = "OFF"; // 변수 선언 추가
    if (checkTimer.isupdate()) {
      //checkTimer.checkSensors(); // 센서 체크
      // A0, A1, A2에서 센서 값 읽기
#ifdef DEBUG
      int temperature = random(15, 22);
      int illuminance = random(50, 70);
      int waterLevel = random(15, 34);
#else
      //int temperature = analogRead(A0);
      //int illuminance = analogRead(A1);
      //int waterLevel = analogRead(A2);
      int temperature = random(15, 22);
      int illuminance = random(50, 70);
      int waterLevel = random(15, 34);
#endif
      // JsonParser 인스턴스에 값 설정
      eepromManager.setTemperature(temperature);
      eepromManager.setilluminance(illuminance);
      eepromManager.setWaterLevel(waterLevel);        


      if (jsonParser.update()) { // JSON 데이터 업데이트
            // read_status 처리
            if (jsonParser.isReadStatusReceived()) {
                char tempData[200];
                String tojon = eepromManager.toJson();
                tojon.toCharArray(tempData, sizeof(tempData)); // char 배열에 복사
                //jsonParser.sendJson(tempData);
                jsonParser.handleReadStatus(temperature,\
                                            illuminance,\
                                            waterLevel,\
                                            eepromManager.getPumpStatus(),\
                                            eepromManager.getLedStatus(), \
                                            eepromManager.getHeaterStatus());
                return;
            }

            // 제어 명령 처리
            if (jsonParser.getPumpStatus() == "ON") {
                digitalWrite(pumpPin, HIGH);
                memcpy(pumpOrder, "ON", sizeof(pumpOrder)+1);
            } else if (jsonParser.getPumpStatus() == "OFF") {
                digitalWrite(pumpPin, LOW);
                memcpy(pumpOrder, "OFF", sizeof(pumpOrder)+1);
            }   
            if (jsonParser.getLedStatus() == "ON") {
                digitalWrite(ledPin, HIGH);
                memcpy(ledOrder,"ON", sizeof(ledOrder)+1);
            } else if (jsonParser.getLedStatus() == "OFF") {
                digitalWrite(ledPin, LOW);
                memcpy(ledOrder,"OFF", sizeof(ledOrder)+1);
            }   
            if (jsonParser.getHeaterStatus() == "ON") {
                digitalWrite(heaterPin, HIGH);
                memcpy(heaterOrder, "ON", sizeof(heaterOrder)+1);
            } else if (jsonParser.getHeaterStatus() == "OFF") {
                digitalWrite(heaterPin, LOW);
                memcpy(heaterOrder, "OFF", sizeof(heaterOrder)+1);
            }
            //eepromManager.setPumpStatus(pumpOrder);
            //eepromManager.setLedStatus(ledOrder);
            //eepromManager.setHeaterStatus(heaterOrder);
            eepromManager.updateDeviceStatus(pumpOrder, ledOrder, heaterOrder); // 장치 상태 업데이트
        }
    }

}