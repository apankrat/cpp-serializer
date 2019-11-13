/*
 *	CPP Seializer - Fast and simple data serialization for C++.
 *
 *	           https://swapped.ch/cpp-serializer
 *
 *	The code is distributed under terms of the BSD license.
 *	Copyright (c) 2019 Alex Pankratov. All rights reserved.
 */

#ifndef _LIBP_BUFFER_H_
#define _LIBP_BUFFER_H_

/*
 *	'buffer' is a binary blob of data.
 *	std::basic_string<uint8_t> basically.
 */
#include <string>
#include <vector>
#include <stdint.h>

/*
 *	speed things up
 */
struct uint8_t_traits : std::char_traits<uint8_t>
{
	typedef char_type E;

	static int compare(const E * x, const E * y, size_t n)
	{ return memcmp(x, y, n); }

	static size_t length(const E * x)
	{ return strlen(reinterpret_cast<const char*>(x)); }

	static E * copy(E * x, const E * y, size_t n)
	{ return (E *)memcpy(x, y, n); }

	static const E * find(const E * x, size_t n, const E & y)
	{ return (E *)memchr(x, y, n); }

	static E * move(E * x, const E * y, size_t n)
	{ return (E *)memmove(x, y, n); }

	static E * assign(E * x, size_t n, const E & y)
	{ return (E *)memset(x, y, n); }

	static void assign(E & x, const E & y)
	{ x = y; }
};

/*
 *	byte buffer
 */
typedef std::basic_string<uint8_t, uint8_t_traits> buffer;

#endif
