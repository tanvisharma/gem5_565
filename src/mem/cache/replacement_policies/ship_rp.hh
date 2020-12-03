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

/**
 * @file
 * Declaration of a Signature-based Hit Predictor (SHiP) on 
 * top of Re-Reference Interval Prediction replacement policy.
 *
 * Re-Reference Interval Prediction (RRIP) is uses a
 * re-reference prediction value to determine if entries are going to be re-
 * used in the near future or not.
 *
 * The higher the value of the RRPV, the more distant the entry is from its
 * next access.
 * 
 * SHiP sets the RRPV based on the Signature History Counter Table (SHCT), 
 * which updates the counter on hits and misses.
 * The signature can be memory-address based or instruction PC based.  
 *
 */

#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_SHIP_RP_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_SHIP_RP_HH__

#include "base/sat_counter.hh"
#include "mem/cache/replacement_policies/brrip_rp.hh"



struct SHIPRPParams;
class SHIPRP : public BRRIPRP
{

  public:
    /** 
     * Declaration of SHCT 
     */
    mutable std::vector<SatCounter> SHCT;

    /** Convenience typedef. */
    typedef SHIPRPParams Params;

    /**
     * Construct and initiliaze this replacement policy.
     */
    SHIPRP(Params *p);

    /**
     * Destructor.
     */
    ~SHIPRP() {}


    /**
     * Touch an entry to update its replacement data.
     *
     * @param replacement_data Replacement data to be touched.
     */
    void touch(const std::shared_ptr<ReplacementData>& replacement_data) const
                                                                     override;

    /**
     * Reset replacement data. Used when an entry is inserted.
     * Set RRPV according to the insertion policy used.
     *
     * @param replacement_data Replacement data to be reset.
     */
    void reset(const std::shared_ptr<ReplacementData>& replacement_data) const
                                                                     override;

    /**
     * Find replacement victim using rrpv.
     *
     * @param cands Replacement candidates, selected by indexing policy.
     * @return Replacement entry to be replaced.
     */
    ReplaceableEntry* getVictim(const ReplacementCandidates& candidates) const
                                                                     override;

};

#endif // __MEM_CACHE_REPLACEMENT_POLICIES_SHIP_RP_HH__
