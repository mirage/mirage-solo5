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

#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/callback.h>
#include <caml/alloc.h>

static char *unused_argv[] = { "mirage", NULL };
static char *solo5_cmdline = "";

CAMLprim value
caml_poll(value v_until)
{
    CAMLparam1(v_until);
    CAMLlocal1(work_to_do);

    uint64_t until = (Int64_val(v_until));
    int rc = solo5_poll(until);

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

int solo5_app_main(char *cmdline)
{
    solo5_cmdline = cmdline;
    caml_startup(unused_argv);

    return 0;
}
