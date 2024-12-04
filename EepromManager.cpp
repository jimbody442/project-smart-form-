// EepromManager.cpp

#include "EepromManager.h"
#include <avr/eeprom.h> 

EepromManager::EepromManager(bool isInit) {
    if (isInit) {
        data.temperature = 0;      // 기본 온도 값
        data.illuminance = 0;     // 기본 조도 값
        data.waterLevel = 0;      // 기본 수위 값
        memcpy(data.pumpStatus, "OFF", sizeof(data.pumpStatus));
        memcpy(data.ledStatus, "OFF", sizeof(data.ledStatus));
        memcpy(data.heaterStatus, "OFF", sizeof(data.heaterStatus));
        writeToEeprom();
    }
}

EepromManager::EepromManager() {
    readFromEeprom(); // readFromEeprom 호출
}

void EepromManager::writeToEeprom() {
    // 각 데이터를 개별적으로 EEPROM에 저장
    eeprom_write_byte((uint8_t*) EEPROM_OFFSET_TEMPERATURE, (uint8_t)data.temperature);
    eeprom_write_byte((uint8_t*) EEPROM_OFFSET_ILLUMINANCE, (uint8_t)data.illuminance);
    eeprom_write_byte((uint8_t*) EEPROM_OFFSET_WATERLEVEL, (uint8_t)data.waterLevel);
    
    

    // 문자열을 전체 저장
    eeprom_write_block(data.pumpStatus, (void*) EEPROM_OFFSET_PUMPSTATUS, sizeof(data.pumpStatus)); // 전체 문자열 저장
    eeprom_write_block(data.ledStatus, (void*) EEPROM_OFFSET_LEDSTATUS, sizeof(data.ledStatus)); // 전체 문자열 저장
    eeprom_write_block(data.heaterStatus, (void*) EEPROM_OFFSET_HEATERSTATUS, sizeof(data.heaterStatus)); // 전체 문자열 저장
}

int EepromManager::readFromEeprom() {
    // EEPROM에서 값을 읽어옵니다.
    data.temperature = eeprom_read_byte((const uint8_t*) EEPROM_OFFSET_TEMPERATURE);
    data.illuminance = eeprom_read_byte((const uint8_t*) EEPROM_OFFSET_ILLUMINANCE);
    data.waterLevel = eeprom_read_byte((const uint8_t*) EEPROM_OFFSET_WATERLEVEL);
    
    // 문자열을 전체 읽기 위해 eeprom_read_block 사용
    eeprom_read_block((void*)data.pumpStatus, (const void*) EEPROM_OFFSET_PUMPSTATUS, sizeof(data.pumpStatus));
    eeprom_read_block((void*)data.ledStatus, (const void*) EEPROM_OFFSET_LEDSTATUS, sizeof(data.ledStatus));
    eeprom_read_block((void*)data.heaterStatus, (const void*) EEPROM_OFFSET_HEATERSTATUS, sizeof(data.heaterStatus));

    // 읽은 값이 유효한지 확인 (예: 온도 값이 특정 범위 내에 있는지 확인)
    if (data.temperature < 0 || data.temperature > 100) { // 예시: 온도 값이 0~100 범위 내에 있어야 함
        return false; // 실패 시 false 반환
    }

    return true; // 성공적으로 읽었음을 나타내는 true 반환
}

int EepromManager::readFromEeprom(SensorData& currentData) {
    // EEPROM에서 값을 읽어옵니다.
    currentData.temperature = eeprom_read_byte((const uint8_t*) EEPROM_OFFSET_TEMPERATURE);
    currentData.illuminance = eeprom_read_byte((const uint8_t*) EEPROM_OFFSET_ILLUMINANCE);
    currentData.waterLevel = eeprom_read_byte((const uint8_t*) EEPROM_OFFSET_WATERLEVEL);
    
    // 문자열을 전체 읽기 위해 eeprom_read_block 사용
    eeprom_read_block((void*)currentData.pumpStatus, (const void*) EEPROM_OFFSET_PUMPSTATUS, sizeof(currentData.pumpStatus));
    eeprom_read_block((void*)currentData.ledStatus, (const void*) EEPROM_OFFSET_LEDSTATUS, sizeof(currentData.ledStatus));
    eeprom_read_block((void*)currentData.heaterStatus, (const void*) EEPROM_OFFSET_HEATERSTATUS, sizeof(currentData.heaterStatus));

    // 읽은 값이 유효한지 확인 (예: 온도 값이 특정 범위 내에 있는지 확인)
    if (currentData.temperature < 0 || currentData.temperature > 100) { // 예시: 온도 값이 0~100 범위 내에 있어야 함
        return false; // 실패 시 false 반환
    }

    return true; // 성공적으로 읽었음을 나타내는 true 반환
}

int EepromManager::getTemperature() {
    // EEPROM에서 온도 값을 읽어 반환
    return eeprom_read_byte((const uint8_t*) EEPROM_OFFSET_TEMPERATURE);
}

void EepromManager::setTemperature(int temp) {
    // 온도 값을 EEPROM에 저장
    eeprom_write_byte((uint8_t*) EEPROM_OFFSET_TEMPERATURE, (uint8_t)temp);
}

int EepromManager::getilluminance() {
    // EEPROM에서 조도 값을 읽어 반환
    return eeprom_read_byte((const uint8_t*) EEPROM_OFFSET_ILLUMINANCE);
}

void EepromManager::setilluminance(int illuminance) {
    // 조도 값을 EEPROM에 저장
    eeprom_write_byte((uint8_t*) EEPROM_OFFSET_ILLUMINANCE, (uint8_t)illuminance);
}

int EepromManager::getWaterLevel() {
    // EEPROM에서 수위 값을 읽어 반환
    return eeprom_read_byte((const uint8_t*) EEPROM_OFFSET_WATERLEVEL);
}

void EepromManager::setWaterLevel(int waterLevel) {
    // 수위 값을 EEPROM에 저장
    eeprom_write_byte((uint8_t*) EEPROM_OFFSET_WATERLEVEL, (uint8_t)waterLevel);
}

char* EepromManager::getPumpStatus() {
    data.pumpStatus[sizeof(data.pumpStatus) - 1] = 0;
    return data.pumpStatus; // 펌프 상태 반환
}

void EepromManager::setPumpStatus(char* pumpStatus) {
    memset(data.pumpStatus, 0, sizeof(data.pumpStatus)); 
    memcpy(data.pumpStatus, pumpStatus, strlen(pumpStatus) + 1); // strcpy를 memcpy로 변경
    // 펌프 상태를 EEPROM에 저장
    eeprom_write_byte((uint8_t*) EEPROM_OFFSET_PUMPSTATUS, data.pumpStatus[0]); // 첫 번째 문자만 저장
    writeToEeprom(); // 변경된 값을 EEPROM에 저장
}

char* EepromManager::getLedStatus() {
    data.ledStatus[sizeof(data.ledStatus) - 1] = 0;
    return data.ledStatus; // LED 상태 반환
}

void EepromManager::setLedStatus(char* ledStatus) {
    memset(data.ledStatus, 0, sizeof(data.ledStatus)); 
    memcpy(data.ledStatus, ledStatus, strlen(ledStatus) + 1); // strcpy를 memcpy로 변경
    // LED 상태를 EEPROM에 저장
    eeprom_write_byte((uint8_t*) EEPROM_OFFSET_LEDSTATUS, data.ledStatus[0]); // 첫 번째 문자만 저장
    writeToEeprom(); // 변경된 값을 EEPROM에 저장
}

char* EepromManager::getHeaterStatus() {
    data.heaterStatus[sizeof(data.heaterStatus) - 1] = 0;
    return data.heaterStatus; // 히터 상태 반환
}

void EepromManager::setHeaterStatus(char* heaterStatus) {
    memset(data.heaterStatus, 0, sizeof(data.heaterStatus)); 
    memcpy(data.heaterStatus, heaterStatus, strlen(heaterStatus) + 1); // strcpy를 memcpy로 변경
    // 히터 상태를 EEPROM에 저장
    eeprom_write_byte((uint8_t*) EEPROM_OFFSET_HEATERSTATUS, data.heaterStatus[0]); // 첫 번째 문자만 저장
    writeToEeprom(); // 변경된 값을 EEPROM에 저장
}

String EepromManager::toJson() {
    StaticJsonDocument<200> doc; // JSON 문서 생성
    doc["temperature"] = data.temperature; // 온도 값 추가
    doc["illuminance"] = data.illuminance; // 조도 값 추가
    doc["waterLevel"] = data.waterLevel; // 수위 값 추가
    doc["pumpStatus"] = String(data.pumpStatus); // 펌프 상태 추가
    doc["ledStatus"] = String(data.ledStatus); // LED 상태 추가
    doc["heaterStatus"] = String(data.heaterStatus); // 히터 상태 추가

    String jsonString;
    serializeJson(doc, jsonString); // JSON 문자열로 직렬화
    return jsonString; // JSON 문자열 반환
}

void EepromManager::updateDeviceStatus() {
    // 장치 상태 업데이트 로직
}

void EepromManager::updateDeviceStatus(char* pumpStatus, char* ledStatus, char* heaterStatus) {
    // EEPROM에서 현재 상태를 읽어와서 비교 후 업데이트
    SensorData currentData;
    readFromEeprom(currentData);

    if (strcmp(currentData.pumpStatus, pumpStatus) != 0) { // memcmp로 문자열 비교
        memset(data.pumpStatus, 0, sizeof(data.pumpStatus)); // 초기화
        memcpy(data.pumpStatus, pumpStatus, strlen(pumpStatus) + 1); 
//        Serial.print(currentData.pumpStatus);
//        Serial.print(" change pump value :");
//        Serial.println(pumpStatus);
    }
    if (strcmp(currentData.ledStatus, ledStatus) != 0) { // memcmp로 문자열 비교
        memset(data.ledStatus, 0, sizeof(data.ledStatus)); // 초기화
        memcpy(data.ledStatus, ledStatus, strlen(ledStatus) + 1);
//        Serial.print(currentData.ledStatus);
//        Serial.print(" change led value :");
//        Serial.println(ledStatus);
    }
    if (strcmp(currentData.heaterStatus, heaterStatus) != 0) { // memcmp로 문자열 비교
        memset(data.heaterStatus, 0, sizeof(data.heaterStatus)); // 초기화
        memcpy(data.heaterStatus, heaterStatus, strlen(heaterStatus) + 1);
//        Serial.print(strlen(currentData.heaterStatus));
//        Serial.print(" change heater value :");
//        Serial.println(heaterStatus);
    }

    writeToEeprom(); // 변경된 값을 EEPROM에 저장
}

void EepromManager::write_data(int &pos, const uint8_t* value, uint16_t size) {
    if (eeprom_write_error) return;
    while (size--) {
        uint8_t * const p = (uint8_t * const)pos;
        const uint8_t v = *value;
        // EEPROM has only ~100,000 write cycles,
        // so only write bytes that have changed!
        if (v != eeprom_read_byte(p)) {
            eeprom_write_byte(p, v);
            if (eeprom_read_byte(p) != v) {
                eeprom_write_error = true;
                return;
            }
        }
        eeprom_checksum += v;
        pos++;
        value++;
    }
}

void EepromManager::read_data(int &pos, uint8_t* value, uint16_t size) {
    do {
        uint8_t c = eeprom_read_byte((unsigned char*)pos);
        if (!eeprom_read_error) *value = c;
        eeprom_checksum += c;
        pos++;
        value++;
    } while (--size);
}