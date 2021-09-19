#include "ProxyReEncryptionPrecompiled.h"
#include <libdevcore/Common.h>
#include <libethcore/ABI.h>
#include <string>

#include <mcl/bls12_381.hpp>

using namespace dev;
using namespace dev::blockverifier;
using namespace dev::precompiled;
using namespace mcl::bn;

/*
contract PRE {
    function generatorGen(string g, string h) public constant returns(string, string);
    function reEncrypt(string ca1, string rk) public constant returns(string);
}
*/

const char* const PRE_GENERATOR_GEN = "generatorGen(string,string)";
const char* const PRE_REENCRYPT = "reEncrypt(string,string)";

ProxyReEncryptionPrecompiled::ProxyReEncryptionPrecompiled()
{
    initPairing(mcl::BLS12_381);
    name2Selector[PRE_GENERATOR_GEN] = getFuncSelector(PRE_GENERATOR_GEN);
    name2Selector[PRE_REENCRYPT] = getFuncSelector(PRE_REENCRYPT);
}

PrecompiledExecResult::Ptr ProxyReEncryptionPrecompiled::call(ExecutiveContext::Ptr, bytesConstRef param, Address const&, Address const&)
{
    auto func = getParamFunc(param);
    auto data = getParamData(param);

    dev::eth::ContractABI abi;
    auto callResult = m_precompiledExecResultFactory->createPrecompiledResult();
    callResult->gasPricer()->setMemUsed(param.size());

    if (func == name2Selector[PRE_GENERATOR_GEN])
    {
        std::string g, h;
        abi.abiOut(data, g, h);
        G1 P;
        G2 Q;
        hashAndMapToG1(P, g);
        hashAndMapToG2(Q, h);
        callResult->setExecResult(abi.abiIn("", P.serializeToHexStr(), Q.serializeToHexStr()));
    }
    else if (func == name2Selector[PRE_REENCRYPT])
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
    else
    {
        PRECOMPILED_LOG(ERROR) << LOG_BADGE("ProxyReEncryptionPrecompiled")
                               << LOG_DESC("call undefined function") << LOG_KV("func", func);
        getErrorCodeOut(callResult->mutableExecResult(), CODE_UNKNOW_FUNCTION_CALL);
    }
    return callResult;
}
