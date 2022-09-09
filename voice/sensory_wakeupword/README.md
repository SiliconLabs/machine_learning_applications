# Sensory Wake Word Detection

This demo showcases how to use Sensory's [TrulyHandsfree](https://www.sensory.com/wake-word/) (THF) solution for wake word detection to control Silicon Labs boards with your voice. In the provided examples we use different wake words to control on-board LEDs. Several models implementing different wake word phrases are included in the provided example projects. All of these models were developed in just a few clicks through Sensory's VoiceHub.

Pre-built binaries are provided for Silicon Labs' [Thunderboard Sense 2](https://www.silabs.com/development-tools/thunderboard/thunderboard-sense-two-kit) and [EFR32xG24](https://www.silabs.com/development-tools/wireless/efr32xg24-dev-kit). The demo can also be built for other Silicon Labs devices that have a mic and at least two LEDs.
You can also create you own wake word model on Sensory's VoiceHub and include it in this demo to try it out. See [#Creating a New model](#creating-a-new-model) for instructions.

## Building and running the demo

You can build and run this demo using either Simplicity Studio or through a command line interface. If you don't have Simplicity Studio installed, feel free to skip to the section on [#Command line tools](#command-line-tools).

### Simplicity Studio

To build and run the demo for your board using Simplicity Studio, you will need to:

1. Add this Git repository as a Gecko SDK extension in Simplicity Studio
2. Create a new project for your board using the Simplicity Studio Launcher
3. Build the code and flash it onto your device using the Simplicity Studio IDE

#### Simplicity Studio: Adding an external repository

Simplicity Studio supports adding Gecko SDK extensions which provide new project templates, prebuilt demos and software components. To add this repository as an SDK extension,

1. Download the code, either by
   - Cloning the repository using `git`:

      ```sh
      git clone https://github.com/SiliconLabs/machine_learning_applications.git
      ```

   - Or navigating to <https://github.com/SiliconLabs/machine_learning_applications.git>, opening the green `Code` dropdown, selecting `Download ZIP`, and extracting the contents.
2. Open Simplicity Studio
3. Open the Preferences view in Simplicity Studio (the cogwheel at the top of the window)
4. Select `Simplicity Studio`→`SDKs`
5. Select your Gecko SDK Suite, then click on `Add Extension...`.
6. In the menu, click on `Browse` and point to the root directory of the downloaded repository. Select `Machine Learning Applications` and click on `OK` to add the extension, then click on `Trust` to trust the contents.
7. Click `Apply and Close` to exit the preferences menu.

*Note: If `Simplicity Studio`→`SDKs` doesn't show up, you might be missing the 32-bit and Wireless MCUs package for Studio. You can install this by opening the Install view (the arrow at the top of the window) and selecting `Install by technology type`.*

#### Simplicity Studio: Flashing the demo

If you have an EFR32xG24 Dev Kit or Thunderboard Sense 2 you can run the prebuilt demo binaries from the Simplicity Studio Launcher. These are compiled with the default `hello_gecko` model. For details on this model and other available models, see the section on [#Making changes to the demo](#making-changes-to-the-demo).

To flash the prebuilt demo onto your device,

1. Open the Simplicity Studio Launcher (the rocket button in the top right corner of the window)
2. Connect your device (e.g. an EFR32xG24 Dev Kit, Thunderboard Sense 2 Dev Kit, or any other Silicon Labs device with a mic and at least two LEDs)
3. Choose your device in the Connected Devices dropdown, and click on `Start`
4. Navigate to `Example Projects & Demos`. Then, in the context menu on the left, scroll down to `Capability`, and select `Machine Learning`.
5. Locate the `Machine Learning - Sensory Wakeup Word Demo` application, and click on `Run`.
6. Try saying "Hello Gecko" to your device, and the LEDs should light up. For more information on the expected behaviour, see the section on [#Example output for the `hello_gecko` model](#example-output-for-the-hello_gecko-model)

#### Simplicity Studio: Building the Sensory Wakeup Word project

If you want to make changes to the demo, you can generate a new project based the project template provided by this repository as follows:

1. Open the Simplicity Studio Launcher (the rocket button in the top right corner of the window)
2. Connect your device (e.g. an EFR32xG24 Dev Kit, Thunderboard Sense 2 Dev Kit, or any other Silicon Labs device with a mic and at least two LEDs)
3. Choose your device in the Connected Devices dropdown, and click on `Start`
4. Navigate to `Example Projects & Demos`. Then, in the context menu on the left, scroll down to `Capability`, and select `Machine Learning`.
5. Locate the `Machine Learning - Sensory Wakeup Word` example project and click on `Create`. You can leave the settings to their defaults, and click on `Finish` to set up the project for your board.
6. Inside your project, you can then compile and run the code from the windows context menu:
   - Click on `Project`→`Build Project` to compile the code
   - Click on `Run`→`Debug` and select your connected device to run the code

Now that the project is set up, proceed to the section on [#Making changes to the demo](#making-changes-to-the-demo) for instructions on how to create your own model and run it on your device.

### Command line tools

To build and run the demo for your board through a CLI, you will need to:

- Install the prerequisites
- Generate a project for your board using Silicon Labs Configurator
- Build the project using Make and flash it onto your device using Simplicity Commander

#### Command line tools: Installing the prerequisites

To flash binaries onto your device, you'll need

- `commander` ([Simplicity Commander](https://www.silabs.com/developers/mcu-programming-options#programming))

To generate and build the project, you'll need

- `make` (for Windows we recommend using WSL or Cygwin)
- `arm-eabi-none` GNU toolchain ([GNU ARM Embedded Toolchain](https://developer.arm.com/downloads/-/gnu-rm), version 10.3-2021.10 or 10.3-2021.07)
- `slc` ([Silicon Labs Configurator](https://www.silabs.com/documents/public/user-guides/ug520-software-project-generation-configuration-with-slc-cli.pdf), installation instructions can be found under Section 2 in the provided link)
- `git`

After installing the tools, make sure that they are available in your `PATH`.

Then, download `Gecko SDK >= v4.1`. You can obtain this from the Github repository [SiliconLabs/gecko_sdk](https://github.com/SiliconLabs/gecko_sdk).

```sh
git clone https://github.com/SiliconLabs/gecko_sdk
cd gecko_sdk
git checkout v4.1.0
```

After this, configure `slc` to use the Gecko SDK and ARM GNU Embedded Toolchain you downloaded,

```sh
slc configuration --sdk=<path/to/gecko_sdk>
slc configuration --gcc-toolchain=<path/to/arm-gnu-gcc-toolchain>
slc signature trust --sdk=<path/to/gecko_sdk>
```

Lastly, clone this repository.

```sh
git clone https://github.com/SiliconLabs/machine_learning_applications
```

#### Command line tools: Flashing the demo

If you have an EFR32xG24 Dev Kit or Thunderboard Sense 2 you can run the provided prebuilt binaries, which are located under `bin/`. These run with the default `hello_gecko` model. For details on this model and other available models, see the section on [#Making changes to the demo](#making-changes-to-the-demo).

You can program your own device with these using `commander`,

```sh
# in sensory_wakeupword/

# Assuming you've connected a EFR32xG24 Dev Kit to your machine over USB,
commander flash bin/efr32xg24_dev_kit/sensory_wakeupword.s37
# Note: If you encounter issues when flashing, try running `commander device recover` first.
```

Try saying "Hello Gecko" to your device, and the LEDs should light up. For more information on the expected behaviour, see the section on [#Example output for the `hello_gecko` model](#example-output-for-the-hello_gecko-model)

#### Command line tools: Building the Sensory Wakeup Word demo

If you want to make changes to the demo, you can generate a new project based the project template provided by this repository using Simplicity Commander. This section documents this procedure using the EFR32xG24 Dev Kit (BRD2601B) as an example.

First, generate a project targeting your device,

```sh
# in sensory_wakeupword/
slc generate app/sensory_wakeupword_series_2.slcp --with brd2601b -d target/brd2601b
```

Then, build the project using `make`,

```sh
# in sensory_wakeupword/
cd target/brd2601b
make -f sensory_wakeupword.Makefile -j
```

The compiled code can then be found under `target/brd2601b/build/debug`, and can be flashed onto your board as described in the section [#Command line tools: Flashing the demo](#command-line-tools-flashing-the-demo).

For instructions on how to create a new model and run it on your device, proceed to the section on [#Making changes to the demo](#making-changes-to-the-demo).

##### Building for Series 1 boards

Follow the same instructions as above, but use the `sensory_wakeupword_series_1.slcp` project file and target a Series 1 board (e.g. BRD4166A) instead.

## Making changes to the demo

### Model selection

We provide multiple models to detect different keywords/commands. The model files are located under [`include/model/<model>`](include/model) in the project. To choose between them, change the model configuration `#include` paths in [`app/app.c`](app/app.c),

```patch
-#include "model/hello_gecko/net.h"
-#include "model/hello_gecko/search.h"
+#include "model/go_silabs/net.h"
+#include "model/go_silabs/search.h"
```

The available models are,

- `hello_gecko`: Detects "Hello Gecko", "Bye Bye Gecko", "Gecko red" and "Gecko green"
- `go_silabs`: Detects "Go Silabs".
- `voice_genie`: Detects "Voice Genie".
- `voice_genie_small`: A smaller model for detecting "Voice Genie".

### Model behaviour

Whenever a keyword phrase is detected, the recognition is written out over the UART serial port.

In addition, the onboard LEDs are controlled based on which keyword is detected - meaning the keyword ordering matters. The default mapping from keywords to LED behaviour is described in the table below. You can change this by modifying the `process_brick_callback` function in [`app/app.c`](app/app.c).

| Keyword              | Event                                 |
|----------------------|---------------------------------------|
| 1st                  | Green and red LEDs turn on            |
| 2nd                  | Green and red LEDs turn off           |
| 3rd                  | Red LED turns on, green LED turns off |
| 4th                  | Green LED turns on, red LED turns off |

For models with $n<4$ keywords, only the $n$ first keyword recognition events can occur. If the model only has a single keyword phrase, the LEDs turn off automatically a short while after a recognition.

#### Example output for the `hello_gecko` model

After compiling the demo with the `hello_gecko` model and flashing it onto your device, try uttering the phrases `Hello Gecko`, `Bye Bye Gecko`, `Gecko Red` and `Gecko Green`.

These phrases should map to the following behaviour:

| `hello_gecko` keyword  |  Event                                 |
|------------------------|----------------------------------------|
| 1st: *"Hello Gecko"*   |  Green and red LEDs turn on            |
| 2nd: *"Bye Bye Gecko"* |  Green and red LEDs turn off           |
| 3rd: *"Gecko Red"*     |  Red LED turns on, green LED turns off |
| 4th: *"Gecko Green"*   |  Green LED turns on, red LED turns off |

In addition, if you read from the USB serial port where your device is connected (e.g. using `screen`), you should see an output similar to what's shown below.

```txt
#==============================================#
#   Sensory TrulyHandsfree Wake Word Example   #
#==============================================#
#   Core Frequency:              78000000 [Hz] #
#   Sample rate:                    16000 [Hz] #
#   Model Name:                    hello_gecko #
#==============================================#
[k=   128] Recognized Hello Gecko   (id=1) with confidence 93%
[k=   285] Recognized Bye Bye Gecko (id=2) with confidence 99%
[k=   538] Recognized Gecko Red     (id=3) with confidence 85%
[k=   730] Recognized Gecko Green   (id=4) with confidence 99%
Average Inference Time [% of 15ms]: 36
```

### Creating a new model

To create a new model for this demo, you will need to *i)* build it on Sensory's VoiceHub, and *ii)* add it to the demo project.

#### Building a model on VoiceHub

To build a new model on Sensory's VoiceHub, follow these steps:

- Go to [sensory.com/voicehub](https://www.sensory.com/voicehub) and click on `Request Access`
- Fill out the form and include `Silicon Labs EFR32` as target DSP
- You will receive login credentials from a Sensory representative
- Visit [voicehub.sensory.com](https://voicehub.sensory.com) and setup your account using your login credentials
- Create a `New Project`
- Choose `Wake Word` as the project type (*not simple commands or natural language*)
- Type your words, and build the project with `DSP: Silicon Labs Cortex-M33/M4` as the output format. The model should have at least one and at most 4 keyword phrases. What happens when a given keyword is detected is described under [#Model behaviour](#model-behaviour).
- Tip: Choose your initial size as follows
  - Thunderboard Sense 2: Under 100kB
  - xG24 Dev Kit: Under 200kB
- Tip: You can listen to the keywords by clicking on the speaker symbol to their right. If needed, play with the spelling to ensure the pronounciation is as expected.
- Once the model is ready, download it by clicking on `Download` and then `Model`. Afterwards, extract the downloaded contents.

#### Adding a model to your project

You can add the new model manually, or automatically (which requires `python`)

##### Manually adding your model

- Under `app/include/model` in this project, create a new directory with your model's name (e.g. `app/include/model/my_wakeword`) and inside this directory create two new files `net.h` and `search.h`.
- In the extracted model folder, locate the files ending with `net.h`, `search.c` and `search.h`.
  - Copy the file contents from `*net.c` in the extracted dir to `net.h` in the directory you created
  - Copy the file contents from `*search.h` in the extracted dir to `search.h`in the directory you created
  - Append the file contents from `*search.c` from the extracted dir to the content in the `search.h` file you created
  - Change `const unsigned short` to `const unsigned short __ALIGNED(4)` in `search.h` and `net.h`. This ensures models are aligned on 4-byte boundaries in memory.
  - Add a `MODEL_NAME` `#define` with a string identifier for your model in `search.h`
- Switch to your new model as described in the section on [#Model Selection](#model-selection)
- Build and run the project as described in the section on [#Building and running the demo](#building-and-running-the-demo)

##### Automatically adding your model

If you've cloned this Git repository and are building with the command line tools, you can use the provided Python script `scripts/add_model.py` to automate adding new model files to your project.

```sh
# in sensory_wakeupword/
python scripts/add_model.py <path/to/extracted/model> -o <name_for_your_model>
```

Then, you can use this model in the project by changing the `#include` statements in [`app/app.c`](app/app.c) as described in the section on [Model selection](#model-selection).

## Implementation notes

### Series 1 and Series 2 boards

There are a couple of differences between builds for Series 1 and Series 2 boards which are described in the following two sections.

#### Floating point ABI

The demo code links against precompiled libraries from Sensory located under `app/lib/`. These libraries use floating point arithmetic. Due to differences in how apps targeting Series 1 boards and Series 2 boards compile floating point code, we link against separate static libraries when compiling the app for Series 1 and Series 2 devices. Specifically, Series 1 uses `softfp` floating point ABI and Series 2 uses `hard` floating point ABI.

For more details on floating point ABIs, check out [this page](https://embeddedartistry.com/blog/2017/10/11/demystifying-arm-floating-point-compiler-options/).

#### Clock frequency

Series 2 boards support scaling up the CPU core frequency using the `device_init_dpll` GSDK component. To increase the throughput when running on Series 2 boards, we pull in the `device_init_dpll` component in the corresponding project file. For EFR32xG24, this enables running at 78Mhz instead of 39MHz.

### Performance

Below you can find performance numbers recorded using EFR32xG24 with a 78MHz CPU core frequency.

The demo application performs inference periodically every 15ms, and the *Load* column indicates the percentage of these 15ms windows that's spent querying the model. The second column shows the same data as the first, expressed as a duration.

| Model                | Load [%] | Inference time [ms] |  Approx. model size [kB] |
|----------------------|----------|---------------------|--------------------------|
|`hello_gecko`         | 36       | 5.4                 |   83                     |
|`go_silabs`           | 26       | 3.9                 |   83                     |
|`voice_genie`         | 57       | 8.                  |   165                    |
|`voice_genie_small`   | 26       | 3.9                 |   61                     |

### Average Inference Time

[`app/app.c`](app/app.c) has a `#define` named `DISPLAY_INFERENCE_TIME` which can be toggled to select whether the inference time should be printed out over the serial port in addition to the recognitions. It's enabled by default.

## License

Silicon Labs Master Software License Agreement (MSLA) in combination with Sensory THF Micro End User License Agreement (Sensory THF Micro EULA).

See [LICENSE](LICENSE) for details.
