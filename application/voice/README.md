
# Models for Voice Applications

## Keyword Spotting Models for Voice Control Applications

- [`keyword_spotting`](./keyword_spotting/) contains Tensorflow-based models for detecting the spoken words "on" and "off" from spectrogram data. These can be used with the Gecko SDK example application [Voice Control Light](https://github.com/SiliconLabs/gecko_sdk/tree/gsdk_4.1/app/common/example/voice_control_light) for keyword spotting using microphone data on supported boards.
- [`sensory_wakeupword`](./sensory_wakeupword/) showcases how we can use Sensory's [TrulyHandsfree](https://www.sensory.com/wake-word/) solution for wake word detection on Silicon Labs boards. The example includes models for detecting various phrases such as "hello gecko" and "bye bye gecko", and demonstrates how these can be used to control on-board LEDs.
