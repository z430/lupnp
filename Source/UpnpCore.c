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
#define NETWORK_LIGHT "urn:schemas-upnp-org:device:BinaryLight:1"

typedef struct {
    GUPnPRootDevice *dev;
    GUPnPServiceInfo *color;
    GUPnPServiceInfo *dimming;
    GUPnPServiceInfo *switch_power;
}NetworkLight;

static GUPnPContextManager *context_manager;
static GHashTable *nl_hash;

static GList *switch_proxies;
static GList *dimming_proxies;
static GList *color_proxies;

static GUPnPXMLDoc *doc;
static char *desc_location;
static char uuid[37];

static NetworkLight *network_light_new(GUPnPRootDevice *dev, GUPnPServiceInfo *color,
                                       GUPnPServiceInfo *dimming, GUPnPServiceInfo *switch_power){
    NetworkLight *network_light;
    network_light = g_slice_new(NetworkLight);
    network_light->dev = dev;
    network_light->color = color;
    network_light->dimming = dimming;
    network_light->switch_power = switch_power;

    return  network_light;
}

static void network_light_free(NetworkLight *network_light){
    g_object_unref(network_light->dev);
    g_object_unref(network_light->color);
    g_object_unref(network_light->dimming);
    g_object_unref(network_light->switch_power);

    g_slice_free(NetworkLight, network_light);
}

void notify_color_change(gint red, gint green, gint blue){
    GList *network_lights;
    GList *nl_node;

    network_lights = g_hash_table_get_values(nl_hash);
    for(nl_node = network_lights;
            nl_node != NULL;
            nl_node = nl_node->next){
        NetworkLight *nl =  (NetworkLight *)nl_node->data;
        gupnp_service_notify(GUPNP_SERVICE(nl->switch_power), "ColorRedStatus", G_TYPE_UINT, red, NULL);
        gupnp_service_notify(GUPNP_SERVICE(nl->switch_power), "ColorGreenStatus", G_TYPE_UINT, green, NULL);
        gupnp_service_notify(GUPNP_SERVICE(nl->switch_power), "ColorBlueStatus", G_TYPE_UINT, blue, NULL);
    }
    g_print("notify color change\n");
}

void notify_load_level_change(gint load_level){
    GList *network_lights;
    GList *nl_node;

    network_lights = g_hash_table_get_values(nl_hash);
    for(nl_node = network_lights;
        nl_node != NULL;
        nl_node = nl_node->next){
        NetworkLight *nl =  (NetworkLight *)nl_node->data;
        gupnp_service_notify(GUPNP_SERVICE(nl->dimming), "LoadLevelStatus", G_TYPE_UINT, load_level, NULL);
    }
    g_print("notify dimming change\n");
}

void notify_status_change(gint status){
    GList *network_lights;
    GList *nl_node;

    network_lights = g_hash_table_get_values(nl_hash);
    for(nl_node = network_lights;
        nl_node != NULL;
        nl_node = nl_node->next){
        NetworkLight *nl =  (NetworkLight *)nl_node->data;
        gupnp_service_notify(GUPNP_SERVICE(nl->switch_power), "Status", G_TYPE_UINT, status, NULL);
    }
    g_print("notify status change\n");
}

static void on_notify_failed(GUPnPService *service, const GList *callback_urls, const GError *reason, gpointer user_data){
    GList *url_node;
    GString *warning;
    warning = g_string_new(NULL);
    g_string_printf(warning, "NOTIFY failed for the following client URLs:\n");
    for(url_node = (GList *) callback_urls;
            url_node;
            url_node = url_node->next){
        g_string_append_printf(warning, "%s\n", (char *) url_node->data);
    }
    g_string_append_printf(warning, "Reason: %s", reason->message);
    g_string_free(warning, true);
}

/* Color Service Module
 * */

G_MODULE_EXPORT
void on_get_color_target(GUPnPService *service, GUPnPServiceAction *action, gpointer user_data){
    gupnp_service_action_set(action, "retRedTarget", G_TYPE_UINT, redLevelChange, NULL);
    gupnp_service_action_set(action, "retGreenTarget", G_TYPE_UINT, greenLevelChange, NULL);
    gupnp_service_action_set(action, "retBlueTarget", G_TYPE_UINT, blueLevelChange, NULL);

    g_print("get_color_target\n");

    gupnp_service_action_return(action);
}

G_MODULE_EXPORT
void on_get_color_status(GUPnPService *service, GUPnPServiceAction *action, gpointer user_data){
    gupnp_service_action_set(action, "RedStatus", G_TYPE_UINT, redLevelChange, NULL);
    gupnp_service_action_set(action, "GreenStatus", G_TYPE_UINT, greenLevelChange, NULL);
    gupnp_service_action_set(action, "BlueStatus", G_TYPE_UINT, blueLevelChange, NULL);

    g_print("get_color_status\n");

    gupnp_service_action_return(action);
}

G_MODULE_EXPORT
void on_set_color_target(GUPnPService *service, GUPnPServiceAction *action, gpointer user_data){
    gupnp_service_action_get(action, "newRedTarget", G_TYPE_UINT, &redLevelChange, NULL);
    gupnp_service_action_get(action, "newGreenTarget", G_TYPE_UINT, &greenLevelChange, NULL);
    gupnp_service_action_get(action, "newBlueTarget", G_TYPE_UINT, &blueLevelChange, NULL);

    colorControl(redLevelChange, greenLevelChange, blueLevelChange);
    g_print("color: %d %d %d\n", redLevelChange, greenLevelChange, blueLevelChange);
    gupnp_service_action_return(action);
}

G_MODULE_EXPORT
void on_query_color_status(GUPnPService *service, const char *variable_name, GValue *value, gpointer user_data){
    g_value_init(value, G_TYPE_UINT);
    g_value_set_uint(value, redLevelChange);
    g_value_set_uint(value, greenLevelChange);
    g_value_set_uint(value, blueLevelChange);
    g_print("color query status\n");
}

G_MODULE_EXPORT
void on_query_color_target(GUPnPService *service, const char *variable_name, GValue *value, gpointer user_data){
    g_value_init(value, G_TYPE_UINT);
    g_value_set_uint(value, redLevelChange);
    g_value_set_uint(value, greenLevelChange);
    g_value_set_uint(value, blueLevelChange);
    g_print("color query target\n");
}

/*----------------------------------------END---------------------------------------------------------*/

/* Dimming Service Module
 * */

G_MODULE_EXPORT
void on_get_load_level_status(GUPnPService *service, GUPnPServiceAction *action, gpointer user_data){
    gupnp_service_action_get(action, "retLoadlevelStatus", G_TYPE_UINT, loadLevel, NULL);
    gupnp_service_action_return(action);
    g_print("load level status\n");
}

G_MODULE_EXPORT
void on_get_load_level_target(GUPnPService *service, GUPnPServiceAction *action, gpointer user_data){
    gupnp_service_action_get(action, "retLoadlevelTarget", G_TYPE_UINT, loadLevel, NULL);
    gupnp_service_action_return(action);
    g_print("load level target\n");
}

G_MODULE_EXPORT
void on_set_load_level_target(GUPnPService *service, GUPnPServiceAction *action, gpointer user_data){
    gupnp_service_action_get(action, "newLoadlevelTarget", G_TYPE_UINT, &loadLevel, NULL);
    dimmingControl(loadLevel);
    g_print("dimming: %d\n", loadLevel);
    gupnp_service_action_return(action);
}

G_MODULE_EXPORT
void on_query_load_level_status(GUPnPService *service, const char *variable_name, GValue *value, gpointer user_data){
    g_value_init(value, G_TYPE_UINT);
    g_value_set_uint(value, loadLevel);
}

G_MODULE_EXPORT
void on_query_load_level_target(GUPnPService *service, const char *variable_name, GValue *value, gpointer user_data){
    g_value_init(value, G_TYPE_UINT);
    g_value_set_uint(value, loadLevel);
}
/*----------------------------------------END---------------------------------------------------------*/

/* Switch Service Module
 * */

G_MODULE_EXPORT
void on_get_status(GUPnPService *service, GUPnPServiceAction *action, gpointer user_data){
    gupnp_service_action_get(action, "ResultStatus", G_TYPE_BOOLEAN, target, NULL);
    gupnp_service_action_return(action);
}

G_MODULE_EXPORT
void on_get_target(GUPnPService *service, GUPnPServiceAction *action, gpointer user_data){
    gupnp_service_action_get(action, "RetTargetValue", G_TYPE_BOOLEAN, target, NULL);
    gupnp_service_action_return(action);
}

G_MODULE_EXPORT
void on_set_target(GUPnPService *service, GUPnPServiceAction *action, gpointer user_data){
    gupnp_service_action_get(action, "newTargetValue", G_TYPE_BOOLEAN, &target, NULL);
    switchControl(target);
    g_print("status: %d\n", target);
//    target = switchStatus;
    gupnp_service_notify (service,  "Status", G_TYPE_BOOLEAN, target,NULL);
    gupnp_service_action_return(action);
}

G_MODULE_EXPORT
void on_query_status (GUPnPService *service, const char *variable_name, GValue *value, gpointer user_data){
    g_value_init (value, G_TYPE_BOOLEAN);
    g_value_set_boolean (value, target);
    g_print("query status\n");
}

G_MODULE_EXPORT
void on_query_target (GUPnPService *service, const char *variable_name, GValue *value, gpointer user_data){
    g_value_init (value, G_TYPE_BOOLEAN);
    g_value_set_boolean (value, target);
    g_print("query target\n");
}

/*----------------------------------------END---------------------------------------------------------*/

void on_service_proxy_action_ret(GUPnPServiceProxy *proxy, GUPnPServiceProxyAction *action, gpointer user_data){
    GError *error = NULL;
    gupnp_service_proxy_end_action(proxy, action, &error, NULL);
    if(error){
        GUPnPServiceInfo *info = GUPNP_SERVICE_INFO(proxy);
        g_warning("Failed to call action \"%s\" on \"%s\": %s", (char *)user_data,
                  gupnp_service_info_get_location(info), error->message);
        g_error_free(error);
    }
    g_free(user_data);
}

void set_all_color_status(gint red, gint green, gint blue){
    GList *proxy_node;
    for (proxy_node = color_proxies; proxy_node; proxy_node = g_list_next(proxy_node)){
        GUPnPServiceProxy *proxy;
        char *action_name;
        proxy = GUPNP_SERVICE_PROXY(proxy_node->data);
        action_name = g_strdup("SetColorChangeTarget");
        gupnp_service_proxy_begin_action(proxy, action_name, on_service_proxy_action_ret, action_name,
                                         "newRedTarget", G_TYPE_UINT, red, NULL);
        gupnp_service_proxy_begin_action(proxy, action_name, on_service_proxy_action_ret, action_name,
                                         "newGreenTarget", G_TYPE_UINT, green, NULL);
        gupnp_service_proxy_begin_action(proxy, action_name, on_service_proxy_action_ret, action_name,
                                         "newBlueTarget", G_TYPE_UINT, blue, NULL);
    }
}

void set_all_load_level (gint load_level)
{
    GList *proxy_node;

    for (proxy_node = dimming_proxies; proxy_node; proxy_node = g_list_next (proxy_node)) {
        GUPnPServiceProxy *proxy;
        char *action_name;

        proxy = GUPNP_SERVICE_PROXY (proxy_node->data);
        action_name = g_strdup ("SetLoadLevelTarget");
        gupnp_service_proxy_begin_action (proxy, action_name, on_service_proxy_action_ret,
                                          action_name, "newLoadlevelTarget", G_TYPE_UINT, load_level, NULL);
    }
}

void set_all_status (gboolean status)
{
    GList *proxy_node;

    for (proxy_node = switch_proxies; proxy_node; proxy_node = g_list_next (proxy_node)) {
        GUPnPServiceProxy *proxy;
        char *action_name;

        proxy = GUPNP_SERVICE_PROXY (proxy_node->data);
        action_name = g_strdup ("SetTarget");
        g_print("error");
        gupnp_service_proxy_begin_action (proxy, action_name, on_service_proxy_action_ret, action_name,
                                          "NewTargetValue", G_TYPE_BOOLEAN, status, NULL);
    }
}

static void on_network_light_available(GUPnPControlPoint *cp, GUPnPDeviceProxy *light_proxy, gpointer user_data){
    GUPnPServiceProxy *color_proxy;
    GUPnPServiceProxy *dimming_proxy;
    GUPnPServiceProxy *switch_proxy;
    GUPnPServiceInfo *info;

    info = gupnp_device_info_get_service(GUPNP_DEVICE_INFO(light_proxy), COLOR_SERVICE);
    color_proxy = GUPNP_SERVICE_PROXY(info);
    if(color_proxy){
        color_proxies = g_list_append(color_proxies, color_proxy);
    }

    info = gupnp_device_info_get_service (GUPNP_DEVICE_INFO (light_proxy), SWITCH_SERVICE);
    switch_proxy = GUPNP_SERVICE_PROXY (info);
    if (switch_proxy) {
        switch_proxies = g_list_append (switch_proxies, switch_proxy);
    }

    info = gupnp_device_info_get_service (GUPNP_DEVICE_INFO (light_proxy),DIMMING_SERVICE);
    dimming_proxy = GUPNP_SERVICE_PROXY (info);
    if (dimming_proxy) {
        dimming_proxies = g_list_append (dimming_proxies, dimming_proxy);
    }
}


static gboolean init_server (GUPnPContext *context)
{
    NetworkLight *network_light;
    GUPnPRootDevice *dev;
    GUPnPServiceInfo *switch_power;
    GUPnPServiceInfo *dimming;
    GUPnPServiceInfo *color;
    GError *error = NULL;

    /* Create root device */
    dev = gupnp_root_device_new_full(context, gupnp_resource_factory_get_default(), NULL, DEVICE_DOC, ".");

    color = gupnp_device_info_get_service (GUPNP_DEVICE_INFO (dev), COLOR_SERVICE);
    if (color) {
        g_signal_connect(color, "action-invoked::SetColorChangeTarget", G_CALLBACK(on_set_color_target), NULL);
        g_signal_connect(color, "action-invoked::GetColorChangeTarget", G_CALLBACK(on_get_color_target), NULL);
        g_signal_connect(color, "action-invoked::GetColorChangeStatus", G_CALLBACK(on_get_color_status), NULL);
        g_signal_connect(color, "query-variable::ColorRedStatus", G_CALLBACK(on_query_color_target), NULL);
        g_signal_connect(color, "query-variable::ColorGreenStatus", G_CALLBACK(on_query_color_target), NULL);
        g_signal_connect(color, "query-variable::ColorBlueStatus", G_CALLBACK(on_query_color_target), NULL);
        g_signal_connect (color, "notify-failed", G_CALLBACK (on_notify_failed), NULL);
    }

    switch_power = gupnp_device_info_get_service (GUPNP_DEVICE_INFO (dev), SWITCH_SERVICE);
    if (switch_power) {
      gupnp_service_signals_autoconnect (GUPNP_SERVICE (switch_power), NULL,&error);
    }

    dimming = gupnp_device_info_get_service (GUPNP_DEVICE_INFO (dev), DIMMING_SERVICE);
    if (dimming) {
        gupnp_service_signals_autoconnect (GUPNP_SERVICE (dimming), NULL, &error);
        g_signal_connect (dimming, "notify-failed", G_CALLBACK (on_notify_failed), NULL);
    }

//    color = gupnp_device_info_get_service (GUPNP_DEVICE_INFO (dev), COLOR_SERVICE);
//    switch_power = gupnp_device_info_get_service (GUPNP_DEVICE_INFO (dev), SWITCH_SERVICE);
//    dimming = gupnp_device_info_get_service (GUPNP_DEVICE_INFO (dev), DIMMING_SERVICE);

    /* Control Handler */
//    g_signal_connect(switch_power, "action-invoked::SetTarget", G_CALLBACK(on_set_target), NULL);
//    g_signal_connect(switch_power, "action-invoked::GetTarget", G_CALLBACK(on_get_target), NULL);
//    g_signal_connect(switch_power, "action-invoked::GetStatus", G_CALLBACK(on_get_status), NULL);
//    g_signal_connect(dimming, "action-invoked::SetLoadLevelTarget", G_CALLBACK(on_set_load_level_target), NULL);
//    g_signal_connect(color, "action-invoked::SetColorChangeTarget", G_CALLBACK(on_set_color_target), NULL);

    /* Subscription Handler */
//    g_signal_connect(switch_power, "query-variable::Status", G_CALLBACK(on_query_status), NULL);
//    g_signal_connect(switch_power, "query-variable::Target", G_CALLBACK(on_query_target), NULL);
//    g_signal_connect(dimming, "query-variable::LoadLevelStatus", G_CALLBACK(on_query_load_level_target), NULL);
//    g_signal_connect(color, "query-variable::ColorRedStatus", G_CALLBACK(on_query_color_target), NULL);
//    g_signal_connect(color, "query-variable::ColorGreenStatus", G_CALLBACK(on_query_color_target), NULL);
//    g_signal_connect(color, "query-variable::ColorBlueStatus", G_CALLBACK(on_query_color_target), NULL);

    network_light = network_light_new (dev, switch_power, dimming, color);
    g_hash_table_insert (nl_hash, g_object_ref (context), network_light);

    /* Run */
    gupnp_root_device_set_available (dev, TRUE);
    g_print ("Attaching to IP/Host %s on port %d\n", gupnp_context_get_host_ip (context), gupnp_context_get_port (context));

    return TRUE;
}


static gboolean init_client (GUPnPContext *context){

    GUPnPControlPoint *cp;
    cp = gupnp_control_point_new (context, NETWORK_LIGHT);
    g_signal_connect (cp, "device-proxy-available", G_CALLBACK (on_network_light_available), NULL);
    gssdp_resource_browser_set_active (GSSDP_RESOURCE_BROWSER (cp), TRUE);
    /* Let context manager take care of the control point life cycle */
    gupnp_context_manager_manage_control_point (context_manager, cp);
    /* We don't need to keep our own references to the control points */
    g_object_unref (cp);
    return TRUE;
}

static void on_context_available (GUPnPContextManager *context_manager, GUPnPContext *context, gpointer user_data){
    /* Initialize client-side stuff */
//    init_client (context);
    /* Then the server-side stuff */
    init_server (context);
}

static void on_context_unavailable (GUPnPContextManager *context_manager,
                        GUPnPContext        *context,
                        gpointer             user_data)
{
    g_print ("Dettaching from IP/Host %s and port %d\n",
             gupnp_context_get_host_ip (context),
             gupnp_context_get_port (context));

    g_hash_table_remove (nl_hash, context);
}

static gboolean context_equal (GUPnPContext *context1, GUPnPContext *context2){
    return g_ascii_strcasecmp (gupnp_context_get_host_ip (context1),
                               gupnp_context_get_host_ip (context2)) == 0;
}

gboolean init_upnp (void) {
    GError *error = NULL;

    switch_proxies = NULL;
    dimming_proxies = NULL;
    color_proxies = NULL;

    nl_hash = g_hash_table_new_full (g_direct_hash, (GEqualFunc) context_equal, g_object_unref,
                                     (GDestroyNotify) network_light_free);

    context_manager = gupnp_context_manager_new (NULL, 0);
    g_assert (context_manager != NULL);

    g_signal_connect (context_manager, "context-available", G_CALLBACK (on_context_available), NULL);
    g_signal_connect (context_manager, "context-unavailable", G_CALLBACK (on_context_unavailable), NULL);

    return TRUE;
}

void deinit_upnp (void)
{
    g_object_unref (context_manager);

    g_hash_table_unref (nl_hash);

    g_list_foreach (switch_proxies, (GFunc) g_object_unref, NULL);
    g_list_foreach (dimming_proxies, (GFunc) g_object_unref, NULL);
    g_list_foreach (color_proxies, (GFunc) g_object_unref, NULL);

    /* Unref the descriptiont doc */
    g_object_unref (doc);

}


