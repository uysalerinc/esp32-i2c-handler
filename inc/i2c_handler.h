#ifndef I2C_HANDLER_H
#define I2C_HANDLER_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "esp_err.h"
#include "driver/gpio.h"
#include "driver/i2c_master.h"

/**
 * @brief Opaque handle for the I2C bus manager
 */
typedef struct i2c_handler_ctx_t *i2c_handler_t;

/**
 * @brief Configuration structure for I2C bus initialization
 */
typedef struct {
    i2c_port_num_t port;
    gpio_num_t sda_pin;
    gpio_num_t scl_pin;
    uint32_t clk_speed;
    bool enable_pullups;
} i2c_handler_config_t;

/**
 * @brief Initialize the I2C Master Bus
 * @param config Pointer to configuration struct
 * @return Handle to the initialized bus, or NULL on failure
 */
i2c_handler_t i2c_handler_init(const i2c_handler_config_t *config);

/**
 * @brief De-initialize the I2C Master Bus and free memory
 * @param handler Bus handle
 */
void i2c_handler_deinit(i2c_handler_t handler);

/**
 * @brief Probe the bus for a device at a specific address
 * @param handler Bus handle
 * @param dev_addr 7-bit device address
 * @return ESP_OK if device responds, ESP_ERR_NOT_FOUND otherwise
 */
esp_err_t i2c_handler_probe(i2c_handler_t handler, uint8_t dev_addr);

/**
 * @brief Add a persistent device to the I2C bus
 * @param handler Bus handle
 * @param dev_addr 7-bit device address
 * @param speed_hz SCL speed for this specific device
 * @param out_dev_handle Pointer to store the created device handle
 * @return ESP_OK on success
 */
esp_err_t i2c_handler_add_device(i2c_handler_t handler, uint8_t dev_addr, uint32_t speed_hz, i2c_master_dev_handle_t *out_dev_handle);

/**
 * @brief Remove a device from the bus
 * @param dev_handle Device handle returned by add_device
 */
void i2c_handler_remove_device(i2c_master_dev_handle_t dev_handle);

/**
 * @brief Perform a master transmit operation
 */
esp_err_t i2c_handler_device_write(i2c_master_dev_handle_t dev_handle, const uint8_t *data, size_t len);

/**
 * @brief Perform a master receive operation
 */
esp_err_t i2c_handler_device_read(i2c_master_dev_handle_t dev_handle, uint8_t *data, size_t len);

#endif // I2C_HANDLER_H