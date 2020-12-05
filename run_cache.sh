#!/bin/bash



declare -a l3size=("256kB" "512kB" "1MB" "2MB")
declare -a repl=("drrip" "ship" "lru")
declare -a benchmark=("hmmer" "bzip2")

for bench in "${benchmark[@]}"; do
	for pol in "${repl[@]}"; do
		for size in "${l3size[@]}"; do
			./build/X86/gem5.opt -d verifySHIP/cacheSensitivity/$bench/$pol/$size configs/spec2k6/run.py -b $bench --maxinsts=250000000 --cpu-type=DerivO3CPU --caches --l2cache --l3cache --fast-forward=1000000000 --warmup-insts=50000000 --standard-switch=50000000 --caches --l2cache --l3cache --l3_repl=$pol --l3_size=$size &
		done
	done
done

wait 
