#include <libgupnp/gupnp.h>
#include <stdlib.h>
#include <gmodule.h>

gboolean status;

G_MODULE_EXPORT
void set_target_cb(GUPnPService *service, GUPnPServiceAction *action,
                    gpointer user_data)
{
    gboolean target;
    gupnp_service_action_get(action, "NewTargetValue", G_TYPE_BOOLEAN, &target, NULL);
    if(target != status)
        status = target;
    gupnp_service_notify(service, "Status", G_TYPE_BOOLEAN, status, NULL);
    g_print("Status : %s.\n", status ? "on" : "off");
    gupnp_service_action_return(action);

}


int main()
{
    /**/
    //by default lamp is off
    status = FALSE;
    g_print("Default Status %s.\n", status ? "on" : "off");

    //create gupnp context or device
    GUPnPContext *context;
    context = gupnp_context_new(NULL, NULL, 0, NULL);
    GUPnPRootDevice *dev;
    dev = gupnp_root_device_new(context, "BinaryLight1.xml", ".");
    gupnp_root_device_set_available(dev, TRUE);

    //define the upnp service
    GUPnPServiceInfo *info;
    info = gupnp_device_info_get_service(GUPNP_DEVICE_INFO(dev),
            "urn:schemas-upnp-org:service:SwitchPower:1");
    //for implemented service
    gupnp_service_signals_autoconnect(GUPNP_SERVICE(info), NULL, NULL);

    GMainLoop *mainLoop;
    mainLoop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(mainLoop);

}