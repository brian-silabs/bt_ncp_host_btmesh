#ifndef SL_BTMESH_H
#define SL_BTMESH_H

#include <stdbool.h>
#include "sl_btmesh_api.h"
#include "sl_btmesh_stack_init.h"
#include "sl_bt_api.h"
#include "sl_btmesh_bgapi.h"

#define SL_BTMESH_FEATURE_BITMASK 0

// Initialize Bluetooth core functionality
void sl_btmesh_init(void);

// Polls bluetooth stack for an event and processes it
void sl_btmesh_step(void);

// Processes a single bluetooth mesh event
void sl_btmesh_process_event(sl_btmesh_msg_t *evt);

bool sl_btmesh_can_process_event(uint32_t len);

void sl_btmesh_on_event(sl_btmesh_msg_t* evt);


#endif // SL_BTMESH_H
