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
/** @file VMFactory.cpp
 * @author wheatli
 * @date 2018.8.28
 * @record copy from aleth, this is a vm factory
 * @record delete LegacyVM, use interpreter as default vm
 */

#include "VMFactory.h"
#include "EVMInstance.h"

#include <libinterpreter/interpreter.h>

#include <evmc/loader.h>
#include <evmone/evmone.h>
#include <hera/hera.h>

namespace po = boost::program_options;

namespace dev
{
namespace eth
{
namespace
{
auto g_kind = VMKind::evmone;

/// The pointer to EVMInstance create function in DLL EVMInstance VM.
///
/// This variable is only written once when processing command line arguments,
/// so access is thread-safe.
evmc_create_fn g_evmcCreateFn;

/// A helper type to build the tabled of VM implementations.
///
/// More readable than std::tuple.
/// Fields are not initialized to allow usage of construction with initializer lists {}.
struct VMKindTableEntry
{
    VMKind kind;
    const char* name;
};

/// The table of available VM implementations.
///
/// We don't use a map to avoid complex dynamic initialization. This list will never be long,
/// so linear search only to parse command line arguments is not a problem.
VMKindTableEntry vmKindsTable[] = {
    {VMKind::Hera, "hera"},
    {VMKind::Interpreter, "interpreter"}, {VMKind::evmone, "evmone"}};

void setVMKind(const std::string& _name)
{
    for (auto& entry : vmKindsTable)
    {
        // Try to find a match in the table of VMs.
        if (_name == entry.name)
        {
            g_kind = entry.kind;
            return;
        }
    }
    // If not match for predefined VM names, try loading it as an EVMInstance DLL.
    evmc_loader_error_code ec;
    g_evmcCreateFn = evmc_load(_name.c_str(), &ec);
    switch (ec)
    {
    case EVMC_LOADER_SUCCESS:
        break;
    case EVMC_LOADER_CANNOT_OPEN:
        BOOST_THROW_EXCEPTION(
            po::validation_error(po::validation_error::invalid_option_value, "vm", _name, 1));
    case EVMC_LOADER_SYMBOL_NOT_FOUND:
        BOOST_THROW_EXCEPTION(std::system_error(std::make_error_code(std::errc::invalid_seek),
            "loading " + _name + " failed: EVMInstance create function not found"));
    default:
        BOOST_THROW_EXCEPTION(
            std::system_error(std::error_code(static_cast<int>(ec), std::generic_category()),
                "loading " + _name + " failed"));
    }
    g_kind = VMKind::DLL;
}
}  // namespace

namespace
{
/// The name of the program option --evmc. The boost will trim the tailing
/// space and we can reuse this variable in exception message.
const char c_evmcPrefix[] = "evmc ";

/// The list of EVM-C options stored as pairs of (name, value).
std::vector<std::pair<std::string, std::string>> s_evmcOptions;

/// The additional parser for EVM-C options. The options should look like
/// `--evmc name=value` or `--evmc=name=value`. The boost pass the strings
/// of `name=value` here. This function splits the name and value or reports
/// the syntax error if the `=` character is missing.
void parseEvmcOptions(const std::vector<std::string>& _opts)
{
    for (auto& s : _opts)
    {
        auto separatorPos = s.find('=');
        if (separatorPos == s.npos)
            throw po::invalid_syntax{po::invalid_syntax::missing_parameter, c_evmcPrefix + s};
        auto name = s.substr(0, separatorPos);
        auto value = s.substr(separatorPos + 1);
        s_evmcOptions.emplace_back(std::move(name), std::move(value));
    }
}
}  // namespace

std::vector<std::pair<std::string, std::string>>& evmcOptions() noexcept
{
    return s_evmcOptions;
}

po::options_description vmProgramOptions(unsigned _lineLength)
{
    // It must be a static object because boost expects const char*.
    static const std::string description = [] {
        std::string names;
        for (auto& entry : vmKindsTable)
        {
            if (!names.empty())
                names += ", ";
            names += entry.name;
        }

        return "Select VM implementation. Available options are: " + names + ".";
    }();

    po::options_description opts("VM OPTIONS", _lineLength);
    auto add = opts.add_options();

    add("vm",
        po::value<std::string>()
            ->value_name("<name>|<path>")
            ->default_value("interpreter")  // we first user interpreter evm
            ->notifier(setVMKind),
        description.data());

    add(c_evmcPrefix,
        po::value<std::vector<std::string>>()
            ->value_name("<option>=<value>")
            ->notifier(parseEvmcOptions),
        "EVM-C option\n");

    return opts;
}


std::unique_ptr<EVMInterface> VMFactory::create()
{
    return create(g_kind);
}

std::unique_ptr<EVMInterface> VMFactory::create(VMKind _kind)
{
    switch (_kind)
    {
    case VMKind::Hera:
        return std::unique_ptr<EVMInterface>(new EVMInstance{evmc_create_hera()});
    case VMKind::evmone:
        return std::unique_ptr<EVMInterface>(new EVMInstance{evmc_create_evmone()});
    case VMKind::DLL:
        return std::unique_ptr<EVMInterface>(new EVMInstance{g_evmcCreateFn()});
#if 0
    case VMKind::Interpreter:
        return std::unique_ptr<EVMInterface>(new EVMInstance{evmc_create_interpreter()});
#endif
    default:
        return std::unique_ptr<EVMInterface>(new EVMInstance{evmc_create_evmone()});
    }
}
}  // namespace eth
}  // namespace dev
