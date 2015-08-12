//
// Created by myth on 8/8/15.
//

#include "UpnpCore.h"
#include <libgupnp/gupnp.h>
#include <stdlib.h>
#include <gmodule.h>

static gboolean status;
static gboolean quiet;

/*
 * Action Routine
 */

/* Set Target */
