/*
 * Copyright (C) 2019 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "net/gcoap.h"
#include "bpf.h"
#include "bpf/shared.h"
#include "net/nanocoap.h"
#include "suit/transport/coap.h"
#include "suit/storage.h"
#include "suit/storage/ram.h"
#include "fmt.h"

#define GCOAP_BPF_APP_SIZE 2048
static uint8_t _stack[512] = { 0 };


static ssize_t _riot_board_handler(coap_pkt_t *pkt, uint8_t *buf, size_t len, void *context)
{
    (void)context;
    return coap_reply_simple(pkt, COAP_CODE_205, buf, len,
            COAP_FORMAT_TEXT, (uint8_t*)RIOT_BOARD, strlen(RIOT_BOARD));
}

static bpf_t _bpf = {
    .application = NULL,
    .application_len = 0,
    .stack = _stack,
    .stack_size = sizeof(_stack),
};

static ssize_t _bpf_handler(coap_pkt_t *pdu, uint8_t *buf, size_t len, void *ctx)
{
    char *location = ctx;
    char reply[12] = { 0 };

    suit_storage_t *storage = suit_storage_find_by_id(location);

    assert(storage);

    suit_storage_set_active_location(storage, location);
    const uint8_t *mem_region;
    size_t length;
    suit_storage_read_ptr(storage, &mem_region, &length);

    _bpf.application = mem_region;
    _bpf.application_len = length;

    bpf_mem_region_t mem_pdu;
    bpf_mem_region_t mem_pkt;

    bpf_coap_ctx_t bpf_ctx = {
        .pkt = pdu,
        .buf = buf,
        .buf_len = len,
    };
    printf("[BPF]: executing gcoap handler\n");

    bpf_add_region(&_bpf, &mem_pdu, pdu->hdr, 256, BPF_MEM_REGION_READ | BPF_MEM_REGION_WRITE);
    bpf_add_region(&_bpf, &mem_pkt, pdu, sizeof(coap_pkt_t), BPF_MEM_REGION_READ | BPF_MEM_REGION_WRITE);

    bpf_setup(&_bpf);
    int64_t result = -1;
    int res = bpf_execute(&_bpf, &bpf_ctx, sizeof(bpf_ctx), &result);

    size_t reply_len = fmt_s32_dfp(reply, result, -2);

    printf("Execution done res=%i, result=%i\n", res, (int)result);
    return coap_reply_simple(pdu, COAP_CODE_204, buf, len, 0, (uint8_t*)reply, reply_len);
}

/* must be sorted by path (ASCII order) */
const coap_resource_t coap_resources[] = {
    COAP_WELL_KNOWN_CORE_DEFAULT_HANDLER,
    { "/bpf/exec/0", COAP_POST, _bpf_handler, ".ram.0" },
    { "/bpf/exec/1", COAP_POST, _bpf_handler, ".ram.1" },
    { "/riot/board", COAP_GET, _riot_board_handler, NULL },

    /* this line adds the whole "/suit"-subtree */
    SUIT_COAP_SUBTREE,
};

const unsigned coap_resources_numof = ARRAY_SIZE(coap_resources);
