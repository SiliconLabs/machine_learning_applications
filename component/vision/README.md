# Vision Frontend

The vision frontend is a SLC component that provides useful functions when working with images. It currently covers the following main areas:

- Bounding boxes
- Histograms
- Image Utilities
  - Image struct representing an image, and on which all functions operate

To use, assuming this extension is already installed, simply add the following into your `*.slcp` file:

```yaml
sdk_extension:
  - id: machine_learning_applications
    version: "1.0.0"
component:
    # ... other components
  - id: vision_frontend
    from: machine_learning_applications
```

After generatic the project using SLC, you can include `vision_frontend.h` in your source code:

```c
#include "vision_frontend.h"

// ... your code
```

For more information on how to use it, please refer to the vision example applications.
