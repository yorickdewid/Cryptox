#include <string>

#pragma once

namespace CryDNS
{

class Nameserver
{
    std::string m_address;

public:
    Nameserver(const std::string& server)
        : m_address{ server }
    {
    }

    Nameserver(const char *server)
        : m_address{ server }
    {
    }

    inline std::string Address() const noexcept
    {
        return m_address;
    }
};

} // namespace CryDNS
