import display_serial_core
import argparse
import io


class Reader(io.IOBase):
    def __init__(self, path):
        self.path = path
        self.fp = open(path, "a+b")
        self.pointer = self.fp.tell()

    def read(self, n):
        data = b""
        while len(data) < n:
            self.fp.seek(self.pointer)
            to_read = n - len(data)
            new_data = self.fp.read(to_read)
            data += new_data
            self.pointer += len(new_data)
        assert len(data) == n, f"{len(data)},{n}"
        return data


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    display_serial_core.add_args(parser)
    args = parser.parse_args()
    ser = Reader("vusb")
    display_serial_core.display_serial(ser, args)
