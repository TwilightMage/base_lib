#include "../include/TextReader.h"

TextReader::TextReader()
    : string("")
    , position(0)
    , line(0)
    , column(0)
{
}

TextReader::TextReader(const String& string)
    : string(string)
    , position(0)
    , line(0)
    , column(0)
{
}

String TextReader::peek(uint char_num) const
{
    if (position >= string.length()) return "";
    
    return string.substring(position, char_num);
}

char TextReader::peek() const
{
    if (position >= string.length()) return '\0';
    
    return string[position];
}

String TextReader::get(uint char_num)
{
    const auto result = peek(char_num);
    position += result.length();
    return result;
}

char TextReader::get()
{
    const auto result = peek();
    position++;
    return result;
}

void TextReader::skip(uint char_num)
{
    for (uint i = 0; i < char_num; i++)
    {
        if (position >= string.length()) return;

        const char ch = string[position];
        if (ch == '\n')
        {
            line++;
            column = 0;
        }
        else
        {
            column++;
        }
        position++;
    }
}

void TextReader::skip(const String& chars)
{
    while (position < string.length())
    {
        const char ch = string[position];
        if (chars.contains(ch))
        {
            if (ch == '\n')
            {
                line++;
                column = 0;
            }
            else
            {
                column++;
            }
            position++;
        }
        else break;
    }
}
