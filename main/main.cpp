#include <stdio.h>

#include "buttonMain.cpp"
#include "crsfMain.cpp"
#include "imuMain.cpp"
#include "pidMain.cpp"

extern "C"  void app_main(void){
    //buttonMain();
    //crsfMain();
    //imuMain();
    pidMain();
}