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

#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/signals.h>
#include <caml/fail.h>
#include <caml/callback.h>
#include <caml/bigarray.h>

CAMLprim value stub_blk_write(value sector, value buffer, value num) {
    CAMLparam3(sector, buffer, num);
    uint64_t sec = Int64_val(sector);
    uint8_t *data = Caml_ba_data_val(buffer);
    int n = Int_val(num);
    int ret = 0;

    assert(Caml_ba_array_val(buffer)->num_dims == 1);
	
    //printf("Solo5 blk write: sec=%d num=%d\n", sec, n);

    ret = solo5_blk_write_sync(sec, data, n);
    if ( ret ) 
        printf("blk write failed... %d to sector=%d\n", n, sec);

#if 0
    {
        int i;
        for (i = 0; i < n; i++) {
            printf("%02x ", (uint8_t) data[i]);
            if ( i % 16 == 15 )
                printf("\n");
        }
        printf("\n");
    }
#endif

    CAMLreturn(Val_bool(!ret));
}

CAMLprim value stub_blk_read(value sector, value buffer, value num) {
    CAMLparam3(sector, buffer, num);
    uint64_t sec = Int64_val(sector);
    uint8_t *data = Caml_ba_data_val(buffer);
    int n = Int_val(num);
    int ret = 0;

    assert(Caml_ba_array_val(buffer)->num_dims == 1);

    //printf("Solo5 blk read: sec=%d num=%d\n", sec, n);

    ret = solo5_blk_read_sync(sec, data, &n);
    if ( ret )
        printf("virtio read failed... %d from sector=%d\n", n, sec);

#if 0
    {
        int i;
        for (i = 0; i < n; i++) {
            printf("%02x ", (uint8_t) data[i]);
            if ( i % 16 == 15 )
                printf("\n");
        }
        printf("\n");
    }
#endif

    CAMLreturn(Val_bool(!ret));
}

CAMLprim value stub_blk_sector_size(value unit) {
    CAMLparam1(unit);
    CAMLreturn(Val_int(solo5_blk_sector_size()));
}
CAMLprim value stub_blk_sectors(value unit) {
    CAMLparam1(unit);
    CAMLreturn(caml_copy_int64(solo5_blk_sectors()));
}
CAMLprim value stub_blk_rw(value unit) {
    CAMLparam1(unit);
    CAMLreturn(Val_bool(solo5_blk_rw()));
}
