# Sensory Wake Word Detection Demos

This demo showcases how to use Sensory's [TrulyHandsfree](https://www.sensory.com/wake-word/) (THF) solution for wake word detection to control Silicon Labs boards with your voice. In the provided examples we use different wake works to control on-board LEDs. Several models implementing different wake word phrases are included in this demo. All of these were trained online in just a few clicks through Sensory's platform.

Pre-built binaries are provided for the Silicon Labs [Thunderboard Sense 2](https://www.silabs.com/development-tools/thunderboard/thunderboard-sense-two-kit) and [EFR32xG24](https://www.silabs.com/development-tools/wireless/efr32xg24-dev-kit) dev kits. The demo can also be built for other Silicon Labs devices that have a mic and at least two LEDs.

You can create you own wake words on Sensory's VoiceHub and update this demo to try them out. See [Creating a New model](#creating-a-new-model) for instructions.

Source code in this repository is adapted from our Embedded World 2022 demo.

## Prerequisites

To run the prebuilt binaries, you'll need

- An [EFR32xG24](https://www.silabs.com/development-tools/wireless/efr32xg24-dev-kit) dev kit or a [Thunderboard Sense 2](https://www.silabs.com/development-tools/thunderboard/thunderboard-sense-two-kit) dev kit.
- `commander` ([Simplicity Commander](https://www.silabs.com/developers/mcu-programming-options#programming)) to flash the binaries onto your device.

To build the source code, you'll need

- `make`
- [GNU ARM Embedded Toolchain](https://developer.arm.com/downloads/-/gnu-rm), version 10.3-2021.10 or 10.3-2021.07
- `slc` ([Silicon Labs Configurator](https://www.silabs.com/documents/public/user-guides/ug520-software-project-generation-configuration-with-slc-cli.pdf)) for project generation. Installation instructions can be found under Section 2 in the provided link.
- [Gecko SDK >= v4.1](https://github.com/SiliconLabs/gecko_sdk)

To configure `slc` to use the Gecko SDK and ARM GNU Embedded Toolchain you downloaded, run

```sh
slc configuration --sdk=<path/to/gecko_sdk>
slc configuration --gcc-toolchain=<path/to/arm-gnu-gcc-toolchain>
```

## Prebuilt binaries

Prebuilt binaries for Silicon Labs' EFR32xG24 Dev Kit and Thunderboard Sense 2 are provided under [`bin/`](bin/). These run with the `hello_gecko_small` model. For details on this model and other available models, see the section on [#Model Selection](#model-selection).

You can program your own device with these using [Simplicity Commander](https://www.silabs.com/developers/mcu-programming-options#programming),

```sh
# Assuming you've connected a EFR32xG24 Dev Kit
# to your machine over USB,
commander flash bin/efr32xg24_dev_kit/sensory_wakeupword.s37
# Note: If you encounter issues when flashing,
# try running `commander device recover` first.
```

### Example output

Here's a sample output, obtained using `screen` to read from to the USB serial port where an EFR32xG24 running this demo was connected.

```txt
#==============================================#
#   Sensory TrulyHandsfree Wake Word Example   #
#==============================================#
#   Core Frequency:              78000000 [Hz] #
#   Sample rate:                    16000 [Hz] #
#   Model Name:              hello_gecko_small #
#==============================================#
[k=   171] Recognized Hello Gecko   (id=1) with confidence 908
[k=   306] Recognized Bye Bye Gecko (id=2) with confidence 1950
[k=   446] Recognized Gecko Red     (id=3) with confidence 2237
[k=   568] Recognized Gecko Green   (id=4) with confidence 1781
Average Inference Time [% of 15ms]: 36
```

## Creating a new model

To create a new model for this demo, you will need to *i)* build it on Sensory's VoiceHub, and *ii)* add it to the demo project.

### Building a model on VoiceHub

To build a new model on Sensory's VoiceHub, follow these steps:

- Go to [sensory.com/voicehub](https://www.sensory.com/voicehub) and click on `Request Access`
- Fill out the form and include `Silicon Labs EFR32` as target DSP
- You will receive login credentials from a Sensory representative
- Visit [voicehub.sensory.com](https://voicehub.sensory.com) and setup your account using your login credentials
- Create a `New Project` for `Wake Word`
- Type your words, and build the project with Silicon Labs as the target. The model should have at least one and at most 4 keyword phrases.
- Once the model is ready, download it by clicking on `Download` and then `Model`. Afterwards, extract the downloaded contents.

### Adding a model to the project

- Under `app/include/model` in this project, create a new directory with your model's name (e.g. `app/include/model/my_wakeword`) and inside this directory create two new files `net.h` and `search.h`.
- Download the model files , and extract the contents.
- In the extracted folder, locate the files ending with `net.h`, `search.c` and `search.h`.
  - Copy the file contents from `*net.h` in the extracted dir to `net.h` in the directory you created
  - Copy the file contents from `*search.h` in the extracted dir to `search.h`in the directory you created
  - Append the file contents from `*search.c` from the extracted dir to the content in the `search.h` file you created
  - Change `const unsigned short` to `const unsigned short __ALIGNED(4)` in `search.h` and `net.h`. This ensures models are aligned on 4-byte boundaries in memory.
  - Optionally add a `MODEL_NAME` `#define` with a string identifier for your model in `search.h`

You can optionally use the provided Python script `scripts/add_model.py` to automate this. To do so,

- In your terminal, navigate to the `sensory_wakeupword` project directory
- Run `python scripts/add_model.py <path/to/extracted/model> -o <name_for_your_model>`

Then, you can use this model in the project by changing the `#include` statements in [`app/app.c`](app/app.c) as described in the section on [Model selection](#model-selection).

## Building

To build the demo for another Silicon Labs board, you will need to:

1. Generate a project for your board based on the demo application. You can do this by importing it into [Simplicity Studio](https://www.silabs.com/developers/simplicity-studio) or by using [Silicon Labs Configurator (SLC)](https://www.silabs.com/documents/public/user-guides/ug520-software-project-generation-configuration-with-slc-cli.pdf) and command line tools.
2. Cross-compile for your target device

### Building with Simplicity Studio

This section shows how you can generate and build the demo for your board using Simplicity Studio. To to so, you will need to:

1. Add this Git repository as an external repository in Simplicity Studio
2. Create a new project using the Simplicity Studio Launcher

#### Adding an external repository to Simplicity Studio

Simplicity Studio can be configured to search for demos and project templates in external repositories. To add an external repository,

1. Open Simplicity Studio
2. Open the Preferences view (the cogwheel at the top of the window)
3. Select `Simplicity Studio`→`External Repos`
4. Click on `Add...` and configure a new external repository with the following settings
    - **Name**: Machine Learning Applications on GitHub
    - **URI**:  <https://github.com/SiliconLabs/machine_learning_applications.git>
5. Select the newly added external repository and click on `Update...`
6. The Sensory Wakeup Word demo application should now be visible under `Example Projects & Demos` in the Simplicity Studio launcher when configuring new projects for EFR32xG24 Dev Kits (BRD2601B) and Thunderboard Sense 2 Dev Kits (BRD4166A).

#### Setting up the Sensory Wakeup Word demo in Simplicity Studio

You can then choose this demo as a template when generating a project through Simplicity Studio's launcher.

1. Open the Simplicity Studio Launcher (the rocket button in the top right corner of the window)
2. Connect your EFR32xG24 Dev Kit or Thunderboard Sense 2
3. Choose your device in the Connected Devices dropdown, and click on `Start`
4. Navigate to `Example Projects & Demos`. In the context menu on the left, scroll down to `Provider`, and select `Machine Learning Applications on GitHub`.
5. Locate the `Sensory Wakeup Word Demo Application (Series 1|2)` application and click on `Create`. You can leave the project conifguration settings to their defaults, and click on `Finish` to set up the project for your board.
6. Inside your project, you can compile the code and run it on your board by clicking on `Run`→`Debug` in the window context menu and selecting your connected board.

### Building with command line tools

This section shows how to generate and build the demo for a Series 2 board, using BRD2601B (EFR32xG24 Dev Kit) as an example. We're using SLC here, but you can also import the demo project into Simplicity Studio by selecting the `.slcp` project file.

First, generate a project targeting your device,

```sh
slc generate app/sensory_wakeupword.slcp --with brd2601b -d target/brd2601b
```

Then, build the project using `make`,

```sh
cd target/brd2601b
make -f sensory_wakeupword.Makefile -j
```

The compiled code can then be found under `target/brd2601b/build/debug`, and can be flashed onto your board as described in the section covering [#Prebuilt Binaries](#prebuilt-binaries).

### Building for Series 1 boards

Follow the same instructions as above, but use the `sensory_wakeupword_series_1.slcp` project file and target a Series 1 board (e.g. BRD4166A) instead.

### Model selection

There's multiple models to detect different keywords/commands. The model files are located under [`include/model/<model>`](include/model). To choose between them, you can change the model configuration `#include` paths in [`app/app.c`](app/app.c) at line 50,

```c
-#include "model/hello_gecko/net.h"
-#include "model/hello_gecko/search.h"

+#include "model/go_silabs/net.h"
+#include "model/go_silabs/search.h"
```

The available models are,

- `hello_gecko`: Detects "Hello Gecko", "Bye Bye Gecko", "Gecko red" and "Gecko green"
- `hello_gecko`: A smaller model for detecting "Hello Gecko", "Bye Bye Gecko", "Gecko red" and "Gecko green"
- `go_silabs`: Detects "Go Silabs".
- `voice_genie`: Detects "Voice Genie".
- `voice_genie_small`: A smaller model for detecting "Voice Genie".

#### Model behaviour

Whenever a keyword phrase is detected, the recognition is written out over the serial port using UART.

In addition, the onboard LEDs are controlled based on which keyword is detected.

| Keyword              | Event                                 |
|----------------------|---------------------------------------|
| 1st                  | Green and red LEDs turn on            |
| 2nd                  | Green and red LEDs turn off           |
| 3rd                  | Red LED turns on, green LED turns off |
| 4th                  | Green LED turns on, red LED turns off |

To give an example, here's an overview of the `hello_gecko` model commands.
| `hello_gecko` keyword  |  Event                                 |
|------------------------|----------------------------------------|
| 1st: *"Hello Gecko"*   |  Green and red LEDs turn on            |
| 2nd: *"Bye Bye Gecko"* |  Green and red LEDs turn off           |
| 3rd: *"Gecko Red"*     |  Red LED turns on, green LED turns off |
| 4th: *"Gecko Green"*   |  Green LED turns on, red LED turns off |

For models that only have one keyword phrase, the LEDs turn off automatically after a short duration.

### Quick Note: Series 1 and Series 2 boards

There are a couple of differences between builds for Series 1 and Series 2 boards which are described in the following two sections.

#### Floating point ABI

The demo code links against precompiled libraries from Sensory. These libraries use floating point arithmetic. Due to differences in how apps targeting Series 1 boards and Series 2 boards compile floating point code, we link against separate static libraries when compiling the app for Series 1 and Series 2. Specifically, Series 1 uses `softfp` floating point ABI and Series 2 uses `hard` floating point ABI.

For more details on floating point ABIs, check out [this page](https://embeddedartistry.com/blog/2017/10/11/demystifying-arm-floating-point-compiler-options/).

#### Clock frequency

Series 2 boards support scaling up the CPU core frequency using the `device_init_dpll` GSDK component. To increase the throughput when running on Series 2 boards, we pull in the `device_init_dpll` component in the corresponding project file. For EFR32xG24, this enables running at 78Mhz instead of 39MHz

## Performance

Below you can find performance numbers recorded using EFR32xG24 with a 78MHz CPU core frequency.

The demo performs inference for voice recognition periodically every 15ms. The inference times shown below indicate how long the inference processing takes, as a percentage of these time windows.

| Model                    |    Inference time [%] | Approx. model size [kB] |
|------------------------- |-----------------------|-------------------------|
|`hello_gecko`             |    72                 |  190                    |
|`hello_gecko_small`       |    36                 |  83                     |
|`go_silabs`               |    63                 |  165                    |
|`voice_genie`             |    57                 |  165                    |
|`voice_genie_small`       |    26                 |  61                     |

### Average Inference Time

[`app/app.c`](app/app.c) has a `#define` named `DISPLAY_INFERENCE_TIME` which can be toggled to select whether the inference time should be printed out over the serial port in addition to the recognitions. It's enabled by default.

## License

Silicon Labs Master Software License Agreement (MSLA) in combination with Sensory THF Micro End User License Agreement (Sensory THF Micro EULA).

See [LICENSE](LICENSE) for details.
