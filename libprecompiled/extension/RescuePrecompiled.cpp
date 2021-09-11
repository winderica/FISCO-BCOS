#include "RescuePrecompiled.h"
#include <libdevcore/Common.h>
#include <libethcore/ABI.h>
#include <string>
#include <boost/algorithm/hex.hpp>

using namespace dev;
using namespace dev::blockverifier;
using namespace dev::precompiled;

extern "C" uint8_t *hash(void *value, void* seed);
extern "C" char *prove(void *value, void* seed, void *result);
extern "C" bool verify(void* seed, void *result, void *proof);

/*
contract Rescue {
    function verify(string seed, string result, string proof) public constant returns(bool);
}
*/

const char* const RESCUE_VERIFY = "verify(string,string,string)";

RescuePrecompiled::RescuePrecompiled()
{
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
        std::string seed_hex, result_hex, proof;
        abi.abiOut(data, seed_hex, result_hex, proof);
	std::vector<uint8_t> seed;
	std::vector<uint8_t> result;
	boost::algorithm::unhex(seed_hex, std::back_inserter(seed));
	boost::algorithm::unhex(result_hex, std::back_inserter(result));
        callResult->setExecResult(abi.abiIn("", verify((void *)seed.data(), (void *)result.data(), (void *)proof.data())));
    }
    else
    {
        PRECOMPILED_LOG(ERROR) << LOG_BADGE("RescuePrecompiled")
                               << LOG_DESC("call undefined function") << LOG_KV("func", func);
        getErrorCodeOut(callResult->mutableExecResult(), CODE_UNKNOW_FUNCTION_CALL);
    }
    return callResult;
}
