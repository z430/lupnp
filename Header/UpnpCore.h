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

void notify_status_change(gboolean status);
void notify_load_level_change(gint load_level);
void notify_color_change(gint red, gint green, gint blue);
gboolean init_upnp();
void deinit_upnp();
void set_all_status(gboolean status);
void set_all_load_level (gint load_level);
void set_all_color_status(gint red, gint green, gint blue);
void upnpDump();


#endif //LUPNP_UPNPCORE_H
