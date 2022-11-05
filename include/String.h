#pragma once

#include <string>
#include <vector>
#include <sstream>

#include "framework.h"
#include "ISerializable.h"
#include "List.h"
#include "Pair.h"

class EXPORT String : public ISerializable
{
public:
	String();
	String(const String& src);
	String(const std::string& str);
	String(char ch, uint count);
	String(const std::initializer_list<char>& il);
	String(const std::vector<char>& vec);
	String(const List<char>& list);
	String(char ch);
	String(const char* c_str);
	String(const char* c_str, uint size);

	String& operator=(const String& rhs);

	~String();

	FORCEINLINE std::string std() const;
	FORCEINLINE const char* c() const;
	// return the cope of inner string
	// may cause memory leak if you are not careful
	char* c_copy() const;
	wchar_t* wc() const;
	std::vector<char> vec() const;
	List<char> list() const;

	char* begin();
	char* end();
	const char* begin() const;
	const char* end() const;

	void write_to_stream(std::ostream& stream) const override;
	void read_from_stream(std::istream& stream) override;;

	FORCEINLINE uint length() const;
	FORCEINLINE uint allocated_length() const;
	int index_of(const String& substr) const;
	int last_index_of(const String& substr) const;
	int index_of_char(const String& chars) const;
	int last_index_of_char(const String& chars) const;
	String substring(int start, uint num) const;
	String substring(int start) const;
	String replace(const String& from, const String& to) const;
	String remove(const String& substring) const;
	String fit(int new_length, char filler = ' ') const;
	String to_lower() const;
	String to_upper() const;
	String trim_start(const String& chars_to_trim = " \r\n\t") const;
	String trim_end(const String& chars_to_trim = " \r\n\t") const;
	String trim(const String& chars_to_trim = " \r\n\t") const;
	bool starts_with(const String& val) const;
	bool ends_with(const String& val) const;
	bool contains(const String& substr) const;
	bool is_empty() const;
	List<String> split(const String& delimiter, bool remove_empty = false) const;
	Pair<String, String> split_at(uint position, bool skip_split_char = false) const;
	List<uint> find(const String& substr) const;

	static bool replace_single(String& src, const String& from, const String& to);

	template<typename T>
	static String join(List<T> items, const String& glue)
	{
		if (items.Length() == 0) return "";

		std::stringstream str;

		for (uint i = 0; i < items.Length(); i++)
		{
			if (i > 0) str << glue.std();
			str << items[i];
		}

		return str.str();
	}

	template<typename T>
	static String make(T val)
	{
		std::stringstream str;
		str << val;
		return str.str();
	}

	template<typename... Args>
	static String format(const String& format, Args... args)
	{
		const int size = snprintf(nullptr, 0, format.c(), std::forward<Args>(args)...);
		char* buffer = new char[size + 1];
		sprintf_s(buffer, size + 1, format.c(), std::forward<Args>(args)...);

		const auto result = String(buffer, size);
		delete buffer;
		return result;
	}

	template<typename T>
	static T parse(const String& string)
	{
		std::stringstream str;
		str << string.c();
		T result;
		str >> result;
		return result;
	}

	template<typename T>
	static bool try_parse(const String& string, T& out_value)
	{
		char* p;
		double converted = strtod(string.c(), &p);
		if (p != string.c())
		{
			return false;
		}
		else
		{
			out_value = (T) converted;
			return true;
		}
	}

	static bool is_number(const String& string);
	
	template<class C>
	static String from_ptr(C* ptr)
	{
		char hexString[20];
		sprintf_s(hexString, 20, "0x%p", ptr);
		return String(hexString).to_lower();
	}

	String operator*(uint rhs) const;

	String operator+(const String& rhs) const;
	void operator+=(const String& rhs);
	bool operator==(const String& rhs) const;
	bool operator!=(const String& rhs) const;
	bool operator<(const String& rhs) const;
	bool operator<=(const String& rhs) const;
	bool operator>(const String& rhs) const;
	bool operator>=(const String& rhs) const;

	char& operator[](uint index);
	const char& operator[](uint index) const;

private:
	char* inner_;
	uint length_;
	uint allocated_length_;
};

template<>
inline String String::join<String>(List<String> items, const String& glue)
{
	if (items.length() == 0) return "";

	String result;

	for (uint i = 0; i < items.length(); i++)
	{
		if (i > 0) result += glue;
		result += items[i];
	}

	return result;
}

template<>
inline bool String::parse<bool>(const String& rhs)
{
	return rhs == "true" || rhs == "True";
}

inline String operator+(const char* const l, const String& r)
{
	return String(l) + r.std();
}

inline std::string operator+(const std::string& l, const String& r)
{
	return l + r.std();
}

template<class Elem, class Traits>
inline std::basic_ostream<Elem, Traits>& operator<<(std::basic_ostream<Elem, Traits>& o_stream, const String& str)
{
	o_stream << str.c();
	return (o_stream);	
}
