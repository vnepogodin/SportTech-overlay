/*
 * Copyright (C) 2021 Vladislav Nepogodin
 *
 * This file is part of SportTech overlay project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef CONNECTION_HPP_
#define CONNECTION_HPP_

#include <vnepogodin/reply.hpp>
#include <vnepogodin/request.hpp>
#include <vnepogodin/request_parser.hpp>

#include <boost/bind/bind.hpp>

#include <array>

namespace vnepogodin {
namespace http {
class request_handler;

// Represents a single connection from a client.
//
class connection
  : public std::enable_shared_from_this<connection> {
 public:
    // Constructor
    //
    inline connection(boost::asio::io_service& io_service, request_handler& handler)
      : m_socket(io_service),
        m_req_handler(handler) {}

    // Get the socket associated with the connection.
    //
    inline auto socket() noexcept -> boost::asio::ip::tcp::socket& {
        return m_socket;
    }

    // The first asynchronous operation.
    //
    inline void start() noexcept {
        m_socket.async_read_some(boost::asio::buffer(m_buffer),
                                 boost::bind(&connection::handle_read, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }

    connection(const connection&) = delete;
    constexpr auto operator=(const connection&) -> connection& = delete;

 private:
    boost::asio::ip::tcp::socket m_socket;

    // The handler used to process the incoming request.
    //
    request_handler& m_req_handler;

    // Buffer of incoming data.
    //
    std::array<char, 8192> m_buffer;

    // The incoming request.
    //
    request m_request;

    // The parser for the incoming request.
    //
    request_parser m_req_parser;

    // The reply to be send back to the client.
    //
    reply m_reply;

    // Handle completion of a read operation.
    //
    void handle_read(const boost::system::error_code& e,
                     std::size_t bytes_transferred);

    // Handle completion of a write operation.
    //
    void handle_write(const boost::system::error_code& e);
};

using connection_ptr = std::shared_ptr<connection>;

}  // namespace http
}  // namespace vnepogodin

#endif  // CONNECTION_HPP_
