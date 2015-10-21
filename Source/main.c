#include <stdio.h>
#include "../Header/HardwareHandler.h"
#include "../Header/UpnpCore.h"


int main()
{
    /**/
    //by default lamp is off
    int i = 0;
    switchStatus = FALSE;

    g_print("Default Status %s.\n", switchStatus ? "on" : "off");

//    hardwareSetup();
    upnpDump();

    printf("Ready to go %d\n", i++);

    GMainLoop *mainLoop;
    printf("Ready to go %d\n", i++);
    mainLoop = g_main_loop_new(NULL, FALSE);
    printf("Ready to go %d\n", i++);
    g_main_loop_run(mainLoop);

    return 0;

}
