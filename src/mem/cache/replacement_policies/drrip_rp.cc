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
    : BRRIPRP(p), srrip(p->srriprpSet), brrip(p->brriprpSet)
{
    SDC = 0;
    std::string temp ="";
    int len = srrip.size();
    int j = 0;
    for (int i=0; i < len; i++){
        if (srrip[i] != ','){
            temp += srrip[i];
            continue;
        }
        if (srrip[i] == ','){
            srriprpSet[j] = stoi(temp);
            j++;
            temp = "";
        }
    }
    temp ="";
    len = brrip.size();
    j = 0;
    for (int i=0; i < len; i++){
        if (brrip[i] != ','){
            temp += brrip[i];
            continue;
        }
        if (brrip[i] == ','){
            brriprpSet[j] = stoi(temp);
            j++;
            temp = "";
        }
    }
        
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

    
    bool srripSetPresent = std::find(std::begin(srriprpSet), std::end(srriprpSet), setNum) != std::end(srriprpSet);
    bool brripSetPresent = std::find(std::begin(brriprpSet), std::end(brriprpSet), setNum) != std::end(brriprpSet);

    if (brripSetPresent){
        if (random_mt.random<unsigned>(1, 100) <= btp){
            casted_replacement_data->rrpv--;
        }
    } else if(srripSetPresent){
        casted_replacement_data->rrpv--;
    } else {
        if (SDC >= 0) { //SRRIP Has more hits
            casted_replacement_data->rrpv--;  
        }
        else {
            if (random_mt.random<unsigned>(1, 100) <= btp){
                casted_replacement_data->rrpv--;
            }
        }
    }

    // Mark entry as ready to be used
    casted_replacement_data->valid = true;

}

void
DRRIPRP::touch(const std::shared_ptr<ReplacementData>& replacement_data) 
{
    int setNum = replacement_data->setnumber;
    std::shared_ptr<BRRIPReplData> casted_replacement_data =
        std::static_pointer_cast<BRRIPReplData>(replacement_data);

    // Update RRPV if not 0 yet
    // Every hit in HP mode makes the entry the last to be evicted, while
    // in FP mode a hit makes the entry less likely to be evicted
    bool srripSetPresent = std::find(std::begin(srriprpSet), std::end(srriprpSet), setNum) != std::end(srriprpSet);
    bool brripSetPresent = std::find(std::begin(brriprpSet), std::end(brriprpSet), setNum) != std::end(brriprpSet);

    if (srripSetPresent){
        SDC++;
    }else if(brripSetPresent){
        SDC--;
    }
    DPRINTF(CacheRepl, "SDCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCC: %d \n", SDC);
    if (hitPriority) {
        casted_replacement_data->rrpv.reset();
    } else {
        casted_replacement_data->rrpv--;
    }
}




DRRIPRP*
DRRIPRPParams::create()
{
    return new DRRIPRP(this);
}
