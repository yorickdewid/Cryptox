#include "nameserver.h"
#include "request.h"

#include <string>
#include <vector>

#pragma once

namespace CryDNS
{

class Resolver
{
    //SQLDatabase m_cache;
    std::vector<Nameserver> m_servers;

protected:
    void PerformRequest(std::pair<void *, size_t>);
    std::pair<void *, size_t> BuildRequest();
    void ParseResponse(void *, size_t);
    Nameserver& SelectNextServer();

    static void QuestionNameConvert(std::string&);
    static void QuestionNameConvertBack(std::string&);

public:
    struct Option
    {
        // Double verifcation will match multiple authoritive DNS server answers
        static const int DOUBLE_VERIFICATION = 1;
    };

public:
    Resolver() = default;

    // explicit Resolver(Nameserver *servers)
    // {
    //     // 
    // }

    Resolver(Nameserver&& server)
    {
        m_servers.push_back(std::move(server));
    }

    Resolver(const Nameserver& server)
    {
        m_servers.push_back(server);
    }
    
    void SetOption(int flags)
    {
        // 
    }

    void Resolve(const std::string& name, RecordType type = CryDNS::RecordType::RT_A)
    {
        PerformRequest(BuildRequest());
    }
};

} // namespace CryDNS
