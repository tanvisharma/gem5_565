#! /bin/bash

#File to compile results from the stats.txt file

declare -a repl=("drrip" "ship" "lru" "brrip" "srrip")
declare -a benchmark=("sjeng" "sphinx3" "cactusADM")

for bench in "${benchmark[@]}"; do
	echo "---------- $bench ---------------"
	echo "Policy,Icache misses,Dcache misses,L2 misses,L3 misses"
	for policy in "${repl[@]}"; do
		ipc=`cat ./verifySHIP/$bench/$policy/stats.txt | grep switch_cpus_1.ipc | tail -1 | awk -F" " '{print $2}'`
		icache_miss=`cat verifySHIP/$bench/$policy/stats.txt | grep icache.demand_misses::.switch_cpus_1 | awk -F" " '{print $2}'`
		dcache_miss=`cat verifySHIP/$bench/$policy/stats.txt | grep dcache.demand_misses::.switch_cpus_1 | awk -F" " '{print $2}'`
		l2_miss=`cat verifySHIP/$bench/$policy/stats.txt     | grep l2.demand_misses::.switch_cpus_1.data | awk -F" " '{print $2}'`
		l3_miss=`cat verifySHIP/$bench/$policy/stats.txt     | grep l3.demand_misses::.switch_cpus_1.data | awk -F" " '{print $2}'`
		echo $policy,$icache_miss, $dcache_miss, $l2_miss, $l3_miss, $ipc 
	done
done

