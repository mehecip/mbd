#pragma once

#include <string>

namespace mbd
{
    class uuid
    {
    public:
        uuid(const std::string &str);
        bool operator==(uuid &other) const;
        const std::string& to_string() const;

    private:
        std::string _uuid;
    };
}