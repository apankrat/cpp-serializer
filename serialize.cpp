/*
 *	CPP Seializer - Fast and simple data serialization for C++.
 *
 *	           https://swapped.ch/cpp-serializer
 *
 *	The code is distributed under terms of the BSD license.
 *	Copyright (c) 2019 Alex Pankratov. All rights reserved.
 */

#include "serialize.h"

void store_size(buffer & buf, size_t n)
{
	/* 
	 *	variable-size lsb in 7bit chunks
	 *
	 *	000zzzzzzzyyyyyyyxxxxxxx -> 1xxxxxxx 1yyyyyyy 0zzzzzzz
	 */
	do 
		buf.push_back( (uint8_t)(((n > 0x7f) ? 0x80 : 0x00) | (n & 0x7f)) );
	while (n >>= 7);
}

bool parse_size(parser & par, size_t & val)
{
	size_t   off;
	uint8_t  byte;

	for (val=0, off=0; ; off += 7)
	{
		if (! par.has(1))
			return false;

		byte = *par.ptr++;
		val |= (byte & 0x7f) << off;

		if (! (byte & 0x80))
			break;
	}

	/* catch 32bit overflows */
	if ( (off > 28) || (off == 28) && (byte > 0x0F) )
		return false;

	return true;
}
