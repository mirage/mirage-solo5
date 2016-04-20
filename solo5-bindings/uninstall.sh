#!/bin/sh -ex

prefix=$1
if [ "$prefix" = "" ]; then
  prefix=`opam config var prefix`
fi

odir=$prefix/lib
rm -f $odir/pkgconfig/mirage-solo5.pc
rm -f $odir/mirage-solo5/libsolo5camlbindings.a
