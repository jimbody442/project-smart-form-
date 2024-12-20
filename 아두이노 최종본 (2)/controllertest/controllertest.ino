#include <Arduino.h>
#include "CheckTimer.h"
#include "JsonParser.h"
#include "EepromManager.h"
#include <OneWire.h>
#include <DallasTemperature.h>
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



OneWire oneWire(A0);
DallasTemperature sensors(&oneWire);


#define RT

const int pumpPin = 2;
const int ledPin = 3;
const int heaterPin = 4;
int temperature = sensors.getTempCByIndex(0);
int illuminance = (analogRead(A1) /1023.0)*1000;;
int waterLevel = analogRead(A2);

void setup() {
    jsonParser.begin();
    pinMode(pumpPin, OUTPUT);
    pinMode(ledPin, OUTPUT);
    pinMode(heaterPin, OUTPUT);
    pinMode(13, OUTPUT);
    
    eepromManager.readFromEeprom();
//#ifdef DEBUG    
//    Serial.println(strcmp(eepromManager.getPumpStatus(),"ON"));
//    Serial.println(strcmp(eepromManager.getLedStatus(),"ON"));
//    Serial.println(strcmp(eepromManager.getHeaterStatus(),"ON"));
#ifdef RT   
    // EEPROM에서 읽은 값을 핀에 출력
    digitalWrite(pumpPin, strncmp(eepromManager.getPumpStatus(),"ON",2) ? LOW : HIGH);
    digitalWrite(ledPin, strncmp(eepromManager.getLedStatus(),"ON", 2) ? LOW : HIGH);
    digitalWrite(heaterPin, strncmp(eepromManager.getHeaterStatus(),"ON",2) ? LOW : HIGH);
#endif
}

void loop() {
  int count = 0;
  int illsum = 0;
  int ill[100] ;
  int illaver=0;

  int watersum = 0;
  int water[100] ;
  int wateraver=0;


  sensors.requestTemperatures();
  int tempsum = 0;
  int temp[100];
  int tempaver = 0;
  
  char pumpOrder[5] = "OFF"; // 변수 선언 추가
  char ledOrder[5] ="OFF"; // 변수 선언 추가
  char heaterOrder[5] = "OFF"; // 변수 선언 추가

  //if (checkTimer.isupdate()) {
    //checkTimer.checkSensors(); // 센서 체크
    // A0, A1, A2에서 센서 값 읽기
#ifdef DEBUG
  int temperature = random(15, 22);
  int illuminance = random(50, 70);
  int waterLevel = random(15, 34);
#else
  int temperature = sensors.getTempCByIndex(0);
  int illuminance = (analogRead(A1) /1023.0)*100;
  int waterLevel = analogRead(A2);

  //int temperature = analogRead(A0);
  //int illuminance = analogRead(A1);
  //int waterLevel = analogRead(A2);
  //int temperature = random(15, 22);
  //int illuminance = random(50, 70);
  //int waterLevel = random(15, 34);
#endif

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

    ill[count] = illuminance;
    water[count] = waterLevel;
    temp[count] = temperature;
      //delay(3); // 50ms 대기 (센서 값 안정화)
            
    qsortWithoutCompare(ill, 100); //qsort가 사용되지 않아 오름차순 함수를 만들어 사용.
    qsortWithoutCompare(water, 100);
    qsortWithoutCompare(temp, 100);
              
    if (count < 100){
      count++;
    }
    else {
      for (int i = 45; i < 55; i++) {   
          illsum += ill[i];             //100개 중 중간값을 10개 누적하여 합산하여 변수에 저장.
          watersum += water[i];
          tempsum += temp[i];
      }                                 

      illaver = illsum / 10;              // 값의 평균값을 위해 10개를 추출하여 평균값 계산.
      wateraver = watersum / 10;          // +
      tempaver = tempsum / 10;            // +        
      count = 0;
      //자동제어 관련 명령어.
      // 물높이
      if (wateraver < 200) {                          //100 이하면 펌프 작동.
            // Water level is too low; turn ON the pump
            digitalWrite(pumpPin, HIGH);
            memcpy(pumpOrder, "ON", sizeof(pumpOrder));
        } else if (wateraver > 400) {                //400 이상이면 펌프 작동 중단.
            // Water level is too high; turn OFF the pump
            digitalWrite(pumpPin, LOW);
            memcpy(pumpOrder, "OFF", sizeof(pumpOrder));
        } else {
            // Maintain the pump's current state based on JSON command
            if (jsonParser.getPumpStatus() == "ON") {     // 100~450 사이에서 ON, OFF를 설정 할 수 있음. 단 100이하는 무조건 켜지고
                digitalWrite(pumpPin, HIGH);              // 450 이상에서는 무조건 OFF된다.
                memcpy(pumpOrder, "ON", sizeof(pumpOrder));
            } else if (jsonParser.getPumpStatus() == "OFF") {
                digitalWrite(pumpPin, LOW);
                memcpy(pumpOrder, "OFF", sizeof(pumpOrder));
            }
        }
        if (illaver < 2) {                          
            // Water level is too low; turn ON the pump
            digitalWrite(ledPin, HIGH);
            memcpy(ledOrder, "ON", sizeof(ledOrder));
        } else if (illaver > 15) {                
            // Water level is too high; turn OFF the pump
            digitalWrite(ledPin, LOW);
            memcpy(ledOrder, "OFF", sizeof(ledOrder));
        } else {
            if (jsonParser.getLedStatus() == "ON" && digitalRead(ledPin) == HIGH) {     
                digitalWrite(ledPin, HIGH);              
                memcpy(ledOrder, "ON", sizeof(ledOrder));
            } else if (jsonParser.getLedStatus() == "OFF"&& digitalRead(ledPin) == LOW) {
                digitalWrite(ledPin, LOW);
                memcpy(ledOrder, "OFF", sizeof(ledOrder));
            }
        }
        //조도
        // if (illaver < 1) {
        //       // 조도가 너무 낮을 경우 LED를 ON     1이하면 무조건 켜짐.
        //       digitalWrite(ledPin, HIGH);
        //       
        //      
        //   } else if (illaver > 10) {       //100이상이면 무조건 꺼짐.
        //       // 조도가 너무 높을 경우 LED를 OFF
        //       digitalWrite(ledPin, LOW);
        //       
        //       
        //   } else {
        //       // 조도가 적정 범위에 있을 경우 JSON 명령에 따라 제어 
        //       if (((illaver >= 3 && illaver <= 10) && jsonParser.getLedStatus() == "ON")|| ledPin ==1) {    
        //         digitalWrite(ledPin, HIGH);    // 빛이 3~10사이에 값이라면 ON OFF를 조정할 수 있고  10을 넘으면 무조건 꺼짐.
        //                            //1 이하는 무조건 켜짐
        //           memcpy(ledOrder, "ON", sizeof(ledOrder));
        //          
        //       } else if (((illaver >= 3 && illaver <= 10) &&jsonParser.getLedStatus() == "OFF")||ledPin ==0) {
        //         digitalWrite(ledPin, LOW);
        //           memcpy(ledOrder, "OFF", sizeof(ledOrder));
        //          
        //       }

        //   }
          //온도
        if (tempaver < 20) {
          // 온도가 너무 낮을 경우 히터를 ON
            digitalWrite(heaterPin, HIGH);        // 20이하면 히터 ON
            memcpy(heaterOrder, "ON", sizeof(heaterOrder));
        } else if (tempaver > 26) {               // 26이상이면 무조건 꺼짐 OFF
            // 온도가 너무 높을 경우 히터를 OFF

            digitalWrite(heaterPin, LOW);
            memcpy(heaterOrder, "OFF", sizeof(heaterOrder));
        } else {
            // 온도가 적정 범위에 있을 경우 JSON 명령에 따라 제어
            if (jsonParser.getHeaterStatus() == "ON") {       //20~26사이에 값은 ON OFF 조절 가능.
                digitalWrite(heaterPin, HIGH);
                memcpy(heaterOrder, "ON", sizeof(heaterOrder));
            } else if (jsonParser.getHeaterStatus() == "OFF") {
                digitalWrite(heaterPin, LOW);
                memcpy(heaterOrder, "OFF", sizeof(heaterOrder));
            }
        }
        eepromManager.updateDeviceStatus(pumpOrder, ledOrder, heaterOrder); // 장치 상태 업데이트
    }
  }
}



void qsortWithoutCompare(int* array, int size) {    //함수를 이용하여 오름차순으로 정의.
  for (int i = 0; i < size - 1; i++) {
    for (int j = 0; j < size - i - 1; j++) {
      if (array[j] > array[j + 1]) {
        int temp = array[j];
        array[j] = array[j + 1];
        array[j + 1] = temp;
      }
    }
  }
}


