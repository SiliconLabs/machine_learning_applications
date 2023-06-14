import display_serial_core
import argparse
import serial
import serial.tools.list_ports
import os
if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    display_serial_core.add_args(parser)
    args = parser.parse_args()
    ports = list(serial.tools.list_ports.comports())
    for i, p in enumerate(ports):
        print(f"{i}: {p}")
        print(p.name)
    port_id = input("Select port to connect to: ")
    port_id = int(port_id)
    prefix = ""
    if(os.name == "posix"):
        prefix = "/dev/"
    ser = serial.Serial(port=prefix + ports[port_id].name)

    display_serial_core.display_serial(ser, args)
