#include "MclPrecompiled.h"
#include <libdevcore/Common.h>
#include <libethcore/ABI.h>
#include <string>
#define BLS_ETH
#include <mcl/bn_c384_256.h>
#include <mcl/bls12_381.hpp>
#include <bls/bls.hpp>
#include <bls/bls384_256.h>

using namespace dev;
using namespace dev::blockverifier;
using namespace dev::precompiled;
using namespace mcl::bn;
using namespace bls;

/*
contract MCL {
    function generatorGen(string g, string h) public constant returns(string, string);
    function reEncrypt(string ca1, string rk) public constant returns(string);
    function aggregateSig(string sig1, string sig2) public constant returns(string);
    function multiAggregateSig(string sig1, string sig2, string pk1, string pk2) public constant returns(string);
    function multiAggregatePK(string pk1, string pk2) public constant returns(string);
}
*/

const char* const MCL_PAIRING = "reEncrypt(string,string)";
const char* const MCL_AGGREGATE = "aggregateSig(string,string)";
const char* const MCL_MULTI_AGGREGATE_SIG = "multiAggregateSig(string,string,string,string)";
const char* const MCL_MULTI_AGGREGATE_PK = "multiAggregatePK(string,string)";

MclPrecompiled::MclPrecompiled()
{
    init(MCL_BLS12_381, MCLBN_COMPILED_TIME_VAR);
    blsSetETHmode(BLS_ETH_MODE_LATEST);
    name2Selector[MCL_PAIRING] = getFuncSelector(MCL_PAIRING);
    name2Selector[MCL_AGGREGATE] = getFuncSelector(MCL_AGGREGATE);
    name2Selector[MCL_MULTI_AGGREGATE_SIG] = getFuncSelector(MCL_MULTI_AGGREGATE_SIG);
    name2Selector[MCL_MULTI_AGGREGATE_PK] = getFuncSelector(MCL_MULTI_AGGREGATE_PK);
}

PrecompiledExecResult::Ptr MclPrecompiled::call(ExecutiveContext::Ptr, bytesConstRef param, Address const&, Address const&)
{
    auto func = getParamFunc(param);
    auto data = getParamData(param);

    dev::eth::ContractABI abi;
    auto callResult = m_precompiledExecResultFactory->createPrecompiledResult();
    callResult->gasPricer()->setMemUsed(param.size());

    if (func == name2Selector[MCL_PAIRING])
    {
        std::string ca1, rk;
        abi.abiOut(data, ca1, rk);
        G1 P;
        G2 Q;
        P.deserializeHexStr(ca1);
        Q.deserializeHexStr(rk);
        Fp12 cb1;
        pairing(cb1, P, Q);
        callResult->setExecResult(abi.abiIn("", cb1.serializeToHexStr()));
    }
    else if (func == name2Selector[MCL_AGGREGATE])
    {
        std::string sig1, sig2;
        abi.abiOut(data, sig1, sig2);
        Signature sigs[2], sig;
        sigs[0].deserializeHexStr(sig1);
        sigs[1].deserializeHexStr(sig2);
        blsAggregateSignature((blsSignature *)&sig, (const blsSignature *)&sigs[0], 2);
        callResult->setExecResult(abi.abiIn("", sig.serializeToHexStr()));
    }
    else if (func == name2Selector[MCL_MULTI_AGGREGATE_SIG])
    {
        std::string sig1, sig2, pk1, pk2;
        abi.abiOut(data, sig1, sig2, pk1, pk2);
        Signature sigs[2], sig;
        PublicKey pks[2];
        sigs[0].deserializeHexStr(sig1);
        sigs[1].deserializeHexStr(sig2);
        pks[0].deserializeHexStr(pk1);
        pks[1].deserializeHexStr(pk2);
        blsMultiAggregateSignature((blsSignature *)&sig, (blsSignature *)&sigs[0], (blsPublicKey *)&pks[0], 2);
        callResult->setExecResult(abi.abiIn("", sig.serializeToHexStr()));
    }
    else if (func == name2Selector[MCL_MULTI_AGGREGATE_PK])
    {
        std::string pk1, pk2;
        abi.abiOut(data, pk1, pk2);
        PublicKey pks[2], pk;
        pks[0].deserializeHexStr(pk1);
        pks[1].deserializeHexStr(pk2);
        blsMultiAggregatePublicKey((blsPublicKey *)&pk, (blsPublicKey *)&pks[0], 2);
        callResult->setExecResult(abi.abiIn("", pk.serializeToHexStr()));
    }
    else
    {
        PRECOMPILED_LOG(ERROR) << LOG_BADGE("MclPrecompiled")
                               << LOG_DESC("call undefined function") << LOG_KV("func", func);
        getErrorCodeOut(callResult->mutableExecResult(), CODE_UNKNOW_FUNCTION_CALL);
    }
    return callResult;
}
