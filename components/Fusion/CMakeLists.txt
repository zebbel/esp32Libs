idf_component_register(
    SRCS
        "FusionAhrs.c"
        "FusionCompass.c"
        "FusionOffset.c"
    INCLUDE_DIRS
        "include"
    REQUIRES
        
)

target_compile_options(${COMPONENT_LIB} PRIVATE -O3 -ffast-math -fno-math-errno)