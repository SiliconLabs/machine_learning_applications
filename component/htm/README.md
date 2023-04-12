# Hierarchical Temporal Memory Component

This component implements the Hierarchical Temporal Memory (HTM) algorithm. It is a spatio-temporal memory system that learns to represent temporal sequences of inputs. Unlike deep learning, it has favorable properties such as online learning and noise tolerance, making it ideal of anomaly detection. It is based on the [BAMI](http://numenta.com/biological-and-machine-intelligence/) book, and optimized for embedded devices.

## Usage

There are two ways to use this component, one is to use the simple API, and the other is to use the advanced API. The simple API is easier to use, but it is less flexible. The advanced API is more flexible, but it requires more knowledge of the HTM algorithm.

The main difference between the two APIs is that the simple API wraps both the Spatial Pooler (SP) and the Temporal Memory (TM) into a single algorithm, while the advanced API allows you to use the SP and TM separately.

To initialize using the simple API, you can use the following code:

```C
#include "sl_htm.h"

#define INPUT_SIZE 27
#define TM_SIZE 27
...


// Create parameters for the HTM instance
sl_htm_sp_params_t params_sp;
sl_htm_tm_params_t params_tm;

// Setup parameters
sl_htm_sp_init_default_params(&params_sp);
sl_htm_tm_init_default_params(&params_tm);
// Modify parameter structs as needed
...

// Initialize the HTM instance
sl_htm_init(INPUT_SIZE, INPUT_SIZE, TM_SIZE, TM_SIZE, params_sp, params_tm);

// Init input SDR
sl_sdr_t input_sdr;
sl_sdr_init(&input_sdr, INPUT_SIZE, INPUT_SIZE);


```

You can then execute the algorithm in a loop as follows:

```C
// Set this to false if/when you want to stop learning
bool learning = true;

while(has_data){
    // Get data from e.g. some sensor
    float sensor_value = get_sensor_value();

    // Convert data to a Sparse Distributed Representation (SDR), and store it in the input_sdr
    sl_htm_encoder_simple_number(sensor_value, -1.0f, 1.0f, 9, &input_sdr);
    float anomaly_score = sl_htm_execute(&input_sdr, learning);

    // Do something with the anomaly score
    ...
}
```

For more usage examples, including the advanced API, see the example application and unit tests.

### Not yet implemented

SP:

- Duty cycles and boost factors
