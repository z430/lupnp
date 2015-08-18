#include "UpnpCore.h"


int main()
{
    /**/
    //by default lamp is off
    status = FALSE;
    dimming = 0;
    redLevel = 0;
    greenLevel = 0;
    blueLevel = 0;

    g_print("Default Status %s.\n", status ? "on" : "off");

    upnpInit();

    GMainLoop *mainLoop;
    mainLoop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(mainLoop);

}