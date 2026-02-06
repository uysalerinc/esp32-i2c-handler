set(SRC_DIRS src)
set(INCLUDE_DIRS inc)
set(REQUIRES driver esp_driver_i2c esp_driver_gpio)

idf_component_register(SRC_DIRS ${SRC_DIRS}
                    INCLUDE_DIRS ${INCLUDE_DIRS}
                    REQUIRES ${REQUIRES})

                    