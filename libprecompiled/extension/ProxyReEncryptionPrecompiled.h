#pragma once
#include <libblockverifier/ExecutiveContext.h>
#include <libprecompiled/Common.h>

namespace dev
{
namespace precompiled
{
class ProxyReEncryptionPrecompiled : public dev::precompiled::Precompiled
{
public:
    typedef std::shared_ptr<ProxyReEncryptionPrecompiled> Ptr;
    ProxyReEncryptionPrecompiled();
    virtual ~ProxyReEncryptionPrecompiled(){};

    PrecompiledExecResult::Ptr call(std::shared_ptr<dev::blockverifier::ExecutiveContext> context,
        bytesConstRef param, Address const& origin = Address(),
        Address const& sender = Address()) override;
};

}  // namespace precompiled

}  // namespace dev
