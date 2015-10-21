//
// Created by myth on 8/8/15.
//

#include "../Header/HardwareHandler.h"
#include "../Header/UpnpCore.h"
#include <libgupnp/gupnp.h>

#define DEVICE_DOC "Resource/BinaryLight1.xml"
#define SWITCH_SERVICE "urn:schemas-upnp-org:service:SwitchPower:1"
#define DIMMING_SERVICE "urn:schemas-upnp-org:service:Dimming:1"
#define COLOR_SERVICE "urn:schemas-upnp-org:service:ColorChange:1"

G_MODULE_EXPORT
void set_target_cb(GUPnPService *service, GUPnPServiceAction *action, gpointer user_data) {

    gboolean target;
    gupnp_service_action_get(action, "NewTargetValue", G_TYPE_BOOLEAN, &target, NULL);

    switchControl(target);
    /*
    gupnp_service_notify(service, "Status", G_TYPE_BOOLEAN, status, NULL);
    g_print("Status : %s.\n", status ? "on" : "off");
    */
    gupnp_service_notify(service, "Status", G_TYPE_BOOLEAN, target, NULL);
    gupnp_service_action_return(action);

}

G_MODULE_EXPORT
void set_loadlevel_cb(GUPnPService *service, GUPnPServiceAction *action, gpointer user_data){
    guint loadLevel;
    gupnp_service_action_get(action, "newLoadlevelTarget", G_TYPE_UINT, &loadLevel, NULL);

    dimmingControl(loadLevel);
    /*
    //dimming = loadLevel;
    //g_print("loadLevel : %d.\n", dimming);
    */
    gupnp_service_notify(service, "LoadLevelStatus", G_TYPE_UINT, loadLevel, NULL);
    gupnp_service_action_return(action);
}

G_MODULE_EXPORT
void set_colorlevel_cb(GUPnPService *service, GUPnPServiceAction *action, gpointer user_data){
    guint redLevelChange;
    guint greenLevelChange;
    guint blueLevelChange;

    gupnp_service_action_get(action, "newRedTarget", G_TYPE_UINT, &redLevelChange, NULL);
    gupnp_service_action_get(action, "newGreenTarget", G_TYPE_UINT, &greenLevelChange, NULL);
    gupnp_service_action_get(action, "newBlueTarget", G_TYPE_UINT, &blueLevelChange, NULL);

    colorControl(redLevelChange, greenLevelChange, blueLevelChange);

    /*
    redLevel = redLevelChange;
    greenLevel = greenLevelChange;
    blueLevel = blueLevelChange;
    g_print("R: %d. G: %d. B: %d.\n", redLevel, greenLevel, blueLevel);
    */
    gupnp_service_notify(service, "ColorRedStatus", G_TYPE_UINT, redLevelChange, NULL);
    gupnp_service_notify(service, "ColorGreenStatus", G_TYPE_UINT, greenLevelChange, NULL);
    gupnp_service_notify(service, "ColorBlueStatus", G_TYPE_UINT, blueLevelChange, NULL);
    gupnp_service_action_return(action);

}

void upnpDump() {

    //create gupnp context or device
    GUPnPContext *context;
    context = gupnp_context_new(NULL, NULL, 0, NULL);
    GUPnPRootDevice *dev;
    dev = gupnp_root_device_new(context, DEVICE_DOC, ".");
    gupnp_root_device_set_available(dev, TRUE);

    //define the upnp service
    GUPnPServiceInfo *switchService;
    switchService = gupnp_device_info_get_service(GUPNP_DEVICE_INFO(dev), SWITCH_SERVICE);

    GUPnPServiceInfo *dimmingService;
    dimmingService = gupnp_device_info_get_service(GUPNP_DEVICE_INFO(dev), DIMMING_SERVICE);

    GUPnPServiceInfo *colorService;
    colorService = gupnp_device_info_get_service(GUPNP_DEVICE_INFO(dev), COLOR_SERVICE);

    g_signal_connect(switchService, "action-invoked::SetTarget", G_CALLBACK(set_target_cb), NULL);
    g_signal_connect(dimmingService, "action-invoked::SetLoadLevelTarget", G_CALLBACK(set_loadlevel_cb), NULL);
    g_signal_connect(colorService, "action-invoked::SetColorChangeTarget", G_CALLBACK(set_colorlevel_cb), NULL);

}
