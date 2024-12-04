#ifndef CHECKTIMER_H
#define CHECKTIMER_H

#include <Arduino.h>

class CheckTimer {
  public:
    CheckTimer(unsigned long interval);
    CheckTimer(unsigned int interval); // 생성자
    //~CheckTimer(); //소멸자
    void update();
    bool isInverval();
    virtual String status();
    void checkSensors(); // 센서 체크 메서드 추가
    bool isupdate();
  protected:
    unsigned long previousMillis;
    unsigned long interval;
};

#endif