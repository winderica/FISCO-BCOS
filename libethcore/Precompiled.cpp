/*
    This file is part of cpp-ethereum.

    cpp-ethereum is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    cpp-ethereum is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with cpp-ethereum.  If not, see <http://www.gnu.org/licenses/>.
*/
/** @file Precompiled.cpp
 * @author Gav Wood <i@gavwood.com>
 * @date 2014
 */

#include "Precompiled.h"
#include "libdevcrypto/Blake2.h"
#include "libdevcrypto/ECDSASignature.h"
#include <libconfig/GlobalConfigure.h>
#include <libdevcrypto/Common.h>
#include <libdevcrypto/Hash.h>
#include <libethcore/Common.h>

using namespace std;
using namespace dev;
using namespace dev::eth;
using namespace dev::crypto;

PrecompiledRegistrar* PrecompiledRegistrar::s_this = nullptr;

PrecompiledExecutor const& PrecompiledRegistrar::executor(std::string const& _name)
{
    if (!get()->m_execs.count(_name))
        BOOST_THROW_EXCEPTION(ExecutorNotFound());
    return get()->m_execs[_name];
}

PrecompiledPricer const& PrecompiledRegistrar::pricer(std::string const& _name)
{
    if (!get()->m_pricers.count(_name))
        BOOST_THROW_EXCEPTION(PricerNotFound());
    return get()->m_pricers[_name];
}

namespace
{
ETH_REGISTER_PRECOMPILED(ecrecover)(bytesConstRef _in)
{
    // When supported_version> = v2.4.0, ecRecover uniformly calls the ECDSA verification function
    if (g_BCOSConfig.version() >= V2_4_0)
    {
        return dev::ecRecover(_in);
    }

    return dev::ecRecover(_in);
}

ETH_REGISTER_PRECOMPILED(sha256)(bytesConstRef _in)
{
    // When supported_version> = v2.4.0, sha256 uniformly calls the secp sha256 function
    if (g_BCOSConfig.version() >= V2_4_0)
    {
        return {true, dev::sha256(_in).asBytes()};
    }

    return {true, dev::sha256(_in).asBytes()};
}

ETH_REGISTER_PRECOMPILED(ripemd160)(bytesConstRef _in)
{
    return {true, h256(dev::ripemd160(_in), h256::AlignRight).asBytes()};
}

ETH_REGISTER_PRECOMPILED(identity)(bytesConstRef _in)
{
    return {true, _in.toBytes()};
}


ETH_REGISTER_PRECOMPILED(blake2_compression)(bytesConstRef _in)
{
    static constexpr size_t roundsSize = 4;
    static constexpr size_t stateVectorSize = 8 * 8;
    static constexpr size_t messageBlockSize = 16 * 8;
    static constexpr size_t offsetCounterSize = 8;
    static constexpr size_t finalBlockIndicatorSize = 1;
    static constexpr size_t totalInputSize = roundsSize + stateVectorSize + messageBlockSize +
                                             2 * offsetCounterSize + finalBlockIndicatorSize;

    if (_in.size() != totalInputSize)
        return {false, {}};

    auto const rounds = fromBigEndian<uint32_t>(_in.cropped(0, roundsSize));
    auto const stateVector = _in.cropped(roundsSize, stateVectorSize);
    auto const messageBlockVector = _in.cropped(roundsSize + stateVectorSize, messageBlockSize);
    auto const offsetCounter0 =
        _in.cropped(roundsSize + stateVectorSize + messageBlockSize, offsetCounterSize);
    auto const offsetCounter1 = _in.cropped(
        roundsSize + stateVectorSize + messageBlockSize + offsetCounterSize, offsetCounterSize);
    uint8_t const finalBlockIndicator =
        _in[roundsSize + stateVectorSize + messageBlockSize + 2 * offsetCounterSize];

    if (finalBlockIndicator != 0 && finalBlockIndicator != 1)
        return {false, {}};

    return {true, dev::crypto::blake2FCompression(rounds, stateVector, offsetCounter0,
                      offsetCounter1, finalBlockIndicator, messageBlockVector)};
}

ETH_REGISTER_PRECOMPILED_PRICER(blake2_compression)
(bytesConstRef _in)
{
    auto const rounds = fromBigEndian<uint32_t>(_in.cropped(0, 4));
    return rounds;
}

}  // namespace
