# control.py
import json
import pymysql
import os
import sys
import fcntl
import time
from creatememory import open_shared_memory, read_shared_memory_data, write_shared_memory_data, print_all_data

DB_CONFIG = {
    'host': 'localhost',
    'database': 'smartfarm',
    'user': 'root',
    'password': 'password'
}

def db_connect():
    """데이터베이스에 연결하는 함수"""
    return pymysql.connect(**DB_CONFIG)

def check_single_instance():
    pid = str(os.getpid())
    lock_file = '/tmp/control.lock'
    try:
        lock = open(lock_file, 'w')
        fcntl.lockf(lock, fcntl.LOCK_EX | fcntl.LOCK_NB)
        return lock
    except IOError:
        #print("프로그램이 이미 실행 중입니다.")
        sys.exit()


def connect_to_database():
    """MariaDB에 연결하고 정상적인지 확인합니다."""
    try:
        connection = pymysql.connect( 
            host='localhost',
            database='smartfarm',
            user='root',
            password='password'
        )
        return connection
    except pymysql.MySQLError as err:  # pymysql 오류로 변경
        #print(f"데이터베이스 연결 오류: {err}")
        sys.exit()

def get_db_setting():
    conn = db_connect()  # 데이터베이스 연결
    db_data = {'temperature': 0, 'illuminance': 0, 'waterLevel': 0}
    try:
        cur = conn.cursor()  # 커서 생성
        cur.execute("SELECT temperature, illuminance, waterLevel, pumpstatus, ledstatus, heaterstatus, autodrive, pumporder, ledorder, heaterorder FROM setting")
        result = cur.fetchone()  # 결과 가져오기
        db_data['temperature'] = result[0]  # 첫 번째 결과 가져오기
        db_data['illuminance'] = result[1]  # 첫 번째 결과 가져오기
        db_data['waterLevel'] = result[2]  # 첫 번째 결과 가져오기
        db_data['pumpstatus'] = result[3]  # 첫 번째 결과 가져오기
        db_data['ledstatus'] = result[4]  # 첫 번째 결과 가져오기
        db_data['heaterstatus'] = result[5]  # 첫 번째 결과 가져오기
        db_data['autodrive'] = result[6]  # 첫 번째 결과 가져오기
        db_data['pumporder'] = result[7]  # 첫 번째 결과 가져오기
        db_data['ledorder'] = result[8]  # 첫 번째 결과 가져오기
        db_data['heaterorder'] = result[9]  # 첫 번째 결과 가져오기
        return db_data
    finally:
        cur.close()  # 커서 닫기
        conn.close()  # 연결 닫기

def update_db_setting(data):
    conn = db_connect()
    cur = conn.cursor()
    cur.execute("UPDATE setting SET temperature = %s, illuminance = %s, waterLevel = %s, pumpstatus = %s, ledstatus = %s, heaterstatus = %s",
                (data['temperature'], data['illuminance'], data['waterLevel'], data['pumpStatus'], data['ledStatus'], data['heaterStatus']))
    conn.commit()  
    cur.close()
    conn.close()

def put_db_history(data):
    conn = db_connect()
    cur = conn.cursor()
    cur.execute("INSERT INTO history (temperature, illuminance, waterLevel, heaterOrder, ledOrder, pumpOrder) VALUES (%s, %s, %s, %s, %s, %s)",
                (data['temperature'], data['illuminance'], data['waterLevel'], data['heaterOrder'], data['ledOrder'], data['pumpOrder']))
    conn.commit()  
    cur.close()
    conn.close()

def main_loop(connection, message=None):
    if message != None:
        index_print = 1
    else:
        index_print = 0
    """주요 로직을 실행하는 무한 루프입니다."""
    memory = open_shared_memory()

    while True:
        shared_data = read_shared_memory_data(memory)
        if shared_data is None:
            continue
        if shared_data["able_pro"] == 0:
            #print("프로그램 종료.")
            break

        if shared_data["read_status"] == 0 and shared_data["send_order"] == 0:
            shared_data["read_status"] = 1
            write_shared_memory_data(memory, shared_data)
            if index_print == 1:
                print(f"read_status: {shared_data['read_status']}")
            time.sleep(1) # 1초 대기

        # 보낼 데이터가 있으면 대기
        if shared_data["send_order"] == 1:
            continue

        database_data = get_db_setting() # setting 테이블에서 데이터 가져오기

        # 자동 제어 활성화 시
        if database_data['autodrive'] == "ON":
            if index_print == 1:
                print("자동 제어 활성화")
            # 온도 비교 및 업데이트
            if database_data['temperature'] > shared_data['temperature'] and shared_data['heaterStatus'] == "OFF":
                shared_data['heaterOrder'] = "ON"
                shared_data['send_order'] = 1
            elif shared_data['heaterStatus'] == "ON" and database_data['temperature'] <= shared_data['temperature']:
                shared_data['heaterOrder'] = "OFF"
                shared_data['send_order'] = 1

            # 조도 비교 및 업데이트
            if database_data['illuminance'] > shared_data['illuminance'] and shared_data['ledStatus'] == "OFF":
                shared_data['ledOrder'] = "ON"
                shared_data['send_order'] = 1
            elif shared_data['ledStatus'] == "ON" and database_data['illuminance'] <= shared_data['illuminance']:
                shared_data['ledOrder'] = "OFF"
                shared_data['send_order'] = 1

            # 수위 비교 및 업데이트
            if database_data['waterLevel'] > shared_data['waterLevel'] and shared_data['pumpStatus'] == "OFF":
                shared_data['pumpOrder'] = "ON"
                shared_data['send_order'] = 1
            elif shared_data['pumpStatus'] == "ON" and database_data['waterLevel'] <= shared_data['waterLevel']:
                shared_data['pumpOrder'] = "OFF"
                shared_data['send_order'] = 1

            write_shared_memory_data(memory, shared_data) 
        else: # 자동 제어 비활성화 시
            if index_print == 1:
                print("자동 제어 비활성화")
            if shared_data['heaterStatus'] == "OFF" and database_data['heaterorder'] == "ON":                
                shared_data['heaterOrder'] = "ON"
                shared_data['send_order'] = 1
            elif shared_data['heaterStatus'] == "ON" and database_data['heaterorder'] == "OFF":
                shared_data['heaterOrder'] = "OFF"
                shared_data['send_order'] = 1
            if shared_data['ledStatus'] == "OFF" and database_data['ledorder'] == "ON":
                shared_data['ledOrder'] = "ON"
                shared_data['send_order'] = 1
            elif shared_data['ledStatus'] == "ON" and database_data['ledorder'] == "OFF":
                shared_data['ledOrder'] = "OFF"
                shared_data['send_order'] = 1
            if shared_data['pumpStatus'] == "OFF" and database_data['pumporder'] == "ON":
                shared_data['pumpOrder'] = "ON"
                shared_data['send_order'] = 1
            elif shared_data['pumpStatus'] == "ON" and database_data['pumporder'] == "OFF":
                shared_data['pumpOrder'] = "OFF"
                shared_data['send_order'] = 1
            write_shared_memory_data(memory, shared_data) 
            
        # 데이터베이스에 이력저장
        if shared_data['send_order'] == 1:
            if index_print == 1:
                print(f"shared_data: {shared_data}")
            data_to_db = {"temperature": shared_data['temperature'], "illuminance": shared_data['illuminance'], "waterLevel": shared_data['waterLevel'], "heaterOrder": shared_data['heaterOrder'], "ledOrder": shared_data['ledOrder'], "pumpOrder": shared_data['pumpOrder']}
            put_db_history(data_to_db)    

        # 데이터 베이스에 상태값 변경확인 및 update 처리
        if shared_data['temperature'] != database_data['temperature'] or shared_data['illuminance'] != database_data['illuminance'] or shared_data['waterLevel'] != database_data['waterLevel'] or \
            shared_data['pumpStatus'] != database_data['pumpstatus'] or shared_data['ledStatus'] != database_data['ledstatus'] or shared_data['heaterStatus'] != database_data['heaterstatus']:
            update_db_setting(shared_data)

        time.sleep(6)  # 5초 대기

if __name__ == "__main__":
    if len(sys.argv) > 1:
        message = sys.argv[1]
    else:
        message = None  
    check_single_instance()
    open_shared_memory() 
    db_connection = connect_to_database()
    main_loop(db_connection, message)
