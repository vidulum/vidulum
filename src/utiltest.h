// Copyright (c) 2016 The Zcash developers
// Copyright (c) 2017-2018 The SnowGem developers
// Copyright (c) 2018 The Vidulum developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "wallet/wallet.h"
#include "vidulum/JoinSplit.hpp"
#include "vidulum/Note.hpp"
#include "vidulum/NoteEncryption.hpp"

CWalletTx GetValidReceive(ZCJoinSplit& params,
                          const libvidulum::SpendingKey& sk, CAmount value,
                          bool randomInputs);
libvidulum::Note GetNote(ZCJoinSplit& params,
                       const libvidulum::SpendingKey& sk,
                       const CTransaction& tx, size_t js, size_t n);
CWalletTx GetValidSpend(ZCJoinSplit& params,
                        const libvidulum::SpendingKey& sk,
                        const libvidulum::Note& note, CAmount value);
