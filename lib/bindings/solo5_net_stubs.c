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

#include <assert.h>

#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/signals.h>
#include <caml/fail.h>
#include <caml/callback.h>
#include <caml/bigarray.h>

CAMLprim value
stub_net_mac(value unit)
{
    CAMLparam1(unit);
    struct solo5_net_info info;

    solo5_net_info(&info);
    CAMLreturn(caml_copy_string((char *) info.mac_address));
}

CAMLprim value
stub_net_read(value buffer, value num)
{
    CAMLparam2(buffer, num);
    uint8_t *data = Caml_ba_data_val(buffer);
    int n = Int_val(num);
    int ret;
    
    assert(Caml_ba_array_val(buffer)->num_dims == 1);
    
    ret = solo5_net_read_sync(data, &n);
    if (ret != 0)
        CAMLreturn(Val_int(-1));
    else
        CAMLreturn(Val_int(n));
}

CAMLprim value
stub_net_write(value buffer, value num)
{
    CAMLparam2(buffer, num);
    uint8_t *data = Caml_ba_data_val(buffer);
    int n = Int_val(num);
    int ret;

    assert(Caml_ba_array_val(buffer)->num_dims == 1);

    ret = solo5_net_write_sync(data, n);
    if (ret != 0)
        CAMLreturn(Val_int(-1));
    else
        CAMLreturn(Val_int(n));
}
