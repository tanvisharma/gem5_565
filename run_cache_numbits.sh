#!/bin/bash


gem5_run(){
	sed -i '92s/.*/''	num_sd_sets = Param.Unsigned('"$2"', \\/g' /home/min/a/saxenau/NANO/565Project/gem5_565/src/mem/cache/replacement_policies/ReplacementPolicies.py	
	sed -i '94s/.*/''	count_bits = Param.Int('"$1"', "number of bits for set duelling counter")/g' /home/min/a/saxenau/NANO/565Project/gem5_565/src/mem/cache/replacement_policies/ReplacementPolicies.py	
	./build/X86/gem5.opt -d verifySHIP/numbits_setsize/hmmer/numbits_$1_setsize_$2/ configs/spec2k6/run.py -b hmmer $bench --maxinsts=250000000 --cpu-type=DerivO3CPU --caches --l2cache --l3cache --fast-forward=1000000000 --warmup-insts=50000000 --standard-switch=50000000 --caches --l2cache --l3cache --l3_repl=drrip
}

gem5_run 10 32 &
gem5_run 11 32 &
gem5_run 12 32 &
gem5_run 11 64 &
gem5_run 11 128 &


