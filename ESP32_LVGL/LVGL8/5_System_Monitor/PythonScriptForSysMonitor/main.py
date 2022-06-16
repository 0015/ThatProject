'''
Make your own System Monitor with ESP32 + LVGL 8
For More Information: https://youtu.be/gliwNg25fLE
Created by Eric N. (ThatProject)
'''
from serial import SerialTimeoutException, SerialException

from RepeatedTimer import RepeatedTimer
import sys
import psutil
import datetime
import json
import time
import argparse
import serial

serial_baudrate = 115200
ser = None
update_interval = 2

def write_to_serial(str_json):
    try:
        ser.write(str_json.encode())
    except SerialTimeoutException:
        print('[Error] Timeout when writing to the serial.')
    except SerialException:
        print('[Error] An unexpected exception occurred while writing to the serial.')

def process_list(delay=1):
    proccesses = list(psutil.process_iter())
    procs = []
    try:
        for proc in proccesses:
            proc.cpu_percent(None)
        sys.stdout.flush()
        time.sleep(delay)

        for proc in proccesses:
            percent = proc.cpu_percent(None)
            if percent:
                procs.append((percent, proc.name()))
    except ProcessLookupError:
        pass
    except psutil.NoSuchProcess:
        pass

    res = sorted(procs, key=lambda x: x[0], reverse=True)[:5]
    return res

def get_system_info():
    data = {}
    # Get System LocalTime
    ts = time.time()
    utc_offset = (datetime.datetime.fromtimestamp(ts) -
                  datetime.datetime.utcfromtimestamp(ts)).total_seconds()
    data['utc_offset'] = int(utc_offset)

    presentDate = datetime.datetime.now()
    unix_timestamp = datetime.datetime.timestamp(presentDate)
    data['local_time'] = int(unix_timestamp)

    # CPU Usage
    cpu_percent_cores = psutil.cpu_percent(interval=2, percpu=True)
    avg = sum(cpu_percent_cores) / len(cpu_percent_cores)
    data['cpu_percent_total'] = round(avg, 2)

    # MEM Usage
    data['mem_percent'] = psutil.virtual_memory().percent

    # Battery Info
    battery = psutil.sensors_battery()
    data['battery_percent'] = battery.percent if battery.percent is not None else 100
    data['power_plugged'] = battery.power_plugged

    # Top 5 CPU Usage Processes
    data['cpu_top5_process'] = ['%.1f%% %s' % x for x in process_list()]
    write_to_serial(json.dumps(data))

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='<Need to set your Serial Port, BaudRate and Interval to fetch system info.> (sudo permission required on Mac)', usage='python main.py --port /dev/cu.SLAB_USBtoUART')
    parser.add_argument('-p', '--port', help='Set the Connected USB Port')
    parser.add_argument('-b', '--baud', help='Set Serial Baud Rate(default=115200)', type=int, default=115200)
    parser.add_argument('-i', '--interval', help='Set Update Interval in Second(default=2)', type=int, default=2)

    args = parser.parse_args()

    if args.port is None:
        print('usage: sudo python main.py --port /dev/cu.SLAB_USBtoUART')
        sys.exit()

    if args.baud is not None:
        serial_baudrate = args.baud

    if args.interval is not None and args.interval > 1:
        update_interval = args.interval

    ser = serial.Serial(args.port, serial_baudrate)
    print(ser)

    if ser is not None and ser.is_open:
        get_system_info()
        rt = RepeatedTimer(update_interval, get_system_info)
    else:
        print('[Error] Check your Serial Port.')
        sys.exit()