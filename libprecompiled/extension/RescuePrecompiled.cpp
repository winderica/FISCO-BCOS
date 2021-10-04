#include "RescuePrecompiled.h"
#include <libdevcore/Common.h>
#include <libethcore/ABI.h>
#include <string>
#include <boost/algorithm/hex.hpp>

using namespace dev;
using namespace dev::blockverifier;
using namespace dev::precompiled;

extern "C" uint8_t *hash(void *prev_digest, void* r);
extern "C" char *prove(void *r0, void *r1, void *digest);
extern "C" bool verify(void* r1, void *digest, void *proof);

const char* const RESCUE_HASH = "hash(string,string)";
const char* const RESCUE_VERIFY = "verify(string,string,string)";

RescuePrecompiled::RescuePrecompiled()
{
    name2Selector[RESCUE_HASH] = getFuncSelector(RESCUE_HASH);
    name2Selector[RESCUE_VERIFY] = getFuncSelector(RESCUE_VERIFY);
}

PrecompiledExecResult::Ptr RescuePrecompiled::call(ExecutiveContext::Ptr, bytesConstRef param, Address const&, Address const&)
{
    auto func = getParamFunc(param);
    auto data = getParamData(param);

    dev::eth::ContractABI abi;
    auto callResult = m_precompiledExecResultFactory->createPrecompiledResult();
    callResult->gasPricer()->setMemUsed(param.size());

    if (func == name2Selector[RESCUE_VERIFY])
    {
        std::string r1_hex, digest_hex, proof;
        abi.abiOut(data, r1_hex, digest_hex, proof);
        std::vector<uint8_t> r1;
        std::vector<uint8_t> digest;
        boost::algorithm::unhex(r1_hex, std::back_inserter(r1));
        boost::algorithm::unhex(digest_hex, std::back_inserter(digest));
        callResult->setExecResult(abi.abiIn("", verify((void *)r1.data(), (void *)digest.data(), (void *)proof.data())));
    }
    else if (func == name2Selector[RESCUE_HASH])
    {
        std::string prev_digest_hex, r_hex;
        abi.abiOut(data, prev_digest_hex, r_hex);
        std::vector<uint8_t> prev_digest;
        std::vector<uint8_t> r;
        boost::algorithm::unhex(prev_digest_hex, std::back_inserter(prev_digest));
        boost::algorithm::unhex(r_hex, std::back_inserter(r));
        std::string digest;
        auto result = hash((void *)prev_digest.data(), (void *)r.data());
        boost::algorithm::hex(result, result + 32, std::back_inserter(digest));
        callResult->setExecResult(abi.abiIn("", digest));
    }
    else
    {
        PRECOMPILED_LOG(ERROR) << LOG_BADGE("RescuePrecompiled")
                               << LOG_DESC("call undefined function") << LOG_KV("func", func);
        getErrorCodeOut(callResult->mutableExecResult(), CODE_UNKNOW_FUNCTION_CALL);
    }
    return callResult;
}
