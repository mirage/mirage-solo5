/*
 * Copyright (c) 2011 Anil Madhavapeddy <anil@recoil.org>
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

#include <stdio.h>

#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/callback.h>

static char *argv[] = { "mirage", NULL };

CAMLprim value
caml_block_domain(value v_until)
{
  CAMLparam1(v_until);
  uint64_t until = (Int64_val(v_until));
  while(solo5_clock_monotonic() < until);
  CAMLreturn(Val_unit);
}

CAMLprim value
stub_evtchn_look_for_work(value v_unit)
{
    CAMLparam1(v_unit);
    CAMLlocal1(work_to_do);
    work_to_do = Val_bool(0);
    CAMLreturn(work_to_do);
}

void start_kernel(void)
{
  printf("Solo5: new bindings\n");

  caml_startup(argv);
}
