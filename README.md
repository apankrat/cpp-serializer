# Serializer++

Small, light and easy to follow data serialization library for C++.

This is an extract from the live source of [Bvckup 2](https://bvckup2.com). It's not meant to be an universal serialization library, but rather a real-life working example of one of many ways to do serialization. Doing it *simply* at that.

## Quick intro

Given two processes that use the same data model and exchange parts of it with each other, this library helps packing/unpacking live data structures to/from binary blobs with *as little fuss as possible*.

No separate schemas, no precompilation, no verbose description of protocols, etc.

Just define your data structures ...

```
struct foo
{
        bool                 aaa;
        map<string, string>  bbb;
};
```

... specify  which fields to serialize ...

```
OBJECT_SPEC( foo )
        __f( aaa ),
        __f( bbb )
END_OF_SPEC
```

... and you are done!

```
// store: data -> blob

buffer  buf;
foo     something;

store(buf, something);

// parse: blob -> data

parser  par;
foo     inbound;

par.init(buf);
parse(par, inbound);

assert( parse.eof() );
```

## How it works

The library revolves around two functions - `store()` and `parse()`.

`Store` appends binary representation of its argument to an existing data blob.

`Parse` extracts an instance of specified type from the blob.

The library knows how to serialize basic types like `bool`, `uint8_t`, etc. That is, it has overloaded versions of

```
void store(buffer & buf, const bool & v);
void store(buffer & buf, const uint8_t & v);
...

bool parse(parser & par, bool & v);
bool parse(parser & par, uint8_t & v);
...
```

It knows how to serialize `string`, `wstring` or any other specialization of `std::basic_string`.

It knows how to serialize `vector`, `set` and `map` and it can be easily extended to support other container types like `list` and `deque`.

However, in order to serialize a container, it obviously needs to know how to store/parse the elements contained within. If they are of known types, e.g. `bool`, then we are good. But if they are custom types, then we need to teach the library how to handle them.

## Serializing structs

If we have `struct foo`, then we can obviously implement `store(..., const foo &)` and `parse(..., foo &)` and that will do it. That's not terribly elegant though.

It is also too verbose. Both functions will end up looking _almost_ the same, except one will be calling `store` for every field and another - `parse`.

It's C++, the most overpowered language in existence. We can do better.

Enter [serialize_obj.h]()

Through a combination of [pointers to members](https://isocpp.org/wiki/faq/pointers-to-members), [variadic templates](https://eli.thegreenplace.net/2014/variadic-templates-in-c/) and a modest sprinkle of macros it coerces the compiler into auto-generating `store` and `parse` overloads from a plain list of struct members.

So we can just go:

```
OBJECT_SPEC( foo )             -- the type we are describing
        __f( aaa ),            -- field we want to be stored/parsed 
        __f( bbb )             -- another field
END_OF_SPEC                    -- close all open brackets and what not
```

and we get `store(..., const foo &)` auto-generated as soon as we call it.

Magic.

## Caveats

Our application code doesn't need any of the following, so neither is supported by the library. It's not hard to add this support however if needs be:

1. Classes with complicated inheritance, members that are references, pointers, etc., i.e. cases that require some sort of pre- or post-processing when instantiating an object.

2. Classes that use getters and setters.

3. Serializing into a tagged format, like JSON or XML. Trivial to support though by capturing field names.

4. Storing integers in a network byte order. Again, trivial to support, but a pointless thing to do for [IPC](https://en.wikipedia.org/wiki/Inter-process_communication) withing the same host, which is what we have.

## Footnotes

For a similar, but a bit more generic library see https://github.com/eliasdaler/MetaStuff

