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
 * (c) 2016-2018 fisco-dev contributors.
 */
/**
 * @brief : Downloading block queue
 * @author: jimmyshi
 * @date: 2018-10-20
 */

#pragma once
#include <libdevcore/Guards.h>
#include <libethcore/Block.h>
#include <queue>
#include <set>
#include <vector>


namespace dev
{
namespace sync
{
class DownloadingBlockQueue
{
    using BlockPtr = std::shared_ptr<dev::eth::Block>;
    using BlockPtrVec = std::vector<BlockPtr>;

public:
    DownloadingBlockQueue() : m_blocks(), m_buffer() {}
    /// Push a block
    void push(BlockPtr _block);

    /// Is the queue empty?
    bool empty();

    size_t size();

    /// Pop the block sequent
    BlockPtrVec popSequent(int64_t _startNumber, int64_t _limit);

    /// Erase unused blocks
    // void clearUnused(int64_t _currentNumber);

private:
    std::map<int64_t, BlockPtr> m_blocks;   //
    std::shared_ptr<BlockPtrVec> m_buffer;  // use buffer for faster push return

    mutable RecursiveMutex x_blocks;
    mutable RecursiveMutex x_buffer;
};
}  // namespace sync
}  // namespace dev