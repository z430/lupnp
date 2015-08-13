#include <libgupnp/gupnp.h>
#include <stdlib.h>
#include <gmodule.h>

gboolean status;
guint dimming;

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

G_MODULE_EXPORT
void set_loadlevel_cb(GUPnPService *service, GUPnPServiceAction *action, gpointer user_data)
{
    guint loadLevel;
    gupnp_service_action_get(action, "newLoadlevelTarget", G_TYPE_UINT, &loadLevel, NULL);
    loadLevel = dimming + loadLevel;
    g_print("loadLeve : %d.\n", loadLevel);
    gupnp_service_action_return(action);
}

int main()
{
    /**/
    //by default lamp is off
    status = FALSE;
    //dimming = 0;

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
    GUPnPServiceInfo *dimmingService;
    dimmingService = gupnp_device_info_get_service(GUPNP_DEVICE_INFO(dev),
                                         "urn:schemas-upnp-org:service:Dimming:1");

    //for implemented service
    gupnp_service_signals_autoconnect(GUPNP_SERVICE(info), NULL, NULL);
    //gupnp_service_signals_autoconnect(GUPNP_SERVICE(dimmingService), NULL, NULL);
    g_signal_connect(dimmingService, "action-invoked::SetLoadLevelTarget", G_CALLBACK(set_loadlevel_cb), NULL);

    char *id = gupnp_service_info_get_id(info);
    //char *dimming_id = gupnp_service_info_get_id(dimmingService);
    g_print("info id : %s.\n", id);

    GMainLoop *mainLoop;
    mainLoop = g_main_loop_new(NULL, FALSE);
    g_main_loop_run(mainLoop);

}