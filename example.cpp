/*
 *	CPP Seializer - Fast and simple data serialization for C++.
 *
 *	           https://swapped.ch/cpp-serializer
 *
 *	The code is distributed under terms of the BSD license.
 *	Copyright (c) 2019 Alex Pankratov. All rights reserved.
 */

#include "serialize.h"
#include "serialize_obj.h"

#include <assert.h>

/*
 *	1. Define your structs
 */

struct foo
{
	bool         val_b;
	std::string  val_s;
	uint16_t     val_u;
	double       val_d;
};

struct bar
{
	std::map<uint32_t, foo>   baz;
	std::vector<std::string>  qux;
};

/*
 *	2. Specify which fields to serialize
 */
OBJECT_SPEC( foo   )
	__f( val_b ),
	__f( val_s ),
	__f( val_u ),
	__f( val_d )
END_OF_SPEC

OBJECT_SPEC( bar )
	__f( baz ),
	__f( qux )
END_OF_SPEC

/*
 *	3. ... and done! Serialize at will.
 */

int main(int argc, char ** argv)
{
	/*
	 *	store
	 */
	buffer  buf;
	bar     in;

	store(buf, in);

	/*
	 *	parse
	 */
	parser  par;
	bar     out;

	par.init(buf);
	parse(par, out);

	assert( par.eof() );

//	"out" is now the same as "in"

	return 0;
}
