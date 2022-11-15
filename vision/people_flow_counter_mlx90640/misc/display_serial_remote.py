import display_serial_core
import argparse
import subprocess
import time
import serial
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--save", action="store_true")
    parser.add_argument("--address", type=str)
    args = parser.parse_args()
    
    proc = subprocess.Popen(f"sudo socat PTY,raw,ospeed=912600,ispeed=912600,echo=0,link=vusb TCP:{args.address}".split(" "))
    time.sleep(1)
    ser = serial.Serial(port="vusb", baudrate=115200)
    display_serial_core.display_serial(ser, args.save)
    proc.terminate()