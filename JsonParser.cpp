#include "JsonParser.h"

JsonParser::JsonParser(Stream& stream, int bps) : _stream(stream), _bps(bps) {
    // 내부 변수 초기화
    temperature = 0;
    illuminance = 0;
    waterLevel = 0;
    pumpStatus = "OFF";
    ledStatus = "OFF";
    heaterStatus = "OFF";
    read_status = 0;
}

void JsonParser::begin() {
    if (&_stream == &Serial) { // _stream이 Serial 객체인지 확인
        Serial.begin(_bps); // Serial.begin() 호출
    } //else {
      //  _stream.begin(_bps); // 시리얼 통신 시작
    //}
}

int JsonParser::update() {
    if (_stream.available() > 0) {
        digitalWrite(13, HIGH);
        String jsonData = _stream.readStringUntil('\n'); // JSON 데이터 수신
        //_stream.println(jsonData);
        parseJson(jsonData); // JSON 파싱 함수 호출
        digitalWrite(13,LOW);
        return true;
    }
    return false;
}

void JsonParser::myparseJson(String jsonData) {
    // JSON 데이터에서 중괄호 제거
    jsonData.trim();
    if (jsonData.startsWith("{") && jsonData.endsWith("}")) {
        jsonData = jsonData.substring(1, jsonData.length() - 1);
    } else {
        _stream.print(F("JSON 형식 오류: "));
        _stream.println(jsonData);
        return;
    }

        
        
    _stream.println(jsonData);
    return;
        
    // 키-값 쌍 분리
    int start = 0;
    while (start < jsonData.length()) {
        int colonIndex = jsonData.indexOf(':', start);
        if (colonIndex == -1) break; // 더 이상 ':'가 없으면 종료

        String key = jsonData.substring(start, colonIndex);
        key.trim(); // 공백 제거
        key.replace("\"", ""); // 따옴표 제거

        int valueStart = jsonData.indexOf(',', colonIndex);
        if (valueStart == -1) valueStart = jsonData.length(); // 마지막 키-값 쌍 처리

        String value = jsonData.substring(colonIndex + 1, valueStart);
        value.trim(); // 공백 제거
        value.replace("\"", ""); // 따옴표 제거

        _stream.print(key);
        _stream.print(": ");
        _stream.println(value);

        start = valueStart + 1; // 다음 키-값 쌍으로 이동
    }
}

void JsonParser::parseJson(String jsonData) {
    StaticJsonDocument<200> doc; // JSON 문서 생성 (메모리 크기 조정 가능)

    DeserializationError error = deserializeJson(doc, jsonData); // JSON 파싱

    if (error) {
        _stream.print(F("JSON 파싱 오류: "));
        _stream.println(error.f_str()); // 오류 메시지 출력
        return;
    }
    for (auto pair : doc.as<JsonObject>()) { // 수정된 부분
        String key = pair.key().c_str(); // 수정된 부분
        String value = pair.value().as<String>(); // 수정된 부분
        _stream.print(key);
        _stream.print(": ");
        _stream.println(value);
    }
    // JSON 데이터에서 값 추출 및 내부 변수에 대입
    if (doc.containsKey("temperature")) {
        temperature = doc["temperature"];
    }
    if (doc.containsKey("illuminance")) {
        illuminance = doc["illuminance"];
    }
    if (doc.containsKey("waterLevel")) {
        waterLevel = doc["waterLevel"];
    }
    if (doc.containsKey("pumpStatus")) {
        pumpStatus = doc["pumpStatus"].as<String>();
    }
    if (doc.containsKey("ledStatus")) {
        ledStatus = doc["ledStatus"].as<String>();
    }
    if (doc.containsKey("heaterStatus")) {
        heaterStatus = doc["heaterStatus"].as<String>();
    }
    if (doc.containsKey("read_status")) {
        read_status = doc["read_status"];
    }    
}

void JsonParser::printParseJson(){
    // 수신된 값 출력
    _stream.print("온도: ");
    _stream.println(temperature);
    _stream.print("조도: ");
    _stream.println(illuminance);
    _stream.print("수위: ");
    _stream.println(waterLevel);
    _stream.print("펌프 상태: ");
    _stream.println(pumpStatus);
    _stream.print("LED 상태: ");
    _stream.println(ledStatus);
    _stream.print("가열기 상태: ");
    _stream.println(heaterStatus);
}

// getter 메서드 구현
int JsonParser::getTemperature() const {
    return temperature;
}

int JsonParser::getilluminance() const {
    return illuminance;
}

int JsonParser::getWaterLevel() const {
    return waterLevel;
}

String JsonParser::getPumpStatus() const {
    return pumpStatus;
}

String JsonParser::getLedStatus() const {
    return ledStatus;
}

String JsonParser::getHeaterStatus() const {
    return heaterStatus;
}

int JsonParser::getread_status() const {
    return read_status;
}

void JsonParser::putread_status() {
    read_status = 0;
}
void JsonParser::putread_status(int read_status) {
    read_status = read_status;
}
void JsonParser::sendJson(const String& jsonData) {
    _stream.println(jsonData); // JSON 데이터를 스트림으로 전송
}
void JsonParser::sendJson(const char* jsonData) {
    _stream.println(jsonData); // JSON 데이터를 스트림으로 전송
}

void JsonParser::handleReadStatus(int temperature, int illuminance, int waterLevel, const String& pumpStatus, const String& ledStatus, const String& heaterStatus) {
    // JSON 형식으로 응답 생성
    String jsonResponse = String("{\"temperature\": ") + temperature +
                          String(", \"illuminance\": ") + illuminance +
                          String(", \"waterLevel\": ") + waterLevel +
                          String(", \"pumpStatus\": \"") + pumpStatus +
                          String("\", \"ledStatus\": \"") + ledStatus +
                          String("\", \"heaterStatus\": \"") + heaterStatus + 
                          String("\"}");
    
    _stream.println(jsonResponse); // 시리얼로 응답 전송
}


bool JsonParser::isReadStatusReceived() {
    bool receivedStatus = false;
    // JSON 데이터에서 read_status 키 확인
    if (getread_status()) {
      //Serial.print("----");
        putread_status();
        receivedStatus = true; // read_status가 존재하면 true 반환
    } //else Serial.print("!!!");
    return receivedStatus; // 예시: 수신된 상태가 true일 경우
}

