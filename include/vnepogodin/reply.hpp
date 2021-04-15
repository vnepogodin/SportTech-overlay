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

#ifndef REPLY_HPP_
#define REPLY_HPP_

#include <vnepogodin/header.hpp>

#include <boost/asio.hpp>
#include <boost/interprocess/file_mapping.hpp>
#include <string>
#include <vector>

namespace vnepogodin {
namespace http {

// A reply to be sent to a client.
struct reply {
    enum status_type {
        ok = 200,
        created = 201,
        accepted = 202,
        no_content = 204,
        multiple_choices = 300,
        moved_permanently = 301,
        moved_temporarily = 302,
        not_modified = 304,
        bad_request = 400,
        unauthorized = 401,
        forbidden = 403,
        not_found = 404,
        internal_server_error = 500,
        not_implemented = 501,
        bad_gateway = 502,
        service_unavailable = 503
    } status;

    // The headers to be included in the reply.
    std::array<header, 2> headers;

    // The content to be sent in the reply.
    std::string content;

    // Convert the reply into a vector of buffers. The buffers do not own the
    // underlying memory blocks, therefore the reply object must remain valid and
    // not be changed until the write operation has completed.
    std::vector<boost::asio::const_buffer> to_buffers();

    // Get a stock reply.
    static reply stock_reply(status_type status);

    struct {
        std::shared_ptr<boost::interprocess::file_mapping> file_mapping;
        std::uint64_t file_size;
        std::uint64_t processed;
    } file;
};

}  // namespace http
}  // namespace vnepogodin

#endif  // REPLY_HPP_
