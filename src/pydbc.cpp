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

#include "pydbc.hpp"
#include "BinaryStream.hpp"

#include <boost/python.hpp>

#include <string>
#include <vector>
#include <cstdint>
#include <cassert>
#include <experimental/filesystem>

namespace fs = std::experimental::filesystem;

#pragma pack(push, 1)
// taken from https://wowdev.wiki/DBC
struct dbc_header
{
    std::uint32_t magic;                // always 'WDBC'
    std::uint32_t record_count;         // records per file
    std::uint32_t field_count;          // fields per record
    std::uint32_t record_size;          // sum (sizeof (field_type_i)) | 0 <= i < field_count. field_type_i is NOT defined in the files.
    std::uint32_t string_block_size;
};
#pragma pack(pop)

DBC::DBC(const std::string &filename)
{
    utility::BinaryStream in(filename);

    dbc_header header;
    in >> header;

    assert(header.field_count * sizeof(std::uint32_t) == header.record_size);

    _recordCount = header.record_count;
    _fieldCount = header.field_count;

    _data.resize(header.record_count*header.field_count);
    in.ReadBytes(&_data[0], _data.size() * sizeof(std::uint32_t));

    // ensure that we have precisely enough space left for the string block
    assert(header.string_block_size + in.rpos() == in.wpos());
    _string.resize(header.string_block_size);
    in.ReadBytes(&_string[0], _string.size());
}

template<>
std::string DBC::GetField<std::string>(int row, int column) const
{
    auto const pos = GetField<std::uint32_t>(row, column);
    return std::string(reinterpret_cast<const char *>(&_string[pos]));
}

using namespace boost::python;

#ifdef PYTHON
BOOST_PYTHON_MODULE(pydbc)
{
    class_<DBC>("DBC", init<std::string>())
        .add_property("recordCount", &DBC::RecordCount)
        .add_property("fieldCount", &DBC::FieldCount)
        .def("GetUInt32", &DBC::GetField<std::uint32_t>);
}
#endif