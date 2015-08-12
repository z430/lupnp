//
// Created by myth on 8/11/15.
//

#include "helper.h"
#include <libgupnp/gupnp.h>

int helper()
{
    /* impelementing device */
    GUPnPContext *context;
    //initialize required sub system
    #if !GLIB_CHECK_VERSION(2, 35, 0)
        g_type_init();
    #endif
    //create gupnp context with default host and port
    context = gupnp_context_new(NULL, NULL, 0, NULL);

    GUPnPRootDevice *rootdev;
    //create root device object
    rootdev = gupnp_root_device_new(context, "BinaryLight1.xml", ".");
    //activated the device
    gupnp_root_device_set_available(rootdev, TRUE);

    /* implementing service */
    GUPnPServiceInfo *info;
    info = gupnp_device_info_get_service(
            GUPNP_DEVICE_INFO(rootdev), "urn:schemas-upnp-org:service:SwitchPower:1"
    );




}