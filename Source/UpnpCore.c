//
// Created by myth on 8/8/15.
//

#include "../Header/HardwareHandler.h"
#include "../Header/UpnpCore.h"
#include <libgupnp/gupnp.h>
#include <string.h>

#define DEVICE_DOC "Resource/BinaryLight1.xml"
#define SWITCH_SERVICE "urn:schemas-upnp-org:service:SwitchPower:1"
#define DIMMING_SERVICE "urn:schemas-upnp-org:service:Dimming:1"
#define NETWORK_LIGHT "urn:schemas-upnp-org:device:BinaryLight:1"
#define COLOR_SERVICE "urn:schemas-upnp-org:service:ColorChange:1"

typedef struct {
    GUPnPRootDevice *dev;
    GUPnPServiceInfo *switchPower;
    GUPnPServiceInfo *dimming;
    GUPnPServiceInfo *color;
} NetworkLight;

static GUPnPContextManager *contextManager;
static GHashTable *nlHash;

static GList *switchProxies;
static GList *dimmingProxies;
static GList *colorProxies;

static GUPnPXMLDoc *doc;
static char *desc_location;

static NetworkLight * networkLightNew (GUPnPRootDevice *dev,
                                       GUPnPServiceInfo *switchPower,
                                       GUPnPServiceInfo *dimming){
    NetworkLight *networkLight;
    networkLight = g_slice_new(NetworkLight);
    networkLight->dev = dev;
    networkLight->switchPower = switchPower;
    networkLight->dimming = dimming;
//    networkLight->color = color;

    return networkLight;
}

static void networkLightFree(NetworkLight *networkLight){
    g_object_unref(networkLight->dev);
    g_object_unref(networkLight->switchPower);
    g_object_unref(networkLight->dimming);
    g_object_unref(networkLight->color);

    g_slice_free(NetworkLight, networkLight);
}

void notifyStatusChange(gboolean status){
    GList *network_lights;
    GList *nl_node;

    network_lights = g_hash_table_get_values (nlHash);

    for (nl_node = network_lights;
         nl_node != NULL;
         nl_node = nl_node->next) {
        NetworkLight *nl = (NetworkLight *) nl_node->data;

        gupnp_service_notify (GUPNP_SERVICE (nl->switchPower),
                              "Status",
                              G_TYPE_BOOLEAN,
                              status,
                              NULL);
    }
}

void notifyLoadLevelChange(gint loadLevel){
    GList *network_lights;
    GList *nl_node;

    network_lights = g_hash_table_get_values (nlHash);

    for (nl_node = network_lights;
         nl_node != NULL;
         nl_node = nl_node->next) {
        NetworkLight *nl = (NetworkLight *) nl_node->data;

        gupnp_service_notify (GUPNP_SERVICE (nl->dimming),
                              "LoadLevelStatus",
                              G_TYPE_UINT,
                              loadLevel,
                              NULL);
    }
}

void notifyColorChange(gint red, gint green, gint blue){
    GList *networkLights;
    GList *nl_node;

    networkLights = g_hash_table_get_values(nlHash);
    for(nl_node = networkLights; nl_node != NULL; nl_node->next){
        NetworkLight *nl = (NetworkLight *)nl_node->data;
        gupnp_service_notify(GUPNP_SERVICE(nl->color), "ColorRedStatus", G_TYPE_UINT, red, NULL);
        gupnp_service_notify(GUPNP_SERVICE(nl->color), "ColorGreenStatus", G_TYPE_UINT, green, NULL);
        gupnp_service_notify(GUPNP_SERVICE(nl->color), "ColorBlueStatus", G_TYPE_UINT, blue, NULL);
    }
}

gint get_load_level(){
    return loadLevel;
}

G_MODULE_EXPORT
void on_get_status(GUPnPService *service, GUPnPServiceAction *action, gpointer user_data){
    gupnp_service_action_set(action, "ResultStatus", G_TYPE_BOOLEAN, target, NULL);
    gupnp_service_action_return(action);
}

G_MODULE_EXPORT
void on_get_target(GUPnPService *service, GUPnPServiceAction *action, gpointer user_data){
    gupnp_service_action_set(action, "RetTargetValue", G_TYPE_BOOLEAN, target, NULL);
    gupnp_service_action_return(action);
}

G_MODULE_EXPORT
void on_set_target(GUPnPService *service, GUPnPServiceAction *action, gpointer user_data) {

    gupnp_service_action_get(action, "NewTargetValue", G_TYPE_BOOLEAN, &target, NULL);
    switchControl(target);
    g_print("Status: %d\n", target);
    gupnp_service_action_return(action);
}

G_MODULE_EXPORT
void on_query_status (GUPnPService *service, const char *variable_name, GValue *value, gpointer user_data){
    g_value_init (value, G_TYPE_BOOLEAN);
    g_value_set_boolean (value, target);
}

G_MODULE_EXPORT
void on_query_target (GUPnPService *service, const char *variable_name, GValue *value, gpointer user_data){
    g_value_init (value, G_TYPE_BOOLEAN);
    g_value_set_boolean (value, target);
}

G_MODULE_EXPORT
void on_get_load_level_status(GUPnPService *service, GUPnPServiceAction *action, gpointer user_data){
    gupnp_service_action_set(action, "retLoadLevelStatus", G_TYPE_UINT, get_load_level(), NULL);
    gupnp_service_action_return(action);
}

G_MODULE_EXPORT
void on_get_load_level_target(GUPnPService *service, GUPnPServiceAction *action, gpointer user_data){
    gupnp_service_action_set(action, "retLoadLevelTarget", G_TYPE_UINT, get_load_level(), NULL);
    gupnp_service_action_return(action);
}

G_MODULE_EXPORT
void on_set_load_level_target(GUPnPService *service, GUPnPServiceAction *action, gpointer user_data){

    gupnp_service_action_get(action, "newLoadlevelTarget", G_TYPE_UINT, &loadLevel, NULL);
    dimmingControl(loadLevel);
    g_print("Level: %d\n", loadLevel);
    gupnp_service_action_return(action);
}

G_MODULE_EXPORT
void on_query_load_level_status (GUPnPService *service, const char *variable, GValue *value, gpointer user_data){
    g_value_init(value, G_TYPE_UINT);
    g_value_set_uint(value, loadLevel);
}

G_MODULE_EXPORT
void on_query_load_level_target (GUPnPService *service, const char *variable, GValue *value, gpointer user_data){
    g_value_init(value, G_TYPE_UINT);
    g_value_set_uint(value, loadLevel);
}

G_MODULE_EXPORT
void on_set_colorlevel(GUPnPService *service, GUPnPServiceAction *action, gpointer user_data){

    gupnp_service_action_get(action, "newRedTarget", G_TYPE_UINT, &redLevelChange, NULL);
    gupnp_service_action_get(action, "newGreenTarget", G_TYPE_UINT, &greenLevelChange, NULL);
    gupnp_service_action_get(action, "newBlueTarget", G_TYPE_UINT, &blueLevelChange, NULL);

    colorControl(redLevelChange, greenLevelChange, blueLevelChange);

    gupnp_service_notify(service, "ColorRedStatus", G_TYPE_UINT, redLevelChange, NULL);
    gupnp_service_notify(service, "ColorGreenStatus", G_TYPE_UINT, greenLevelChange, NULL);
    gupnp_service_notify(service, "ColorBlueStatus", G_TYPE_UINT, blueLevelChange, NULL);
    gupnp_service_action_return(action);

}

G_MODULE_EXPORT
void on_query_color_status (GUPnPService *service, gchar *variable, gpointer value, gpointer user_data){
    g_value_init(value, G_TYPE_UINT);
    g_value_set_uint(value, redLevelChange);
    g_value_set_uint(value, greenLevelChange);
    g_value_set_uint(value, blueLevelChange);
}

static void on_notify_failed(GUPnPService *service, const GList *callback_urls, const GError *reason, gpointer user_data){
    GList *url_node;
    GString *warning;

    warning = g_string_new(NULL);
    g_string_printf(warning, "NOTIFY failed for the following client URLs:\n");
    for(url_node = (GList *) callback_urls; url_node; url_node = url_node->next){
        g_string_append_printf(warning, "%s\n", (char *) url_node->data);
    }

    g_string_append_printf(warning, "Reason: %s", reason->message);
    g_string_free(warning, TRUE);
}

static void on_network_light_available (GUPnPControlPoint *cp, GUPnPDeviceProxy  *light_proxy, gpointer user_data){

    GUPnPServiceProxy *switch_proxy;
    GUPnPServiceProxy *dimming_proxy;
    GUPnPServiceInfo  *info;

    info = gupnp_device_info_get_service (GUPNP_DEVICE_INFO (light_proxy),
                                          SWITCH_SERVICE);
    switch_proxy = GUPNP_SERVICE_PROXY (info);

    if (switch_proxy) {
        switchProxies = g_list_append (switchProxies, switch_proxy);
    }

    info = gupnp_device_info_get_service (GUPNP_DEVICE_INFO (light_proxy),
                                          DIMMING_SERVICE);
    dimming_proxy = GUPNP_SERVICE_PROXY (info);

    if (dimming_proxy) {
        dimmingProxies = g_list_append (dimmingProxies,
                                         dimming_proxy);
    }
}

static gint compare_service(GUPnPServiceInfo *info1, GUPnPServiceInfo *info2){
    const char *udn1;
    const char *udn2;

    udn1 = gupnp_service_info_get_udn(info1);
    udn2 = gupnp_service_info_get_udn(info2);

    return strcmp(udn1, udn2);
}

static void remove_service_from_list(GUPnPServiceInfo *info, GList **list){
    GList *proxy_node;
    proxy_node = g_list_find_custom(*list, info, (GCompareFunc) compare_service);

    if(proxy_node){
        g_object_unref(proxy_node->data);
        *list = g_list_remove(*list, proxy_node->data);
    }
}

static void on_network_light_unavailable(GUPnPControlPoint *controlPoint, GUPnPDeviceProxy *lightProxy, gpointer user_data){
    GUPnPServiceInfo *info;
    info = gupnp_device_info_get_service(GUPNP_DEVICE_INFO(lightProxy), SWITCH_SERVICE);
    if(info){
        remove_service_from_list(info, &switchProxies);
    }
    info = gupnp_device_info_get_service(GUPNP_DEVICE_INFO(lightProxy), DIMMING_SERVICE);
    if(info){
        remove_service_from_list(info, &dimmingProxies);
    }
}

static gboolean init_server(GUPnPContext *context){
    NetworkLight *networkLight;
    GUPnPRootDevice *dev;
    GUPnPServiceInfo *switchPower;
    GUPnPServiceInfo *dimming;
    GError *error = NULL;

    /* Create root device */
    dev = gupnp_root_device_new_full(context, gupnp_resource_factory_get_default(), NULL, DEVICE_DOC, ".");

    switchPower = gupnp_device_info_get_service(GUPNP_DEVICE_INFO(dev), SWITCH_SERVICE);

    if(switchPower){
        gupnp_service_signals_autoconnect(GUPNP_SERVICE(switchPower), NULL, &error);
        g_signal_connect(switchPower, "notify-failed", G_CALLBACK(on_notify_failed), NULL);
    }

    dimming = gupnp_device_info_get_service(GUPNP_DEVICE_INFO(dev), DIMMING_SERVICE);

    if(dimming){
        gupnp_service_signals_autoconnect(GUPNP_SERVICE(dimming), NULL, &error);
        g_signal_connect(dimming, "notify-failed", G_CALLBACK(on_notify_failed), NULL);
    }

    networkLight = networkLightNew(dev, switchPower, dimming);
    g_hash_table_insert(nlHash, g_object_ref(context), networkLight);
    /* RUN */
    gupnp_root_device_set_available(dev, true);
    g_print("Attaching to IP/HOST %s on port %d\n", gupnp_context_get_host_ip(context), gupnp_context_get_port(context));

    return true;
}

static gboolean init_client (GUPnPContext *context){
    GUPnPControlPoint *cp;

    cp = gupnp_control_point_new (context, NETWORK_LIGHT);

    g_signal_connect (cp,
                      "device-proxy-available",
                      G_CALLBACK (on_network_light_available),
                      NULL);
    g_signal_connect (cp,
                      "device-proxy-unavailable",
                      G_CALLBACK (on_network_light_unavailable),
                      NULL);

    gssdp_resource_browser_set_active (GSSDP_RESOURCE_BROWSER (cp), TRUE);

    /* Let context manager take care of the control point life cycle */
    gupnp_context_manager_manage_control_point (contextManager, cp);

    /* We don't need to keep our own references to the control points */
    g_object_unref (cp);

    return TRUE;
}

static void on_context_available (GUPnPContextManager *context_manager, GUPnPContext *context, gpointer user_data){
    /* Initialize client-side stuff */
    init_client (context);

    /* Then the server-side stuff */
    init_server (context);
}

static void on_context_unavailable (GUPnPContextManager *context_manager, GUPnPContext *context, gpointer user_data){
    g_print ("Dettaching from IP/Host %s and port %d\n",
             gupnp_context_get_host_ip (context),
             gupnp_context_get_port (context));

    g_hash_table_remove (nlHash, context);
}

static gboolean context_equal (GUPnPContext *context1, GUPnPContext *context2) {
    return g_ascii_strcasecmp (gupnp_context_get_host_ip (context1),
                               gupnp_context_get_host_ip (context2)) == 0;
}

gboolean initUPnP (void) {
    GError *error = NULL;

    switchProxies = NULL;
    dimmingProxies = NULL;

    nlHash = g_hash_table_new_full (g_direct_hash,
                                     (GEqualFunc) context_equal,
                                     g_object_unref,
                                     (GDestroyNotify) networkLightFree);

    contextManager = gupnp_context_manager_new (NULL, 0);
    g_assert (contextManager != NULL);

    g_signal_connect (contextManager,
                      "context-available",
                      G_CALLBACK (on_context_available),
                      NULL);
    g_signal_connect (contextManager,
                      "context-unavailable",
                      G_CALLBACK (on_context_unavailable),
                      NULL);

    return true;
}

void deinitUPnP(void){
    g_object_unref (contextManager);

    g_hash_table_unref (nlHash);

    g_list_foreach (switchProxies, (GFunc) g_object_unref, NULL);
    g_list_foreach (dimmingProxies, (GFunc) g_object_unref, NULL);

    /* Unref the descriptiont doc */
    g_object_unref (doc);
    g_free (desc_location);
}
