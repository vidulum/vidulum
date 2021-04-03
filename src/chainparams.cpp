// Copyright (c) 2010 Satoshi Nakamoto
// Copyright (c) 2009-2014 The Bitcoin Core developers
// Distributed under the MIT software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.

#include "key_io.h"
#include "main.h"
#include "crypto/equihash.h"

#include "util.h"
#include "utilstrencodings.h"

#include <assert.h>

#include <boost/assign/list_of.hpp>

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
        bip44CoinType = 370; // As registered in https://github.com/satoshilabs/slips/blob/master/slip-0044.md
        consensus.fCoinbaseMustBeProtected = true;
        //consensus.nSubsidySlowStartInterval = 8000;
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
        consensus.nSelfishMiningDepth = 5; // how many blocks back to check for selfish mining
        consensus.nPowAllowMinDifficultyBlocksAfterHeight = boost::none;
        consensus.vUpgrades[Consensus::BASE_SPROUT].nProtocolVersion = 170007;
        consensus.vUpgrades[Consensus::BASE_SPROUT].nActivationHeight =
            Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nProtocolVersion = 170007;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].nProtocolVersion = 170008;
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].nActivationHeight = 430000;
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].nProtocolVersion = 170008;
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].nActivationHeight = 430000;
        consensus.vUpgrades[Consensus::UPGRADE_DIFA].nProtocolVersion = 170008;
        consensus.vUpgrades[Consensus::UPGRADE_DIFA].nActivationHeight = 430000;
        consensus.vUpgrades[Consensus::UPGRADE_DENNIS].nProtocolVersion = 170010;
        consensus.vUpgrades[Consensus::UPGRADE_DENNIS].nActivationHeight = 780000;
        consensus.vUpgrades[Consensus::UPGRADE_LIQUID].nProtocolVersion = 170011;
        consensus.vUpgrades[Consensus::UPGRADE_LIQUID].nActivationHeight = 1290000;

        consensus.nZawyLWMA3AveragingWindow = 60;
        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x0000000000000000000000000000000000000000000000000000097bf7cced53");
        
        /**
         * The message start string should be awesome! Ⓢ❤
         */
        pchMessageStart[0] = 0xde;
        pchMessageStart[1] = 0xad;
        pchMessageStart[2] = 0xc0;
        pchMessageStart[3] = 0x01;
        vAlertPubKey = ParseHex("04081b1f4f0d39e4bbb81c3fb654b0777b9ca5db9ef791e3b05c952d7b4cac2330a6e5d1cb5fcdc27124c387910dafa439e98848d3345a473c2390b33ceb234d7e");
        nDefaultPort = 7676;
        nMaxTipAge = 24 * 60 * 60;
        nPruneAfterHeight = 100000;
        newTimeRule = 246600;
        eh_epoch_1 = eh200_9;
        eh_epoch_2 = eh192_7;
        eh_epoch_1_endblock = 100020;
        eh_epoch_2_startblock = 100000;
        // eh_epoch_1_endtime = 1546745505;
        // eh_epoch_2_starttime = 1546741005;

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

        //vSeeds.push_back(CDNSSeedData("dnsseed1.vidulum.app", "dnsseed1.vidulum.app")); //Vidulum seed node

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

        bech32HRPs[SAPLING_PAYMENT_ADDRESS]      = "zs";
        bech32HRPs[SAPLING_FULL_VIEWING_KEY]     = "zviews";
        bech32HRPs[SAPLING_INCOMING_VIEWING_KEY] = "zivks";
        bech32HRPs[SAPLING_EXTENDED_SPEND_KEY]   = "secret-extended-key-main";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_main, pnSeed6_main + ARRAYLEN(pnSeed6_main));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = false;
		fHeadersFirstSyncingActive = false;
        checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (0, consensus.hashGenesisBlock)
            (395555, uint256S("0x0035505c0e14edf996f954e8fe11a048edb62bcf98c5bd55cc6a8d79d589bb"))
            (695555, uint256S("0x0000014cac14299e3750264fc6e0fb4036f7f10d784be26bef8f4209e49265b2"))
            (1260000, uint256S("0x00002271d5a9c0394fafd8ca17484d8f81a6e8bb2ea4fbbbb2dc753fb7780b01")),
            1617292606,  // * UNIX timestamp of last checkpoint block
            2168671,     // * total number of transactions between genesis and last checkpoint
                         //   (the tx=... number in the SetBestChain debug.log lines)
            991          //   total number of tx / (checkpoint block height / (24 * 24))
        };

        // Vidulum Rewards System script expects a vector of 2-of-3 multisig addresses
        vVRewardSystemAddress = {
            "v3mYjaryMy47xwX8Hgehj8yWLtr8XBWKrFB", /* main-index: 0*/
        };

        nPoolMaxTransactions = 3;
        strSporkKey = "044d7df3280836c25cbe093424bf164527e8e469aa45ff3c12460efaa3eda393b7558ba722f376f4fb9750b08027a8534e25ad5f14d29b41889ee354cfc0e026c1";

        strObfuscationPoolDummyAddress = "v1XFNouBdZ6B4yZxNhR2YyzWQwn4nKNejZN";
        nStartMasternodePayments = 1539727200; //2018-10-16
        nBudget_Fee_Confirmations = 6; // Number of confirmations for the finalization fee
        masternodeProtectionBlock = 430000;
        masternodeCollateral = 15000;
        assert(vVRewardSystemAddress.size() <= consensus.GetLastVRewardSystemBlockHeight());
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
        bip44CoinType = 1;
        consensus.fCoinbaseMustBeProtected = true;
        //consensus.nSubsidySlowStartInterval = 8000;
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
        consensus.nSelfishMiningDepth = 5; // how many blocks back to check for selfish mining
        consensus.nPowAllowMinDifficultyBlocksAfterHeight = 13000;
        consensus.vUpgrades[Consensus::BASE_SPROUT].nProtocolVersion = 170006;
        consensus.vUpgrades[Consensus::BASE_SPROUT].nActivationHeight =
            Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nProtocolVersion = 170006;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].nProtocolVersion = 170007;
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].nActivationHeight = 200;
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].nProtocolVersion = 170008;
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].nActivationHeight = 200;
        consensus.vUpgrades[Consensus::UPGRADE_DIFA].nProtocolVersion = 170008;
        consensus.vUpgrades[Consensus::UPGRADE_DIFA].nActivationHeight = 210;
        consensus.vUpgrades[Consensus::UPGRADE_DENNIS].nProtocolVersion = 170010;
        consensus.vUpgrades[Consensus::UPGRADE_DENNIS].nActivationHeight = 1210;
        consensus.vUpgrades[Consensus::UPGRADE_LIQUID].nProtocolVersion = 170011;
        consensus.vUpgrades[Consensus::UPGRADE_LIQUID].nActivationHeight = 6600;

        consensus.nMasternodePaymentsStartBlock = 105;
        // consensus.nMasternodePaymentsIncreasePeriod = 200;
        consensus.nZawyLWMA3AveragingWindow = 60;

		// The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");
        pchMessageStart[0] = 0xfa;
        pchMessageStart[1] = 0x1a;
        pchMessageStart[2] = 0xf9;
        pchMessageStart[3] = 0xbf;
        vAlertPubKey = ParseHex("044e7a1553392325c871c5ace5d6ad73501c66f4c185d6b0453cf45dec5a1322e705c672ac1a27ef7cdaf588c10effdf50ed5f95f85f2f54a5f6159fca394ed0c6");
        nDefaultPort = 7677;
        nMaxTipAge = 24 * 60 * 60;
        nPruneAfterHeight = 1000;
        eh_epoch_1 = eh200_9;
        eh_epoch_2 = eh192_7;
        // eh_epoch_1_endtime = 1539727200;
        // eh_epoch_2_starttime = 1539727100;
        eh_epoch_1_endblock = 10;
        eh_epoch_2_startblock = 0;

		
    	genesis = CreateGenesisBlock(
            1539727200,
            uint256S("0000000000000000000000000000000000000000000000000000000000000000"),
            ParseHex("00938afe2d4c52f598e413ee0072dc6d65eb7f6f592b1344d5f85bd967639092c87b58ac7ee53b3cd7d20263b9f0f6c6fa19135db0347f9571823ac67686041a6ff5d425a525d377f4b2147f1ee779a919d95cbb139e68e1f9924b74fbcb048707c31401fd28510e6d1db0aa4ceee661493c2678aa144ab49643efd99e0714185727f69c16e9f552720d12b9b35ce0d7c99eee39e2ce74edd1d92d83ad34c1593a2d5e7f037a858001c92d5f8980d6d8492726ea29e58705c658fc0c13222461d2125b739b1a83760768521a418556db6ad1067d9d76b94db54153cbc291d19778bdb2be11bb372b10d6e14a8e456787d434b92753a4da10e8fa478e049ac785c0e0a0994e13553055615e7656de39dc5f0b8902bfa683e6e6f692c12041110a5887a4edac1610b4dea90465a7e1d1afd4dffb6785be07783f5ab31d9fa1bbf4e46217746ba98d50eaec333ed57ede90019e4f5d528f1d619683c4daf0f4ab55a00830560809358557f0613df9a71424f606d0dbddbaf06f581d02037630434b1845783c3040af8ed65c39a231705f6d800723cfa4c6cd8d5027e889e0bfbeef2d5a71280f5d216b39b21c61fe90a34a9ed1e7f2254472ca05145a63ef83ac621dc04c831b3919b32f27899af1df46fcf38f5f696bbf99c238a6f8ecc24e559b7f9dd86a272e4ab01b75c9d6cd96e69a465b7671e4b613ad10b6bab58d0791835048e3d3f8eb20d54e0bfdf9d02f61649740221185ced4250208d288997b1ed70c8d1dd484d0e7d9ccb3d31362d4f26bd431386835729e466417d9d9e988c7de0594c492b88ef5aa07ae09041d0faa47378823ef3ac4050d1ee0d6c20df5909a80205ee1bc3028de99e1fc021f2bd2ff208f68bcf34e2726a5810f32f84ff9a4873fba61b61e630fdbdd8243e15e2d06fa48a3e23a2b2c036cb0173966feb1c400f4b38aed9a264d7f12e0a6fe3c8a44a096fff36b04644b0bc705ba456514b1b01c21e9507721b6869104de57ca61e1b8cdd39094e974e9ce026d51578b9a17c66fa25f4f71e541224361b92aed6a038879515306f519b6d14b95e14ae5a21bf0fe27411b711b533d3af1e773745c6b0bd42ec3d41b4c724903074d5dec0bb115d46a2ce6abd9d42101fdb3d582a5dc386fea3151a789c775e439fd137667f5fbd02729acdc1cdc04d96a844d18ddd7b83a315684ed51e695b6bb4c3a331daf21416da61fac51539be8db48ca4b0575e63a428287ffc9208fc57acc94d9be35ddc66c077f2513549e0e34d1192067b1b9a766d33d2aca462efb325110f90112b55033488e7912d185fcfffd6cb0716adb136c4ea86f1c7dff7b8b07f14f6306a719181b7d6d270a7bf1ca93b0978d1a26a4d74cc721ff725d2d57293a2c30575b741adc14b3474a25caddc642d4952002b04a31d4c404cf7e31809d310a172e3bb795dac60f42cee9ae0fb091a101110b802557c1db3871cbf107385fccaa9bf794e629457dbae7c8362fad5fedee12d490f4ffd64f2b2e438219e21174c169062d161006f781862771dbe3c0a85086e62525b6cbf2d6d0ff0f66fbab8586f0a142deb6ca5af56f61b4ef9b8fc717fca67bf29b386557c135bef256fbd9b13a3153af2677df6defddc3b9a0029572a068bf1628b5fea1f408f0e9bf860be8a560cf72e615e3c01d02c4ac7068173564b7e6885cf9b7c3c2713c400125cb9799737b0b023cd41f23a1fb6deef3a637ba46c94710f78e1f21b9f22a5ead152ff431e25bfd4476e9cc38115d801763340d9cd01d57d55fc4b9c96100b9b8017d405d34a4d4b8d3ed6dc3bfc1c82156c5fc3a3ed3b6c8be287fb96763991302ef3f56489f58fdca74fd1d75e430e7bd24819facfb55dc2a7170fbf792bb1d97a617"),
            0x2007ffff, 4, 0);
			
		consensus.hashGenesisBlock = genesis.GetHash();

        assert(consensus.hashGenesisBlock == uint256S("0x05a8853de8a16bed21cb4c60472d6642dee066b3d914e37aba7f7b76ef87002c"));

        vFixedSeeds.clear();
        vSeeds.clear();
        //vSeeds.push_back(CDNSSeedData("vidulum.app", "testnet.explorer.vidulum.app"));

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

        bech32HRPs[SAPLING_PAYMENT_ADDRESS]      = "ztestsapling";
        bech32HRPs[SAPLING_FULL_VIEWING_KEY]     = "zviewtestsapling";
        bech32HRPs[SAPLING_INCOMING_VIEWING_KEY] = "zivktestsapling";
        bech32HRPs[SAPLING_EXTENDED_SPEND_KEY]   = "secret-extended-key-test";

        vFixedSeeds = std::vector<SeedSpec6>(pnSeed6_test, pnSeed6_test + ARRAYLEN(pnSeed6_test));

        fMiningRequiresPeers = true;
        fDefaultConsistencyChecks = false;
        fRequireStandard = true;
        fMineBlocksOnDemand = false;
        fTestnetToBeDeprecatedFieldRPC = true;

		checkpointData = (CCheckpointData) {
            boost::assign::map_list_of
            (0, consensus.hashGenesisBlock)
            (500, uint256S("0x00077370398b0ce0f2ecacbb127afff66394d7d525de6b2be608292c015473e6")),
            1586391598,  // * UNIX timestamp of last checkpoint block
            504,         // * total number of transactions between genesis and last checkpoint
                         //   (the tx=... number in the SetBestChain debug.log lines)
            580         //   total number of tx / (checkpoint block height / (24 * 24))
        };

        // Vidulum Rewards System script expects a vector of 2-of-3 multisig addresses
        vVRewardSystemAddress = {
            "t274DoPuogFcsgPXr9Qt49u19VScq2ggCyz"
            };
        assert(vVRewardSystemAddress.size() <= consensus.GetLastVRewardSystemBlockHeight());

        nStartMasternodePayments = 1539727200; //2018-10-16
        masternodeCollateral = 150;
        masternodeProtectionBlock = 210;
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
        bip44CoinType = 1;
        consensus.fCoinbaseMustBeProtected = false;
        consensus.nSubsidySlowStartInterval = 0;
        // consensus.nSubsidyHalvingInterval = 150;
        consensus.nMajorityEnforceBlockUpgrade = 750;
        consensus.nMajorityRejectBlockOutdated = 950;
        consensus.nMajorityWindow = 1000;
        consensus.powLimit = uint256S("0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f");
        consensus.nPowAveragingWindow = 17;
        assert(maxUint/UintToArith256(consensus.powLimit) >= consensus.nPowAveragingWindow);
        consensus.nPowMaxAdjustDown = 0; // Turn off adjustment down
        consensus.nPowMaxAdjustUp = 0; // Turn off adjustment up
        consensus.nPowTargetSpacing = 1 * 60;
        consensus.nSelfishMiningDepth = 5; // how many blocks back to check for selfish mining
        consensus.nPowAllowMinDifficultyBlocksAfterHeight = 0;
        consensus.vUpgrades[Consensus::BASE_SPROUT].nProtocolVersion = 170002;
        consensus.vUpgrades[Consensus::BASE_SPROUT].nActivationHeight =
            Consensus::NetworkUpgrade::ALWAYS_ACTIVE;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nProtocolVersion = 170002;
        consensus.vUpgrades[Consensus::UPGRADE_TESTDUMMY].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].nProtocolVersion = 170006;
        consensus.vUpgrades[Consensus::UPGRADE_OVERWINTER].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].nProtocolVersion = 170007;
        consensus.vUpgrades[Consensus::UPGRADE_SAPLING].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_DENNIS].nProtocolVersion = 170010;
        consensus.vUpgrades[Consensus::UPGRADE_DENNIS].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;
        consensus.vUpgrades[Consensus::UPGRADE_DENNIS].nProtocolVersion = 170011;
        consensus.vUpgrades[Consensus::UPGRADE_DENNIS].nActivationHeight =
            Consensus::NetworkUpgrade::NO_ACTIVATION_HEIGHT;

        // The best chain should have at least this much work.
        consensus.nMinimumChainWork = uint256S("0x00");

        pchMessageStart[0] = 0xaa;
        pchMessageStart[1] = 0xe8;
        pchMessageStart[2] = 0x3f;
        pchMessageStart[3] = 0x5f;
        nDefaultPort = 7678;
        nMaxTipAge = 24 * 60 * 60;
        nPruneAfterHeight = 1000;
        eh_epoch_1 = eh48_5;
        eh_epoch_2 = eh48_5;
        eh_epoch_1_endblock = 1;
        eh_epoch_2_startblock = 1;

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
        // These prefixes are the same as the testnet prefixes
        base58Prefixes[PUBKEY_ADDRESS]     = {0x1D,0x25};
        base58Prefixes[SCRIPT_ADDRESS]     = {0x1C,0xBA};
        base58Prefixes[SECRET_KEY]         = {0xEF};
        // do not rely on these BIP32 prefixes; they are not specified and may change
        base58Prefixes[EXT_PUBLIC_KEY]     = {0x04,0x35,0x87,0xCF};
        base58Prefixes[EXT_SECRET_KEY]     = {0x04,0x35,0x83,0x94};
        base58Prefixes[ZCPAYMENT_ADDRRESS] = {0x16,0xB6};
        base58Prefixes[ZCVIEWING_KEY]      = {0xA8,0xAC,0x0C};
        base58Prefixes[ZCSPENDING_KEY]     = {0xAC,0x08};

        bech32HRPs[SAPLING_PAYMENT_ADDRESS]      = "zregtestsapling";
        bech32HRPs[SAPLING_FULL_VIEWING_KEY]     = "zviewregtestsapling";
        bech32HRPs[SAPLING_INCOMING_VIEWING_KEY] = "zivkregtestsapling";
        bech32HRPs[SAPLING_EXTENDED_SPEND_KEY]   = "secret-extended-key-regtest";

        // Vidulum Rewards System script expects a vector of 2-of-3 multisig addresses
        vVRewardSystemAddress = { "t274DoPuogFcsgPXr9Qt49u19VScq2ggCyz" };
        assert(vVRewardSystemAddress.size() <= consensus.GetLastVRewardSystemBlockHeight());
    }

    void UpdateNetworkUpgradeParameters(Consensus::UpgradeIndex idx, int nActivationHeight)
    {
        assert(idx > Consensus::BASE_SPROUT && idx < Consensus::MAX_NETWORK_UPGRADES);
        consensus.vUpgrades[idx].nActivationHeight = nActivationHeight;
    }
};
static CRegTestParams regTestParams;

static CChainParams *pCurrentParams = 0;

int32_t MAX_BLOCK_SIZE(int32_t height)
{
    if ( height >= Params().GetConsensus().vUpgrades[Consensus::UPGRADE_DIFA].nActivationHeight )
        return(MAX_BLOCK_SIZE_AFTER_UPGRADE);
    else return(MAX_BLOCK_SIZE_BEFORE_UPGRADE);
}

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


// Block height must be >0 and <=last Vidulum Rewards System block height
// Index variable i ranges from 0 - (vVRewardSystemAddress.size()-1)
std::string CChainParams::GetVRewardSystemAddressAtHeight(int nHeight) const {
    // int maxHeight = consensus.GetVRewardSystemRepeatInterval();
    // assert(nHeight > 0 && nHeight <= maxHeight);

    // size_t addressChangeInterval = (maxHeight + vVRewardSystemAddress.size()) / vVRewardSystemAddress.size();
    // size_t i = (nHeight / addressChangeInterval) % vVRewardSystemAddress.size();
    return vVRewardSystemAddress[0];
}

// Block height must be >0 and <=last Vidulum Rewards System block height
// The Vidulum Rewards System address is expected to be a multisig (P2SH) address
CScript CChainParams::GetVRewardSystemScriptAtHeight(int nHeight) const {
    assert(nHeight > 0 && nHeight <= consensus.GetLastVRewardSystemBlockHeight());

    CTxDestination address = DecodeDestination(GetVRewardSystemAddressAtHeight(nHeight).c_str());
    assert(IsValidDestination(address));
    assert(boost::get<CScriptID>(&address) != nullptr);
    CScriptID scriptID = boost::get<CScriptID>(address); // address is a boost variant
    CScript script = CScript() << OP_HASH160 << ToByteVector(scriptID) << OP_EQUAL;
    return script;
}

std::string CChainParams::GetVRewardSystemAddressAtIndex(int i) const {
    assert(i >= 0 && i < vVRewardSystemAddress.size());
    return vVRewardSystemAddress[i];
}


int validEHparameterList(EHparameters *ehparams, unsigned long blockheight, const CChainParams& params){
    //if in overlap period, there will be two valid solutions, else 1.
    //The upcoming version of EH is preferred so will always be first element
    //returns number of elements in list
    if(blockheight>=params.eh_epoch_2_start() && blockheight>params.eh_epoch_1_end()){
        ehparams[0]=params.eh_epoch_2_params();
        return 1;
    }
    if(blockheight<params.eh_epoch_2_start()){
        ehparams[0]=params.eh_epoch_1_params();
        return 1;
    }
    ehparams[0]=params.eh_epoch_2_params();
    ehparams[1]=params.eh_epoch_1_params();
    return 2;
}

void UpdateNetworkUpgradeParameters(Consensus::UpgradeIndex idx, int nActivationHeight)
{
    regTestParams.UpdateNetworkUpgradeParameters(idx, nActivationHeight);
}
