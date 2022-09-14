/*
 * Copyright (C) 2022 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "saul_reg.h"
#include "wasm_export.h"


static uintptr_t wasm_saul_reg_find_type(wasm_exec_env_t exec_env, int type)
{
    (void)exec_env;
    return (uintptr_t)saul_reg_find_type((uint8_t)type);
}

static float wasm_saul_reg_read(wasm_exec_env_t exec_env, uintptr_t device)
{
    (void)exec_env;
    saul_reg_t *requested = (saul_reg_t*)device;
    unsigned num = 0;
    saul_reg_t *found = NULL;
    do {
        found = saul_reg_find_nth(num);
        if (found == requested) {
            break;
        }
        num++;
    } while (found);

    if (found == NULL) {
        return 0;
    }

    phydat_t data;
    saul_reg_read(found, &data);

    return (float)((data.val[0]) / 100.0);
}

static NativeSymbol saul_symbols[] =
{
    {
        .symbol = "saul_reg_find_type",
        .func_ptr = wasm_saul_reg_find_type,
        .signature = "(i)i",
    },
    {
        .symbol = "saul_reg_read",
        .func_ptr = wasm_saul_reg_read,
        .signature = "(i)f",
    }
};

bool iwasm_saul_reg_funcs(void)
{
    if (!wasm_runtime_register_natives("env",
                                   saul_symbols,
                                   ARRAY_SIZE(saul_symbols))) {
        return false;
    }

    return true;
}
