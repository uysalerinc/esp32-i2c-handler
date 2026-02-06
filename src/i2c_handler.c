#include "i2c_handler.h"
#include "esp_log.h"
#include <stdlib.h>
#include <string.h>

/** * @brief I2C Probe Timeout (ms)
 * 100ms is a safe margin that accounts for:
 * 1. Clock Stretching: Slow slaves holding SCL low.
 * 2. Bus Capacitance: Large networks with slower rise times.
 * 3. RTOS Preemption: Ensuring the driver doesn't timeout if the task 
 * is briefly preempted by the scheduler during the ACK phase.
 */
#define I2C_HANDLER_PROBE_TIMEOUT_MS  100

static const char *TAG = "I2C_HDL";

struct i2c_handler_ctx_t {
    i2c_master_bus_handle_t bus_handle;
};

i2c_handler_t i2c_handler_init(const i2c_handler_config_t *config) {
    if (!config) return NULL;

    struct i2c_handler_ctx_t *handler = malloc(sizeof(struct i2c_handler_ctx_t));
    if (!handler) {
        ESP_LOGE(TAG, "Memory allocation failed");
        return NULL;
    }
    
    memset(handler, 0, sizeof(struct i2c_handler_ctx_t));

    i2c_master_bus_config_t bus_config = {
        .i2c_port = config->port,
        .sda_io_num = config->sda_pin,
        .scl_io_num = config->scl_pin,
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .glitch_ignore_cnt = 7,
        .intr_priority = 0,
        .trans_queue_depth = 0,
        .flags.enable_internal_pullup = config->enable_pullups,
    };

    esp_err_t ret = i2c_new_master_bus(&bus_config, &handler->bus_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Bus init failed: %s", esp_err_to_name(ret));
        free(handler);
        return NULL;
    }

    ESP_LOGI(TAG, "I2C Master Bus Initialized (Port %d)", config->port);
    return handler;
}

void i2c_handler_deinit(i2c_handler_t handler) {
    if (handler) {
        i2c_del_master_bus(handler->bus_handle);
        free(handler);
        ESP_LOGI(TAG, "I2C Master Bus De-initialized");
    }
}

esp_err_t i2c_handler_probe(i2c_handler_t handler, uint8_t dev_addr) {
    if (!handler) return ESP_ERR_INVALID_ARG;

    // Probing involves sending the address and waiting for an ACK.
    // We use a generous timeout to support slow responders and noisy lines.
    return i2c_master_probe(handler->bus_handle, dev_addr, I2C_HANDLER_PROBE_TIMEOUT_MS);
}

esp_err_t i2c_handler_add_device(i2c_handler_t handler, uint8_t dev_addr, uint32_t speed_hz, i2c_master_dev_handle_t *out_dev_handle) {
    if (!handler || !out_dev_handle) return ESP_ERR_INVALID_ARG;

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = dev_addr,
        .scl_speed_hz = speed_hz,
    };

    return i2c_master_bus_add_device(handler->bus_handle, &dev_cfg, out_dev_handle);
}

void i2c_handler_remove_device(i2c_master_dev_handle_t dev_handle) {
    if (dev_handle) {
        i2c_master_bus_rm_device(dev_handle);
    }
}

esp_err_t i2c_handler_device_write(i2c_master_dev_handle_t dev_handle, const uint8_t *data, size_t len) {
    /*
    #define I2C_HANDLER_XFER_TIMEOUT_MS  50
    return i2c_master_transmit(dev_handle, data, len, pdMS_TO_TICKS(I2C_HANDLER_XFER_TIMEOUT_MS));
    
    Above line can be /will added if timeout control is needed in future.
    */
    return i2c_master_transmit(dev_handle, data, len, -1); // Wait indefinitely
}

esp_err_t i2c_handler_device_read(i2c_master_dev_handle_t dev_handle, uint8_t *data, size_t len) {
    return i2c_master_receive(dev_handle, data, len, -1);
}