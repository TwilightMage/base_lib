#include "../include/base_lib/TeeBuf.h"

TeeBuf::TeeBuf(List<std::streambuf*> buffs)
	: buffs(buffs)
{
}

void TeeBuf::AddBuff(std::streambuf* buff)
{
	buffs.add(buff);
}

void TeeBuf::RemoveBuff(std::streambuf* buff)
{
    buffs.remove(buff);
}

int TeeBuf::overflow(int c)
{
    if (c == EOF)
    {
        return !EOF;
    }
    else
    {
        bool eof = false;
        for (auto& buff : buffs)
        {
            if (buff->sputc(c) == EOF)
            {
                eof = true;
            }
        }

        return eof ? EOF : c;
    }
}

int TeeBuf::sync()
{
    bool zero = true;
    for (auto buff : buffs)
    {
        if (buff->pubsync() != 0)
        {
            zero = false;
        }
    }

    return zero ? 0 : -1;
}
