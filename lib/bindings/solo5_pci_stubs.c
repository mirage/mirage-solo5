#include "solo5.h"

#define CAML_NAME_SPACE

#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/signals.h>
#include <caml/fail.h>
#include <caml/callback.h>
#include <caml/bigarray.h>

#define FLAGS (CAML_BA_UINT8 | CAML_BA_C_LAYOUT | CAML_BA_EXTERNAL)
#define ba_empty() caml_ba_alloc_dims(FLAGS, 1, NULL, 0)

CAMLprim value
mirage_solo5_pci_acquire(value v_name)
{
    CAMLparam1(v_name);
    CAMLlocal2(v_info, v_result);
    solo5_result_t result;
    struct solo5_pci_info pi;

    result = solo5_pci_acquire(String_val(v_name), &pi);
    v_info = caml_alloc(12, 0);
    if (result == SOLO5_R_OK) {
        Store_field(v_info, 0, Val_long(pi.vendor_id));
        Store_field(v_info, 1, Val_long(pi.device_id));
        Store_field(v_info, 2, Val_long(pi.class_code));
        Store_field(v_info, 3, Val_long(pi.subclass_code));
        Store_field(v_info, 4, Val_long(pi.progif));
        Store_field(v_info, 5, Val_bool(pi.bus_master_enable));

#define map_bar(n) \
    if (pi.bar ## n ## _size != 0) { \
        Store_field(v_info, 6 + n, \
                caml_ba_alloc_dims(FLAGS, 1, pi.bar ## n, pi.bar ## n ## _size)); \
    } else { \
        Store_field(v_info, 6 + n, ba_empty()); \
    }

        map_bar(0);
        map_bar(1);
        map_bar(2);
        map_bar(3);
        map_bar(4);
        map_bar(5);
    } else {
        Store_field(v_info, 0, Val_long(0xffff)); /* vendor_id */
        Store_field(v_info, 1, Val_long(0xffff)); /* device_id */
        Store_field(v_info, 2, Val_long(0xff)); /* class_code */
        Store_field(v_info, 3, Val_long(0xff)); /* subclass_code */
        Store_field(v_info, 4, Val_long(0xff)); /* progif */
        Store_field(v_info, 5, Val_bool(false)); /* bus_master_enable */
        Store_field(v_info, 6, ba_empty()); /* bar0 */
        Store_field(v_info, 7, ba_empty()); /* bar1 */
        Store_field(v_info, 8, ba_empty()); /* bar2 */
        Store_field(v_info, 9, ba_empty()); /* bar3 */
        Store_field(v_info, 10, ba_empty()); /* bar4 */
        Store_field(v_info, 11, ba_empty()); /* bar5 */
    }

    v_result = caml_alloc_tuple(2);
    Store_field(v_result, 0, Val_int(result));
    Store_field(v_result, 1, v_info);
    CAMLreturn(v_result);
}

CAMLprim value
mirage_solo5_dma_acquire(value v_unit)
{
    CAMLparam1(v_unit);
    CAMLlocal2(v_result, v_buffer);
    solo5_result_t result;

    uint8_t *buffer;
    size_t size;
    result = solo5_dma_acquire(&buffer, &size);
    if (result == SOLO5_R_OK) {
        v_buffer = caml_ba_alloc_dims(FLAGS, 1, (void *) buffer, size);
    } else {
        v_buffer = ba_empty();
    }

    v_result = caml_alloc_tuple(2);
    Store_field(v_result, 0, Val_int(result));
    Store_field(v_result, 1, v_buffer);
    CAMLreturn(v_result);
}
