# watchdog.py
import os
import time
from creatememory import create_shared_memory, read_shared_memory_data, save_individual_data  

def get_able_pro(memory):
    """공유 메모리에서 able_pro 값을 가져오거나 기본값을 설정합니다."""
    try:
        data = read_shared_memory_data(memory)  # able_pro 값을 공유 메모리에서 읽기
        if data is None:
            return 1
        return data["able_pro"]  # able_pro 값을 반환
    except Exception as e:
        #print(f"Error reading able_pro: {e}")
        # 기본값 설정   
        save_individual_data(memory, "able_pro", 1)  # 기본값 저장
        return 1  # 기본값 반환

# 프로세스를 실행하는 함수
def start_process(process_name):
    if is_process_running(process_name):  # 프로세스가 이미 실행 중인지 확인
        #print(f"{process_name} is already running.")    
        pass
    try:
        if process_name == "control.py":
            os.system("python3 control.py &")
            #print(f"control.py started")    
        elif process_name == "communication.py":
            os.system("python3 communication.py &")
            #print(f"communication.py started")
    except Exception as e:
        #print(f"Error starting {process_name}: {e}")
        pass

# 프로세스를 중지하는 함수
def stop_process(process_name):
    os.system(f"pkill -f {process_name}")

def monitor_processes(memory):
    while True:
        able_pro = get_able_pro(memory)
        #print(f"able_pro: {able_pro} {ascii(able_pro)}")
        if able_pro == 1:
            if not is_process_running("control.py"):
                #print("Starting control.py...")
                start_process("control.py")
            else:
                #print("control.py is already running.")
                pass
            if not is_process_running("communication.py"):
                #print("Starting communication.py...")
                start_process("communication.py")
            else:
                #print("communication.py is already running.")
                pass
        elif able_pro == 0:
            #print("Stopping processes.............................")
            stop_process("control.py")
            stop_process("communication.py")
            if not is_process_running("control.py") and not is_process_running("communication.py"):
                #print("control.py and communication.py are not running.")
                exit()
        time.sleep(5)  # 5초마다 체크

def is_process_running(process_name):
    try:
        # ps 명령어를 사용하여 프로세스 목록을 가져오고 필터링
        output = os.popen(f"ps aux | grep {process_name} | grep -v grep").read().strip()
        running = len(output) > 0  # 출력이 있으면 프로세스가 실행 중임
        if not running:
            #print(f"{process_name} is not running.")
            pass
    except Exception as e:
        #print(f"Error checking process: {e}")
        running = False  # 오류 발생 시 프로세스가 실행 중이 아님

    return running  # 프로세스가 실행 중인지 여부 반환

if __name__ == "__main__":
    memory = create_shared_memory()
    monitor_processes(memory)