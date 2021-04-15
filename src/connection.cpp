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

#include <vnepogodin/connection.hpp>
#include <vnepogodin/request_handler.hpp>

#include <boost/interprocess/mapped_region.hpp>

namespace vnepogodin {
namespace http {

void connection::handle_read(const boost::system::error_code& e,
                             std::size_t bytes_transferred) {
    if (!e) {
        boost::tribool result;
        std::tie(result, std::ignore) = m_req_parser.parse(
            m_request,
            m_buffer.data(),
            m_buffer.data() + bytes_transferred);

        if (result) {
            m_req_handler.handle_request(m_reply);
            boost::asio::async_write(m_socket, m_reply.to_buffers(), std::bind(&connection::handle_write, shared_from_this(), std::placeholders::_1));
        } else if (!result) {
            m_reply = reply::stock_reply(reply::bad_request);
            boost::asio::async_write(m_socket, m_reply.to_buffers(), std::bind(&connection::handle_write, shared_from_this(), std::placeholders::_1));
        } else {
            m_socket.async_read_some(boost::asio::buffer(m_buffer),
                                     std::bind(&connection::handle_read, shared_from_this(), std::placeholders::_1, std::placeholders::_2));
        }
    }

    // If an error occurs then no new asynchronous operations are started. This
    // means that all shared_ptr references to the connection object will
    // disappear and the object will be destroyed automatically after this
    // handler returns. The connection class's destructor closes the socket.
}

void connection::handle_write(const boost::system::error_code& e) {
    if (!e) {
        if (m_reply.file.file_mapping && (m_reply.file.file_size > m_reply.file.processed)) {
            uint64_t mem_len = m_reply.file.file_size - m_reply.file.processed;

            constexpr auto MEM_CACHE_SIZE{1024 * 1024};
            if (mem_len > MEM_CACHE_SIZE) {
                mem_len = MEM_CACHE_SIZE;
            }
            boost::interprocess::mapped_region region(*m_reply.file.file_mapping, boost::interprocess::read_only, m_reply.file.processed, mem_len);
            uint64_t processed = region.get_size();
            m_reply.content.assign((const char*)region.get_address(), processed);
            m_reply.file.processed += processed;
            boost::asio::async_write(m_socket, boost::asio::buffer(m_reply.content), std::bind(&connection::handle_write, shared_from_this(), std::placeholders::_1));
        } else {
            // Initiate graceful connection closure.
            boost::system::error_code ignored_ec;
            m_socket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
        }
    }
}

}  // namespace http
}  // namespace vnepogodin
