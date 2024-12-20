import sysv_ipc
import json

global key
key = 12345678  # 키 값
    
def creatememory():
    """공유 메모리를 생성합니다."""
    data = {
        "temperature": 0,
        "illuminance": 0,
        "waterLevel": 0,
        "pumpStatus": "OFF",
        "ledStatus": "OFF",
        "heaterStatus": "OFF",
        "able_pro": 1,
        "send_order": 0,
        "read_status": 0,
        "pumpOrder": "OFF",
        "ledOrder": "OFF",
        "heaterOrder": "OFF",        
    }
    # 공유 메모리 생성 및 초기 데이터 저장
    memory = sysv_ipc.SharedMemory(key, sysv_ipc.IPC_CREAT, 0o600, 1024)
    memory.write(json.dumps(data).encode('utf-8'))  # 초기 데이터 저장

def save_individual_data(memory, field, value):
    current_data = read_shared_memory_data(memory)
    if current_data is None:
        return

    if field in current_data:
        current_data[field] = value  # 필드 업데이트
    else:
        #print(f"경고: '{field}' 필드는 현재 데이터에 존재하지 않습니다.")  # 필드가 없을 경우 경고 메시지 출력
        pass
    
    # 업데이트된 데이터 저장
    updated_data = json.dumps(current_data)
    memory.write(updated_data.encode('utf-8')) 


def create_shared_memory():
    """공유 메모리를 생성합니다."""
    try:
        # 공유 메모리 생성 (크기: 1024 바이트)
        memory = creatememory()
        #print("공유 메모리 생성 성공.")
    except sysv_ipc.ExistentialError:
        #print("공유 메모리가 이미 존재합니다.")
        memory = open_shared_memory()
    finally:
        return memory

def open_shared_memory():
    """공유 메모리를 엽니다."""
    memory = sysv_ipc.SharedMemory(key)
    return memory

def delete_shared_memory():
    """공유 메모리를 삭제합니다."""
    try:
        memory = sysv_ipc.SharedMemory(key)
        memory.remove()  # 공유 메모리 삭제
        #print("공유 메모리 삭제 성공.")
    except sysv_ipc.ExistentialError:
        #print("공유 메모리가 존재하지 않습니다.")
        pass

def modify_shared_memory(message):
    message = message.decode('utf-8')
    #print(f"type(message): {type(message)}")
    try:
        if message is not None:
            index = message.find('}')  # 첫 번째 '}'의 인덱스 찾기
        else:
            return None
        modified_message = message[:index + 1]  # 수정된 메시지 생성 (널 문자 제거)
        return modified_message
    except ValueError:
        print("경고: '}' 문자가 메시지에 존재하지 않습니다.")
        return message 

def modify_json_data(message):
    current_data = json.loads(message) 
    return current_data

def read_shared_memory_data(memory):
    message = memory.read(1024).strip(b'\x00')  # 읽은 데이터에서 null 바이트 제거    
    message = message.decode('utf-8')
    if message is not None:
        index = message.find('}')  # 첫 번째 '}'의 인덱스 찾기
        modified_message = message[:index + 1]  # 수정된 메시지 생성 (널 문자 제거)
    #print(f"modified_message: {modified_message}")
    try:
        current_data = json.loads(modified_message) 
    except json.JSONDecodeError as e:
        #print(f"경고: 유효하지 않은 JSON 데이터입니다. 오류: {e}")  # JSON 파싱 오류 처리
        return None
    return current_data

def write_shared_memory_data(memory, data):
    while '}}' in data:
        data = data.replace('}}', '}')
    #print(f"data: {data}")
    memory.write(b'\x00' * 1024) 
    memory.write(json.dumps(data).encode('utf-8')) 


def print_all_data(memory):
    message = memory.read(1024).strip(b'\x00')  # 읽은 데이터에서 null 바이트 제거
    #print(f"message: {message}")
    #print(f"len(message): {len(message)}")


if __name__ == "__main__":
    delete_shared_memory()
    creatememory()
