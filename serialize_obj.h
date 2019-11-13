/*
 *	CPP Seializer - Fast and simple data serialization for C++.
 *
 *	           https://swapped.ch/cpp-serializer
 *
 *	The code is distributed under terms of the BSD license.
 *	Copyright (c) 2019 Alex Pankratov. All rights reserved.
 */

#ifndef _LIBP_SERIALIZE_OBJ_H_
#define _LIBP_SERIALIZE_OBJ_H_

#include "serialize.h"

/*
 *	member_ptr
 *	make_member_ptr()
 */
template <typename C, typename M>
struct member_ptr
{
	member_ptr(M C::* _raw) { raw = _raw; }

	M C::* raw;
};

//
template <typename C, typename T>
//inline
member_ptr<C, T> make_member_ptr(T C::* ptr)
{
	return member_ptr<C,T>(ptr);
}

/*
 *	object_spec         ~ tuple<member_ptr, member_ptr, ...>
 *	make_object_spec()
 */
template <typename... Ts> 
struct object_spec 
{
};

template <typename T, typename... Ts>
struct object_spec<T, Ts...> : object_spec<Ts...>
{
	object_spec() : object_spec( T() ) { }
	object_spec(T t, Ts... ts) : object_spec<Ts...>(ts...), m(t) {}

	T m;
};

//
template <typename... CMs>
//inline
auto make_object_spec(CMs... args)
{
	return object_spec<CMs...>(args...);
}

/*
 *	get_object_spec()
 *	has_object_spec<>
 */
template <typename T>
auto get_object_spec()
{
	static_assert( false, "This type has no serialization spec" );
	return 0;
}

//
template <typename... Ts>
using __exists = void;

template <typename T, typename = void>
struct has_object_spec { static constexpr bool value = false; };

template <typename T>
struct has_object_spec<T, __exists<decltype( get_object_spec<T> )> > { static constexpr bool value = true; };

/*
 *	store_member()
 *	store_object()
 *	store()        - generic for types that have get_object_spec<T>()
 */
template <typename C, typename... CMs>
//inline
void store_member(buffer & buf, const C & obj, object_spec<CMs...> & spec)
{}

template <typename C, typename CM, typename... CMs>
//inline
void store_member(buffer & buf, const C & obj, object_spec<CM, CMs...> & spec)
{
	store(buf, obj.*spec.m.raw);
	store_member(buf, obj, (object_spec<CMs...> &)spec);
}

template <typename C>
//inline
void store_object(buffer & buf, const C & obj)
{
	auto spec = get_object_spec<C>();
	store_member(buf, obj, spec);
}

//
template <typename C>
//inline
void store(buffer & b, const C & obj)
{
	static_assert( has_object_spec<C>::value, "This type has no serialization spec and no specialized store()" );
	store_object(b, obj);
}

/*
 *	parse_member()
 *	parse_object()
 *	parse()        - generic for types that have get_object_spec<T>()
 */
template <typename C, typename... CMs>
//inline
bool parse_member(parser & par, C & obj, object_spec<CMs...> & spec)
{
	return true;
}

template <typename C, typename CM, typename... CMs>
//inline
bool parse_member(parser & par, C & obj, object_spec<CM, CMs...> & spec)
{
	return par.ok &&
	       parse(par, obj.*spec.m.raw) && 
	       parse_member(par, obj, (object_spec<CMs...> &)spec);
}

template <typename C>
//inline
bool parse_object(parser & par, C & obj)
{
	auto spec = get_object_spec<C>();
	return parse_member(par, obj, spec);
}

//
template <typename C>
//inline
bool parse(parser & par, C & obj)
{
	static_assert( has_object_spec<C>::value, "This type has no serialization spec and no specialized store()" );
	return parse_object(par, obj);
}

/*
 *	macros
 */
#define OBJECT_SPEC(T)                                \
                                                      \
	template <>                                   \
	auto ::get_object_spec<T>()                   \
	{                                             \
		typedef T obj_type;                   \
		return make_object_spec               \
		(

#define __f(f)                                        \
			make_member_ptr(&obj_type::f)

#define __e(f)                                        \
			make_member_ptr( (uint32_t obj_type::*) &obj_type::f)

#define END_OF_SPEC                                   \
		);                                    \
	}

#endif
