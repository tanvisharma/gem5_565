# Copyright (c) 2020 Purdue University
# All rights reserved.
#
# Redistribution and use in source and binary forms, with o without
# modification, are permitted provided that the following coditions are
# met: redistributions of source code must retain the above cpyright
# notice, this list of conditions and the following disclaimer
# redistributions in binary form must reproduce the above copyrght
# notice, this list of conditions and the following disclaimer i the
# documentation and/or other materials provided with the distribuion;
# neither the name of the copyright holders nor the names of its
# contributors may be used to endorse or promote products derived fom
# this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
# Authors: Malek Musleh, modified by Tim Rogers to work with gem5 20.x
### The following file was referenced from the following site:
### http://www.m5sim.org/SPEC_CPU2006_benchmarks
###
### and subsequent changes were made

from __future__ import print_function
from __future__ import absolute_import

import optparse
import sys
import os
import spec2k6

import m5
from m5.defines import buildEnv
from m5.objects import *
from m5.params import NULL
from m5.util import addToPath, fatal, warn

addToPath('../')

from ruby import Ruby

from common import Options
from common import Simulation
from common import CacheConfig
from common import CpuConfig
from common import ObjectList
from common import MemConfig
from common.FileSystemConfig import config_filesystem
from common.Caches import *
from common.cpu2000 import *
# from newCacheConfig import *

def get_processes(options):
    if options.benchmark == 'perlbench':
       process = spec2k6.perlbench
    elif options.benchmark == 'bzip2':
       process = spec2k6.bzip2
    elif options.benchmark == 'gcc':
       process = spec2k6.gcc
    elif options.benchmark == 'bwaves':
       process = spec2k6.bwaves
    elif options.benchmark == 'gamess':
       process = spec2k6.gamess
    elif options.benchmark == 'mcf':
       process = spec2k6.mcf
    elif options.benchmark == 'milc':
       process = spec2k6.milc
    elif options.benchmark == 'zeusmp':
       process = spec2k6.zeusmp
    elif options.benchmark == 'gromacs':
       process = spec2k6.gromacs
    elif options.benchmark == 'cactusADM':
       process = spec2k6.cactusADM
    elif options.benchmark == 'leslie3d':
       process = spec2k6.leslie3d
    elif options.benchmark == 'namd':
       process = spec2k6.namd
    elif options.benchmark == 'gobmk':
       process = spec2k6.gobmk;
    elif options.benchmark == 'dealII':
       process = spec2k6.dealII
    elif options.benchmark == 'soplex':
       process = spec2k6.soplex
    elif options.benchmark == 'povray':
       process = spec2k6.povray
    elif options.benchmark == 'calculix':
       process = spec2k6.calculix
    elif options.benchmark == 'hmmer':
       process = spec2k6.hmmer
    elif options.benchmark == 'sjeng':
       process = spec2k6.sjeng
    elif options.benchmark == 'GemsFDTD':
       process = spec2k6.GemsFDTD
    elif options.benchmark == 'libquantum':
       process = spec2k6.libquantum
    elif options.benchmark == 'h264ref':
       process = spec2k6.h264ref
    elif options.benchmark == 'tonto':
       process = spec2k6.tonto
    elif options.benchmark == 'lbm':
       process = spec2k6.lbm
    elif options.benchmark == 'omnetpp':
       process = spec2k6.omnetpp
    elif options.benchmark == 'astar':
       process = spec2k6.astar
    elif options.benchmark == 'wrf':
       process = spec2k6.wrf
    elif options.benchmark == 'sphinx3':
       process = spec2k6.sphinx3
    elif options.benchmark == 'xalancbmk':
       process = spec2k6.xalancbmk
    elif options.benchmark == 'specrand_i':
       process = spec2k6.specrand_i
    elif options.benchmark == 'specrand_f':
       process = spec2k6.specrand_f
    return [process], 1


parser = optparse.OptionParser()
Options.addCommonOptions(parser)
Options.addSEOptions(parser)
parser.add_option("-b", "--benchmark", default="",
                 help="The benchmark to be loaded.")

if '--ruby' in sys.argv:
    Ruby.define_options(parser)

(options, args) = parser.parse_args()

if args:
    print("Error: script doesn't take any positional arguments")
    sys.exit(1)

multiprocesses = []
numThreads = 1

if options.bench:
    apps = options.bench.split("-")
    if len(apps) != options.num_cpus:
        print("number of benchmarks not equal to set num_cpus!")
        sys.exit(1)

    for app in apps:
        try:
            if buildEnv['TARGET_ISA'] == 'arm':
                exec("workload = %s('arm_%s', 'linux', '%s')" % (
                        app, options.arm_iset, options.spec_input))
            else:
                exec("workload = %s(buildEnv['TARGET_ISA', 'linux', '%s')" % (
                        app, options.spec_input))
            multiprocesses.append(workload.makeProcess())
        except:
            print("Unable to find workload for %s: %s" %
                  (buildEnv['TARGET_ISA'], app),
                  file=sys.stderr)
            sys.exit(1)
elif options.benchmark:
    multiprocesses, numThreads = get_processes(options)
else:
    print("No workload specified. Exiting!\n", file=sys.stderr)
    sys.exit(1)

(CPUClass, test_mem_mode, FutureClass) = Simulation.setCPUClass(options)
CPUClass.numThreads = numThreads

# Check -- do not allow SMT with multiple CPUs
if options.smt and options.num_cpus > 1:
    fatal("You cannot use SMT with multiple CPUs!")

np = options.num_cpus
system = System(cpu = [CPUClass(cpu_id=i) for i in range(np)],
                mem_mode = test_mem_mode,
                mem_ranges = [AddrRange(options.mem_size)],
                cache_line_size = options.cacheline_size,
                workload = NULL)

if numThreads > 1:
    system.multi_thread = True

# Create a top-level voltage domain
system.voltage_domain = VoltageDomain(voltage = options.sys_voltage)

# Create a source clock for the system and set the clock period
system.clk_domain = SrcClockDomain(clock =  options.sys_clock,
                                   voltage_domain = system.voltage_domain)

# Create a CPU voltage domain
system.cpu_voltage_domain = VoltageDomain()

# Create a separate clock domain for the CPUs
system.cpu_clk_domain = SrcClockDomain(clock = options.cpu_clock,
                                       voltage_domain =
                                       system.cpu_voltage_domain)

# If elastic tracing is enabled, then configure the cpu and attach the elastic
# trace probe
if options.elastic_trace_en:
    CpuConfig.config_etrace(CPUClass, system.cpu, options)

# All cpus belong to a common cpu_clk_domain, therefore running at a common
# frequency.
for cpu in system.cpu:
    cpu.clk_domain = system.cpu_clk_domain

if ObjectList.is_kvm_cpu(CPUClass) or ObjectList.is_kvm_cpu(FutureClass):
    if buildEnv['TARGET_ISA'] == 'x86':
        system.kvm_vm = KvmVM()
        for process in multiprocesses:
            process.useArchPT = True
            process.kvmInSE = True
    else:
        fatal("KvmCPU can only be used in SE mode with x86")

# Sanity check
if options.simpoint_profile:
    if not ObjectList.is_noncaching_cpu(CPUClass):
        fatal("SimPoint/BPProbe should be done with an atomic cpu")
    if np > 1:
        fatal("SimPoint generation not supported with more than one CPUs")

for i in range(np):
    if options.smt:
        system.cpu[i].workload = multiprocesses
    elif len(multiprocesses) == 1:
        system.cpu[i].workload = multiprocesses[0]
    else:
        system.cpu[i].workload = multiprocesses[i]

    if options.simpoint_profile:
        system.cpu[i].addSimPointProbe(options.simpoint_interval)

    if options.checker:
        system.cpu[i].addCheckerCpu()

    if options.bp_type:
        bpClass = ObjectList.bp_list.get(options.bp_type)
        system.cpu[i].branchPred = bpClass()

    if options.indirect_bp_type:
        indirectBPClass = \
            ObjectList.indirect_bp_list.get(options.indirect_bp_type)
        system.cpu[i].branchPred.indirectBranchPred = indirectBPClass()

    system.cpu[i].createThreads()

if options.ruby:
    Ruby.create_system(options, False, system)
    assert(options.num_cpus == len(system.ruby._cpu_ports))

    system.ruby.clk_domain = SrcClockDomain(clock = options.ruby_clock,
                                        voltage_domain = system.voltage_domain)
    for i in range(np):
        ruby_port = system.ruby._cpu_ports[i]

        # Create the interrupt controller and connect its ports to Ruby
        # Note that the interrupt controller is always present but only
        # in x86 does it have message ports that need to be connected
        system.cpu[i].createInterruptController()

        # Connect the cpu's cache ports to Ruby
        system.cpu[i].icache_port = ruby_port.slave
        system.cpu[i].dcache_port = ruby_port.slave
        if buildEnv['TARGET_ISA'] == 'x86':
            system.cpu[i].interrupts[0].pio = ruby_port.master
            system.cpu[i].interrupts[0].int_master = ruby_port.slave
            system.cpu[i].interrupts[0].int_slave = ruby_port.master
            system.cpu[i].itb.walker.port = ruby_port.slave
            system.cpu[i].dtb.walker.port = ruby_port.slave
else:
    MemClass = Simulation.setMemClass(options)
    system.membus = SystemXBar()
    system.system_port = system.membus.slave
    CacheConfig.config_cache(options, system)
    MemConfig.config_mem(options, system)
    config_filesystem(system, options)

if options.wait_gdb:
    for cpu in system.cpu:
        cpu.wait_for_remote_gdb = True

root = Root(full_system = False, system = system)
Simulation.run(options, root, system, FutureClass)
