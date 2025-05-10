#include "crsf.h"

void CRSF::register_parameter(crsf_parameter_uint8_t *parameter, crsf_parameter_folder_t folder){
    parameters[deviceInfo.parameterTotal].parameterNumber = deviceInfo.parameterTotal ;
    parameters[deviceInfo.parameterTotal].chunksRemaining = 0;
    parameters[deviceInfo.parameterTotal].parent = folders[folder.name];
    parameters[deviceInfo.parameterTotal].dataType = CRSF_UINT8;
    parameters[deviceInfo.parameterTotal].parameterPointer = (int*)parameter;
    deviceInfo.parameterTotal ++;
}

void CRSF::register_parameter(crsf_parameter_int8_t *parameter, crsf_parameter_folder_t folder){
    parameters[deviceInfo.parameterTotal].parameterNumber = deviceInfo.parameterTotal ;
    parameters[deviceInfo.parameterTotal].chunksRemaining = 0;
    parameters[deviceInfo.parameterTotal].parent = folders[folder.name];
    parameters[deviceInfo.parameterTotal].dataType = CRSF_INT8;
    parameters[deviceInfo.parameterTotal].parameterPointer = (int*)parameter;
    deviceInfo.parameterTotal ++;
}

void CRSF::register_parameter(crsf_parameter_uint16_t *parameter, crsf_parameter_folder_t folder){
    parameters[deviceInfo.parameterTotal].parameterNumber = deviceInfo.parameterTotal ;
    parameters[deviceInfo.parameterTotal].chunksRemaining = 0;
    parameters[deviceInfo.parameterTotal].parent = folders[folder.name];
    parameters[deviceInfo.parameterTotal].dataType = CRSF_UINT16;
    parameters[deviceInfo.parameterTotal].parameterPointer = (int*)parameter;
    deviceInfo.parameterTotal ++;
}

void CRSF::register_parameter(crsf_parameter_int16_t *parameter, crsf_parameter_folder_t folder){
    parameters[deviceInfo.parameterTotal].parameterNumber = deviceInfo.parameterTotal ;
    parameters[deviceInfo.parameterTotal].chunksRemaining = 0;
    parameters[deviceInfo.parameterTotal].parent = folders[folder.name];
    parameters[deviceInfo.parameterTotal].dataType = CRSF_INT16;
    parameters[deviceInfo.parameterTotal].parameterPointer = (int*)parameter;
    deviceInfo.parameterTotal ++;
}

void CRSF::register_parameter(crsf_parameter_uint32_t *parameter, crsf_parameter_folder_t folder){
    parameters[deviceInfo.parameterTotal].parameterNumber = deviceInfo.parameterTotal ;
    parameters[deviceInfo.parameterTotal].chunksRemaining = 0;
    parameters[deviceInfo.parameterTotal].parent = folders[folder.name];
    parameters[deviceInfo.parameterTotal].dataType = CRSF_UINT32;
    parameters[deviceInfo.parameterTotal].parameterPointer = (int*)parameter;
    deviceInfo.parameterTotal ++;
}

void CRSF::register_parameter(crsf_parameter_int32_t *parameter, crsf_parameter_folder_t folder){
    parameters[deviceInfo.parameterTotal].parameterNumber = deviceInfo.parameterTotal ;
    parameters[deviceInfo.parameterTotal].chunksRemaining = 0;
    parameters[deviceInfo.parameterTotal].parent = folders[folder.name];
    parameters[deviceInfo.parameterTotal].dataType = CRSF_INT32;
    parameters[deviceInfo.parameterTotal].parameterPointer = (int*)parameter;
    deviceInfo.parameterTotal ++;
}

void CRSF::register_parameter(crsf_parameter_float_t *parameter, crsf_parameter_folder_t folder){
    parameters[deviceInfo.parameterTotal].parameterNumber = deviceInfo.parameterTotal ;
    parameters[deviceInfo.parameterTotal].chunksRemaining = 0;
    parameters[deviceInfo.parameterTotal].parent = folders[folder.name];
    parameters[deviceInfo.parameterTotal].dataType = CRSF_FLOAT;
    parameters[deviceInfo.parameterTotal].parameterPointer = (int*)parameter;
    deviceInfo.parameterTotal ++;
}

void CRSF::register_parameter(crsf_parameter_text_selection_t *parameter, crsf_parameter_folder_t folder){
    parameters[deviceInfo.parameterTotal].parameterNumber = deviceInfo.parameterTotal ;
    parameters[deviceInfo.parameterTotal].chunksRemaining = 0;
    parameters[deviceInfo.parameterTotal].parent = folders[folder.name];
    parameters[deviceInfo.parameterTotal].dataType = CRSF_TEXT_SELECTION;
    parameters[deviceInfo.parameterTotal].parameterPointer = (int*)parameter;
    deviceInfo.parameterTotal ++;
}

void CRSF::register_parameter(crsf_parameter_string_t *parameter, crsf_parameter_folder_t folder){
    parameters[deviceInfo.parameterTotal].parameterNumber = deviceInfo.parameterTotal ;
    parameters[deviceInfo.parameterTotal].chunksRemaining = 0;
    parameters[deviceInfo.parameterTotal].parent = folders[folder.name];
    parameters[deviceInfo.parameterTotal].dataType = CRSF_STRING;
    parameters[deviceInfo.parameterTotal].parameterPointer = (int*)parameter;
    deviceInfo.parameterTotal ++;
}

void CRSF::register_parameter(crsf_parameter_folder_t *parameter, crsf_parameter_folder_t folder){
    parameters[deviceInfo.parameterTotal].parameterNumber = deviceInfo.parameterTotal ;
    parameters[deviceInfo.parameterTotal].chunksRemaining = 0;
    parameters[deviceInfo.parameterTotal].parent = folders[folder.name];
    parameters[deviceInfo.parameterTotal].dataType = CRSF_FOLDER;
    parameters[deviceInfo.parameterTotal].parameterPointer = (int*)parameter;
    folders[parameter->name] = deviceInfo.parameterTotal ;
    deviceInfo.parameterTotal ++;
}

void CRSF::register_parameter(crsf_parameter_info_t *parameter, crsf_parameter_folder_t folder){
    parameters[deviceInfo.parameterTotal].parameterNumber = deviceInfo.parameterTotal ;
    parameters[deviceInfo.parameterTotal].chunksRemaining = 0;
    parameters[deviceInfo.parameterTotal].parent = folders[folder.name];
    parameters[deviceInfo.parameterTotal].dataType = CRSF_INFO;
    parameters[deviceInfo.parameterTotal].parameterPointer = (int*)parameter;
    deviceInfo.parameterTotal ++;
}

void CRSF::register_parameter(crsf_parameter_command_t *parameter, crsf_parameter_folder_t folder){
    parameters[deviceInfo.parameterTotal].parameterNumber = deviceInfo.parameterTotal ;
    parameters[deviceInfo.parameterTotal].chunksRemaining = 0;
    parameters[deviceInfo.parameterTotal].parent = folders[folder.name];
    parameters[deviceInfo.parameterTotal].dataType = CRSF_COMMAND;
    parameters[deviceInfo.parameterTotal].parameterPointer = (int*)parameter;
    deviceInfo.parameterTotal ++;
}