/*
    MIT License

    Copyright (c) 2019 namreeb (legal@namreeb.org) http://github.com/namreeb/pydbc

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
 */

#include "BinaryStream.hpp"

#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstring>
#include <stdexcept>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

namespace utility
{
BinaryStream::BinaryStream(std::vector<std::uint8_t> &buffer) : _buffer(std::move(buffer)), _rpos(0) {}

BinaryStream::BinaryStream(const fs::path &path) : _rpos(0)
{
    std::ifstream stream(path, std::ifstream::binary);

    if (stream.fail())
        throw std::runtime_error("Failed to open file for BinaryStream");

    stream.seekg(0, std::ios::end);
    auto const wpos = static_cast<size_t>(stream.tellg());
    stream.seekg(0, std::ios::beg);

    _buffer.resize(static_cast<size_t>(wpos));
    stream.read(reinterpret_cast<char *>(&_buffer[0]), _buffer.size());
}

BinaryStream::BinaryStream(BinaryStream &&other) noexcept : _buffer(std::move(other._buffer)), _rpos(other._rpos)
{
    other._rpos = 0;
}

BinaryStream& BinaryStream::operator = (BinaryStream&& other) noexcept
{
    _buffer = std::move(other._buffer);
    _rpos = other._rpos;
    other._rpos = 0;
    return *this;
}

std::string BinaryStream::ReadString()
{
    std::string ret;

    for (auto c = Read<std::int8_t>(); !!c; c = Read<std::int8_t>())
        ret += c;

    return ret;
}

std::string BinaryStream::ReadString(size_t length)
{
    std::string ret;

    for (auto i = 0u; i < length; ++i)
        ret += Read<std::int8_t>();

    return ret;
}

void BinaryStream::ReadBytes(void *dest, size_t length)
{
    if (length == 0)
        return;

    if (_rpos + length > _buffer.size())
        throw std::domain_error("Read past end of buffer");

    memcpy(dest, &_buffer[_rpos], length);
    _rpos += length;
}
}