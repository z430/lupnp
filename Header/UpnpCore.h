//
// Created by myth on 8/8/15.
//



#ifndef LUPNP_UPNPCORE_H
#define LUPNP_UPNPCORE_H

#include <gmodule.h>


gboolean target;
guint loadLevel;

guint redLevelChange;
guint greenLevelChange;
guint blueLevelChange;

void notifyStatusChange(gboolean status);
void notifyLoadLevelChange(gint loadLevel);
void notifyColorChange(gint red, gint green, gint blue);
gboolean initUPnP();
void deinitUPnP();
void setAllStatus(gboolean status);
void setAllLoadLevel(gint loadLevel);
void setAllColor(gint red, gint green, gint blue);
void upnpDump();


#endif //LUPNP_UPNPCORE_H
