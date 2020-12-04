#!/bin/bash

'usage: run.sh <benchmark_name> <l3_replacement_policy>'


./build/X86/gem5.opt -d verifySHIP/$1/$2/ configs/spec2k6/run.py -b $1 --maxinsts=1000000000 --cpu-type=DerivO3CPU --caches --l2cache --l3cache --fast-forward=1000000000 --warmup-insts=50000000 --standard-switch=50000000 --caches --l2cache --l3cache --l3_repl=$2
