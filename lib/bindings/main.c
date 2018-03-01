/*
 * Copyright (c) 2016 Martin Lucina <martin.lucina@docker.com>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "solo5.h"

#define CAML_NAME_SPACE
#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/callback.h>
#include <caml/alloc.h>

static char *unused_argv[] = { "mirage", NULL };
static const char *solo5_cmdline = "";

CAMLprim value
caml_poll(value v_until)
{
    CAMLparam1(v_until);
    CAMLlocal1(work_to_do);

    uint64_t until = (Int64_val(v_until));
    int rc = solo5_yield(until);

    work_to_do = Val_bool(rc);
    CAMLreturn(work_to_do);
}

CAMLprim value
caml_get_cmdline(value unit)
{
    CAMLparam1(unit);
    CAMLlocal1(result);

    result = caml_copy_string(solo5_cmdline);
    CAMLreturn(result);
}

extern void _nolibc_init(uintptr_t, size_t);

int solo5_app_main(const struct solo5_start_info *bi)
{
    solo5_cmdline = (char *) bi->cmdline;
    _nolibc_init(bi->heap_start, bi->heap_size);
    caml_startup(unused_argv);

    return 0;
}
