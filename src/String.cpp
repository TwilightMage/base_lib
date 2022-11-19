#include "base_lib/String.h"

#include "base_lib/BasicTypes.h"
#include "base_lib/framework.h"
#include "base_lib/Math.h"
#include "base_lib/StreamUtils.h"
#include "base_lib/Map.h"

String::String()
	: inner_(new char('\0'))
    , length_(0)
    , allocated_length_(1)
{
}

String::String(const String& src)
	: inner_(new char[src.length_ + 1])
    , length_(src.length_)
    , allocated_length_(src.allocated_length_)
{
	memcpy(inner_, src.inner_, length_);
	inner_[length_] = '\0';
}

String::String(const std::string& str)
	: inner_(new char[str.length() + 1])
    , length_(static_cast<uint>(str.length()))
    , allocated_length_(static_cast<uint>(str.length() + 1))
{
	memcpy(inner_, str.data(), length_);
	inner_[length_] = '\0';
}

String::String(char ch, uint count)
	: inner_(new char[count + 1])
    , length_(count)
    , allocated_length_(count + 1)
{
	for (uint i = 0; i < count; i++)
	{
		inner_[i] = ch;
	}
	inner_[length_] = '\0';
}

String::String(const std::initializer_list<char>& il)
	: inner_(new char[il.size() + 1])
	, length_(static_cast<uint>(il.size()))
	, allocated_length_(static_cast<uint>(il.size() + 1))
{
	memcpy(inner_, il.begin(), il.size());
	inner_[length_] = '\0';
}

String::String(const std::vector<char>& vec)
	: inner_(new char[vec.size() + 1])
    , length_(static_cast<uint>(vec.size()))
    , allocated_length_(static_cast<uint>(vec.size() + 1))
{
	memcpy(inner_, vec.data(), vec.size());
	inner_[length_] = '\0';
}

String::String(const List<char>& list)
	: inner_(new char[list.length() + 1])
	, length_(list.length())
	, allocated_length_(list.length() + 1)
{
	memcpy(inner_, list.get_data(), list.length());
	inner_[length_] = '\0';
}

String::String(char ch)
	: inner_(new char[2])
    , length_(1)
    , allocated_length_(2)
{
	inner_[0] = ch;
	inner_[1] = '\0';
}

String::String(const char* c_str)
{
	if (c_str == nullptr)
	{
		String();
	}
	else
	{
		inner_ = new char[strlen(c_str) + 1];
		length_ = static_cast<uint>(strlen(c_str));
		allocated_length_ = length_ + 1;
		memcpy(inner_, c_str, strlen(c_str) + 1);
	}
}

String::String(const char* c_str, uint size)
	: inner_(new char[size + 1])
	, length_(size)
    , allocated_length_(size + 1)
{
	memcpy(inner_, c_str, size);
	inner_[length_] = '\0';
}

String& String::operator=(const String& rhs)
{
	if (this == &rhs) return *this;

	delete[] inner_;

	inner_ = new char[rhs.length_ + 1];
	memcpy(inner_, rhs.inner_, rhs.length_ + 1);
	length_ = rhs.length_;
	allocated_length_ = rhs.allocated_length_;

	return *this;
}

String::~String()
{
	delete[] inner_;
}

std::string String::std() const
{
	return inner_;
}

const char* String::c() const
{
	return inner_;
}

char* String::c_copy() const
{
	char* result = new char[length_ + 1];
	memcpy(result, inner_, length_ + 1);
	return result;
}

wchar_t* String::wc() const
{
	wchar_t* result = new wchar_t[length_];

	mbstowcs_s(nullptr, result, length_ + 1, inner_, length_);

	return result;
}

std::vector<char> String::vec() const
{
	return std::vector<char>(inner_, inner_ + length_);
}

List<char> String::list() const
{
	return List<char>(inner_, length_);
}

char* String::begin()
{
	return inner_;
}

char* String::end()
{
	return inner_ + length_;
}

const char* String::begin() const
{
	return inner_;
}

const char* String::end() const
{
	return inner_ + length_;
}

void String::write_to_stream(std::ostream& stream) const
{
	StreamUtils::write_c_string(stream, inner_, length_);
}

void String::read_from_stream(std::istream& stream)
{
	delete[] inner_;
	StreamUtils::read_c_string(stream, inner_, allocated_length_);
	length_ = allocated_length_ - 1;
}

uint String::length() const
{
	return length_;
}

uint String::allocated_length() const
{
	return allocated_length_;
}

uint String::index_of(const String& substr) const
{
	if (substr.length_ > length_) return BAD_INDEX;

	for (uint i = 0; i < length_ - substr.length_ + 1; i++)
	{
		bool flag = true;
		for (uint j = 0; j < substr.length_; j++)
		{
		    if (inner_[j + i] != substr.inner_[j])
		    {
				flag = false;
				break;
		    }
		}

		if (flag)
		{
			return i;
		}
	}

	return BAD_INDEX;
}

uint String::last_index_of(const String& substr) const
{
	if (substr.length_ > length_) return BAD_INDEX;

	for (uint i = substr.length_; i < length_; i++)
	{
		bool flag = true;
		for (uint j = 0; j < substr.length_; j++)
		{
			if (inner_[length_ - i + j] != substr.inner_[j])
			{
				flag = false;
				break;
			}
		}

		if (flag)
		{
			return length_ - i;
		}
	}

	return BAD_INDEX;
}

uint String::index_of_char(const String& chars) const
{
	for (uint i = 0; i < length_; i++)
	{
		for (auto ch : chars)
		{
			if (inner_[i] == ch) return i;
		}
	}

	return BAD_INDEX;
}

uint String::last_index_of_char(const String& chars) const
{
	for (uint i = 0; i < length_; i++)
	{
		for (auto ch : chars)
		{
			if (inner_[length_ - i - 1] == ch) return length_ - i - 1;
		}
	}

	return BAD_INDEX;
}

String String::substring(int start, uint num) const
{
	if (start >= 0)
	{
		if (static_cast<uint>(start) >= length_) return "";
	}
	else
	{
		start += static_cast<int>(length_);
	}
	
	const uint result_end = Math::min(length_, start + num);

	String result(' ', result_end - start);
	
	memcpy(result.inner_, inner_ + start, result_end - start);

	return result;
}

String String::substring(int start) const
{
	if (start < 0) start += length_;
	
	return substring(start, length_ - start);
}

String String::replace(const String& from, const String& to) const
{
	List<uint> positions = find(from);
	const int offset = to.length_ - from.length_;

	const uint newLength = length_ + positions.length() * offset;
    String result = String(' ', newLength);

	uint counter = 0;
	for (uint i = 0; i < newLength; i++)
	{
		if (counter < positions.length() && i - offset * counter == positions[counter])
		{
			memcpy(&result[i], to.inner_, to.length_);
			counter++;
			i += to.length_ - 1;
		}
		else
		{
			result[i] = inner_[i - offset * counter];
		}
	}

	return result;
}

String String::remove(const String& substring) const
{
	return replace(substring, "");
}

String String::fit(int new_length, char filler) const
{
	const bool keepLeft = new_length > 0;
	new_length = abs(new_length);

	if (new_length < static_cast<int>(length_))
	{
		if (keepLeft) return substring(0, new_length);
		return substring(length_ - new_length);
	}

	if (static_cast<int>(length_) < new_length)
	{
		String result = String(' ', new_length);

		if (keepLeft)
		{
			memcpy(result.inner_, inner_, length_);
			memcpy(result.inner_ + length_, std::string(new_length - length_, filler).c_str(), (new_length - length_));
		}
		else
		{
			memcpy(result.inner_, std::string(new_length - length_, filler).c_str(), (new_length - length_));
			memcpy(result.inner_ + (new_length - length_), inner_, length_);
		}

		return result;
	}

	return *this;
}

String String::to_lower() const
{
	String result = *this;

	for (uint i = 0; i < length_; i++)
	{
		result.inner_[i] = std::tolower(result.inner_[i]);
	}

	return result;
}

String String::to_upper() const
{
	String result = *this;

	for (uint i = 0; i < length_; i++)
	{
		result.inner_[i] = std::toupper(result.inner_[i]);
	}

	return result;
}

String String::trim_start(const String& chars_to_trim) const
{
	uint num = 0;

	while (num < length_ && chars_to_trim.contains(inner_[num]))
	{
		num++;
	}

	return substring(num);
}

String String::trim_end(const String& chars_to_trim) const
{
	uint num = 0;

	while (num < length_ && chars_to_trim.contains(inner_[length_ - 1 - num]))
	{
		num++;
	}

	return substring(0, length_ - num);
}

String String::trim(const String& chars_to_trim) const
{
	return trim_start(chars_to_trim).trim_end(chars_to_trim);
}

bool String::starts_with(const String& val) const
{
	if (val.length_ > length_) return false;

	for (uint i = 0; i < val.length_; i++)
	{
		if (inner_[i] != val.inner_[i]) return false;
	}

	return true;
}

bool String::ends_with(const String& val) const
{
	if (val.length_ > length_) return false;

	for (uint i = 0; i < val.length_; i++)
	{
		if (inner_[length_ - val.length_ + i] != val.inner_[i]) return false;
	}

	return true;
}

bool String::contains(const String& substr) const
{
	return index_of(substr) != BAD_INDEX;
}

bool String::is_empty() const
{
	return length_ == 0;
}

List<String> String::split(const String& delimiter, bool remove_empty) const
{
	if (length_ == 0) return List<String>();

	auto positions = find(delimiter);

	List<String> result;

	uint i = 0;
	for (uint c = 0; c < positions.length(); c++)
	{
		const auto substr = substring(i, positions[c] - i);
		if (substr.length_ > 0 || !remove_empty)
		{
			result.add(substr);
		}
		i = positions[c] + delimiter.length_;
	}

	if (positions.length() > 0)
	{
		result.add(substring(positions[positions.length() - 1] + delimiter.length_));
	}
	else
	{
		return { *this };
	}

	return result;
}

Pair<String, String> String::split_at(uint position, bool skip_split_char) const
{
	if (skip_split_char)
	{
		if (position >= length_) return { *this, "" };
		if (position == 0) return { "", *this };

		Pair result = { String(' ', position), String(' ', length_ - position) };

		for (uint i = 0; i < position; i++)
		{
			result.key.inner_[i] = inner_[i];
		}

		for (uint i = position; i < length_; i++)
		{
			result.value.inner_[i] = inner_[position + i];
		}

		return result;
	}
	else
	{
		if (position >= length_) return { *this, "" };

		Pair result = { String(' ', position), String(' ', length_ - position - 1) };

		for (uint i = 0; i < position; i++)
		{
			result.key.inner_[i] = inner_[i];
		}

		for (uint i = position + 1; i < length_; i++)
		{
			result.value.inner_[i] = inner_[position + 1 + i];
		}

		return result;
	}
}

List<uint> String::find(const String& substr) const
{
	List<uint> result;

	for (uint i = 0; i < length_ - substr.length_ + 1; i++)
	{
		bool flag = true;

		for (uint j = 0; j < substr.length_; j++)
		{
			if (inner_[i + j] != substr.inner_[j])
			{
				flag = false;
				break;
			}
		}

		if (flag)
		{
			result.add(i);
			i += substr.length_ - 1;
		}
	}

	return result;
}

Map<char, char> escape = {
	{'\x07', 'a'},
	{'\x08', 'b'},
	{'\x09', 't'},
	{'\x0A', 'n'},
	{'\x0B', 'v'},
	{'\x0C', 'f'},
	{'\x0D', 'r'},
	{'\x1B', 'e'},

	{'"', '"'},
	{'\'', '\''},
	{'?', '?'},
	{'\\', '\\'}
};

String String::escape_chars() const
{
	uint new_length = length_;
	List<uint> positions;

	for (uint i = 0; i < length_; i++)
	{
		if (escape.contains(inner_[i]))
		{
			positions += i;
			++new_length;
		}
	}

	String result(' ', new_length);

	uint ptr_src = 0;
	uint ptr_dst = 0;
	uint last_pos = 0;
	for (uint i = 0; i < positions.length(); i++)
	{
		memcpy(result.inner_ + ptr_dst, inner_ + ptr_src, positions[i] - last_pos);
		
		ptr_src += positions[i] - last_pos;
		ptr_dst += positions[i] - last_pos;
		
		result.inner_[ptr_dst++] = '\\';
		result.inner_[ptr_dst++] = escape[inner_[ptr_src++]];
		
		last_pos = positions[i] + 1;
	}
	memcpy(result.inner_ + ptr_dst, inner_ + ptr_src, length_ - last_pos);

	return result;
}

String String::escape_char(char ch)
{
	if (escape.contains(ch)) return String("\\") + escape[ch];
	return ch;
}

const static Map<char, char> unescape = {
	{'a', '\x07'},
	{'b', '\x08'},
	{'t', '\x09'},
	{'n', '\x0A'},
	{'v', '\x0B'},
	{'f', '\x0C'},
	{'r', '\x0D'},
	{'e', '\x1B'},

	{'"', '"'},
	{'\'', '\''},
	{'?', '?'},
	{'\\', '\\'}
};

String String::unescape_chars() const
{
	uint new_length = length_;
	List<uint> positions;

	for (uint i = 0; i < length_; i++)
	{
		if (inner_[i] == '\\')
		{
			positions += i;
			--new_length;
		}
	}

	String result(' ', new_length);

	uint ptr_src = 0;
	uint ptr_dst = 0;
	uint last_pos = 0;
	for (uint i = 0; i < positions.length(); i++)
	{
		memcpy(result.inner_ + ptr_dst, inner_ + ptr_src, positions[i] - last_pos);
		
		ptr_src += positions[i] - last_pos;
		ptr_dst += positions[i] - last_pos;
		
		ptr_src++;
		result.inner_[ptr_dst++] = unescape[inner_[ptr_src++]];
		
		last_pos = positions[i] + 2;
	}
	memcpy(result.inner_ + ptr_dst, inner_ + ptr_src, length_ - last_pos);

	return result;
}

char String::unescape_char(char ch)
{
	return unescape.find_or_default(ch, ch);
}

bool String::replace_single(String& src, const String& from, const String& to)
{
	const uint pos = src.index_of(from);
	if (pos != BAD_INDEX)
	{
		const int offset = to.length_ - from.length_;

		char* target = src.inner_;
		const bool extend = src.length_ + offset > src.allocated_length_ - 1;

		if (extend)
		{
			target = new char[src.length_ + offset + 1];
			memcpy(target, src.inner_, pos);
		}

		if (offset > 0)
		{
			memcpy_b(target + (pos + to.length_), src.inner_ + (pos + from.length_), src.length_ - from.length_ - pos);
		}
		else if (offset < 0)
		{
			memcpy(target + (pos + to.length_), src.inner_ + (pos + from.length_), src.length_ - from.length_ - pos);
		}

		memcpy(target + pos, to.inner_, to.length_);

		src.length_ += offset;
		target[src.length_] = '\0';

		if (extend)
		{
			delete[] src.inner_;
			src.inner_ = target;
			src.allocated_length_ = src.length_ + 1;
		}

		return true;
	}

	return false;
}

bool String::is_number(const String& string)
{
	char* p;
	double converted = strtod(string.c(), &p);
	return p != string.c();
}

String String::operator*(uint rhs) const
{
	if (rhs == 0) return "";

	String result = String(' ', length_ * rhs);

	for (uint i = 0; i < rhs; i++)
	{
		for (uint j = 0; j < length_; j++)
		{
			result[i * length_ + j] = inner_[j];
		}
	}

	return result;
}

String String::operator+(const String& rhs) const
{
	String result = String(' ', length_ + rhs.length_);

	memcpy(result.inner_, inner_, length_);
	memcpy(result.inner_ + length_, rhs.inner_ , rhs.length_);

	return result;
}

void String::operator+=(const String& rhs)
{
	const bool extend = length_ + rhs.length_ + 1 > allocated_length_;

	char* target = inner_;
	if (extend)
	{
		target = new char[length_ + rhs.length_ + 1];
		memcpy(target, inner_, length_);
	}

	memcpy(target + length_, rhs.inner_, rhs.length_ + 1);

	length_ += rhs.length_;

	if (extend)
	{
		delete[] inner_;
		inner_ = target;
		allocated_length_ = length_ + 1;
	}
}

bool String::operator==(const String& rhs) const
{
	if (length_ != rhs.length_) return false;

	for (uint i = 0; i < length_; i++)
	{
		if (inner_[i] != rhs.inner_[i]) return false;
	}

	return true;
}

bool String::operator!=(const String& rhs) const
{
	return !operator==(rhs);
}

bool String::operator<(const String& rhs) const
{
	for (uint i = 0; i < Math::min(length_, rhs.length_); i++)
	{
		if (inner_[i] < rhs.inner_[i]) return true;
		if (inner_[i] > rhs.inner_[i]) return false;
	}

	return length_ < rhs.length_;
}

bool String::operator<=(const String& rhs) const
{
	for (uint i = 0; i < Math::min(length_, rhs.length_); i++)
	{
		if (inner_[i] < rhs.inner_[i]) return true;
		if (inner_[i] > rhs.inner_[i]) return false;
	}

	return length_ <= rhs.length_;
}

bool String::operator>(const String& rhs) const
{
	for (uint i = 0; i < Math::min(length_, rhs.length_); i++)
	{
		if (inner_[i] > rhs.inner_[i]) return true;
		if (inner_[i] < rhs.inner_[i]) return false;
	}

	return length_ > rhs.length_;
}

bool String::operator>=(const String& rhs) const
{
	for (uint i = 0; i < Math::min(length_, rhs.length_); i++)
	{
		if (inner_[i] > rhs.inner_[i]) return true;
		if (inner_[i] < rhs.inner_[i]) return false;
	}

	return length_ >= rhs.length_;
}

String String::operator+(char ch) const
{
	String result = String(' ', length_ + 1);

	memcpy(result.inner_, inner_, length_);
	result.inner_[length_] = ch;

	return result;
}

void String::operator+=(char ch)
{
	const bool extend = length_ + 1 + 1 > allocated_length_;

	char* target = inner_;
	if (extend)
	{
		target = new char[length_ + 1 + 1];
		memcpy(target, inner_, length_);
	}

	target[length_] = ch;
	target[length_ + 1] = '\0';

	length_++;

	if (extend)
	{
		delete[] inner_;
		inner_ = target;
		allocated_length_ = length_ + 1;
	}
}

char& String::operator[](uint index)
{
	if (index >= length_)
	{
		throw std::out_of_range(format("Parameter \"index\" is greater than last character index - %i", length_ - 1).c());
	}

	return inner_[index];
}

const char& String::operator[](uint index) const
{
	if (index >= length_)
	{
		throw std::out_of_range(format("Parameter \"index\" is greater than last character index - %i", length_ - 1).c());
	}

	return inner_[index];
}
