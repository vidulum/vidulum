// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "main.h"
#include "crypto/equihash.h"

#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

#include "base58.h"

#include "chainparamsseeds.h"

static CBlock CreateGenesisBlock(const char* pszTimestamp, const CScript& genesisOutputScript, uint32_t nTime, const uint256& nNonce, const std::vector<unsigned char>& nSolution, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    CMutableTransaction txNew;
    txNew.nVersion = 1;
    txNew.vin.resize(1);
    txNew.vout.resize(1);
    txNew.vin[0].scriptSig = CScript() << 520617983 << CScriptNum(4) << std::vector<unsigned char>((const unsigned char*)pszTimestamp, (const unsigned char*)pszTimestamp + strlen(pszTimestamp));
    txNew.vout[0].nValue = genesisReward;
    txNew.vout[0].scriptPubKey = genesisOutputScript;

    CBlock genesis;
    genesis.nTime    = nTime;
    genesis.nBits    = nBits;
    genesis.nNonce   = nNonce;
    genesis.nSolution = nSolution;
    genesis.nVersion = nVersion;
    genesis.vtx.push_back(txNew);
    genesis.hashPrevBlock.SetNull();
    genesis.hashMerkleRoot = genesis.BuildMerkleTree();
    return genesis;
}

/**
 * Build the genesis block. Note that the output of its generation
 * transaction cannot be spent since it did not originally exist in the
 * database (and is in any case of zero value).
 *
 * >>> from pyblake2 import blake2s
 * >>> 'Vidulum' + blake2s(b'10/16/2018 Vidulum is born.').hexdigest()
*/

static CBlock CreateGenesisBlock(uint32_t nTime, const uint256& nNonce, const std::vector<unsigned char>& nSolution, uint32_t nBits, int32_t nVersion, const CAmount& genesisReward)
{
    const char* pszTimestamp = "Vidulum06f1d5699f9d2543de5b94c771cf5f32449ebf848789f2bd41695a30547f7ae6";   
    const CScript genesisOutputScript = CScript() << ParseHex("048491c81f5495acf2e93e5c62178db5c5f147fff8741e769227d3fa9e20cefe7458025c040c6dfbdf81b5ed61d2f4ed721afcd16fbfe11dd9ac7aad0f903534d4") << OP_CHECKSIG;
    return CreateGenesisBlock(pszTimestamp, genesisOutputScript, nTime, nNonce, nSolution, nBits, nVersion, genesisReward);
}

/**
 * Main network
 */
/**
 * What makes a good checkpoint block?
 * + Is surrounded by blocks with reasonable timestamps
 *   (no blocks before with a timestamp after, none after with
 *    timestamp before)
 * + Contains no strange transactions
 */

const arith_uint256 maxUint = UintToArith256(uint256S("ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"));

class CMainParams : public CChainParams {
public:
    CMainParams() {
        strNetworkID = "main";
        strCurrencyUnits = "VDL";
        consensus.fCoinbaseMustBeProtected = true;
        //consensus.nSubsidySlowStartInterval = 0;
        //consensus.nSubsidyHalvingInterval = 60 * 24 * 365 * 4;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 4000;
        consensus.powLimit = uint256S("0007ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowAveragingWindow = 17;
        consensus.nMasternodePaymentsStartBlock = 105;
        //consensus.nMasternodePaymentsIncreasePeriod = 43200; // 1 month

        assert(maxUint/UintToArith256(consensus.powLimit) >= consensus.nPowAveragingWindow);
        consensus.nPowMaxAdjustDown = 32; // 32% adjustment down
        consensus.nPowMaxAdjustUp = 16; // 16% adjustment up
        consensus.nPowTargetSpacing = 1 * 60; // 1 min
        /**
         * The message start string should be awesome! Ⓢ❤
         */
        pchMessageStart[0] = 0xde;
        pchMessageStart[1] = 0xad;
        pchMessageStart[2] = 0xc0;
        pchMessageStart[3] = 0x01;
        vAlertPubKey = ParseHex("04e382452a592966a6ff09470865a86e360341fdc7bc2d0b2a218bacc6d46ec9540b90e35c9d888c95a127373d4661afb74c5d038c3248e7b5402b6a601734c3d5");
        nDefaultPort = 7676;
        nMaxTipAge = 24 * 60 * 60;
        nPruneAfterHeight = 100000;
        newTimeRule = 246600;

        //Reinstate Equihash 200_9
        const size_t N = 200, K = 9;
        BOOST_STATIC_ASSERT(equihash_parameters_acceptable(N, K));
        nEquihashN = N;
        nEquihashK = K;

        nMasternodeCountDrift = 0;

        genesis = CreateGenesisBlock(
            1539727200,
            uint256S("0x0000000000000000000000000000000000000000000000000000000000002e30"),
            ParseHex("00afe13c5c40dc5abb9832ff7e720ef54377f063a4054fc35c994b053ea0c5631c03bb5688d7c7c7b14c056b19fabc43bda26e6d02ce1bf022dcd0a987a65609730ed6db9319878debf538f8de10f20498f4051c04ba9897e303bb615bbea15c3194eb1a9713b904e42dedcdfd73a8dc9b98f6a66af47cb8faeacad82ff406e7c3bc3c923503d30d70c2f7bfe4b09607f5842809a82fbb9a9b13a70886540e5caf87bd60918fe83f02412ed690c1c6b7afe7007dd063aa121e27b789c72d578f6770116331f525e2e836ecfbbf519d3e23f602afc5230ed046e94d3ee1327643b3c96824110fdc045d67ee18e252897c612138e0fa7154f9eebb0553059432ba8949188c4c8b4808b05441e6981fdef8140dbaf0f43e4618ab427b233fd05b0710d434ab30451782da72b6348997c551f962c9e124f34f665d7c631e7ba4fcef4a8557da92a701aa50fa46cb229b87c9013f4fcf44c6fbddbf9f6738d9ec218667b05aa375167c224bb4b92997d9d0020b4cc32dab15ff3b0ad7081096759808362f52194158e19311788acb46439b2de2f7adb84ffd91340203009cd79f553611eb491301f34490929037c19b14a06eca30b278b1339917575b7b4f7dba26bec5cdee56c5df449372ad6bba89cd1eb609e62cdd511d308d84d10af280f13ed3ce161c479b37e2d0198ee97093673ba3f9d7af0ccafcf27b078604b5aa2e8c0fb5ec9582727be4ad83fb532b372aa64eb52a633ea159a7e34dec3795ba360d57e26816d29b5c339e3717a5f7a47e48f23429889aede2ed17dd10f17da7fcf3426092e50f7a326a46b77d187e0a03f49037134cd5f4bd8236796d8a213ad514015f507f449a1158b9e0fea9a874c75ed54257bb72eea911fee59733d25a1123316b2e2972ca2b5a7b1c6f8d342abcbbdb9724d902c815b6a930018a90a7388a0b02a21df47399a3a796a3c06446b418b13978ed503a0fe98de223d419f8b019f4454badaeb9a424585444158b0c93fee235e92ed159814cefde86f0c0be94691db35700b3c7dba6fcaad2be225788cab601365908048a3aeca4f7f349dcd6c6bf1cbcf54e3caeda766f11d9c7f97a0e398ef235737656a4b5056d563c306c22ac0181fc092bbdbda242dfb271ecc91667b4a49f5a46a6380bddd9f38079e653aa333687182bdb9bc6034b7a2cb723eb7bc7f8d7fb61f8a4fe4a9c7af3781ecf445ed70c7bc1a24a3741f6519541e93fffbacb077d48fd9f16f3d18d58c5ee85bd26a38a639fc1841034654faec54f39132814ac25c086c1575a3c95af09a09ba96bdb86c37f68d4a2605d129a18e7d991da284674af73d9b69df13da3ee07ef7c5e277db472a1327ac57e09e49a2fcecc1328bd5c85fe1f00f8388b4c82374cc2166c09178c874be55c48267f2976717a040e6787fed6f6130cfdb38badcde5cf25f3fa5e5c09a6467974aaeae3653990cb1cb636805a4473ab680889a667a860434d61204144f161c8b1c2a7b793af1e8cb73c3b0a0c614dd359852855fb5ae504dfc9330ace317074ce9c15ba1c911d31427a2169b71ac1442520e22b703084f7dea338686be62aaa25c3bef00114484e710ae6ddefd95dd7a6ecc76edf488d3b427b37029e5ff4e7913dd39965bc9cb8abe9fa1e9191bb09310d3b3dfd9ac1edfe445cc9631ae232aadd1a52154fd6786c352507cce5b3df777d70811c1d1d4ae2096b7af50093de63fa90456090d4d95a32d4dd160815775c2d820c6337f26bd1e82e7bca2276e2bacb1c0eef2719b45d13316298159732f84c61acf756f7aa1e5dacf5b0cc4626939c8300eaa769bce6da1d1bfe2c0149c99d7412efbb8b03e722d0bf21624219c22c432363de4e1db4e3c09fbafa71fb0d92d936b736cd"),
            0x1f07ffff, 4, 0);
        consensus.hashGenesisBlock = genesis.GetHash();
        assert(consensus.hashGenesisBlock == uint256S("0x0001d9baed52947276fac344e3a91ec2de0f13495919c53f7316112d8bc5eda8"));
        assert(genesis.hashMerkleRoot == uint256S("0x5553f657860d32ad22878847a023c515a8836d5fda055f2c484491a9a3347541"));

        vFixedSeeds.clear();
        vSeeds.clear();

        //vSeeds.push_back(CDNSSeedData("dnsseed1.vidulum.org", "dnsseed1.vidulum.org")); //Vidulum seed node

        // guarantees the first 2 characters, when base58 encoded, are "v1"
        base58Prefixes[PUBKEY_ADDRESS]     = {0x1D,0xD8};
        // guarantees the first 2 characters, when base58 encoded, are "v3"
        base58Prefixes[SCRIPT_ADDRESS]     = {0x1D,0xDE};
        // the first character, when base58 encoded, is "5" or "K" or "L" (as in Bitcoin)
        base58Prefixes[SECRET_KEY]         = {0x80};
        // do not rely on these BIP32 prefixes; they are not specified and may change
        base58Prefixes[EXT_PUBLIC_KEY]     = {0x04,0x88,0xB2,0x1E};
        base58Prefixes[EXT_SECRET_KEY]     = {0x04,0x88,0xAD,0xE4};
        // guarantees the first 2 characters, when base58 encoded, are "zc"
        base58Prefixes[ZCPAYMENT_ADDRRESS] = {0x16,0x9A};
        // guarantees the first 4 characters, when base58 encoded, are "ZiVK"
        base58Prefixes[ZCVIEWING_KEY]      = {0xA8,0xAB,0xD3};
        // guarantees the first 2 characters, when base58 encoded, are "SK"
        base58Prefixes[ZCSPENDING_KEY]     = {0xAB,0x36};

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;
		fHeadersFirstSyncingActive = false;
        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (0, consensus.hashGenesisBlock),
            1539727200,  // * UNIX timestamp of last checkpoint block
            0,           // * total number of transactions between genesis and last checkpoint
                         //   (the tx=... number in the SetBestChain debug.log lines)
            0            //   total number of tx / (checkpoint block height / (24 * 24))
        };


        nPoolMaxTransactions = 3;
        strSporkKey = "044d7df3280836c25cbe093424bf164527e8e469aa45ff3c12460efaa3eda393b7558ba722f376f4fb9750b08027a8534e25ad5f14d29b41889ee354cfc0e026c1";

        strObfuscationPoolDummyAddress = "v1XFNouBdZ6B4yZxNhR2YyzWQwn4nKNejZN";
        nStartMasternodePayments = 1539727200; //2018-10-16
        nBudget_Fee_Confirmations = 6; // Number of confirmations for the finalization fee
    }
};
static CMainParams mainParams;

/**
 * Testnet (v3)
 */
class CTestNetParams : public CChainParams {
public:
    CTestNetParams() {
        strNetworkID = "test";
        strCurrencyUnits = "VDLT";
        consensus.fCoinbaseMustBeProtected = true;
        //consensus.nSubsidySlowStartInterval = 0;
        //consensus.nSubsidyHalvingInterval = 60 * 24 * 365 * 4;
        consensus.nMajorityEnforceBlockUpgrade = 51;
        consensus.nMajorityRejectBlockOutdated = 75;
        consensus.nMajorityWindow = 400;
        consensus.powLimit = uint256S("07ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff");
        consensus.nPowAveragingWindow = 17;
        assert(maxUint/UintToArith256(consensus.powLimit) >= consensus.nPowAveragingWindow);
        consensus.nPowMaxAdjustDown = 32; // 32% adjustment down
        consensus.nPowMaxAdjustUp = 16; // 16% adjustment up
        consensus.nPowTargetSpacing = 1 * 60;
        consensus.nMasternodePaymentsStartBlock = 150;
        //consensus.nMasternodePaymentsIncreasePeriod = 200;
        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0x1a;
        pchMessageStart[2] = 0xf9;
        pchMessageStart[3] = 0xbf;
        vAlertPubKey = ParseHex("044e7a1553392325c871c5ace5d6ad73501c66f4c185d6b0453cf45dec5a1322e705c672ac1a27ef7cdaf588c10effdf50ed5f95f85f2f54a5f6159fca394ed0c6");
        nDefaultPort = 7676;
		nMaxTipAge = 24 * 60 * 60 * 5;
        nPruneAfterHeight = 1000;
        
        //Reinstate Equihash 200_9
        const size_t N = 200, K = 9;
        BOOST_STATIC_ASSERT(equihash_parameters_acceptable(N, K));
        nEquihashN = N;
        nEquihashK = K;
		
    	genesis = CreateGenesisBlock(
            1539727200,
            uint256S("0000000000000000000000000000000000000000000000000000000000000003"),
            ParseHex("01a1c314995552e7853cd90923f5510743053c6ff413344d3fd5eedf63aa8356ed93bd9745e835b6782f17d9b636906f5733cd1d033681df302e6356fb088230e34520a429eb27cb82f353a6c1f822a7709b09b3030262bb56275adf41de060f7a7e97478fbe5ff94b2989c590c31b8d137d8da44e216e9ea5f29f3e4dff04633035261089bb0da7421a39b27614d03babf8c32d9b820acd8bc339d983258fcef3f4de50071f0dec02ae2e6ec6905581852bc0a531b3ac21dee3ef4f543a6acce2c21d90fd961b33c07bd51686843174dfa303fbea0a478316716c3955ef3ee34ce1b456f374e1058704419b18088be782b608aa763a7382efbc7a7107c3c063a4aff1f3e04f92a1a6cfedfdafb5b7042720ef918019a447d57115d52bcc2b01b59a94facbe7149eb682b89886fbae98525d74b50e90d94853d223398cac854dcf9479c49b44339747e6953b0acfbb4d096c6ed5b2475eccf8a8b3bc9cbee7c50740596dff1c951d4e3f2ae32fe19382d0b172938950fe4e4ddd193edf8d6973bcddac2124c9d7d54f1af5ea3fe5b845df2b2371d7573728ba068df1b629c70e753cff950be6afaa4bd258f6e5f8d5d780649b9597f20ec7392b951f8462a1b6798bf875d7aadcb741bd93bf3db2128136fb6ee0e7e77dd38987b379966f6dc35cd38f19439c0e61cc8f69538831e2e1cdc1f0e34256fcec0a6a55004fc454ed23d4954287766fca49bff4c85d1d98532e251725f2f0ae01e00c6091f24383b5033a0dbe92be9269a22ba837773a4370b579e5fb50923522ed2d19402d64718ed2d50f6337f545d36f5954bb1805e441890ab1f9b5b0e22ecc9e870cf899ba460437981d0cb4902abbefec23afda2afde5940615f05c237f1db0fa1c2dddf5f037610ad7384246d67585fd2ea6f2bd15dec0476fdd88e966672f9311a699fe1301e081f63159e0c8cf45e058bd75685031a7c837ad13723c6ace5b9b776372d720733aefedf64a9e81f1040cec75df52a0238a182217a1aceef643cbf7463208e790d05a2fb6d7c7c5a1e4ae13568eae477d545908d76ce6f54851469c98d0b32eedb344495adc814d1ce2dd98f78af813aaf163412adb24e25a6a791f131bd125c39719c1b93a3a126a40c6a56119c7ec5c081cea7ab5424ac55fc9ae62a8c7ccc4816cdb90543d0279c9bb3641231bebea3360a340e159a0ea0f5e7914ab78c616daaf832425227554aafffd35f11d606f04b03107256e6a2f99e8cc5b92f48433bca93f17e1239f0465aea5ec5d3304f540226cbef18d6b38a000067d3b12aeee33db8ff33383c3dfdc398371586a2c18822e45ca6edadfbc13c5bf88e19cf30f695e9b5b44796c18a014899b4308c73cac3db8fafb21be0cd54d819de06666070be367184b6e642bc78827dfe6d7049b853e0fc8fab79e6d0213aa35200249adb3cc961658b35d0c52d2b11bb183c232e7429268577f3c0f1e1f22291de56c51463834cd68f53fd2528699bb2d56d75f3db356276971dd35e64ed47669d5a8cfab830840c766c1021b72f3b973351f9a686566a7bb6d056aaf6fb814abb72d77c55a1d5c6a351fde369fa9bb0aaa9253a594c1196d9e22fa407c898590a27803d90f7b6c4f5408a99731dac3b813b585c5452bf9c38e054fb8caa308e4232349f093097fdbf1a95c7cd41f4962ccb834533b0764ad74ec7fda128da873d113ff219bfd6e7dd1a341c62aa2718a7a930e3453bfa15629e629c3714cb4b1c3f0d69b14742a9750c51ebbb206b790cd770f607199e573cb30f9620a7fd3f455cc23f1ca8169538e0d9bda126928b3f3fae2b37d52b31c72a3d78e48c87d508e15982db4338df9cf5b3765492e365378e9d5abe9c4459d186d8ae6d2f63d6c46"),
            0x2007ffff, 4, 0);
			
		consensus.hashGenesisBlock = genesis.GetHash();

        //assert(consensus.hashGenesisBlock == uint256S("0x00ebbe7c54aaf95ba74473266cff64ba00ee2178d2d88375b9d155b4e612e886"));

        vFixedSeeds.clear();
        vSeeds.clear();
        //vSeeds.push_back(CDNSSeedData("vidulum.org", "testnet.explorer.vidulum.org"));

        // guarantees the first 2 characters, when base58 encoded, are "tm"
        base58Prefixes[PUBKEY_ADDRESS]     = {0x1D,0x25};
        // guarantees the first 2 characters, when base58 encoded, are "t2"
        base58Prefixes[SCRIPT_ADDRESS]     = {0x1C,0xBA};
        // the first character, when base58 encoded, is "9" or "c" (as in Bitcoin)
        base58Prefixes[SECRET_KEY]         = {0xEF};
        // do not rely on these BIP32 prefixes; they are not specified and may change
        base58Prefixes[EXT_PUBLIC_KEY]     = {0x04,0x35,0x87,0xCF};
        base58Prefixes[EXT_SECRET_KEY]     = {0x04,0x35,0x83,0x94};
        // guarantees the first 2 characters, when base58 encoded, are "zt"
        base58Prefixes[ZCPAYMENT_ADDRRESS] = {0x16,0xB6};
        // guarantees the first 4 characters, when base58 encoded, are "ZiVt"
        base58Prefixes[ZCVIEWING_KEY]      = {0xA8,0xAC,0x0C};
        // guarantees the first 2 characters, when base58 encoded, are "ST"
        base58Prefixes[ZCSPENDING_KEY]     = {0xAC,0x08};

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

		checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (0, consensus.hashGenesisBlock),
            1539727200,  // * UNIX timestamp of last checkpoint block
            0,       // * total number of transactions between genesis and last checkpoint
                         //   (the tx=... number in the SetBestChain debug.log lines)
            0          //   total number of tx / (checkpoint block height / (24 * 24))
        };
		
		nStartMasternodePayments = 1520121600; //2018-03-04
    }
};
static CTestNetParams testNetParams;

/**
 * Regression test
 */
class CRegTestParams : public CChainParams {
public:
    CRegTestParams() {
        strNetworkID = "regtest";
        strCurrencyUnits = "REG";
        consensus.fCoinbaseMustBeProtected = false;
        //consensus.nSubsidySlowStartInterval = 0;
        //consensus.nSubsidyHalvingInterval = 150;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.powLimit = uint256S("0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f");
        consensus.nPowAveragingWindow = 17;
        assert(maxUint/UintToArith256(consensus.powLimit) >= consensus.nPowAveragingWindow);
        consensus.nPowMaxAdjustDown = 0; // Turn off adjustment down
        consensus.nPowMaxAdjustUp = 0; // Turn off adjustment up
        consensus.nPowTargetSpacing = 1 * 60;

        pchMessageStart[0] = 0xaa;
        pchMessageStart[1] = 0xe8;
        pchMessageStart[2] = 0x3f;
        pchMessageStart[3] = 0x5f;
        nDefaultPort = 27676;
        nMaxTipAge = 24 * 60 * 60 * 5;
        nPruneAfterHeight = 1000;
        
        //Reinstate Equihash 200_9
        const size_t N = 48, K = 5;
        BOOST_STATIC_ASSERT(equihash_parameters_acceptable(N, K));
        nEquihashN = N;
        nEquihashK = K;

    	genesis = CreateGenesisBlock(
            1539727200,
            uint256S("0000000000000000000000000000000000000000000000000000000000000003"),
            ParseHex("04829bd8444aa91d19163c0a94c3f529eafe0adb8e48275d9adbb325b8e35623d5c5f3d9"),
            0x200f0f0f, 4, 0);
			
        consensus.hashGenesisBlock = genesis.GetHash();

        //assert(consensus.hashGenesisBlock == uint256S("0x0602c7cbfe694a573df593aa1b57077fa5b76bb79fa1a1fab55c7e480d476559"));

        vFixedSeeds.clear(); //! Regtest mode doesn't have any fixed seeds.
        vSeeds.clear();  //! Regtest mode doesn't have any DNS seeds.

        fMiningRequiresPeers = false;
        fDefaultConsistencyChecks = true;
        fRequireStandard = false;
        fMineBlocksOnDemand = true;
        fTestnetToBeDeprecatedFieldRPC = false;

        checkpointData = (CCheckpointData){
            boost::assign::map_list_of
            ( 0, consensus.hashGenesisBlock),
            0,
            0,
            0
        };
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

const CChainParams &Params() {
    assert(pCurrentParams);
    return *pCurrentParams;
}

CChainParams &Params(CBaseChainParams::Network network) {
    switch (network) {
        case CBaseChainParams::MAIN:
            return mainParams;
        case CBaseChainParams::TESTNET:
            return testNetParams;
        case CBaseChainParams::REGTEST:
            return regTestParams;
        default:
            assert(false && "Unimplemented network");
            return mainParams;
    }
}

void SelectParams(CBaseChainParams::Network network) {
    SelectBaseParams(network);
    pCurrentParams = &Params(network);

    // Some python qa rpc tests need to enforce the coinbase consensus rule
    if (network == CBaseChainParams::REGTEST && mapArgs.count("-regtestprotectcoinbase")) {
        regTestParams.SetRegTestCoinbaseMustBeProtected();
    }
}

bool SelectParamsFromCommandLine()
{
    CBaseChainParams::Network network = NetworkIdFromCommandLine();
    if (network == CBaseChainParams::MAX_NETWORK_TYPES)
        return false;

    SelectParams(network);
    return true;
}
