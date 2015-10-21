// ----------------------------------------------------------------------------
// -                       Open3DV: www.open3dv.org                           -
// ----------------------------------------------------------------------------
// The MIT License (MIT)
//
// Copyright (c) 2015 Qianyi Zhou <Qianyi.Zhou@gmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
// FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
// IN THE SOFTWARE.
// ----------------------------------------------------------------------------

#include "TriangleMeshIO.h"

#include <unordered_map>
#include "IOHelper.h"

namespace three{
	
namespace {
	
static const std::unordered_map<std::string,
		std::function<bool(const std::string &, TriangleMesh &)>>
		file_extension_to_trianglemesh_read_function
		{{"ply", ReadTriangleMeshFromPLY},
		};

static const std::unordered_map<std::string,
		std::function<bool(const std::string &, const TriangleMesh &,
		const bool, const bool)>>
		file_extension_to_trianglemesh_write_function
		{{"ply", WriteTriangleMeshToPLY},
		};

}	// unnamed namespace

bool ReadTriangleMesh(const std::string &filename, TriangleMesh &mesh)
{
	std::string filename_ext = IOHelper::GetFileExtensionInLowerCase(filename);
	if (filename_ext.empty()) {
		PrintWarning("Read TriangleMesh failed: unknown file extension.\n");
		return false;
	}
	auto map_itr = 
			file_extension_to_trianglemesh_read_function.find(filename_ext);
	if (map_itr == file_extension_to_trianglemesh_read_function.end()) {
		PrintWarning("Read TriangleMesh failed: unknown file extension.\n");
		return false;
	}
	return map_itr->second(filename, mesh);
}

bool WriteTriangleMesh(const std::string &filename, const TriangleMesh &mesh,
		const bool write_ascii/* = false*/, const bool compressed/* = false*/)
{
	std::string filename_ext = IOHelper::GetFileExtensionInLowerCase(filename);
	if (filename_ext.empty()) {
		PrintWarning("Write TriangleMesh failed: unknown file extension.\n");
		return false;
	}
	auto map_itr =
			file_extension_to_trianglemesh_write_function.find(filename_ext);
	if (map_itr == file_extension_to_trianglemesh_write_function.end()) {
		PrintWarning("Write TriangleMesh failed: unknown file extension.\n");
		return false;
	}
	return map_itr->second(filename, mesh, write_ascii, compressed);
}

}	// namespace three