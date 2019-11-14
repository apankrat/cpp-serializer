/*
 *	CPP Seializer - Fast and simple data serialization for C++.
 *
 *	           https://swapped.ch/cpp-serializer
 *
 *	The code is distributed under terms of the BSD license.
 *	Copyright (c) 2019 Alex Pankratov. All rights reserved.
 */

#ifndef _LIBP_SERIALIZE_H_
#define _LIBP_SERIALIZE_H_

#include "buffer.h"
#include "parser.h"

#include <stdint.h>

#include <string>
#include <vector>
#include <set>
#include <map>

/*
 *	simple types
 */
template <class T>
void store_simple(buffer & buf, const T & v)
{
	size_t was = buf.size();
	buf.resize(was + sizeof(v));
	*(T*)&buf[was] = v;
}

template <class T>
bool parse_simple(parser & par, T & v)
{
	if (! par.has(sizeof v))
		return false;

	v = *(const T*)par.ptr;
	par.ptr += sizeof(v);
	return true;
}

inline void store(buffer & buf, const bool     & v) { store_simple(buf, v); }
inline void store(buffer & buf, const uint8_t  & v) { store_simple(buf, v); }
inline void store(buffer & buf, const uint16_t & v) { store_simple(buf, v); }
inline void store(buffer & buf, const uint32_t & v) { store_simple(buf, v); }
inline void store(buffer & buf, const uint64_t & v) { store_simple(buf, v); }
inline void store(buffer & buf, const float    & v) { store_simple(buf, v); }
inline void store(buffer & buf, const double   & v) { store_simple(buf, v); }

inline bool parse(parser & par, bool     & v) { return parse_simple(par, v); }
inline bool parse(parser & par, uint8_t  & v) { return parse_simple(par, v); }
inline bool parse(parser & par, uint16_t & v) { return parse_simple(par, v); }
inline bool parse(parser & par, uint32_t & v) { return parse_simple(par, v); }
inline bool parse(parser & par, uint64_t & v) { return parse_simple(par, v); }
inline bool parse(parser & par, float    & v) { return parse_simple(par, v); }
inline bool parse(parser & par, double   & v) { return parse_simple(par, v); }

/*
 *	arrays
 */
template <class T, size_t N>
//inline
void store(buffer & buf, const T (&v)[N])
{
	buf.append((uint8_t*)v, N * sizeof(T));
}

template <class T, size_t N>
//inline
bool parse(parser & par, T (&v)[N])
{
	size_t bytes = N * sizeof(T);
		
	if (! par.has(bytes))
		return false;

	memmove(v, par.ptr, bytes);
	par.ptr += bytes;

	return true;
}

/*
 *	var-sized int - size_t
 */
void store_size(buffer & buf, size_t n);
bool parse_size(parser & par, size_t & n);

/*
 *	std::basic_string - buffer, string, wstring
 */
template <typename E, typename T, typename A>
//inline
void store(buffer & buf, const std::basic_string<E,T,A> & v)
{
	size_t bytes = v.size() * sizeof(E);
	store_size(buf, bytes);
	if (bytes)
		buf.append( (uint8_t*)v.data(), bytes );
}

template <typename E, typename T, typename A>
//inline
bool parse(parser & par, std::basic_string<E,T,A> & v)
{
	size_t bytes;

	if (! parse_size(par, bytes))
		return false;

	if (bytes % sizeof(E))
		return par.stop();

	if (! par.has(bytes))
		return false;

	v.assign( (E*)par.ptr, bytes/sizeof(E) );
	par.ptr += bytes;
	
	return true;
}

/*
 *	std::pair
 */
template <typename T1, typename T2>
inline
void store(buffer & buf, const std::pair<T1,T2> & v)
{
	store(buf, v.first);
	store(buf, v.second);
}

template <typename T1, typename T2>
bool parse(parser & par, std::pair<T1,T2> & v)
{
	return par.ok && 
	       parse(par, v.first) &&
	       parse(par, v.second);
}

/*
 *	containers
 */
template <typename C>
void store_container(buffer & buf, const C & v)
{
	store_size(buf, v.size());
	for (auto & x : v) store(buf, x);
}

/*
 *	std::vector
 */
template <typename T, typename A>
//inline 
void store(buffer & buf, const std::vector<T,A> & v)
{
	return store_container(buf, v);
}

template <typename T, typename A>
//inline
bool parse(parser & par, std::vector<T,A> & v) 
{
	size_t  count;
	T       dummy;

	if (! parse_size(par, count))
		return false;

	v.clear();
	v.reserve(count);

	while (count--)
	{
		v.push_back(dummy);
		if (! parse(par, v.back()))
			return false;
	}

	return true;
}

/*
 *	std::set
 */
template <typename K, typename C, typename A>
//inline 
void store(buffer & buf, const std::set<K,C,A> & v)
{
	return store_container(buf, v);
}

template <typename K, typename C, typename A>
bool parse(parser & par, std::set<K,C,A> & v)
{
	size_t  count;
	K       tmp;

	if (! parse_size(par, count))
		return false;

	v.clear();
	
	while (count--)
	{
		if (! parse(par, tmp))
			return false;
		if (! v.insert(tmp).second)
			return par.stop();
	}
	
	return true;
}

/*
 *	std::map
 */
template <typename K, typename D, typename C, typename A>
//inline 
void store(buffer & buf, const std::map<K,D,C,A> & v)
{
	return store_container(buf, v);
}

template <typename K, typename D, typename C, typename A>
//inline
bool parse(parser & par, std::map<K,D,C,A> & v)
{
	size_t          count;
	std::pair<K,D>  tmp;

	if (! parse_size(par, count))
		return false;

	v.clear();

	while (count--)
	{
		if (! parse(par, tmp))
			return false;
		if (! v.insert(tmp).second)
			return par.stop();
	}
	
	return true;
}

#endif
