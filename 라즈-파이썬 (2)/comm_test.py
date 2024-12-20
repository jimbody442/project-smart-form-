import time
#from writesharedmemory import save_data
#from readsharedmemory import read_data, print_all_data
from creatememory import open_shared_memory, read_shared_memory_data, write_shared_memory_data, print_all_data

def print_shared_memory_data(memory):
    """공유 메모리에서 특정 필드 값을 출력합니다."""
    data = read_shared_memory_data(memory)
    if data:
        print("Temperature:", data.get("temperature"))
        print("Illuminance:", data.get("illuminance"))
        print("Water Level:", data.get("waterLevel"))
        print("Pump Status:", data.get("pumpStatus"))
        print("LED Status:", data.get("ledStatus"))
        print("Heater Status:", data.get("heaterStatus"))
    else:
        print("공유 메모리에서 데이터를 읽을 수 없습니다.")

def set_read_status(memory, value):
    """read_status 값을 설정합니다."""
    current_data = read_shared_memory_data(memory)
    if current_data is None:
        return
    if current_data:
        current_data['read_status'] = value
        write_shared_memory_data(memory, current_data )
        print(f"read_status가 {value}로 설정되었습니다.")
    else:
        print("공유 메모리에서 데이터를 읽을 수 없습니다.")

def set_send_order(memory, value):
    """send_order 값을 설정합니다."""
    current_data = read_shared_memory_data(memory)
    if current_data is None:
        return
    if current_data:
        current_data['send_order'] = value
        write_shared_memory_data(memory, current_data )
        print(f"send_order가 {value}로 설정되었습니다.")
    else:
        print("공유 메모리에서 데이터를 읽을 수 없습니다.")

def set_able_pro(memory, value):
    """send_order 값을 설정합니다."""
    current_data = read_shared_memory_data(memory)
    if current_data is None:
        return
    if current_data:
        current_data['able_pro'] = value
        write_shared_memory_data(memory, current_data )
        print(f"able_pro가 {value}로 설정되었습니다.")
    else:
        print("공유 메모리에서 데이터를 읽을 수 없습니다.")

def update_orders(memory, pumpOrder, ledOrder, heaterOrder):
    """pumpOrder, ledOrder, heaterOrder 값을 설정합니다."""
    current_data = read_shared_memory_data(memory)
    if current_data is None:
        return
    if current_data:
        current_data['pumpOrder'] = pumpOrder
        current_data['ledOrder'] = ledOrder
        current_data['heaterOrder'] = heaterOrder
        write_shared_memory_data(memory, current_data )
        print("주문 값이 업데이트되었습니다.")
    else:
        print("공유 메모리에서 데이터를 읽을 수 없습니다.")

def print_all_shared_memory_data(memory):
    """전체 공유 메모리 데이터를 출력합니다."""
    data = read_shared_memory_data(memory)
    if data is None:
        return
    if data:
        print("전체 공유 메모리 데이터:", data)
    else:
        print("공유 메모리에서 데이터를 읽을 수 없습니다.")

def main():
    """메인 함수로 사용자 입력을 받아 각 기능을 실행합니다."""
    memory = open_shared_memory()
    while True:
        print("\n1. read_status")
        print("2. send_order")
        print("3. all_status")
        print("4. all_data")
        print("5. able_pro")
        choice = input("실행할 기능의 번호를 입력하세요 (1, 2, 3, 4 또는 'q'로 종료): ")

        if choice == '1':
            #read_status_value = int(input("read_status 값을 입력하세요 (0 또는 1): "))
            #set_read_status(read_status_value)
            set_read_status(memory, 1)
            time.sleep(1)
            print_shared_memory_data(memory)
        
        elif choice == '2':
            pumpOrder = input("pumpOrder 값을 입력하세요 (ON 또는 OFF): ")
            ledOrder = input("ledOrder 값을 입력하세요 (ON 또는 OFF): ")
            heaterOrder = input("heaterOrder 값을 입력하세요 (ON 또는 OFF): ")
            print(f"pumpOrder: {pumpOrder}, ledOrder: {ledOrder}, heaterOrder: {heaterOrder}")
            update_orders(memory, pumpOrder, ledOrder, heaterOrder)
            set_send_order(memory, 1)
        
        elif choice == '3':
            print_all_shared_memory_data(memory)
        
        elif choice == '4':
            print_all_data(memory)
        
        elif choice == '5':
            able_pro = input("able_pro 값을 입력하세요 (0/1): ")
            set_able_pro(memory, int(able_pro))

        elif choice.lower() == 'q':
            print("프로그램을 종료합니다.")
            break
        
        
        else:
            print("잘못된 입력입니다. 다시 시도하세요.")

if __name__ == "__main__":
    main()

