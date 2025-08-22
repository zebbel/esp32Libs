idf_component_register(
    SRCS
        "AHRS.cpp"
    INCLUDE_DIRS
        "."
    REQUIRES
        "esp_timer"
        "nvs_flash"
        "LSM9DS1"
        "Fusion"
        "LowPassFilter"
)

target_compile_options(${COMPONENT_LIB} PRIVATE -O3)