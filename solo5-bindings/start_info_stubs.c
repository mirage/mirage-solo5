/*
 * Copyright (c) 2012 Citrix Systems Inc
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

#include <stdio.h>
#include <stdint.h>
#include <string.h>
#endif

#include "solo5.h"

#include <caml/mlvalues.h>
#include <caml/memory.h>
#include <caml/alloc.h>
#include <caml/fail.h>
#include <caml/bigarray.h>

#if 0
#include <xen/xen.h>
#include <mini-os/os.h>
#endif

#define EVTCHN_CONSOLE 0x2000
#define EVTCHN_STORE   0x2001

static uint8_t console_page[PAGE_SIZE] ALIGN_4K;
static uint8_t xenstore_page[PAGE_SIZE] ALIGN_4K;

struct start_info {
    /* THE FOLLOWING ARE FILLED IN BOTH ON INITIAL BOOT AND ON RESUME.    */
    char magic[32];             /* "xen-<version>-<platform>".            */
    unsigned long nr_pages;     /* Total pages allocated to this domain.  */
    unsigned long shared_info;  /* MACHINE address of shared info struct. */
    uint32_t flags;             /* SIF_xxx flags.                         */
    uint64_t store_mfn;        /* MACHINE page number of shared page.    */
    uint32_t store_evtchn;      /* Event channel for store communication. */
    union {
        struct {
            uint64_t mfn;      /* MACHINE page number of console page.   */
            uint32_t  evtchn;   /* Event channel for console page.        */
        } domU;
        struct {
            uint32_t info_off;  /* Offset of console_info struct.         */
            uint32_t info_size; /* Size of console_info struct from start.*/
        } dom0;
    } console;
    /* THE FOLLOWING ARE ONLY FILLED IN ON INITIAL BOOT (NOT RESUME).     */
    unsigned long pt_base;      /* VIRTUAL address of page directory.     */
    unsigned long nr_pt_frames; /* Number of bootstrap p.t. frames.       */
    unsigned long mfn_list;     /* VIRTUAL address of page-frame list.    */
    unsigned long mod_start;    /* VIRTUAL address of pre-loaded module   */
                                /* (PFN of pre-loaded module if           */
                                /*  SIF_MOD_START_PFN set in flags).      */
    unsigned long mod_len;      /* Size (bytes) of pre-loaded module.     */
#define MAX_GUEST_CMDLINE 1024
    int8_t cmd_line[MAX_GUEST_CMDLINE];
    /* The pfn range here covers both page table and p->m table frames.   */
    unsigned long first_p2m_pfn;/* 1st pfn forming initial P->M table.    */
    unsigned long nr_p2m_frames;/* # of pfns forming initial P->M table.  */
};
typedef struct start_info start_info_t;

struct start_info start_info;

void dump_console_page_head(void) { 
    int i;
    for ( i = 0; i < PAGE_SIZE / 8; i++ ) {
        printf("%02x", console_page[i]);
        if ( i % 4 == 3 )
            printf(" ");
        if ( i % 32 == 31 )
            printf("\n");
    }
}

/* create a fake start_info structure */
void start_info_init(void) {
    strcpy(start_info.magic, "xen-fake");
    start_info.nr_pages = (uint64_t)mem_max_addr / PAGE_SIZE;
    start_info.shared_info = 0;
    start_info.flags = 0;
    start_info.store_mfn = (((uint64_t)xenstore_page) >> PAGE_SHIFT);
    start_info.store_evtchn = EVTCHN_STORE;
    start_info.console.domU.mfn = (((uint64_t)console_page) >> PAGE_SHIFT);
    start_info.console.domU.evtchn = EVTCHN_CONSOLE;
    start_info.pt_base = 0;
    start_info.nr_pt_frames = 0;
    start_info.mfn_list = 0;
    start_info.mod_start = 0;
    start_info.mod_len = 0;
    start_info.cmd_line[0] = 0;
    start_info.first_p2m_pfn = 0;
    start_info.nr_p2m_frames = 0;
}

CAMLprim value
stub_start_info_get(value unit)
{
  CAMLparam1(unit);
  CAMLlocal2(result, tmp);
  char buf[MAX_GUEST_CMDLINE+1];

  result = caml_alloc_tuple(16);
  memcpy(buf, start_info.magic, sizeof(start_info.magic));
  buf[sizeof(start_info.magic)] = 0;
  tmp = caml_copy_string(buf);
  Store_field(result, 0, tmp);
  Store_field(result, 1, Val_int(start_info.nr_pages));
  Store_field(result, 2, Val_int(start_info.shared_info));
  Store_field(result, 3, Val_int(start_info.flags));
  Store_field(result, 4, Val_int(start_info.store_mfn));
  Store_field(result, 5, Val_int(start_info.store_evtchn));
  Store_field(result, 6, Val_int(start_info.console.domU.mfn));
  Store_field(result, 7, Val_int(start_info.console.domU.evtchn));
  Store_field(result, 8, Val_int(start_info.pt_base));
  Store_field(result, 9, Val_int(start_info.nr_pt_frames));
  Store_field(result, 10, Val_int(start_info.mfn_list));
  Store_field(result, 11, Val_int(start_info.mod_start));
  Store_field(result, 12, Val_int(start_info.mod_len));
  memcpy(buf, start_info.cmd_line, MAX_GUEST_CMDLINE);
  buf[MAX_GUEST_CMDLINE] = 0;
  tmp = caml_copy_string(buf);
  Store_field(result, 13, tmp);
  Store_field(result, 14, Val_int(start_info.first_p2m_pfn));
  Store_field(result, 15, Val_int(start_info.nr_p2m_frames));

  CAMLreturn(result);
}

CAMLprim value
caml_console_start_page(value v_unit)
{
  CAMLparam1(v_unit);
  CAMLreturn(caml_ba_alloc_dims(CAML_BA_UINT8 | CAML_BA_C_LAYOUT,
                                1,
                                (void *)console_page,
                                (long)PAGE_SIZE));
}

CAMLprim value
caml_xenstore_start_page(value v_unit)
{
  CAMLparam1(v_unit);
  CAMLreturn(caml_ba_alloc_dims(CAML_BA_UINT8 | CAML_BA_C_LAYOUT,
                                1,
                                //mfn_to_virt(start_info.store_mfn),
                                (void *)xenstore_page,
                                (long)PAGE_SIZE));
}

