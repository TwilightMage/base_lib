#pragma once

#include "BasicTypes.h"
#include "Compound.h"
#include "framework.h"
#include "IConvertible.h"
#include "String.h"

struct EXPORT Version : public IData, IConvertible<Compound::Array>
{
    Version();
    Version(uint version_number, uint patch_number, uint fix_number);
    Version(const String& version_string);

    String to_string() const;

    bool operator<(const Version& rhs) const;
    bool operator>(const Version& rhs) const;
    bool operator<=(const Version& rhs) const;
    bool operator>=(const Version& rhs) const;
    bool operator==(const Version& rhs) const;
    bool operator!=(const Version& rhs) const;

    void convert_to(Compound::Array& to) const override;
    void convert_from(const Compound::Array& from) override;

    uint version_number;
    uint patch_number;
    uint fix_number;
};
