#ifndef EEPROMMANAGER_H 
#define EEPROMMANAGER_H 

#include <ArduinoJson.h>
#include <avr/eeprom.h> // EEPROM 관련 함수 정의 추가

struct SensorData {
    int temperature;
    int illuminance;
    int waterLevel;
    char pumpStatus[5];
    char ledStatus[5];
    char heaterStatus[5];
};

// 각 필드의 오프셋을 정의
const uint16_t EEPROM_OFFSET_TEMPERATURE = 0;
const uint16_t EEPROM_OFFSET_ILLUMINANCE = 2;
const uint16_t EEPROM_OFFSET_WATERLEVEL = 4;
const uint16_t EEPROM_OFFSET_PUMPSTATUS = 9;
const uint16_t EEPROM_OFFSET_LEDSTATUS = 14;
const uint16_t EEPROM_OFFSET_HEATERSTATUS = 19;

class EepromManager {
public:
    EepromManager();
    EepromManager(bool isInit);
    void writeToEeprom();
    int readFromEeprom();
    int readFromEeprom(SensorData& currentData);
    int getTemperature(); // 온도 값 읽기
    void setTemperature(int temp); // 온도 값 쓰기    
    int getilluminance(); // 조도 값 읽기
    void setilluminance(int illuminance); // 조도 값 쓰기
    int getWaterLevel(); // 수위 값 읽기
    void setWaterLevel(int waterLevel); // 수위 값 쓰기
    char* getPumpStatus(); // 펌프 상태 읽기
    void setPumpStatus(char* pumpStatus); // 펌프 상태 쓰기
    char* getLedStatus(); // LED 상태 읽기
    void setLedStatus(char* ledStatus); // LED 상태 쓰기
    char* getHeaterStatus(); // 히터 상태 읽기
    void setHeaterStatus(char* heaterStatus); // 히터 상태 쓰기
    String toJson(); // JSON 문자열로 변환
    void updateDeviceStatus(); // 장치 상태 업데이트 메서드 추가
    void updateDeviceStatus(char* pumpStatus, char* ledStatus, char* heaterStatus);
    void write_data(int &pos, const uint8_t* value, uint16_t size);
    void read_data(int &pos, uint8_t* value, uint16_t size);

private:
    SensorData data;
    uint8_t eeprom_data[sizeof(SensorData)];
    int eeprom_pos = 0;
    bool eeprom_write_error = false;
    bool eeprom_read_error = false;
    uint8_t eeprom_checksum = 0;

    //uint8_t eeprom_read_byte(const uint8_t* address);
    //void eeprom_write_byte(uint8_t* address, uint8_t value);
};

#endif