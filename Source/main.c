#include <stdio.h>
#include "../Header/HardwareHandler.h"
#include "../Header/UpnpCore.h"


int main()
{
    /**/
    //by default lamp is off
    int i = 0;
    switchStatus = FALSE;

    g_print("Status %s.\n", switchStatus ? "on" : "off");

    hardwareSetup();
    upnpDump();

    GMainLoop *mainLoop;
    mainLoop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(mainLoop);

    return 0;

}
