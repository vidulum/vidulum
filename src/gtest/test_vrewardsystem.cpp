#include <gtest/gtest.h>

#include "main.h"
#include "utilmoneystr.h"
#include "chainparams.h"
#include "utilstrencodings.h"
#include "vidulum/Address.hpp"
#include "wallet/wallet.h"
#include "amount.h"
#include <memory>
#include <string>
#include <set>
#include <vector>
#include <boost/filesystem.hpp>
#include "util.h"

// To run tests:
// ./vidulum-gtest --gtest_filter="vidulum_reward_system_test.*"

//
// Enable this test to generate and print 48 testnet 2-of-3 multisig addresses.
// The output can be copied into chainparams.cpp.
// The temporary wallet file can be renamed as wallet.dat and used for testing with vidulumd.
//
#if 0
TEST(vidulum_reward_system_test, create_testnet_2of3multisig) {
    SelectParams(CBaseChainParams::TESTNET);
    boost::filesystem::path pathTemp = boost::filesystem::temp_directory_path() / boost::filesystem::unique_path();
    boost::filesystem::create_directories(pathTemp);
    mapArgs["-datadir"] = pathTemp.string();
    bool fFirstRun;
    auto pWallet = std::make_shared<CWallet>("wallet.dat");
    ASSERT_EQ(DB_LOAD_OK, pWallet->LoadWallet(fFirstRun));
    pWallet->TopUpKeyPool();
    std::cout << "Test wallet and logs saved in folder: " << pathTemp.native() << std::endl;
    
    int numKeys = 48;
    std::vector<CPubKey> pubkeys;
    pubkeys.resize(3);
    CPubKey newKey;
    std::vector<std::string> addresses;
    for (int i = 0; i < numKeys; i++) {
        ASSERT_TRUE(pWallet->GetKeyFromPool(newKey));
        pubkeys[0] = newKey;
        pWallet->SetAddressBook(newKey.GetID(), "", "receive");

        ASSERT_TRUE(pWallet->GetKeyFromPool(newKey));
        pubkeys[1] = newKey;
        pWallet->SetAddressBook(newKey.GetID(), "", "receive");

        ASSERT_TRUE(pWallet->GetKeyFromPool(newKey));
        pubkeys[2] = newKey;
        pWallet->SetAddressBook(newKey.GetID(), "", "receive");

        CScript result = GetScriptForMultisig(2, pubkeys);
        ASSERT_FALSE(result.size() > MAX_SCRIPT_ELEMENT_SIZE);
        CScriptID innerID(result);
        pWallet->AddCScript(result);
        pWallet->SetAddressBook(innerID, "", "receive");

        std::string address = EncodeDestination(innerID);
        addresses.push_back(address);
    }
    
    // Print out the addresses, 4 on each line.
    std::string s = "vVRewardSystemAddress = {\n";
    int i=0;
    int colsPerRow = 4;
    ASSERT_TRUE(numKeys % colsPerRow == 0);
    int numRows = numKeys/colsPerRow;
    for (int row=0; row<numRows; row++) {
        s += "    ";
        for (int col=0; col<colsPerRow; col++) {
            s += "\"" + addresses[i++] + "\", ";
        }
        s += "\n";
    }
    s += "    };";
    std::cout << s << std::endl;

    pWallet->Flush(true);
}
#endif


// Utility method to check the number of unique addresses from height 1 to maxHeight
void checkNumberOfUniqueAddresses(int nUnique) {
    int maxHeight = Params().GetConsensus().GetLastVRewardSystemBlockHeight();
    std::set<std::string> addresses;
    for (int i = 1; i <= maxHeight; i++) {
        addresses.insert(Params().GetVRewardSystemAddressAtHeight(i));
    }
    ASSERT_TRUE(addresses.size() == nUnique);
}


TEST(vidulum_reward_system_test, general) {
    SelectParams(CBaseChainParams::TESTNET);

    CChainParams params = Params();
    
    // Fourth testnet reward:
    // address = t2ENg7hHVqqs9JwU5cgjvSbxnT2a9USNfhy
    // script.ToString() = OP_HASH160 55d64928e69829d9376c776550b6cc710d427153 OP_EQUAL
    // HexStr(script) = a91455d64928e69829d9376c776550b6cc710d42715387
    EXPECT_EQ(HexStr(params.GetVRewardSystemScriptAtHeight(1)), "a914ef775f1f997f122a062fff1a2d7443abd1f9c64287");
    EXPECT_EQ(params.GetVRewardSystemAddressAtHeight(1), "t2UNzUUx8mWBCRYPRezvA363EYXyEpHokyi");
    EXPECT_EQ(HexStr(params.GetVRewardSystemScriptAtHeight(53126)), "a914ac67f4c072668138d88a86ff21b27207b283212f87");
    EXPECT_EQ(params.GetVRewardSystemAddressAtHeight(53126), "t2NGQjYMQhFndDHguvUw4wZdNdsssA6K7x2");
    EXPECT_EQ(HexStr(params.GetVRewardSystemScriptAtHeight(53127)), "a91455d64928e69829d9376c776550b6cc710d42715387");
    EXPECT_EQ(params.GetVRewardSystemAddressAtHeight(53127), "t2ENg7hHVqqs9JwU5cgjvSbxnT2a9USNfhy");

    int maxHeight = params.GetConsensus().GetLastVRewardSystemBlockHeight();
    
    // If the block height parameter is out of bounds, there is an assert.
    EXPECT_DEATH(params.GetVRewardSystemScriptAtHeight(0), "nHeight");
    EXPECT_DEATH(params.GetVRewardSystemScriptAtHeight(maxHeight+1), "nHeight");
    EXPECT_DEATH(params.GetVRewardSystemAddressAtHeight(0), "nHeight");
    EXPECT_DEATH(params.GetVRewardSystemAddressAtHeight(maxHeight+1), "nHeight"); 
}


#define NUM_MAINNET_VRS_ADDRESSES 1

TEST(vidulum_reward_system_test, mainnet) {
    SelectParams(CBaseChainParams::MAIN);
    checkNumberOfUniqueAddresses(NUM_MAINNET_VRS_ADDRESSES);
}


#define NUM_TESTNET_VRS_ADDRESSES 1

TEST(vidulum_reward_system_test, testnet) {
    SelectParams(CBaseChainParams::TESTNET);
    checkNumberOfUniqueAddresses(NUM_TESTNET_VRS_ADDRESSES);
}


#define NUM_REGTEST_VRS_ADDRESSES 1

TEST(vidulum_reward_system_test, regtest) {
    SelectParams(CBaseChainParams::REGTEST);
    checkNumberOfUniqueAddresses(NUM_REGTEST_VRS_ADDRESSES);
}



TEST(vidulum_reward_system_test, slow_start_subsidy) {
    SelectParams(CBaseChainParams::MAIN);
    CChainParams params = Params();

    int maxHeight = params.GetConsensus().GetLastVRewardSystemBlockHeight();    
    CAmount totalSubsidy = 0;
    for (int nHeight = 1; nHeight <= maxHeight; nHeight++) {
        CAmount nSubsidy = GetBlockSubsidy(nHeight, params.GetConsensus()) / 20;
        totalSubsidy += nSubsidy;
    }
    
    ASSERT_TRUE(totalSubsidy == MAX_MONEY/10.0);
}


// For use with mainnet and testnet which each have 48 addresses.
// Verify the number of rewards each individual address receives.
void verifyNumberOfRewards() {
    CChainParams params = Params();
    int maxHeight = params.GetConsensus().GetLastVRewardSystemBlockHeight();
    std::multiset<std::string> ms;
    for (int nHeight = 1; nHeight <= maxHeight; nHeight++) {
        ms.insert(params.GetVRewardSystemAddressAtHeight(nHeight));
    }

    ASSERT_TRUE(ms.count(params.GetVRewardSystemAddressAtIndex(0)) == 17708);
    // for (int i = 1; i <= 46; i++) {
    //     ASSERT_TRUE(ms.count(params.GetVRewardSystemAddressAtIndex(i)) == 17709);
    // }
    // ASSERT_TRUE(ms.count(params.GetVRewardSystemAddressAtIndex(47)) == 17677);
}

// Verify the number of rewards going to each mainnet address
TEST(vidulum_reward_system_test, per_address_reward_mainnet) {
    SelectParams(CBaseChainParams::MAIN);
    verifyNumberOfRewards();
}

// Verify the number of rewards going to each testnet address
TEST(vidulum_reward_system_test, per_address_reward_testnet) {
    SelectParams(CBaseChainParams::TESTNET);
    verifyNumberOfRewards();
}
