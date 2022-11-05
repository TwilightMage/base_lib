#pragma once

#include "Compound.h"
#include "framework.h"
#include "IConvertible.h"
#include "ISerializable.h"

class EXPORT Margins : public IData
{
public:
    Margins();
    Margins(int left, int right, int top, int bottom);

    void convert_to(Compound::Object& to) const;
    void convert_from(const Compound::Object& from);
    
    int left;
    int right;
    int top;
    int bottom;
};
