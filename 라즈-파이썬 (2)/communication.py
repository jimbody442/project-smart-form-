import json
import serial
import os
import sys
import fcntl
import time
import threading
# 기존 라이브러리 임포트
from creatememory import open_shared_memory, read_shared_memory_data, write_shared_memory_data, print_all_data

# 프로그램이 이미 실행 중인지 확인하는 함수
def check_single_instance():
    pid = str(os.getpid())
    lock_file = '/tmp/communication.lock'
    try:
        lock = open(lock_file, 'w')
        fcntl.lockf(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        return lock
    except IOError:
        #print("프로그램이 이미 실행 중입니다.")
        sys.exit()


def main(message=None):
    if message != None:
        index_print = 1
    else:
        index_print = 0
                
    # 단일 인스턴스 확인
    lock = check_single_instance()

    # 시리얼 포트 설정
    ser = serial.Serial('/dev/ttyACM0', 9600)  # 포트와 보드레이트 설정

    # 공유 메모리 생성
    memory = open_shared_memory()
    time.sleep(5)

    while True:
        time.sleep(1)
        # 공유 메모리 접근
        current_data = read_shared_memory_data(memory)
        if current_data is None:
            #print("공유 메모리에서 데이터를 읽을 수 없습니다.")
            continue

        if current_data.get("read_status") == 1:
            if index_print == 1:
                print(f"read_status: 상태값 요구")
            json_data = json.dumps({"read_status": 1})
            ser.write(json_data.encode('ascii'))
            if index_print == 1:
                print(f"전송된 데이터: {json_data}")
            current_data['read_status'] = 0
            write_shared_memory_data(memory, current_data)
            data = read_shared_memory_data(memory)
            while data['read_status'] == 1:
                data['read_status'] = 0
                time.sleep(1)
                write_shared_memory_data(memory, data)
                data = read_shared_memory_data(memory)
                if index_print == 1:
                    print(f"data['read_status']: {data['read_status']}")
        elif current_data.get("send_order") == 1:
            if index_print == 1:
                print(f"send_order: 명령 전송")
            filtered_data = {}
            if current_data["pumpOrder"] != current_data["pumpStatus"]:
                filtered_data["pumpStatus"] = current_data["pumpOrder"]
            if current_data["ledOrder"] != current_data["ledStatus"]:
                filtered_data["ledStatus"] = current_data["ledOrder"]
            if current_data["heaterOrder"] != current_data["heaterStatus"]:
                filtered_data["heaterStatus"] = current_data["heaterOrder"] 
            if filtered_data != {}: # 데이터가 있으면 전송
                if index_print == 1:
                    print(f"전송할 데이터: {filtered_data}")
                # 아스키 코드 형식으로 전체 JSON 문자열을 한 번에 전송
                json_data = json.dumps(filtered_data)
                ser.write(json_data.encode('ascii'))
                if index_print == 1:
                    print(f"전송된 데이터: {json_data}")
                    if json_data.find('pumpStatus') != -1:
                        print(" pumpStatus order ========================================================")
            current_data['send_order'] = 0    
            write_shared_memory_data(memory, current_data)



        currentMillis = time.time()
        while currentMillis + 1 > time.time():
            # 시리얼 포트에서 데이터 읽기
            if ser.in_waiting > 0:
                #print(f"시리얼 포트에서 데이터를 받았습니다.")   
                incoming_data = ser.readline()
                if index_print == 1:
                    print(f"arduino로 부터 값을 받았습니다.{incoming_data}")   
                try:
                    incoming_data = incoming_data.decode('utf-8').strip()
                except UnicodeDecodeError:
                    #print("유니코드 디코딩 오류")
                    continue
                
                # JSON 데이터 파싱
                try:
                    # 수신된 데이터가 유효한 JSON인지 확인
                    json_objects = []
                    start = 0
                    brace_count = 0
                    
                    for i, char in enumerate(incoming_data):
                        if char == '{':
                            if brace_count == 0:
                                start = i  # 새로운 JSON 객체의 시작 인덱스
                            brace_count += 1
                        elif char == '}':
                            brace_count -= 1
                            if brace_count == 0:
                                json_objects.append(incoming_data[start:i + 1])  # JSON 객체 추가

                    # 각 JSON 객체 파싱
                    for json_str in json_objects:
                        try:
                            parsed_data = json.loads(json_str)  # 수신된 데이터 파싱
                            for field, value in parsed_data.items():
                                #print(f"field: {field}, value: {value}")
                                if field != "read_status" and field != "send_order":
                                    current_data[field] = value  # 각 필드를 current_data에 저장
                            write_shared_memory_data(memory, current_data)  # 공유 메모리에 데이터 쓰기
                            #print(f"current_data saved: {current_data}")
                            #print_all_data(memory)   
                        except json.JSONDecodeError as e:
                            #print(f"경고: 유효하지 않은 JSON 데이터입니다. 오류: {e}")  # JSON 파싱 오류 처리
                            pass

                except Exception as e:
                    #print(f"예외 발생: {e}")  # 일반 예외 처리
                    pass

            #time.sleep(1)

if __name__ == "__main__":
    if len(sys.argv) > 1:
        message = sys.argv[1]
    else:
        message = None  
    main(message)