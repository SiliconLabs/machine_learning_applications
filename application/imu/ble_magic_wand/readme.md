# Magic Wand

This application demonstrates a model trained to recognize various hand gestures with an accelerometer. The detected gestures are printed to the serial port. The gestures are then converted into media key presses, which allows the board to act as a media controller over BLE.

With the board facing up, btn0 pressed down, and the USB port pointed towards you, you should be able to perform one of the three gestures: Swipe Up (Arrow Up), Swipe Down (Arrow Down), or Circle (Play/Pause) and have them correctly detected and sent over BLE.

While btn0 is held, the board collects accelerometer data into a buffer. The data is sampled at 25Hz, and the model does inference every 100ms (every 4th sample that enters the buffer). When btn0 is released, the buffer is cleared and the device is ready for the next gesture.

The machine learning model is a CNN that treats the accelerometer data as a picture and recognizes patterns. Use Netron or similar tools to see the full model architecture.
