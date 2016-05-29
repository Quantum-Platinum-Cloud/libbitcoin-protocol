/*
 * Copyright (c) 2011-2016 libbitcoin developers (see AUTHORS)
 *
 * This file is part of libbitcoin-protocol.
 *
 * libbitcoin-protocol is free software: you can redistribute it and/or
 * modify it under the terms of the GNU Affero General Public License with
 * additional permissions to the one published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) 
 * any later version. For more information see LICENSE.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef LIBBITCOIN_PROTOCOL_ZMQ_AUTHENTICATOR_HPP
#define LIBBITCOIN_PROTOCOL_ZMQ_AUTHENTICATOR_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <bitcoin/bitcoin.hpp>
#include <bitcoin/protocol/define.hpp>
#include <bitcoin/protocol/zmq/context.hpp>
#include <bitcoin/protocol/zmq/poller.hpp>
#include <bitcoin/protocol/zmq/socket.hpp>

namespace libbitcoin {
namespace protocol {
namespace zmq {

/// This class is thread safe (including dynamic configuration).
class BCP_API authenticator
  : public context
{
public:
    /// A shared authenticator pointer.
    typedef std::shared_ptr<authenticator> ptr;

    /// Start the ZAP monitor for the context.
    /// The threadpool must be joined prior to destruct.
    /// There may be only one authenticator per process (otherwise bridge).
    authenticator(threadpool& threadpool);

    /// Cause all sockets of this authenticated context to close.
    virtual ~authenticator();

    /// Start the ZAP monitor.
    virtual bool start() override;

    /// Stop the ZAP monitor and close the context.
    virtual bool stop() override;

    /// Allow clients with the following public keys (whitelist).
    virtual void allow(const hash_digest& public_key);

    /// Allow clients with the following ip addresses (whitelist).
    virtual void allow(const config::authority& address);

    /// Allow clients with the following ip addresses (blacklist).
    virtual void deny(const config::authority& address);

    /// Set the server private key (required for curve security).
    virtual void set_private_key(const config::sodium& private_key);

    /// Apply authentication to the socket for the given arbitrary domain.
    /// Set secure false to enable null security, otherwise curve is required.
    virtual bool apply(socket& socket, const std::string& domain, bool secure);

private:
    void monitor();
    bool allowed_address(const std::string& address) const;
    bool allowed_key(const hash_digest& public_key) const;
    bool allowed_weak(const std::string& domain) const;

    // These are protected by socket mutex.
    socket socket_;
    dispatcher dispatch_;
    mutable shared_mutex socket_mutex_;

    // These are protected by config mutex.
    bool require_address_;
    config::sodium private_key_;
    std::unordered_set<hash_digest> keys_;
    std::unordered_set<std::string> weak_domains_;
    std::unordered_map<std::string, bool> adresses_;
    mutable shared_mutex config_mutex_;
};

} // namespace zmq
} // namespace protocol
} // namespace libbitcoin

#endif

