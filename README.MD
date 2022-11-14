# base_lib
A library I wrote myself for all my stuff. It is inspired by C# types and LINQ feature, also have something from UnrealEngine

## Features
### [Array2D](https://github.com/TwilightMage/base_lib/blob/master/include/base_lib/Array2D.h)
Handy implementation of 2-dimensional array
### [Binary stream reader/writer](https://github.com/TwilightMage/base_lib/blob/master/include/base_lib/BinaryStream.h)
2 classes that allows you to simply operate with binary streams. Some classes may implement [Serializable](https://github.com/TwilightMage/base_lib/blob/master/include/base_lib/ISerializable.h "Serializable") concept and override the process of writing and reading themselves into and from a stream. Binary stream reader and writer can determine, wether object bits should be copied into memory or custom function should be executed
### [Compound](https://github.com/TwilightMage/base_lib/blob/master/include/base_lib/Compound.h)
Simply, a JSON alternative. Supported types are:
- _null_
- _char (8 bit integer)_
- _short (16 bit integer)_
- _int32 (32 bit integer)_
- _int64 (64 bit integer)_
- _bool_
- _float_
- _double_
- _array_
- _object_
- _binary data blob_

This data container may be serialized into binary stream. Conversion to textual formats also possible, you can even easily implement you own converter.

Current state of `Compound` converters:

|           |            Comment            | JSON |   YAML    |    XML     |
|:---------:|:-----------------------------:|:----:|:---------:|:----------:|
|  Parser   | `String` -> `Compound::Value` |  ✔️   | *Planned* | *Planned*  |
| Formatter | `Compound::Value` -> `String` |  ✔️   |  ✔️   | *Planned*  |

### [Delagete](https://github.com/TwilightMage/base_lib/blob/master/include/base_lib/Delegate.h)
Simple implementation of C#-like delegates in C++

Following bindings are supported:
- global or lambda function
- static function
- member function

### [List](https://github.com/TwilightMage/base_lib/blob/master/include/base_lib/List.h)
Actually a vector. Better alternative to `std::vector`. First of all, it have a function to remove an element by it's index... And **a lot** of other things.

### [Map](https://github.com/TwilightMage/base_lib/blob/master/include/base_lib/Map.h)
Tree-based implementation of map.

Recommended to use when you have to often add, search and remove elements.

### [SimpleMap](https://github.com/TwilightMage/base_lib/blob/master/include/base_lib/SimpleMap.h)
A map, based on a sorted key-value pair list.

Recommended to use when you have to often bypass all elements in map.

### [String](https://github.com/TwilightMage/base_lib/blob/master/include/base_lib/String.h)
String implementation