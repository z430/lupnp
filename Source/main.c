#include "../Header/HardwareHandler.h"
#include "../Header/UpnpCore.h"


int main(){

    /**/
    //by default lamp is off


    int i = 0;
    switchStatus = FALSE;

    g_print("Status %s.\n", switchStatus ? "on" : "off");

//    hardwareSetup();
    if (!initUPnP ()) {
        return -2;
    }

    GMainLoop *mainLoop;
    mainLoop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(mainLoop);

    deinitUPnP();

    return 0;

}
