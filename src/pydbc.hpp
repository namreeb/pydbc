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

#include <string>
#include <cstdint>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

class DBC
{
    private:
        static constexpr std::uint32_t Magic = 'CBDW';

        std::vector<std::uint32_t> _data;
        std::vector<std::uint8_t> _string;

        size_t _recordCount;
        size_t _fieldCount;

    public:
        DBC(const std::string &filename);

        template <typename T>
        T GetField(int row, int column) const;

        size_t RecordCount() const { return _recordCount; }
        size_t FieldCount() const { return _fieldCount; }
};

template<>
std::string DBC::GetField<std::string>(int row, int column) const;

template <typename T>
T DBC::GetField(int row, int column) const
{
    auto const offset = row * _fieldCount + column;

    if (offset > _data.size())
        throw std::runtime_error("Invalid row, column requested from DBC");

    return *reinterpret_cast<const T *>(&_data[offset]);
}