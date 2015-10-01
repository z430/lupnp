#include "UpnpCore.h"
#include "HardwareHandler.h"


int main()
{
    /**/
    //by default lamp is off
    //switchStatus = FALSE;

    g_print("Default Status %s.\n", switchStatus ? "on" : "off");

    upnpInit();

    GMainLoop *mainLoop;
    mainLoop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(mainLoop);

}
