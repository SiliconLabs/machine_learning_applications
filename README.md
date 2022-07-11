# Silicon Labs Machine Learning Applications

Silicon Labs provides integrated hardware, software and development tools to help you quickly create secure, intelligent devices suitable for both industrial and commercial use cases. Our development platform has first class support for embedded machine learning (TinyML) model inference, backed by the [Tensorflow Lite for Microcontrollers (TFLM)](https://www.tensorflow.org/lite/microcontrollers) framework. We offer devices such as the [EFR32xG24](https://www.silabs.com/wireless/zigbee/efr32mg24-series-2-socs) dev kit that have hardware accelerators specifically built for [high-performant and energy efficient](https://mlcommons.org/en/inference-tiny-07/) AI/ML edge computing.

This repository contains a collection of embedded applications that leverage ML for voice, vision, and inertial measurement unit data processing. You can use these to program your own Silicon Labs device, or as a starting point to develop your own TinyML application.

Feel free to open an issue if you have any questions or encounter problems, but take note that unless otherwise stated, all examples are considered to be EXPERIMENTAL QUALITY. The provided code has not been formally tested and is provided as-is. It is not suitable for production environments. In addition, there may be no bug maintenance planned for these resources. Silicon Labs may update the repository from time to time.

## About

The repository is organized by use case category. All applications are self contained, and include their own documentation.

Within an application's directory you will generally find,

- Source code for training the ML model and exporting it to Tensorflow Lite
- Trained model artefacts, including a Tensorflow Lite model.
- Documentation on model training and usage

## License

Content in this repository is generally available under the Apache 2.0 license. Some files have different licensing terms, and these will be clearly marked in the beginning of the file.
