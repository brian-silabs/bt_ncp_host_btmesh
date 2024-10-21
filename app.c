/***************************************************************************//**
 * @file
 * @brief Core application logic.
 *******************************************************************************
 * # License
 * <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 ******************************************************************************/
#include "em_common.h"
#include "app.h"
#include "app_assert.h"
#include "sl_bluetooth.h"
#include "gatt_db.h"
#include "sl_component_catalog.h"
#if defined(SL_CATALOG_WAKE_LOCK_PRESENT)
#include "sl_wake_lock.h"
#endif // SL_CATALOG_WAKE_LOCK_PRESENT
#include "sl_btmesh_ncp_host.h"

// The advertising set handle allocated from Bluetooth stack.
static uint8_t advertising_set_handle = 0xff;

uint8_t     A_BLE_OOB_Authentication[32]   =              {0x11,0x11,0x11,0x11,
                                                           0x11,0x11,0x11,0x11,
                                                           0x11,0x11,0x11,0x11,
                                                           0x11,0x11,0x11,0x11,
                                                           0x11,0x11,0x11,0x11,
                                                           0x11,0x11,0x11,0x11,
                                                           0x11,0x11,0x11,0x11,
                                                           0x11,0x11,0x11,0x11}; // Temporary OOB Key for BLE Mesh Provisioning


/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
SL_WEAK void app_init(void)
{
  // Reset NCP to ensure it gets into a defined state.
  // Once the chip successfully boots, boot event should be received.
  sl_bt_system_reset(sl_bt_system_boot_mode_normal);

  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application init code here!                         //
  // This is called once during start-up.                                    //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
SL_WEAK void app_process_action(void)
{
  /////////////////////////////////////////////////////////////////////////////
  // Put your additional application code here!                              //
  // This is called infinitely.                                              //
  // Do not call blocking functions from here!                               //
  /////////////////////////////////////////////////////////////////////////////
}

/**************************************************************************//**
 * Bluetooth stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth stack.
 *****************************************************************************/
void sl_bt_on_event(sl_bt_msg_t *evt)
{
  sl_status_t sc;

  switch (SL_BT_MSG_ID(evt->header)) {
    // -------------------------------
    // This event indicates the device has started and the radio is ready.
    // Do not call any stack command before receiving this boot event!
    case sl_bt_evt_system_boot_id:
      // Initialize Mesh stack in Node operation mode,
      // wait for initialized event
      sc = sl_btmesh_node_set_provisioning_algorithms(  sl_btmesh_node_algorithm_flag_ecdh_p256_cmac_aes128_aes_ccm
                                                      | sl_btmesh_node_algorithm_flag_ecdh_p256_hmac_sha256_aes_ccm
                                                      ); // SL_STATUS_INVALID_PARAMETER if aes128 only
      app_assert(sc == SL_STATUS_OK,
                 "[E: 0x%04x] Failed to set algorithms\n",
                 (int)sc);
      sc = sl_btmesh_node_init_oob(0x00,0x02,0x0,0x0,0x0,0x0,0x0);
      //sc = sl_btmesh_node_init();
      app_assert(sc == SL_STATUS_OK,
                 "[E: 0x%04x] Failed to init node\n",
                 (int)sc);
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////

        case sl_bt_evt_connection_opened_id:
      {
        uint32_t conn_handle;
        conn_handle     = evt->data.evt_connection_opened.connection;

      }
      break;

    case sl_bt_evt_connection_closed_id:
      {
        uint32_t closedReason;
        closedReason = evt->data.evt_connection_closed.reason;
      }
      break;

    // -------------------------------
    // Default event handler.
    default:
      break;
  }
}

/**************************************************************************//**
 * Bluetooth Mesh stack event handler.
 * This overrides the dummy weak implementation.
 *
 * @param[in] evt Event coming from the Bluetooth Mesh stack.
 *****************************************************************************/
void sl_btmesh_on_event(sl_btmesh_msg_t *evt)
{
  sl_status_t sc;
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_btmesh_evt_node_initialized_id:
      if (!evt->data.evt_node_initialized.provisioned) {
        // The Node is now initialized,
        // start unprovisioned Beaconing using PB-ADV and PB-GATT Bearers
        sc = sl_btmesh_node_start_unprov_beaconing(0x3);
        app_assert(sc == SL_STATUS_OK,
                   "[E: 0x%04x] Failed to start unprovisioned beaconing\n",
                   (int)sc);
      }
      break;

    ///////////////////////////////////////////////////////////////////////////
    // Add additional event handlers here as your application requires!      //
    ///////////////////////////////////////////////////////////////////////////

      /*Provisioning process started event*/
    case sl_btmesh_evt_node_provisioning_started_id:
      {
        uint32_t result;
        result = evt->data.evt_node_provisioning_started.result;
      }
      break;

      /*  Static OOB connection request event*/
    case sl_btmesh_evt_node_static_oob_request_id:
      {
        sc = sl_btmesh_node_send_input_oob_request_response(
            sizeof(A_BLE_OOB_Authentication),(void *)&A_BLE_OOB_Authentication); // OOB response with static OOB key
        app_assert(sc == SL_STATUS_OK,
            "[E: 0x%04x] Failed to send OOB response\n",
            (int)sc);
      }
      break;

      /*Provisioning process success event*/
    case sl_btmesh_evt_node_provisioned_id:
      {
      }
      break;

      /* Provisioning process failure event*/
    case sl_btmesh_evt_node_provisioning_failed_id:
      {
        uint32_t result;
        result = evt->data.evt_node_provisioning_failed.result;
        app_assert(sc == SL_STATUS_OK,
            "[E: 0x%04x] Failed to perform provisioning\n",
            (int)result);
      }
      break;

    // -------------------------------
    // Default event handler.
    default:
      break;
  }
}

