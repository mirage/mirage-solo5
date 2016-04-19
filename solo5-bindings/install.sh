#!/bin/sh -ex

prefix=${1:-$PREFIX}
if [ "$prefix" = "" ]; then
  prefix=`opam config var prefix`
fi

odir=$prefix/lib
mkdir -p $odir/pkgconfig $odir/mirage-solo5
cp mirage-solo5.pc $odir/pkgconfig/mirage-solo5.pc
cp libsolo5camlbindings.a $odir/mirage-solo5/
