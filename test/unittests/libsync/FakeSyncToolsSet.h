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
 * @brief : fake sync tools set
 * @author: catli
 * @date: 2018-10-25
 */

#pragma once
#include <libconfig/GlobalConfigure.h>
#include <libp2p/Common.h>
#include <libp2p/Service.h>
#include <test/unittests/libethcore/FakeBlock.h>
#include <test/unittests/libtxpool/FakeBlockChain.h>
#include <memory>
namespace dev
{
namespace test
{
class FakeBaseSession : public dev::network::SessionFace
{
public:
    FakeBaseSession()
    {
        m_endpoint = NodeIPEndpoint(boost::asio::ip::make_address("127.0.0.1"), 30303);
    }
    virtual NodeIPEndpoint nodeIPEndpoint() const override { return m_endpoint; }
    virtual void start() override {}
    virtual void disconnect(dev::network::DisconnectReason) override {}

    virtual void asyncSendMessage(dev::network::Message::Ptr, dev::network::Options = Options(),
        CallbackFunc = CallbackFunc()) override
    {}
    void setMessageHandler(std::function<void(NetworkException,
            std::shared_ptr<dev::network::SessionFace>, dev::network::Message::Ptr)>) override
    {}
    bool actived() const override { return true; }

    virtual std::shared_ptr<dev::network::SocketFace> socket() override
    {
        return std::shared_ptr<dev::network::SocketFace>();
    }

private:
    NodeIPEndpoint m_endpoint;
};
class FakeSession : public P2PSession
{
public:
    FakeSession(NodeID _id = NodeID()) : P2PSession(), m_id(_id)
    {
        m_session = std::make_shared<FakeBaseSession>();
    };
    virtual ~FakeSession(){};

    virtual bool actived() override { return m_run; }

    virtual void start() override { m_run = true; }

    virtual void stop(dev::network::DisconnectReason) override { m_run = false; }

    virtual NodeID nodeID() override { return m_id; }

    bool m_run = false;

    dev::network::SessionFace::Ptr session() override { return m_session; }

    NodeID m_id;
    dev::network::SessionFace::Ptr m_session;
};

class FakeSyncToolsSet
{
    using TxPoolPtr = std::shared_ptr<dev::txpool::TxPoolInterface>;
    using ServicePtr = std::shared_ptr<dev::p2p::Service>;
    using BlockChainPtr = std::shared_ptr<dev::blockchain::BlockChainInterface>;
    using SessionPtr = std::shared_ptr<dev::network::SessionFace>;
    using TransactionPtr = std::shared_ptr<dev::eth::Transaction>;

public:
    FakeSyncToolsSet(uint64_t _blockNum = 5, size_t const& _transSize = 5)
      : m_txPoolFixture(_blockNum, _transSize), m_fakeBlock()
    {
        // m_host = createFakeHost(m_clientVersion, m_listenIp, m_listenPort);
    }

    TxPoolPtr getTxPoolPtr() const { return m_txPoolFixture.m_txPool; }

    ServicePtr getServicePtr() const { return m_txPoolFixture.m_topicService; }

    BlockChainPtr getBlockChainPtr() const { return m_txPoolFixture.m_blockChain; }

    Block& getBlock() { return *m_fakeBlock.getBlock(); }

    P2PSession::Ptr createSession(std::string _ip = "127.0.0.1")
    {
        NodeIPEndpoint peer_endpoint(boost::asio::ip::make_address(_ip), m_listenPort);
        KeyPair key_pair = KeyPair::create();
#if 0
        std::shared_ptr<Peer> peer = std::make_shared<Peer>(key_pair.pub(), peer_endpoint);
        PeerSessionInfo peer_info({key_pair.pub(), peer_endpoint.address.to_string(),
            chrono::steady_clock::duration(), 0});

        std::shared_ptr<SessionFace> session =
            std::make_shared<FakeSessionForHost>(m_host, peer, peer_info);
#endif
        std::shared_ptr<P2PSession> session = std::make_shared<FakeSession>(NodeID());
        session->start();
        return session;
    }

    P2PSession::Ptr createSessionWithID(NodeID _peerID, std::string _ip = "127.0.0.1")
    {
        NodeIPEndpoint peer_endpoint(boost::asio::ip::make_address(_ip), m_listenPort);
#if 0
        dev::p2p::P2PSessionInfo peer_info(
            {_peerID, peer_endpoint.address.to_string(), std::chrono::steady_clock::duration(), 0});
#endif

#if 0
        std::shared_ptr<SessionFace> session =
            std::make_shared<FakeSessionForHost>(m_host, peer, peer_info);
#endif
        std::shared_ptr<P2PSession> session = std::make_shared<FakeSession>(_peerID);
        session->start();
        return session;
    }

    TransactionPtr createTransaction(int64_t _currentBlockNumber)
    {
        bytes c_txBytes;
        {
            c_txBytes = fromHex(
                "f8ef9f65f0d06e39dc3c08e32ac10a5070858962bc"
                "6c0f5760baca823f2d5582d03f85174876e7ff"
                "8609184e729fff82020394d6f1a71052366dbae2f7"
                "ab2d5d5845e77965cf0d80b86448f85bce000000"
                "000000000000000000000000000000000000000000"
                "000000000000001bf5bd8a9e7ba8b936ea704292"
                "ff4aaa5797bf671fdc8526dcd159f23c1f5a05f44e"
                "9fa862834dc7cb4541558f2b4961dc39eaaf0af7"
                "f7395028658d0e01b86a371ca00b2b3fabd8598fef"
                "dda4efdb54f626367fc68e1735a8047f0f1c4f84"
                "0255ca1ea0512500bc29f4cfe18ee1c88683006d73"
                "e56c934100b8abf4d2334560e1d2f75e");
            if (g_BCOSConfig.version() >= RC2_VERSION)
            {
                c_txBytes = fromHex(
                    "f8d3a003922ee720bb7445e3a914d8ab8f507d1a647296d563100e49548d83fd98865c8411e1a3"
                    "0084"
                    "11e1a3008201f894d6c8a04b8826b0a37c6d4aa0eaa8644d8e35b79f80a466c991390000000000"
                    "0000"
                    "000000000000000000000000000000000000000000000000040101a466c9913900000000000000"
                    "0000"
                    "00000000000000000000000000000000000000000000041ba08e0d3fae10412c584c977721aeda"
                    "88df"
                    "932b2a019f084feda1e0a42d199ea979a016c387f79eb85078be5db40abe1670b8b480a12c7eab"
                    "719b"
                    "edee212b7972f775");
            }
        }
        const u256 c_maxBlockLimit = u256(1000);
        auto keyPair = KeyPair::create();
        TransactionPtr txPtr =
            std::make_shared<Transaction>(ref(c_txBytes), CheckTransaction::Everything);
        txPtr->setNonce(txPtr->nonce() + utcTime() + u256(1));
        txPtr->setBlockLimit(u256(_currentBlockNumber) + c_maxBlockLimit);
        auto sig = crypto::Sign(keyPair, txPtr->hash(WithoutSignature));
        txPtr->updateSignature(sig);
        return txPtr;
    }

private:
    std::string m_clientVersion = "2.0";
    std::string m_listenIp = "127.0.0.1";
    uint16_t m_listenPort = 30304;

    TxPoolFixture m_txPoolFixture;
    FakeBlock m_fakeBlock;
};
}  // namespace test
}  // namespace dev
