/*
 *	CPP Seializer - Fast and simple data serialization for C++.
 *
 *	           https://swapped.ch/cpp-serializer
 *
 *	The code is distributed under terms of the BSD license.
 *	Copyright (c) 2019 Alex Pankratov. All rights reserved.
 */

#ifndef _LIBP_PARSER_H_
#define _LIBP_PARSER_H_

#include <stdint.h>
#include "buffer.h"

/*
 *	'parser' is a pair of pointers that frame yet unparsed data
 *	and a boolean flag that is cleared if parsing runs into trouble.
 */
struct parser
{
	parser()
	{
		ptr = end = NULL;
		ok = false;
	}

	void init(const buffer & buf)
	{
		init(buf.data(), buf.size());
	}

	void init(const uint8_t * buf, size_t len)
	{
		ptr = buf;
		end = buf + len;
		ok = true;
	}

	//
	bool stop()
	{
		ok = false;
		return false;
	}

	//
	bool has(size_t bytes)
	{
		if (ok && ptr + bytes > end)
			ok = false;
		return ok;
	}

	bool eof() const
	{
		return ok && (ptr == end);
	}

public:
	const uint8_t * ptr;
	const uint8_t * end;
	bool ok;
};

#endif
