import asyncio
from bleak import BleakScanner, BleakClient

import io


class Writer(io.IOBase):
    def __init__(self, path):
        self.fp = open(path, "w+b")
        self.pointer = 0

    def write(self, data):
        self.fp.seek(self.pointer)
        self.fp.write(data)
        self.pointer += len(data)


def create_handler(buf: io.BytesIO):
    def handle_data(sender, data: bytearray):
        tty_file.write(data)

    return handle_data


async def run(client: BleakClient):
    print("Connecting...")
    await client.connect()

    handler = create_handler(None)
    await client.start_notify("16480002-0525-4ad5-b4fb-6dd83f49546b", handler)
    print("Connected. Do not exit the application.")
    while True:
        await asyncio.sleep(10)


async def main():
    _devices = await BleakScanner.discover(timeout=2)
    i = 0
    devices = []
    for d in _devices:
        if d.name is None:
            continue
        print(f"{i}: {d.name} - {d.address}")
        i += 1
        devices.append(d)
    if len(devices) == 0:
        print("No devices found. Exiting...")
        return

    port_id = input("Select BLE device to connect to: ")
    port_id = int(port_id)
    
    
    while(True):
        try:
            device = devices[port_id]
            client = BleakClient(device)
            await run(client)
        except Exception as inst:
            print("Retrying....")
            await client.disconnect()


if __name__ == "__main__":
    tty_file = Writer("vusb")
    asyncio.run(main())
