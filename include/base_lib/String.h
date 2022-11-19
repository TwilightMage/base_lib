#pragma once

#include <iomanip>
#include <sstream>

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

	std::string std() const;
	const char* c() const;
	// return the copy of inner string
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

	uint length() const;
	uint allocated_length() const;
	uint index_of(const String& substr) const;
	uint last_index_of(const String& substr) const;
	uint index_of_char(const String& chars) const;
	uint last_index_of_char(const String& chars) const;
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

	String escape_chars() const;
	static String escape_char(char ch);
	
	String unescape_chars() const;
	static char unescape_char(char ch);

	static bool replace_single(String& src, const String& from, const String& to);

	template<typename T>
	static String join(const List<T>& items, const String& glue)
	{
		uint out_len = (items.length() - 1) * glue.length();

		List<String> strings(items.length());

		for (uint i = 0; i < items.length(); i++)
		{
			strings[i] = String::make(items[i]);
			out_len += strings[i].length();
		}

		String result(' ', out_len);

		char* ptr = result.inner_;
		uint i = 0;
		while (true)
		{
			memcpy(ptr, strings[i].inner_, strings[i].length());
			ptr += strings[i].length();

			if (ptr >= result.end()) return result;
			
			memcpy(ptr, glue.inner_, glue.length());
			ptr += glue.length();

			++i;
		}
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
		delete[] buffer;
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

	String operator+(char ch) const;
	void operator+=(char ch);

	char& operator[](uint index);
	const char& operator[](uint index) const;

private:
	char* inner_;
	uint length_;
	uint allocated_length_;
};

template<>
inline String String::join<String>(const List<String>& items, const String& glue)
{
	if (items.length() == 0) return "";

    uint new_length = (items.length() - 1) * glue.length();
    for (const auto& item : items) {
        new_length += item.length();
    }

	String result(' ', new_length);

    char* ptr = result.begin();
	for (uint i = 0; i < items.length(); i++)
	{
		if (i > 0) {
		    memcpy(ptr, glue.begin(), glue.length());
		    ptr += glue.length();
		}
	    memcpy(ptr, items[i].begin(), items[i].length());
		ptr += items[i].length();
	}

	return result;
}

template<>
inline bool String::parse<bool>(const String& rhs)
{
	return rhs == "true" || rhs == "True";
}

template<>
inline  String String::make<double>(double d)
{
	std::stringstream str;
	str << std::setprecision(16) << d;
	return str.str();
}

template<>
inline  String String::make<float>(float f)
{
	std::stringstream str;
	str << std::setprecision(8) << f;
	return str.str();
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
