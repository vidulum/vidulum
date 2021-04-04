// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "pow.h"

#include "arith_uint256.h"
#include "chain.h"
#include "chainparams.h"
#include "rpc/server.h" // GetDifficulty()
#include "crypto/equihash.h"
#include "primitives/block.h"
#include "streams.h"
#include "uint256.h"
#include "util.h"

#include "sodium.h"

unsigned int static LDAv1NextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params) {
    /* Vidulum - Liquid Difficulty Algorithm (LDAv1), written by Corey Miller - coreym@vidulum.org */
    LogPrint("pow", "Liquid Difficulty Algo\n");

    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    const int64_t height = pindexLast->nHeight;
    int64_t nLiquidDepth = 120;
    int64_t nTimeDepth = 20; //work with time deltas between last nTimeDepth blocks

    //  TODO: SET THIS TO A DEFAULT DIFF FOR THE nLiquidDepth BLOCK WINDOW
    // reset diff over first nLiquidDepth blocks after LDA activation
    if (height < (params.vUpgrades[Consensus::UPGRADE_LIQUID].nActivationHeight + nLiquidDepth)) {
       //  TODO: MIGHT MOVE THIS TO CHAINPARAMS
       // Should be about 105 diff
       const arith_uint256 bnLiquidPowLimit = UintToArith256(uint256S("001325b61d49f49f49f49f49f49f49f49f49f49f49f49f49f49f49f49f49f49d"));
       return bnLiquidPowLimit.GetCompact();
    }

    arith_uint256 bnTargetTotal;
    const CBlockIndex *pindex = pindexLast;

    int64_t nWeightedTimespan = 0;
    int64_t nTimeWeightFactor = (nTimeDepth / 2) * (1 + nTimeDepth); 
    int64_t nTargetTimespan = nTimeWeightFactor * params.nPowTargetSpacing;

    // Aquire target avg over nLiquidDepth and weighted solve time over nTimeDepth
    for (unsigned int nCountBlocks = 1; nCountBlocks <= nLiquidDepth; nCountBlocks++) {
        arith_uint256 bnWork = arith_uint256().SetCompact(pindex->nBits);
        bnTargetTotal += bnWork;

        int64_t nFirstDelta = pindex->GetBlockTime();
        
        assert(pindex->pprev); // should never fail
        pindex = pindex->pprev;

        if(nTimeDepth >= 1){
            int64_t nTimeDelta = nFirstDelta - pindex->GetBlockTime();
            LogPrint("pow", "Diff: %f   Height: %d   tDelta: %d \n", GetDifficulty(pindex), pindex->nHeight, nTimeDelta);

            // We care about actual and target time but let's not care too much
            if (nTimeDelta <= params.nPowTargetSpacing/10){
                nTimeDelta = params.nPowTargetSpacing/10;
                LogPrint("pow", "very low nTimeDelta override to: %d \n", nTimeDelta);
            }
            if (nTimeDelta >= params.nPowTargetSpacing*10){
                nTimeDelta = params.nPowTargetSpacing*10;
                LogPrint("pow", "very high nTimeDelta override to: %d \n", nTimeDelta);
            }

            // Add weight to more important time deltas
            nWeightedTimespan += (nTimeDelta * nTimeDepth);
            nTimeDepth--;
        }
    }

    bnTargetTotal = bnTargetTotal / (nLiquidDepth * 2);
    arith_uint256 bnPrevTargetAvg = bnTargetTotal * 2;

    LogPrint("pow", "nTargetTimespan: %d \n", nTargetTimespan);
    LogPrint("pow", "nWeightedTimespan: %d \n", nWeightedTimespan);
    double nRetargetFactor = (nWeightedTimespan / double(nTargetTimespan) * 100);
    LogPrint("pow", "nRetargetFactor: %d \n", nRetargetFactor);

    arith_uint256 bnLastTarget = arith_uint256().SetCompact(pindexLast->nBits);
    arith_uint256 bnNextDiff = bnLastTarget;
    if (nRetargetFactor >= 150) {
        LogPrint("pow", "nRetargetFactor > 150 override to: 150 of prev target\n");
        bnNextDiff = (bnLastTarget * 150) / 100;
    } else if (nRetargetFactor <= 67) {
        LogPrint("pow", "nRetargetFactor < 67 override to: 67 of prev target\n");
        bnNextDiff = (bnLastTarget * 67) / 100;
    } else if (nRetargetFactor < 83 || nRetargetFactor > 120) {
        LogPrint("pow", "nRetargetFactor < 83 or > 120 adjust by bnPrevTargetAvg\n");
        bnNextDiff = (bnPrevTargetAvg * nRetargetFactor) / 100;
    }else{
        LogPrint("pow", "Timing looks great - use last target \n");
    }

    if (bnNextDiff > bnPowLimit) {
        LogPrint("pow", "bnNextDiff more than bnPowLimit - Resetting \n");
        bnNextDiff = bnPowLimit;
    }

    return bnNextDiff.GetCompact();
}

unsigned int GetNextWorkRequired(const CBlockIndex* pindexLast, const CBlockHeader *pblock, const Consensus::Params& params)
{
    const CChainParams& chainParams = Params();
    unsigned int nProofOfWorkLimit = UintToArith256(params.powLimit).GetCompact();

    // Genesis block
    if (pindexLast == NULL)
        return nProofOfWorkLimit;

    // Reset the difficulty after the algo fork
    if (pindexLast->nHeight >= chainParams.eh_epoch_1_end() - params.nPowAveragingWindow
        && pindexLast->nHeight < chainParams.eh_epoch_1_end()) {
        LogPrint("pow", "Reset the difficulty for the eh_epoch_2 algo change: %d\n", nProofOfWorkLimit);
        return nProofOfWorkLimit;
    }

	{
        // Comparing to pindexLast->nHeight with >= because this function
        // returns the work required for the block after pindexLast.
        if (params.nPowAllowMinDifficultyBlocksAfterHeight != boost::none &&
            pindexLast->nHeight >= params.nPowAllowMinDifficultyBlocksAfterHeight.get())
        {
            // Special difficulty rule for testnet:
            // If the new block's timestamp is more than 6 * 2.5 minutes
            // then allow mining of a min-difficulty block.
            if (pblock && pblock->GetBlockTime() > pindexLast->GetBlockTime() + params.nPowTargetSpacing * 6)
                return nProofOfWorkLimit;
        }
    }

    // Find the first block in the averaging interval
    const CBlockIndex* pindexFirst = pindexLast;
    arith_uint256 bnTot {0};
    for (int i = 0; pindexFirst && i < params.nPowAveragingWindow; i++) {
        arith_uint256 bnTmp;
        bnTmp.SetCompact(pindexFirst->nBits);
        bnTot += bnTmp;
        pindexFirst = pindexFirst->pprev;
    }

    // Check we have enough blocks
    if (pindexFirst == NULL)
        return nProofOfWorkLimit;

    arith_uint256 bnAvg {bnTot / params.nPowAveragingWindow};

    //Difficulty algo
    int nHeight = pindexLast->nHeight + 1;
    if (nHeight < params.vUpgrades[Consensus::UPGRADE_DIFA].nActivationHeight) {
        return DigishieldCalculateNextWorkRequired(bnAvg, pindexLast->GetMedianTimePast(), pindexFirst->GetMedianTimePast(), params);
    } else if(nHeight < params.vUpgrades[Consensus::UPGRADE_LIQUID].nActivationHeight) {
        return Lwma3CalculateNextWorkRequired(pindexLast, params);
    } else {
        return LDAv1NextWorkRequired(pindexLast, pblock, params);
    }
}

unsigned int DigishieldCalculateNextWorkRequired(arith_uint256 bnAvg,
                                       int64_t nLastBlockTime, int64_t nFirstBlockTime,
                                       const Consensus::Params& params)
{
    // Limit adjustment step
    // Use medians to prevent time-warp attacks
    int64_t nActualTimespan = nLastBlockTime - nFirstBlockTime;
    LogPrint("pow", "  nActualTimespan = %d  before dampening\n", nActualTimespan);
    nActualTimespan = params.AveragingWindowTimespan() + (nActualTimespan - params.AveragingWindowTimespan())/4;
    LogPrint("pow", "  nActualTimespan = %d  before bounds\n", nActualTimespan);

    if (nActualTimespan < params.MinActualTimespan())
        nActualTimespan = params.MinActualTimespan();
    if (nActualTimespan > params.MaxActualTimespan())
        nActualTimespan = params.MaxActualTimespan();

    // Retarget
    const arith_uint256 bnPowLimit = UintToArith256(params.powLimit);
    arith_uint256 bnNew {bnAvg};
    bnNew /= params.AveragingWindowTimespan();
    bnNew *= nActualTimespan;

    if (bnNew > bnPowLimit)
        bnNew = bnPowLimit;

    /// debug print
    LogPrint("pow", "GetNextWorkRequired RETARGET\n");
    LogPrint("pow", "params.AveragingWindowTimespan() = %d    nActualTimespan = %d\n", params.AveragingWindowTimespan(), nActualTimespan);
    LogPrint("pow", "Current average: %08x  %s\n", bnAvg.GetCompact(), bnAvg.ToString());
    LogPrint("pow", "After:  %08x  %s\n", bnNew.GetCompact(), bnNew.ToString());

    return bnNew.GetCompact();
}


unsigned int Lwma3CalculateNextWorkRequired(const CBlockIndex* pindexLast, const Consensus::Params& params)
{
    const int64_t T = params.nPowTargetSpacing;
    const int64_t N = params.nZawyLWMA3AveragingWindow;
    const int64_t k = N * (N + 1) * T / 2;
    const int64_t height = pindexLast->nHeight;
    const arith_uint256 powLimit = UintToArith256(params.powLimit);
    
    if (height < N) { return powLimit.GetCompact(); }

    arith_uint256 sumTarget, previousDiff, nextTarget;
    int64_t thisTimestamp, previousTimestamp;
    int64_t t = 0, j = 0, solvetimeSum = 0;

    const CBlockIndex* blockPreviousTimestamp = pindexLast->GetAncestor(height - N);
    previousTimestamp = blockPreviousTimestamp->GetBlockTime();

    // Loop through N most recent blocks. 
    for (int64_t i = height - N + 1; i <= height; i++) {
        const CBlockIndex* block = pindexLast->GetAncestor(i);
        thisTimestamp = (block->GetBlockTime() > previousTimestamp) ? block->GetBlockTime() : previousTimestamp + 1;

        int64_t solvetime = std::min(6 * T, thisTimestamp - previousTimestamp);
        previousTimestamp = thisTimestamp;

        j++;
        t += solvetime * j; // Weighted solvetime sum.
        arith_uint256 target;
        target.SetCompact(block->nBits);
        sumTarget += target / (k * N);

      //  if (i > height - 3) { solvetimeSum += solvetime; } // deprecated
        if (i == height) { previousDiff = target.SetCompact(block->nBits); }
    }

    nextTarget = t * sumTarget;
    
    // 150% diff change
    if (nextTarget > (previousDiff * 150) / 100) { nextTarget = (previousDiff * 150) / 100; }
    if ((previousDiff * 67) / 100 > nextTarget) { nextTarget = (previousDiff * 67)/100; }
    if (nextTarget > powLimit) { nextTarget = powLimit; }

    return nextTarget.GetCompact();
}

bool CheckEquihashSolution(const CBlockHeader *pblock, const CChainParams& params)
{
    //Set parameters N,K from solution size. Filtering of valid parameters
    //for the givenblock height will be carried out in main.cpp/ContextualCheckBlockHeader
    unsigned int n,k;
    size_t nSolSize = pblock->nSolution.size();
    switch (nSolSize){
        case 1344: n=200; k=9; break;
        case 400:  n=192; k=7; break;
        case 100:  n=144; k=5; break;
        case 68:   n=96;  k=5; break;
        case 36:   n=48;  k=5; break;
        default: return error("CheckEquihashSolution: Unsupported solution size of %d", nSolSize);
    }

    LogPrint("pow", "selected n,k : %d, %d \n", n,k);

    // Hash state
    crypto_generichash_blake2b_state state;
    EhInitialiseState(n, k, state);

    // I = the block header minus nonce and solution.
    CEquihashInput I{*pblock};
    // I||V
    CDataStream ss(SER_NETWORK, PROTOCOL_VERSION);
    ss << I;
    ss << pblock->nNonce;

    // H(I||V||...
    crypto_generichash_blake2b_update(&state, (unsigned char*)&ss[0], ss.size());

    bool isValid;
    EhIsValidSolution(n, k, state, pblock->nSolution, isValid);
    if (!isValid)
        return error("CheckEquihashSolution(): invalid solution");

    return true;
}

bool CheckProofOfWork(uint256 hash, unsigned int nBits, const Consensus::Params& params)
{
    bool fNegative;
    bool fOverflow;
    arith_uint256 bnTarget;

    bnTarget.SetCompact(nBits, &fNegative, &fOverflow);

    // Check range
    if (fNegative || bnTarget == 0 || fOverflow || bnTarget > UintToArith256(params.powLimit))
        return error("CheckProofOfWork(): nBits below minimum work");

    // Check proof of work matches claimed amount
    if (UintToArith256(hash) > bnTarget)
        return error("CheckProofOfWork(): hash doesn't match nBits");

    return true;
}

arith_uint256 GetBlockProof(const CBlockIndex& block)
{
    arith_uint256 bnTarget;
    bool fNegative;
    bool fOverflow;
    bnTarget.SetCompact(block.nBits, &fNegative, &fOverflow);
    if (fNegative || fOverflow || bnTarget == 0)
        return 0;
    // We need to compute 2**256 / (bnTarget+1), but we can't represent 2**256
    // as it's too large for a arith_uint256. However, as 2**256 is at least as large
    // as bnTarget+1, it is equal to ((2**256 - bnTarget - 1) / (bnTarget+1)) + 1,
    // or ~bnTarget / (nTarget+1) + 1.
    return (~bnTarget / (bnTarget + 1)) + 1;
}

int64_t GetBlockProofEquivalentTime(const CBlockIndex& to, const CBlockIndex& from, const CBlockIndex& tip, const Consensus::Params& params)
{
    arith_uint256 r;
    int sign = 1;
    if (to.nChainWork > from.nChainWork) {
        r = to.nChainWork - from.nChainWork;
    } else {
        r = from.nChainWork - to.nChainWork;
        sign = -1;
    }
    r = r * arith_uint256(params.nPowTargetSpacing) / GetBlockProof(tip);
    if (r.bits() > 63) {
        return sign * std::numeric_limits<int64_t>::max();
    }
    return sign * r.GetLow64();
}
