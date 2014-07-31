#!/bin/bash

debug="0"; if [ "$1" = "-d" ]; then debug=1; fi

CTESTS="fib-tsu4"
RTESTS="fib-tsu4"
INPUTS="10"
INPUT2S="-"
STRINGOK="SUCCESS"
FSIZES="100000"
COMPILEROPTS="-"
out=".out"
ref="-ref"


IFS=' ' read -a ctv <<< "$CTESTS"
IFS=' ' read -a rtv <<< "$RTESTS"
IFS=' ' read -a iv <<< "$INPUTS"
IFS=' ' read -a i2v <<< "$INPUT2S"
IFS=' ' read -a sokv <<< "$STRINGOK"
IFS=' ' read -a fsv <<< "$FSIZES"
IFS=' ' read -a cov <<< "$COMPILEROPTS"

echo "-- Compiling..."
#for t in $TESTS; do
for it in "${!ctv[@]}"; do
   t=${ctv[it]}
   co=${cov[it]}; if [ "$co" = "-" ]; then co=""; fi
   echo "   COMPILING: $t"
   o=`cc -o $t $t.c $co 2>&1`
   if [ "$o" = "" ]; then
      echo "OK: ${t}"
   else
      echo "-------"
      echo "COMPILATION ERRORS: ${t}"
      echo "$o"
      exit 1
   fi
done


export DRT_DEBUG=0
export DRT_FSIZE=100000
echo "-- Testing..."
#for t in $TESTS; do
for it in "${!rtv[@]}"; do
   t=${rtv[it]}
   i=${iv[it]}; if [ "$i" = "-" ]; then i=""; fi
   i2=${i2v[it]}; if [ "$i2" = "-" ]; then i2=""; fi
   s=${sokv[it]}
   fs=${fsv[it]}
export DRT_FSIZE=$fs
   echo "   TESTING($it): ${t} $i $i2 (expecting: $s) FSIZE=$fs"
   ./${t} $i $i2  2>&1 >${t}${out}
   if [ -s ${t}${ref}${out} ]; then
#      o=`diff ${t}${ref}${out} ${t}${out}`
      #o=`grep $s ${t}${ref}${out}`
      o=`grep $s ${t}${out}`
      if [ "$o" != "" ]; then
         echo "OK: ${t}"
      else
         echo "PROBLEMS: ${t}"
      fi
   else
      echo "CREATING REFERENCE: ${t}${ref}${out}"
      mv ${t}${out} ${t}${ref}${out}
   fi
done

if [ "$debug" = "1" ]; then
export DRT_DEBUG=4
echo "-- Debug Testing (DEBUG LEVEL DRT_DEBUG=$DRT_DEBUG)..."
for it in "${!rtv[@]}"; do
   t=${rtv[it]}
   i=${iv[it]}; if [ "$i" = "-" ]; then i=""; fi
   i2=${i2v[it]}; if [ "$i2" = "-" ]; then i2=""; fi
   s=${sokv[it]}
   fs=${fsv[it]}
export DRT_FSIZE=$fs
   echo "   TESTING($it): ${t} $i $i2 (expecting: $s) FSIZE=$fs"
   o1=`./${t} $i $i2  2>&1`
   o=`echo "$o1" |grep $s`
   if [ "$o" != "" ]; then
      echo "OK: ${t}"
   else
      echo "PROBLEMS: ${t}"
   fi
done
fi




