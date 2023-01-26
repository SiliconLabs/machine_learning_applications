"""Model training script for `keyword_spotting_on_off` model.

Corresponds with the [MLTK] model with the same name.

# References
MLTK: https://github.com/SiliconLabs/mltk/blob/0.8.0/mltk/models/siliconlabs/keyword_spotting_on_off.py
"""
# See https://siliconlabs.github.io/mltk/docs/audio/audio_feature_generator.html
frontend_settings = AudioFeatureGeneratorSettings()

frontend_settings.sample_rate_hz = (
    8000  # This can also be 16k for slightly better performance at the cost of more RAM
)
frontend_settings.sample_length_ms = 1000
frontend_settings.window_size_ms = 30
frontend_settings.window_step_ms = 20
frontend_settings.filterbank_n_channels = 32
frontend_settings.filterbank_upper_band_limit = (
    4000.0 - 1
)  # Spoken language usually only goes up to 4k
frontend_settings.filterbank_lower_band_limit = 100.0
frontend_settings.noise_reduction_enable = True
frontend_settings.noise_reduction_smoothing_bits = 5
frontend_settings.noise_reduction_even_smoothing = 0.004
frontend_settings.noise_reduction_odd_smoothing = 0.004
frontend_settings.noise_reduction_min_signal_remaining = 0.05
frontend_settings.pcan_enable = False
frontend_settings.pcan_strength = 0.95
frontend_settings.pcan_offset = 80.0
frontend_settings.pcan_gain_bits = 21
frontend_settings.log_scale_enable = True
frontend_settings.log_scale_shift = 6


#################################################
# ParallelAudioDataGenerator Settings
#
# Configure the data generator settings
# This specifies how to augment the training samples
# See the command: "mltk view_audio"
# to get a better idea of how these augmentations affect
# the samples
my_model.datagen = ParallelAudioDataGenerator(
    dtype=my_model.tflite_converter["inference_input_type"],
    frontend_settings=frontend_settings,
    cores=0.45,  # Adjust this as necessary for your PC setup
    debug=False,  # Set this to true to enable debugging of the generator
    max_batches_pending=16,  # Adjust this as necessary for your PC setup (smaller -> less RAM)
    validation_split=0.10,
    validation_augmentation_enabled=True,
    samplewise_center=False,
    samplewise_std_normalization=False,
    rescale=None,
    unknown_class_percentage=2.0,  # Increasing this may help model robustness at the expense of training time
    silence_class_percentage=0.3,
    offset_range=(0.0, 1.0),
    trim_threshold_db=30,
    noise_colors=None,
    loudness_range=(0.2, 1.0),
    speed_range=(0.9, 1.1),
    pitch_range=(0.9, 1.1),
    # vtlp_range=(0.9,1.1),
    bg_noise_range=(0.1, 0.4),
    bg_noise_dir="_background_noise_",  # This is a directory provided by the google speech commands dataset, can also provide an absolute path
)


#################################################
# Model Layout
#
# This defines the actual model layout
# using the Keras API.
# This particular model is a relatively standard
# sequential Convolution Neural Network (CNN).
#
# It is important to the note the usage of the
# "model" argument.
# Rather than hardcode values, the model is
# used to build the model, e.g.:
# Dense(model.n_classes)
#
# This way, the various model properties above can be modified
# without having to re-write this section.
#
def my_model_builder(model: MyModel):
    weight_decay = 1e-4
    regularizer = regularizers.l2(weight_decay)
    input_shape = model.input_shape
    filters = 8

    keras_model = Sequential(
        name=model.name,
        layers=[
            Conv2D(
                filters,
                (3, 3),
                padding="same",
                kernel_regularizer=regularizer,
                input_shape=input_shape,
                strides=(2, 2),
            ),
            BatchNormalization(),
            Activation("relu"),
            Conv2D(
                2 * filters,
                (3, 3),
                padding="same",
                kernel_regularizer=regularizer,
                strides=(2, 2),
            ),
            BatchNormalization(),
            Activation("relu"),
            Dropout(rate=0.1),
            Conv2D(
                4 * filters,
                (3, 3),
                padding="same",
                kernel_regularizer=regularizer,
                strides=(2, 2),
            ),
            BatchNormalization(),
            Activation("relu"),
            Dropout(rate=0.3),
            MaxPooling2D(pool_size=[7, 1]),
            Flatten(),
            Dense(model.n_classes, activation="softmax"),
        ],
    )

    keras_model.compile(
        loss=model.loss, optimizer=model.optimizer, metrics=model.metrics
    )
    return keras_model


my_model.build_model_function = my_model_builder


#################################################
# Audio Classifier Settings
#
# These are additional parameters to include in
# the generated .tflite model file.
# The settings are used by the "classify_audio" command
# or audio_classifier example application.
# NOTE: Corresponding command-line options will override these values.


# Controls the smoothing.
# Drop all inference results that are older than <now> minus window_duration
# Longer durations (in milliseconds) will give a higher confidence that the results are correct, but may miss some commands
my_model.model_parameters["average_window_duration_ms"] = 1000

# Minimum averaged model output threshold for a class to be considered detected, 0-255. Higher values increase precision at the cost of recall
my_model.model_parameters["detection_threshold"] = 160

# Amount of milliseconds to wait after a keyword is detected before detecting new keywords
my_model.model_parameters["suppression_ms"] = 750

# The minimum number of inference results to average when calculating the detection value
my_model.model_parameters["minimum_count"] = 3

# Set the volume gain scaler (i.e. amplitude) to apply to the microphone data. If 0 or omitted, no scaler is applied
my_model.model_parameters["volume_gain"] = 2

# This the amount of time in milliseconds an audio loop takes.
my_model.model_parameters["latency_ms"] = 100

# Enable verbose inference results
my_model.model_parameters["verbose_model_output_logs"] = False
