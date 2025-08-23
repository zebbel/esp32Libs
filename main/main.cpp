#include <stdio.h>

//#include "buttonMain.cpp"
#include "crsfMain.cpp"
//#include "hobbywingMain.cpp"
//#include "imuMain.cpp"
//#include "pidMain.cpp"
//#include "servoMain.cpp"
//#include "SRXL2Main.cpp"
//#include "telemtryMain.cpp"
//#include "zBusMain.cpp"
//#include "rpm_hardwareMain.cpp"
//#include "rpm_interruptMain.cpp"

extern "C" void app_main(void){
    //buttonMain();
    crsfMain();
    //hobbywingMain();
    //imuMain();
    //pidMain();
    //servoMain();
    //SRXL2Main();
    //telemetryMain();
    //zBusMain();
    //rpmMain();
}