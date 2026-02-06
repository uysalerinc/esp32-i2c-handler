# I2C Handler Component for ESP-IDF

A lightweight and robust Hardware Abstraction Layer (HAL) for managing I2C communication on ESP32-series chips. This component is optimized for ESP-IDF v5.x and follows the modern "Master Bus -> Device Handle" architecture.

## Key Features

- **Resource Efficient:** Uses persistent device handles to avoid repetitive heap allocation.
- **Modern Driver Support:** Built exclusively on the `i2c_master.h` driver.
- **Opaque Architecture:** Encapsulates bus logic to keep your application code clean.
- **Discovery Tools:** Includes a high-performance `probe` function for I2C bus scanning and health checks.

## Installation

1. Copy this folder into your project's `components/` directory.
2. The component name is `esp32-i2c-handler`..

## Usage Guide

### 1. Initialize the Master Bus
Initialize the bus once at the start of your application.

```c
#include "i2c_handler.h"

i2c_handler_config_t config = {
    .port = I2C_NUM_0,
    .sda_pin = GPIO_NUM_6,
    .scl_pin = GPIO_NUM_7,
    .clk_speed = 400000, // 400kHz Fast Mode
    .enable_pullups = true
};

i2c_handler_t i2c_bus = i2c_handler_init(&config);
```

### 2. Device Discovery (Scanning)
Check if a specific sensor is present on the bus without registering it.


```c
esp_err_t err = i2c_handler_probe(i2c_bus, 0x44);
if (err == ESP_OK) {
    printf("Sensor detected at address 0x44\n");
}```


### 3. Persistent Communication
For production use, register a device to get a handle. This is the most efficient way to communicate in a loop.


```c
i2c_master_dev_handle_t sensor_handle;

// Register device (e.g., SHT4x at 0x44)
i2c_handler_add_device(i2c_bus, 0x44, 400000, &sensor_handle);

// Transmit/Receive using the handle
uint8_t cmd = 0xFD;
uint8_t rx_data[6];

i2c_handler_device_write(sensor_handle, &cmd, 1);
i2c_handler_device_read(sensor_handle, rx_data, 6);

// Remove device only when no longer needed
i2c_handler_remove_device(sensor_handle);
```

## Architecture Overview

This component sits between your specific Sensor Drivers and the ESP-IDF Peripheral Drivers, ensuring that timing and bus management are handled centrally.

[ Application ] -> [ Sensor Driver ] -> [ I2C Handler ] -> [ ESP-IDF HAL ]

## License
MIT