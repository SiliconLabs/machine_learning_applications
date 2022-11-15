import sys
import serial
import serial.tools.list_ports
import numpy as np
import matplotlib.pyplot as plt
import struct
import imageio
import argparse
import pathlib
import celluloid
types = ["UINT8", "FLOAT"]
type_sizes = [1, 4]
def data_to_value(data, type):
    if types[type] == "UINT8":
        return struct.unpack('B'*(len(data)//type_sizes[type]), data)
    elif types[type] == "FLOAT":
        return struct.unpack('f'*(len(data)//type_sizes[type]), data)
def to_image(data, w, h, type):
    v = data_to_value(data, type)
    img = np.array(v).reshape(h, w)
    return img

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("--save", type=str)
    args = parser.parse_args()
    if args.save:
        pathlib.Path(args.save).mkdir(exist_ok=True)

    fig,ax = plt.subplots(1,1)
    camera = celluloid.Camera(fig)
    im = None
    expected_image_dims = None
    type = 0
    title = ""
    counter = 0
    ports = list(serial.tools.list_ports.comports())
    
    for i, p in enumerate(ports):
        print(f"{i}: {p}")
        print(p.name)
    port_id = input("Select port to connect to: ")
    port_id = int(port_id)

    ser = serial.Serial(port="/dev/"+ports[port_id].name)
    try:
        while(True):
            if(expected_image_dims == None):
                line=ser.readline()
                if(len(line) == 0):
                    continue
                try:
                    utf_line = line.decode("utf-8").strip()
                    if(len(utf_line) > 0 and utf_line[0] == "$"):
                        info = utf_line[1:].split(",")
                        width = int(info[0])
                        height = int(info[1])
                        type = int(info[2])
                        title = str(info[3])
                        expected_image_dims=(width, height)
                        continue
                    else:
                        print(utf_line)
                except UnicodeDecodeError:
                    pass
            else:
                to_read = expected_image_dims[0]*expected_image_dims[1]*type_sizes[type]
                data = ser.read(size=to_read)
                img = to_image(data, expected_image_dims[0], expected_image_dims[1], type)
                if(args.save):
                    np.save(f"data/{args.save}/{counter}.npy", img)
                counter+=1
                ax.imshow(img, cmap="gray")
                fig.suptitle(title)
                fig.canvas.draw()
                plt.pause(0.01)
                camera.snap()
                expected_image_dims = None
    except KeyboardInterrupt:
        animation = camera.animate(1000)
        print(animation)
        animation.save("animation.gif")
        sys.exit(0)