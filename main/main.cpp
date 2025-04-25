#include <stdio.h>

#include "buttonMain.cpp"
#include "crsfMain.cpp"
#include "imuMain.cpp"

extern "C"  void app_main(void){
    //buttonMain();
    //crsfMain();
    imuMain();
}