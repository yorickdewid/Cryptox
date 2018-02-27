#pragma once

namespace CryDNS {

enum RecordType : uint16_t
{
    RT_A = 1,
    RT_NS = 2,
    RF_MD = 3,
    RF_MF = 4,
    RT_CNAME = 5,
    RT_SOA = 6,
    RT_PTR = 12,
    RT_MX = 15,
    RT_TXT = 16,
    RT_AAAA = 28,
    RT_SRV = 33,
    RT_CAA = 257,
};

enum RecordClass : uint16_t
{
    RC_IN = 1,
    RC_CH = 3,
    RC_HS = 4,
};

} // namespace CryDNS
