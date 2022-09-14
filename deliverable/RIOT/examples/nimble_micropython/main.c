/*
 * Copyright (C) 2019 Freie Universität Berlin
 * Copyright (C) 2021 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       (Mock-up) BLE heart rate sensor example with BPF
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "assert.h"
#include "event/timeout.h"
#include "nimble_riot.h"
#include "net/bluetil/ad.h"
#include "timex.h"
#include "saul.h"

#include "host/ble_hs.h"
#include "host/ble_gatt.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

#include "micropython.h"
#include "py/stackctrl.h"
#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "lib/utils/pyexec.h"

#include "blob/hrs.py.h"
#include "blob/temperature.py.h"

#include "blob/boot.py.h"

#define HRS_FLAGS_DEFAULT       (0x01)      /* 16-bit BPM value */
#define SENSOR_LOCATION         (0x02)      /* wrist sensor */
#define UPDATE_INTERVAL         (250U * US_PER_MS)
#define BPM_MIN                 (80U)
#define BPM_MAX                 (210U)
#define BPM_STEP                (2)
#define BAT_LEVEL               (42U)

static const char *_device_name = "RIOT BPF Heart Rate Sensor";
static const char *_manufacturer_name = "Unfit Bit Inc.";
static const char *_model_number = "2A";
static const char *_serial_number = "a8b302c7f3-29183-x8";
static const char *_fw_ver = "13.7.12";
static const char *_hw_ver = "V3B";

static char mp_heap[MP_RIOT_HEAPSIZE];


static mp_obj_t mp_temperature;
static mp_obj_t mp_hrs;

static struct __attribute__((packed)) {
    uint8_t flags;
    uint16_t bpm;
} _hr_data = { HRS_FLAGS_DEFAULT, (BPM_MIN + BPM_STEP) };


static event_queue_t _eq;
static event_t _update_evt;
static event_timeout_t _update_timeout_evt;

static uint16_t _conn_handle;
static uint16_t _hrs_val_handle;


static int _hrs_handler(uint16_t conn_handle, uint16_t attr_handle,
                        struct ble_gatt_access_ctxt *ctxt, void *arg);
static int _temp_handler(uint16_t conn_handle, uint16_t attr_handle,
                        struct ble_gatt_access_ctxt *ctxt, void *arg);

static int _devinfo_handler(uint16_t conn_handle, uint16_t attr_handle,
                            struct ble_gatt_access_ctxt *ctxt, void *arg);

static int _bas_handler(uint16_t conn_handle, uint16_t attr_handle,
                        struct ble_gatt_access_ctxt *ctxt, void *arg);

static void _start_advertising(void);
static void _start_updating(void);
static void _stop_updating(void);

/* GATT service definitions */
static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        /* Heart Rate Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLE_GATT_SVC_HRS),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_HEART_RATE_MEASURE),
            .access_cb = _hrs_handler,
            .val_handle = &_hrs_val_handle,
            .flags = BLE_GATT_CHR_F_NOTIFY,
        }, {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_BODY_SENSE_LOC),
            .access_cb = _hrs_handler,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            0, /* no more characteristics in this service */
        }, }
    },
    {
        /* Temperature Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLE_GATT_SVC_ENVIRON),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_TEMPERATURE),
            .access_cb = _temp_handler,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            0, /* no more characteristics in this service */
        }, }
    },
    {
        /* Device Information Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLE_GATT_SVC_DEVINFO),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_MANUFACTURER_NAME),
            .access_cb = _devinfo_handler,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_MODEL_NUMBER_STR),
            .access_cb = _devinfo_handler,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_SERIAL_NUMBER_STR),
            .access_cb = _devinfo_handler,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_FW_REV_STR),
            .access_cb = _devinfo_handler,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_HW_REV_STR),
            .access_cb = _devinfo_handler,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            0, /* no more characteristics in this service */
        }, }
    },
    {
        /* Battery Level Service */
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = BLE_UUID16_DECLARE(BLE_GATT_SVC_BAS),
        .characteristics = (struct ble_gatt_chr_def[]) { {
            .uuid = BLE_UUID16_DECLARE(BLE_GATT_CHAR_BATTERY_LEVEL),
            .access_cb = _bas_handler,
            .flags = BLE_GATT_CHR_F_READ,
        }, {
            0, /* no more characteristics in this service */
        }, }
    },
    {
        0, /* no more services */
    },
};

static int _hrs_handler(uint16_t conn_handle, uint16_t attr_handle,
                        struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    if (ble_uuid_u16(ctxt->chr->uuid) != BLE_GATT_CHAR_BODY_SENSE_LOC) {
        return BLE_ATT_ERR_UNLIKELY;
    }

    puts("[READ] heart rate service: body sensor location value");

    uint8_t loc = SENSOR_LOCATION;
    int res = os_mbuf_append(ctxt->om, &loc, sizeof(loc));
    return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

static int _temp_handler(uint16_t conn_handle, uint16_t attr_handle,
                        struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    int64_t result;
    /* let MicroPython know the top of this thread's stack */
    uint32_t stack_dummy;
    mp_stack_set_top((char*)&stack_dummy);

    /* Make MicroPython's stack limit somewhat smaller than actual stack limit */
    mp_stack_set_limit(THREAD_STACKSIZE_MAIN - 256);

    mp_obj_t mp_result = MP_OBJ_SMALL_INT_VALUE(0);
    //mp_obj_t mp_arg = mp_obj_new_float(SAUL_SENSE_TEMP);

    static const char func[] = "get_temperature";
    mp_obj_t function = mp_obj_new_str_via_qstr(func, strlen(func));

    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t func_temperature = mp_load_name(MP_OBJ_QSTR_VALUE(function));
        mp_result = mp_call_function_0(func_temperature);
        nlr_pop();
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
    }
    result = mp_obj_get_int(mp_result);
    //result = 1;

    if (result == -1) {
        return BLE_ATT_ERR_UNLIKELY;
    }
    int16_t fmt_result = result;

    int res = os_mbuf_append(ctxt->om, &fmt_result, sizeof(fmt_result));
    return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

static int _devinfo_handler(uint16_t conn_handle, uint16_t attr_handle,
                            struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;
    const char *str;

    switch (ble_uuid_u16(ctxt->chr->uuid)) {
        case BLE_GATT_CHAR_MANUFACTURER_NAME:
            puts("[READ] device information service: manufacturer name value");
            str = _manufacturer_name;
            break;
        case BLE_GATT_CHAR_MODEL_NUMBER_STR:
            puts("[READ] device information service: model number value");
            str = _model_number;
            break;
        case BLE_GATT_CHAR_SERIAL_NUMBER_STR:
            puts("[READ] device information service: serial number value");
            str = _serial_number;
            break;
        case BLE_GATT_CHAR_FW_REV_STR:
            puts("[READ] device information service: firmware revision value");
            str = _fw_ver;
            break;
        case BLE_GATT_CHAR_HW_REV_STR:
            puts("[READ] device information service: hardware revision value");
            str = _hw_ver;
            break;
        default:
            return BLE_ATT_ERR_UNLIKELY;
    }

    int res = os_mbuf_append(ctxt->om, str, strlen(str));
    return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

static int _bas_handler(uint16_t conn_handle, uint16_t attr_handle,
                        struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    (void)conn_handle;
    (void)attr_handle;
    (void)arg;

    puts("[READ] battery level service: battery level value");

    uint8_t level = BAT_LEVEL;  /* this battery will never drain :-) */
    int res = os_mbuf_append(ctxt->om, &level, sizeof(level));
    return (res == 0) ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
}

static int gap_event_cb(struct ble_gap_event *event, void *arg)
{
    (void)arg;

    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            if (event->connect.status) {
                _stop_updating();
                _start_advertising();
                return 0;
            }
            _conn_handle = event->connect.conn_handle;
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            _stop_updating();
            _start_advertising();
            break;

        case BLE_GAP_EVENT_SUBSCRIBE:
            if (event->subscribe.attr_handle == _hrs_val_handle) {
                if (event->subscribe.cur_notify == 1) {
                    _start_updating();
                }
                else {
                    _stop_updating();
                }
            }
            break;
    }

    return 0;
}

static void _start_advertising(void)
{
    struct ble_gap_adv_params advp;
    int res;

    memset(&advp, 0, sizeof advp);
    advp.conn_mode = BLE_GAP_CONN_MODE_UND;
    advp.disc_mode = BLE_GAP_DISC_MODE_GEN;
    advp.itvl_min  = BLE_GAP_ADV_FAST_INTERVAL1_MIN;
    advp.itvl_max  = BLE_GAP_ADV_FAST_INTERVAL1_MAX;
    res = ble_gap_adv_start(nimble_riot_own_addr_type, NULL, BLE_HS_FOREVER,
                            &advp, gap_event_cb, NULL);
    assert(res == 0);
    (void)res;
}

static void _start_updating(void)
{
    event_timeout_set(&_update_timeout_evt, UPDATE_INTERVAL);
    puts("[NOTIFY_ENABLED] heart rate service");
}

static void _stop_updating(void)
{
    event_timeout_clear(&_update_timeout_evt);
    puts("[NOTIFY_DISABLED] heart rate service");
}

static void _hr_update(event_t *e)
{
    (void)e;
    struct os_mbuf *om;

    int result = 0;
    uint32_t stack_dummy;
    mp_stack_set_top((char*)&stack_dummy);

    /* Make MicroPython's stack limit somewhat smaller than actual stack limit */
    mp_stack_set_limit(THREAD_STACKSIZE_MAIN - 256);

    mp_obj_t mp_result = MP_OBJ_SMALL_INT_VALUE(0);
    //mp_obj_t mp_arg = mp_obj_new_float(SAUL_SENSE_TEMP);

    static const char func[] = "get_heart_rate";
    mp_obj_t function = mp_obj_new_str_via_qstr(func, strlen(func));

    int res = 0;
    nlr_buf_t nlr;
    if (nlr_push(&nlr) == 0) {
        mp_obj_t func_heart_rate = mp_load_name(MP_OBJ_QSTR_VALUE(function));
        mp_result = mp_call_function_0(func_heart_rate);
        nlr_pop();
        result = mp_obj_get_int(mp_result);
    } else {
        // uncaught exception
        mp_obj_print_exception(&mp_plat_print, (mp_obj_t)nlr.ret_val);
        res = -1;
    }

    if (res < 0) {
        puts("[NOTIFY] heart rate service error\n");
    }
    else {
        _hr_data.bpm = result;

        printf("[NOTIFY] heart rate service: measurement %i\n", (int)_hr_data.bpm);

        /* send heart rate data notification to GATT client */
        om = ble_hs_mbuf_from_flat(&_hr_data, sizeof(_hr_data));
        assert(om != NULL);
        res = ble_gattc_notify_custom(_conn_handle, _hrs_val_handle, om);
        assert(res == 0);
        (void)res;
    }

    /* schedule next update event */
    event_timeout_set(&_update_timeout_evt, UPDATE_INTERVAL);
}

static mp_obj_t _mp_from_str(const char *src, size_t len)
{
    mp_lexer_t *lex = mp_lexer_new_from_str_len(MP_QSTR__lt_stdin_gt_, src, len, 0);
    if (lex == NULL) {
        printf("MemoryError: lexer could not allocate memory\n");
        return MP_OBJ_NULL;
    }
    qstr source_name = lex->source_name;
    nlr_buf_t nlr;
    mp_obj_t module_fun = MP_OBJ_NULL;
    if (nlr_push(&nlr) == 0) {
        mp_parse_tree_t parse_tree = mp_parse(lex, MP_PARSE_FILE_INPUT);
        module_fun = mp_compile(&parse_tree, source_name, MP_EMIT_OPT_NONE, false);
        nlr_pop();
    }
    return module_fun;

}

int main(void)
{
    puts("NimBLE Heart Rate Sensor Example");

    int res = 0;
    (void)res;

    /* let MicroPython know the top of this thread's stack */
    uint32_t stack_dummy;
    mp_stack_set_top((char*)&stack_dummy);

    /* Make MicroPython's stack limit somewhat smaller than actual stack limit */
    mp_stack_set_limit(THREAD_STACKSIZE_MAIN - MP_STACK_SAFEAREA);
    mp_riot_init(mp_heap, sizeof(mp_heap));

    mp_do_str((const char *)hrs_py, hrs_py_len);
    mp_do_str((const char *)temperature_py, temperature_py_len);
    mp_hrs = _mp_from_str((const char *)hrs_py, hrs_py_len);
    mp_temperature = _mp_from_str((const char *)temperature_py, temperature_py_len);

    /* setup local event queue (for handling heart rate updates) */
    event_queue_init(&_eq);
    _update_evt.handler = _hr_update;
    event_timeout_init(&_update_timeout_evt, &_eq, &_update_evt);

    /* verify and add our custom services */
    res = ble_gatts_count_cfg(gatt_svr_svcs);
    assert(res == 0);
    res = ble_gatts_add_svcs(gatt_svr_svcs);
    assert(res == 0);

    /* set the device name */
    ble_svc_gap_device_name_set(_device_name);
    /* reload the GATT server to link our added services */
    ble_gatts_start();

    /* configure and set the advertising data */
    uint8_t buf[BLE_HS_ADV_MAX_SZ];
    bluetil_ad_t ad;
    bluetil_ad_init_with_flags(&ad, buf, sizeof(buf), BLUETIL_AD_FLAGS_DEFAULT);
    uint16_t hrs_uuid = BLE_GATT_SVC_HRS;
    bluetil_ad_add(&ad, BLE_GAP_AD_UUID16_INCOMP, &hrs_uuid, sizeof(hrs_uuid));
    bluetil_ad_add_name(&ad, _device_name);
    ble_gap_adv_set_data(ad.buf, ad.pos);

    /* start to advertise this node */
    _start_advertising();

    /* run an event loop for handling the heart rate update events */
    event_loop(&_eq);

    return 0;
}
