/*
 * Copyright (c) 2010 Anil Madhavapeddy <anil@recoil.org>
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

#if 0
#include <time.h>
#include <sys/time.h>
#endif

#include "solo5.h"

#include <caml/mlvalues.h>
#include <caml/alloc.h>
#include <caml/memory.h>
#include <caml/fail.h>

/* The definition of struct timeval should be in a header file, but
 * some libraries are still taking the native (system) time.h causing
 * a redefinition.  So for now, we define this here, although it's
 * fragile and a recipe for disaster.  It's also in stubs.c in the
 * solo5 kernel. */
struct timeval {
    long tv_sec;
    long tv_usec;
};

CAMLprim value
unix_gettimeofday(value v_unit)
{
  CAMLparam1(v_unit);
  struct timeval tp;
  if (gettimeofday(&tp, NULL) == -1)
    caml_failwith("gettimeofday");
  CAMLreturn(caml_copy_double((double) tp.tv_sec + (double) tp.tv_usec / 1e6));
}


CAMLprim value
caml_get_monotonic_time(value v_unit)
{
  CAMLparam1(v_unit);
  CAMLreturn(caml_copy_int64(time_monotonic_ns()));
}

#if 0
static value alloc_tm(struct tm *tm)
{
  value res;
  res = caml_alloc_small(9, 0);
  Field(res,0) = Val_int(tm->tm_sec);
  Field(res,1) = Val_int(tm->tm_min);
  Field(res,2) = Val_int(tm->tm_hour);
  Field(res,3) = Val_int(tm->tm_mday);
  Field(res,4) = Val_int(tm->tm_mon);
  Field(res,5) = Val_int(tm->tm_year);
  Field(res,6) = Val_int(tm->tm_wday);
  Field(res,7) = Val_int(tm->tm_yday);
  Field(res,8) = tm->tm_isdst ? Val_true : Val_false;
  return res;
}

CAMLprim value
unix_gmtime(value t)
{
  CAMLparam1(t);
  time_t clock;
  struct tm * tm;
  clock = (time_t) Double_val(t);
  tm = gmtime(&clock);
  if (tm == NULL) caml_failwith("gmtime");
  CAMLreturn(alloc_tm(tm));
}
#endif
