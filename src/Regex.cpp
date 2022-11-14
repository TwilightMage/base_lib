#include "../include/base_lib/Regex.h"

#include <regex>

Regex::Regex(const String& expression)
    : expression_(_strdup(expression.c()))
{
}

bool Regex::check(const String& subject) const
{
    return std::regex_search(subject.c(), std::regex(expression_));
}

Shared<List<String>> Regex::capture(const String& subject) const
{
    std::cmatch match_results;
    if (std::regex_match(subject.c(), match_results, std::regex(expression_)))
    {
        Shared<List<String>> result = MakeShared<List<String>>();
        for (auto& mr : match_results)
        {
            result->add(String(mr.first, static_cast<uint>(mr.length())));
        }
        return result;
    }

    return nullptr;
}
