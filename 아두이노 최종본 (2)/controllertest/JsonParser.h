#ifndef JSONPARSER_H
#define JSONPARSER_H

#include <Arduino.h>
#include <ArduinoJson.h>

class JsonParser {
public:
    JsonParser(Stream& stream, int bps); // 생성자
    void begin(); // 초기화 메서드
    int update(); // 업데이트 메서드
    void printParseJson(); // 수신된 값 출력 메서드
    // 내부 변수에 접근하기 위한 getter 메서드
    int getTemperature() const;
    int getilluminance() const;
    int getWaterLevel() const;
    String getPumpStatus() const;
    String getLedStatus() const;
    String getHeaterStatus() const;
    int getread_status() const;
    void putread_status();
    void putread_status(int read_status);
    void sendJson(const String& jsonData);
    void sendJson(const char* jsonData);
    void handleReadStatus(int temperature, int illuminance, int waterLevel, const String& pumpStatus, const String& ledStatus, const String& heaterStatus);
    bool isReadStatusReceived(); // 상태 수신 확인 메서드 추가

private:
    Stream& _stream; // 시리얼 스트림
    int _bps; // 통신 속도

    // 내부 변수
    int temperature;
    int illuminance;
    int waterLevel;
    String pumpStatus;
    String ledStatus;
    String heaterStatus;
    int read_status;

    void parseJson(String jsonData); // JSON 파싱 메서드
    void myparseJson(String jsonData); // JSON 파싱 메서드
};

#endif
/*
{
    "temperature": 25,
    "illuminance": 300,
    "waterLevel": 75,
    "pumpStatus": "ON",
    "ledStatus": "OFF",
    "heaterStatus": "ON"
}
*/