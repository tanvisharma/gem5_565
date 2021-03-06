/**
 * Copyright (c) 2018 Inria
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "mem/cache/replacement_policies/drrip_rp.hh"
#include <string>
#include <memory>
#include "debug/CacheRepl.hh"
#include "base/random.hh"
#include "params/DRRIPRP.hh"  

DRRIPRP::DRRIPRP(Params *p)
    : BRRIPRP(p), num_sd_sets(p->num_sd_sets),count_bits(p->count_bits)
{
    SDC = pow(2, p->count_bits-1);
}

void
DRRIPRP::reset(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    int setNum = replacement_data->setnumber;
    std::shared_ptr<BRRIPReplData> casted_replacement_data =
        std::static_pointer_cast<BRRIPReplData>(replacement_data);
    // Reset RRPV
    // Replacement data is inserted as "long re-reference" if lower than btp,
    // "distant re-reference" otherwise
    
    casted_replacement_data->rrpv.saturate();
    uint32_t constituency = floor(setNum/num_sd_sets);
    uint32_t offset = setNum % num_sd_sets; 
    
    bool srripSetPresent = constituency == offset;
    bool brripSetPresent = (constituency+1)*(num_sd_sets-1) == setNum;

    if (brripSetPresent){
        SDC++;
        if (SDC > pow(2,count_bits)){SDC--;}
        if (random_mt.random<unsigned>(1, 100) <= btp){
            casted_replacement_data->rrpv--;
        }
    } else if(srripSetPresent){
        casted_replacement_data->rrpv--;
        SDC--;
        if (SDC < 0){SDC++;}
        
        

    } else {
        if (SDC > pow(2, count_bits-1)) { //SRRIP Has more hits
            casted_replacement_data->rrpv--;  
        }
        else {
            if (random_mt.random<unsigned>(1, 100) <= btp){
                casted_replacement_data->rrpv--;
            }
        }
    }
    // printf("SDC: %d \n",SDC);
    // Mark entry as ready to be used
    casted_replacement_data->valid = true;

}

// void
// DRRIPRP::touch(const std::shared_ptr<ReplacementData>& replacement_data) const
// {
//     int setNum = replacement_data->setnumber;
//     std::shared_ptr<BRRIPReplData> casted_replacement_data =
//         std::static_pointer_cast<BRRIPReplData>(replacement_data);

//     // Update RRPV if not 0 yet
//     // Every hit in HP mode makes the entry the last to be evicted, while
//     // in FP mode a hit makes the entry less likely to be evicted
//     uint32_t constituency = floor(setNum/num_sd_sets);
//     uint32_t offset = setNum % num_sd_sets; 
    
//     bool srripSetPresent = constituency == offset;
//     bool brripSetPresent = (constituency+1)*(num_sd_sets-1) == setNum;
//     // printf("constituency: %d offset: %d \n", constituency, offset);
//     // printf("offset: %d \n", offset);

//     if (srripSetPresent){
//         SDC++;
//         // printf("srrip: %d \n", SDC);
//         if (SDC>pow(2,count_bits)){
//             SDC = pow(2,count_bits);
//         }
//     }

//     else if(brripSetPresent){
//         SDC--;
//         // printf("brrip: %d \n", SDC);
//         if (SDC<0){
//             SDC = 0;
//         }
//     }
//     printf("SDC: %d \n", SDC);
//     if (hitPriority) {
//         casted_replacement_data->rrpv.reset();
//     } else {
//         casted_replacement_data->rrpv--;
//     }
// }




DRRIPRP*
DRRIPRPParams::create()
{
    return new DRRIPRP(this);
}
