/*
 * @CopyRight:
 * FISCO-BCOS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * FISCO-BCOS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with FISCO-BCOS.  If not, see <http://www.gnu.org/licenses/>
 * (c) 2016-2019 fisco-dev contributors.
 *
 *  @file CryptoPrecompiled.cpp
 *  @author yujiechen
 *  @date 20201202
 */
#include "CryptoPrecompiled.h"
#include "libdevcrypto/Hash.h"
#include "libethcore/ABI.h"

using namespace dev;
using namespace dev::eth;
using namespace dev::crypto;
using namespace dev::precompiled;
using namespace dev::blockverifier;

// Note: the interface here can't be keccak256k1 for naming conflict
const char* const CRYPTO_METHOD_KECCAK256_STR = "keccak256Hash(bytes)";

CryptoPrecompiled::CryptoPrecompiled()
{
    name2Selector[CRYPTO_METHOD_KECCAK256_STR] = getFuncSelector(CRYPTO_METHOD_KECCAK256_STR);
}

PrecompiledExecResult::Ptr CryptoPrecompiled::call(
    std::shared_ptr<ExecutiveContext>, bytesConstRef _param, Address const&, Address const&)
{
    auto funcSelector = getParamFunc(_param);
    auto paramData = getParamData(_param);
    ContractABI abi;
    auto callResult = m_precompiledExecResultFactory->createPrecompiledResult();
    callResult->gasPricer()->setMemUsed(_param.size());
    do
    {
        if (funcSelector == name2Selector[CRYPTO_METHOD_KECCAK256_STR])
        {
            bytes inputData;
            abi.abiOut(paramData, inputData);
            auto keccak256Hash = keccak256(inputData);
            PRECOMPILED_LOG(TRACE)
                << LOG_DESC("CryptoPrecompiled: keccak256") << LOG_KV("input", toHex(inputData))
                << LOG_KV("result", toHex(keccak256Hash));
            callResult->setExecResult(abi.abiIn("", dev::eth::toString32(keccak256Hash)));
            break;
        }
        // no defined function
        callResult->setExecResult(abi.abiIn("", u256((uint32_t)CODE_UNKNOW_FUNCTION_CALL)));
    } while (0);
    return callResult;
}
