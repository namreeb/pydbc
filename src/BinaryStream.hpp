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

#pragma once

#include <vector>
#include <string>
#include <map>
#include <memory>
#include <type_traits>
#include <ostream>
#include <fstream>
#include <experimental/filesystem>

namespace utility
{
class BinaryStream
{
    private:
        static constexpr size_t DEFAULT_BUFFER_LENGTH = 4096;

        friend std::ostream & operator << (std::ostream &, const BinaryStream &);
        friend BinaryStream & operator << (BinaryStream &, const BinaryStream &);
        friend BinaryStream & operator << (BinaryStream &, const std::string &);

        std::vector<std::uint8_t> _buffer;
        size_t _rpos;

    public:
        BinaryStream(std::vector<std::uint8_t> &buffer);
        BinaryStream(const std::experimental::filesystem::path &path);
        BinaryStream(BinaryStream &&other) noexcept;

        BinaryStream& operator = (BinaryStream&& other) noexcept;

        template <typename T> T Read()
        {
            static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");
            static_assert(sizeof(T) <= 4, "Stack return read is meant only for small values");

            T ret;
            Read(ret);
            return ret;
        }

        template <typename T> void Read(T &out)
        {
            static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");

            ReadBytes(&out, sizeof(T));
        }

        void ReadBytes(void *dest, size_t length);

        std::string ReadString();
        std::string ReadString(size_t length);

        size_t rpos() const { return _rpos; }
        void rpos(size_t pos) { _rpos = pos; }

        size_t wpos() const { return _buffer.size(); }
};

template <typename T>
BinaryStream & operator >> (BinaryStream &stream,  T &data)
{
    static_assert(std::is_trivially_copyable<T>::value, "T must be trivially copyable");

    stream.Read(data);
    return stream;
}

std::ostream & operator << (std::ostream &, const BinaryStream &);
BinaryStream & operator << (BinaryStream &, const BinaryStream &);
BinaryStream & operator << (BinaryStream &, const std::string &);
std::ostream & operator << (std::ostream &stream, const BinaryStream &data);
}