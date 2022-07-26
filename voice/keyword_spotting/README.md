# Keyword Spotting

Two neural network models for keyword spotting are provided here: `keyword_spotting_on_off` and `keyword_spotting_on_off_v2`. These can be used with the Gecko SDK example application [`voice_control_light`](https://github.com/SiliconLabs/gecko_sdk/tree/gsdk_4.1/app/common/example/voice_control_light). Both models are adapted from the corresponding [MLTK models](https://github.com/SiliconLabs/mltk/tree/master/mltk/models/siliconlabs).

## Reproducing the models

The [MLTK](https://siliconlabs.github.io/mltk/index.html) can be used to train the keyword spotting models.

Examples below show the `mltk` commands you can use for `keyword_spotting_on_off_v2`, and the same commands can also be used with `keyword_spotting_on_of`.

To train a model and produce the `.mltk.zip` folder, run

```sh
# in model/
mltk train keyword_spotting_on_off_v2
```

To evaluate the trained model's `.tflite` (Tensorflow Lite model) file,

```sh
# in model/
mltk evaluate keyword_spotting_on_off_v2 --tflite
```

To profile the model in the MVP hardware accelerator simulator,

```sh
# in model/
mltk profile keyword_spotting_on_off_v2 --accelerator MVP
```

To profile the model on a physical development board,

```sh
# in model/
mltk profile keyword_spotting_on_off_v2  --accelerator MVP --device
```

To run the model in the audio classifier on the local PC

```sh
# in model/
mltk classify_audio keyword_spotting_on_off_v2 --verbose
```

To run the model in the audio classifier on the physical device

```sh
# in model/
mltk classify_audio keyword_spotting_on_off_v2 --device --verbose
```

## keyword_spotting_on_off

This model is a CNN classifier which detects the keywords `on` and `off`.

It's trained on the `speech_commands_v2` dataset provided by Google, preprocessed into spectrograms.

## keyword_spotting_on_off_v2

This model is a CNN classifier which detects the keywords `on` and `off`.

### Changes from v1

This model is based on `keyword_spotting_on_off`. Similarly to that model, it's trained on the `speech_commands_v2` dataset provided by Google. Aside from that, it has the following changes:

- Model architecture uses more convolutional filters: This gives the model better accuracy at the expense of RAM and execution latency
- Dynamic spectrogram scaling: The audio spectrogram is quantized from uint16 to int8 using a dynamic scaling method
- Higher resolution spectrograms

### License

These models were developed by Silicon Labs and are covered by a standard
`Silicon Labs MSLA <https://www.silabs.com/about-us/legal/master-software-license-agreement>`_.
