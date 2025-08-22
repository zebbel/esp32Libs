idf_component_register(
    SRCS
        "RPM.cpp"
    INCLUDE_DIRS
        "."
    REQUIRES
        "driver"
        "esp_timer"
)

target_compile_options(${COMPONENT_LIB} PRIVATE -O3)