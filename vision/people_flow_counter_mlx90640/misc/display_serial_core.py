import numpy as np
import matplotlib.pyplot as plt
import matplotlib.patches as patches
import struct
import imageio
import time
import io
import time
import uuid
types = ["UINT8", "FLOAT"]
type_sizes = [1, 4]
def add_args(parser):
    parser.add_argument(
        "--save", 
        action="store_true", 
        help="If this is set, the program will record the frames into a .csv file. This is used for gathering more data."
    )
    parser.add_argument(
        "--animate", 
        action="store_true", 
        help="If this is set, the program will record the frames into a .mp4 file, useful for making demo videos."
    )
def figure_to_img(fig):
    io_buf = io.BytesIO()
    fig.savefig(io_buf, format='raw')
    io_buf.seek(0)
    w, h = fig.canvas.get_width_height()
    fig_img = np.frombuffer(io_buf.getvalue(), dtype=np.uint8).reshape(int(h), int(w), -1)
    io_buf.close()
    return fig_img
def data_to_value(data, type):
    if types[type] == "UINT8":
        return struct.unpack('B'*(len(data)//type_sizes[type]), data)
    elif types[type] == "FLOAT":
        return struct.unpack('f'*(len(data)//type_sizes[type]), data)
def to_image(data, w, h,d, type):
    v = data_to_value(data, type)
    img = np.array(v).reshape(h, w, d)
    return img
def wait_for_image(handle, ser):
        raw_line = ser.readline()
        try:
            line = raw_line.decode("utf-8").strip()
        except:
            return None, None
        if len(line) == 0:
            return None, None
        print(line)
        if not line.startswith(handle):
            return None, None
        line_info = line.split(",")
        w = int(line_info[1])
        h = int(line_info[2])
        d = int(line_info[3])
        type = int(line_info[4])
        misc_info = ",".join(line_info[5:])
        # The next bytes are the image itself
        img_data = ser.read(w*h*d*type_sizes[type])
        img = to_image(img_data, w, h,d, type)
        return img, misc_info
def wait_for_bboxes(ser):
    raw_line = ser.readline()
    try:
        line = raw_line.decode("utf-8").strip()
    except:
        return None
    if len(line) == 0:
        return None
    print(line)
    if not line.startswith("bboxes"):
        return None
    line_info = line.split(":")
    num_bboxes = int(line_info[1])
    # The next lines are the centroids
    bboxes = []
    for i in range(num_bboxes):
        bbox_line = ser.readline().decode("utf-8").strip()
        print(bbox_line)
        bbox_info = bbox_line.split(",")
        bbox = np.zeros(shape=(4,))
        bbox[0] = float(bbox_info[0])
        bbox[1] = float(bbox_info[1])
        bbox[2] = float(bbox_info[2])
        bbox[3] = float(bbox_info[3])
        bboxes.append(bbox)
    return bboxes
def wait_for_centroids(ser):
    raw_line = ser.readline()
    if len(raw_line) == 0:
        return None
    try:
        line = raw_line.decode("utf-8").strip()
    except:
        return None
    if len(line) == 0:
        return None
    print(line)
    if not line.startswith("centroids"):
        return None
    line_info = line.split(":")
    num_centroids = int(line_info[1])
    # The next lines are the centroids
    centroids = []
    for i in range(num_centroids):
        centroid_line = ser.readline().decode("utf-8").strip()
        print(centroid_line)
        centroid_info_prev = None
        if "-" in centroid_line:
            track_info = centroid_line.split("-")
            centroid_info = track_info[0].split(",")
            centroid_info_prev = track_info[1].split(",")
        else:
            centroid_info = centroid_line.split(",")

        res = []
        x = float(centroid_info[0])
        y = float(centroid_info[1])
        count = int(centroid_info[2])
        res.append(x)
        res.append(y)
        res.append(count)
        if centroid_info_prev is not None:
            x_prev = float(centroid_info_prev[0])
            y_prev = float(centroid_info_prev[1])
            count_prev = int(centroid_info_prev[2])
            dist_squared = float(centroid_info_prev[3])
            res.append(x_prev)
            res.append(y_prev)
            res.append(count_prev)
            res.append(dist_squared)
        centroids.append(res)
    return centroids

def display_serial(ser, args): 


    fig,axs = plt.subplots(1,2)
    if args.animate:
        writer = imageio.get_writer("animation.mp4", fps=1)
    img = None
    heatmap = None
    centroids = None
    bboxes = None
    timings = []
    sample_count = 0
    sample_name = str(uuid.uuid4())
    if args.save:
        save_file = open(f"data/{sample_name}.csv", "a")
    try:
        while(True):
            start = time.time()
            while img is None:
                img, img_misc = wait_for_image("image:image", ser)
            while heatmap is None:
                heatmap, heatmap_misc = wait_for_image("image:heatmap", ser)
            while bboxes is None:
                bboxes = wait_for_bboxes(ser)
            while centroids is None:
                centroids = wait_for_centroids(ser)

            heatmap = heatmap[:, :, 4]*heatmap[:,:, 5]
            print(img.shape)
            axs[0].imshow(img, cmap="jet", vmax=32, vmin=25, extent=(0, img.shape[1], img.shape[0], 0))
            axs[1].imshow(heatmap, cmap="jet", vmin=0, vmax=1)
            axs[0].set_title(img_misc)
            axs[1].set_title(heatmap_misc)
            axs[0].axvline(20, color="red")
            for bbox in bboxes:
                rect = patches.Rectangle((bbox[0], bbox[1]), bbox[2], bbox[3], edgecolor="r", facecolor="none")
                axs[0].add_patch(rect)
            for centroid in centroids:
                x = centroid[0]
                y = centroid[1]
                count = centroid[2]
                circle_patch = patches.Circle(xy=(x, y), radius=1, facecolor="white", edgecolor="black", linewidth=1)
                axs[0].add_patch(circle_patch)
                #axs[0].annotate(f"{count}", (x,y), ha="center", va="center", c="black")
                if len(centroid) > 3:
                    x_prev = centroid[3]
                    y_prev = centroid[4]
                    count_prev = centroid[5]
                    dist_squared = centroid[6]
                    if np.sqrt(dist_squared) < 1:
                        continue
                    arrow_patch = patches.Arrow(x=x_prev, y=y_prev, dx=x-x_prev, dy=y-y_prev, edgecolor="black", facecolor="white", width=4)
                    axs[0].add_patch(arrow_patch)
                    #axs[0].annotate(f"{count_prev}", (x,y), ha="center", va="center", c="black")

            fig.tight_layout()
            fig.canvas.draw()
            plt.pause(0.01)
            if args.animate:
                fig_img = figure_to_img(fig)
                writer.append_data(fig_img)
            if args.save:
                save_string = f"{sample_count},{','.join(map(str, img.flatten().tolist()))}\n"
                save_file.write(save_string)
            sample_count+=1
            img = None
            heatmap = None
            centroids = None
            bboxes = None
            for ax in axs.flatten():
                ax.clear()

            timings.append(time.time()-start)

    except KeyboardInterrupt:
        if args.save:
            save_file.close()
        if args.animate:
            delay = np.mean(timings)
            print(f"Saving animation with FPS {1/delay:.3f}s")
            writer.close()
            gif = imageio.mimread("animation.mp4", memtest=False)
            imageio.mimwrite("animation.mp4", gif, fps=1/delay)

    