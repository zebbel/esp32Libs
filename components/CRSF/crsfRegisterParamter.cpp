#include "crsf.h"

/**
 * @brief register a parameter
 *
 * @param dataType data type of paramter
 * @param parameterPointer pointer to parameter structure
 */
void CRSF::register_parameter(crsf_value_type_e dataType, int *parameterPointer){
    parameters[deviceInfo.parameterTotal] = {dataType, parameterPointer};
    deviceInfo.parameterTotal ++;
}

void CRSF::register_parameter(crsf_parameter_uint8_t *parameter, uint8_t parent){
    parameter->common.parameterNumber = deviceInfo.parameterTotal+1;
    parameter->common.chunksRemaining = 0;
    parameter->common.parent = parent;
    parameter->common.dataType = CRSF_UINT8;
    register_parameter(CRSF_UINT8, (int*)parameter);
}

void CRSF::register_parameter(crsf_parameter_int8_t *parameter, uint8_t parent){
    parameter->common.parameterNumber = deviceInfo.parameterTotal;
    parameter->common.chunksRemaining = 0;
    parameter->common.parent = parent;
    parameter->common.dataType = CRSF_INT8;
    register_parameter(CRSF_INT8, (int*)parameter);
}

void CRSF::register_parameter(crsf_parameter_uint16_t *parameter, uint8_t parent){
    parameter->common.parameterNumber = deviceInfo.parameterTotal;
    parameter->common.chunksRemaining = 0;
    parameter->common.parent = parent;
    parameter->common.dataType = CRSF_UINT16;
    register_parameter(CRSF_UINT16, (int*)parameter);
}

void CRSF::register_parameter(crsf_parameter_int16_t *parameter, uint8_t parent){
    parameter->common.parameterNumber = deviceInfo.parameterTotal;
    parameter->common.chunksRemaining = 0;
    parameter->common.parent = parent;
    parameter->common.dataType = CRSF_INT16;
    register_parameter(CRSF_INT16, (int*)parameter);
}

void CRSF::register_parameter(crsf_parameter_uint32_t *parameter, uint8_t parent){
    parameter->common.parameterNumber = deviceInfo.parameterTotal;
    parameter->common.chunksRemaining = 0;
    parameter->common.parent = parent;
    parameter->common.dataType = CRSF_UINT32;
    register_parameter(CRSF_UINT32, (int*)parameter);
}

void CRSF::register_parameter(crsf_parameter_int32_t *parameter, uint8_t parent){
    parameter->common.parameterNumber = deviceInfo.parameterTotal;
    parameter->common.chunksRemaining = 0;
    parameter->common.parent = parent;
    parameter->common.dataType = CRSF_INT32;
    register_parameter(CRSF_INT32, (int*)parameter);
}

void CRSF::register_parameter(crsf_parameter_float_t *parameter, uint8_t parent){
    parameter->common.parameterNumber = deviceInfo.parameterTotal;
    parameter->common.chunksRemaining = 0;
    parameter->common.parent = parent;
    parameter->common.dataType = CRSF_FLOAT;
    register_parameter(CRSF_FLOAT, (int*)parameter);
}

void CRSF::register_parameter(crsf_parameter_text_selection_t *parameter, uint8_t parent){
    parameter->common.parameterNumber = deviceInfo.parameterTotal;
    parameter->common.chunksRemaining = 0;
    parameter->common.parent = parent;
    parameter->common.dataType = CRSF_TEXT_SELECTION;
    register_parameter(CRSF_TEXT_SELECTION, (int*)parameter);
}

void CRSF::register_parameter(crsf_parameter_string_t *parameter, uint8_t parent){
    parameter->common.parameterNumber = deviceInfo.parameterTotal;
    parameter->common.chunksRemaining = 0;
    parameter->common.parent = parent;
    parameter->common.dataType = CRSF_STRING;
    register_parameter(CRSF_STRING, (int*)parameter);
}

void CRSF::register_parameter(crsf_parameter_folder_t *parameter, uint8_t parent){
    parameter->common.parameterNumber = deviceInfo.parameterTotal;
    parameter->common.chunksRemaining = 0;
    parameter->common.parent = parent;
    parameter->common.dataType = CRSF_FOLDER;
    register_parameter(CRSF_FOLDER, (int*)parameter);
}

void CRSF::register_parameter(crsf_parameter_info_t *parameter, uint8_t parent){
    parameter->common.parameterNumber = deviceInfo.parameterTotal;
    parameter->common.chunksRemaining = 0;
    parameter->common.parent = parent;
    parameter->common.dataType = CRSF_INFO;
    register_parameter(CRSF_INFO, (int*)parameter);
}

void CRSF::register_parameter(crsf_parameter_command_t *parameter, uint8_t parent){
    parameter->common.parameterNumber = deviceInfo.parameterTotal+1;
    parameter->common.chunksRemaining = 0;
    parameter->common.parent = parent;
    parameter->common.dataType = CRSF_COMMAND;
    register_parameter(CRSF_COMMAND, (int*)parameter);
}