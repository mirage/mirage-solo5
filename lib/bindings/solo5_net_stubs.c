/* Copyright (c) 2015, IBM 
 * Author(s): Dan Williams <djwillia@us.ibm.com> 
 *
 * Permission to use, copy, modify, and/or distribute this software
 * for any purpose with or without fee is hereby granted, provided
 * that the above copyright notice and this permission notice appear
 * in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
 * WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
 * AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT,
 * NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "solo5.h"

#include <string.h>

#define CAML_NAME_SPACE
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/signals.h>
#include <caml/fail.h>
#include <caml/callback.h>
#include <caml/bigarray.h>

CAMLprim value
mirage_solo5_net_info(value v_unit)
{
    CAMLparam1(v_unit);
    CAMLlocal2(v_mac_address, v_result);
    struct solo5_net_info ni;

    solo5_net_info(&ni);
    v_mac_address = caml_alloc_string(SOLO5_NET_ALEN);
#if defined(Bytes_val)
    memcpy(Bytes_val(v_mac_address), ni.mac_address, SOLO5_NET_ALEN);
#else
    memcpy(String_val(v_mac_address), ni.mac_address, SOLO5_NET_ALEN);
#endif
    v_result = caml_alloc(2, 0);
    Store_field(v_result, 0, v_mac_address);
    Store_field(v_result, 1, Val_long(ni.mtu));

    CAMLreturn(v_result);
}

CAMLprim value
mirage_solo5_net_read(value v_buf, value v_size)
{
    CAMLparam2(v_buf, v_size);
    CAMLlocal1(v_result);
    uint8_t *buf = Caml_ba_data_val(v_buf);
    size_t size = Long_val(v_size);
    size_t read_size;
    solo5_result_t result;
    
    result = solo5_net_read(buf, size, &read_size);
    v_result = caml_alloc_tuple(2);
    Field(v_result, 0) = Val_int(result);
    Field(v_result, 1) = Val_long(read_size);
    CAMLreturn(v_result);
}

CAMLprim value
mirage_solo5_net_write(value v_buf, value v_size)
{
    CAMLparam2(v_buf, v_size);
    const uint8_t *buf = Caml_ba_data_val(v_buf);
    size_t size = Long_val(v_size);
    solo5_result_t result;

    result = solo5_net_write(buf, size);
    CAMLreturn(Val_int(result));
}
