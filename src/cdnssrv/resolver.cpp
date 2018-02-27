#include <cstdint>
#include <iostream>

#include "resolver.h"
#include "request.h"
#include "byteorder.h"

#include <boost/asio.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/endian/conversion.hpp>

#define DEFAULT_SERVICE_PORT 53

//TODO: htons

using namespace CryDNS;
using boost::asio::ip::udp;

static int g_serverCounter = 0;

#pragma pack(push, 1)
struct DnsPacketHeader
{
    uint16_t Identifier;        // Packet identifier
    
    struct
    {
        uint8_t RD : 1;             // Recursion desired
        uint8_t TC : 1;             // Truncated message
        uint8_t AA : 1;             // Authoritive answer
        uint8_t Opcode : 4;         // Purpose of message
        uint8_t QR : 1;             // Query/Response flag
        
        uint8_t Rcode : 4;          // Response code
        uint8_t CD : 1;             // Checking disabled
        uint8_t AD : 1;             // Authenticated data
        uint8_t Z : 1;              // Reserved
        uint8_t RA : 1;             // Recursion available
    } Options;
 
    uint16_t QuestionCount;     // Number of question entries
    uint16_t AnswerCount;       // Number of answer entries
    uint16_t AuthorityCount;    // Number of authority entries
    uint16_t AdditionalCount;   // Number of resource entries

    void Serialize(uint8_t *datablock)
    {
        size_t offset = 0;
        uint16_t _Identifier = BYTE_ORDER_16(Identifier);

        uint16_t _QuestionCount = BYTE_ORDER_16(QuestionCount);
        uint16_t _AnswerCount = BYTE_ORDER_16(AnswerCount);
        uint16_t _AuthorityCount = BYTE_ORDER_16(AuthorityCount);
        uint16_t _AdditionalCount = BYTE_ORDER_16(AdditionalCount);
        
        memcpy(datablock + offset, &_Identifier, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        memcpy(datablock + offset, &Options, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        memcpy(datablock + offset, &_QuestionCount, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        memcpy(datablock + offset, &_AnswerCount, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        memcpy(datablock + offset, &_AuthorityCount, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        memcpy(datablock + offset, &_AdditionalCount, sizeof(uint16_t));
    }

    void Unserialize(uint8_t *datablock)
    {
        size_t offset = 0;
        uint16_t _Identifier;

        uint16_t _QuestionCount;
        uint16_t _AnswerCount;
        uint16_t _AuthorityCount;
        uint16_t _AdditionalCount;
        
        memcpy(&_Identifier, datablock + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        memcpy(&Options, datablock + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        memcpy(&_QuestionCount, datablock + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        memcpy(&_AnswerCount, datablock + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        memcpy(&_AuthorityCount, datablock + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        memcpy(&_AdditionalCount, datablock + offset, sizeof(uint16_t));

        Identifier = BYTE_ORDER_16(_Identifier);
        QuestionCount = BYTE_ORDER_16(_QuestionCount);
        AnswerCount = BYTE_ORDER_16(_AnswerCount);
        AuthorityCount = BYTE_ORDER_16(_AuthorityCount);
        AdditionalCount = BYTE_ORDER_16(_AdditionalCount);
    }
};
#pragma pack(pop)

#pragma pack(push, 1)
struct Request
{
    RecordType QueryType;
    RecordClass QueryClass;

    void Serialize(uint8_t *datablock)
    {
        size_t offset = 0;
        uint16_t _QueryType = BYTE_ORDER_16(QueryType);
        uint16_t _QueryClass = BYTE_ORDER_16(QueryClass);
        memcpy(datablock + offset, &_QueryType, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        memcpy(datablock + offset, &_QueryClass, sizeof(uint16_t));
    }

    void Unserialize(uint8_t *datablock)
    {
        size_t offset = 0;
        uint16_t _QueryType;
        uint16_t _QueryClass;

        memcpy(&_QueryType, datablock + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        memcpy(&_QueryClass, datablock + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);

        QueryType = static_cast<RecordType>(BYTE_ORDER_16(_QueryType));
        QueryClass = static_cast<RecordClass>(BYTE_ORDER_16(_QueryClass));
    }
};
#pragma pack(pop)

#pragma pack(push, 1)
struct ResourceRecord : public Request
{
    uint32_t TTL;
    uint16_t RdataLenth;
    uint16_t Rdata;

    void Unserialize(uint8_t *datablock)
    {
        Request::Unserialize(datablock);

        size_t offset = sizeof(Request);
        uint32_t _TTL;
        uint16_t _RdataLenth;
        uint16_t _Rdata;

        memcpy(&_TTL, datablock + offset, sizeof(uint32_t));
        offset += sizeof(uint32_t);
        memcpy(&_RdataLenth, datablock + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);
        memcpy(&_Rdata, datablock + offset, sizeof(uint16_t));
        offset += sizeof(uint16_t);

        TTL = BYTE_ORDER_32(_TTL);
        RdataLenth = BYTE_ORDER_16(_RdataLenth);
        Rdata = BYTE_ORDER_16(_Rdata);
    }
};
#pragma pack(pop)

boost::asio::ip::address NameserverAddress(const Nameserver& server)
{
    return boost::asio::ip::address::from_string(server.Address());
}

Nameserver& Resolver::SelectNextServer()
{
    return m_servers.at(g_serverCounter++ % m_servers.size());
}

std::pair<void *, size_t> Resolver::BuildRequest()
{
    DnsPacketHeader packetHeader;
    packetHeader.Identifier = (uint32_t)getpid();
    
    packetHeader.Options.QR = 0;
    packetHeader.Options.Opcode = 0;
    packetHeader.Options.AA = 0;
    packetHeader.Options.TC = 0;
    packetHeader.Options.RD = 1;
    packetHeader.Options.RA = 0;
    packetHeader.Options.Z = 0;
    packetHeader.Options.AD = 0;
    packetHeader.Options.CD = 0;
    packetHeader.Options.Rcode = 0;

    packetHeader.QuestionCount = 1;
    packetHeader.AnswerCount = 0;
    packetHeader.AuthorityCount = 0;
    packetHeader.AdditionalCount = 0;

    std::string queryName{ "www.northeastern.edu" };
    QuestionNameConvert(queryName);

    Request query;
    query.QueryType = RecordType::RT_A;
    query.QueryClass = RecordClass::RC_IN;

    size_t totalPacketSize = sizeof(DnsPacketHeader) + queryName.size() + 1 + sizeof(Request);
    uint8_t *data = new uint8_t[totalPacketSize];
    memset(data, '\0', totalPacketSize);
    
    //
    uint8_t *ptr = data;
    packetHeader.Serialize(ptr);

    //
    memcpy(data + sizeof(DnsPacketHeader), queryName.data(), queryName.size());
    
    //
    ptr = data + sizeof(DnsPacketHeader) + queryName.size() + 1;
    query.Serialize(ptr);
    
    return {data, totalPacketSize};
}
void DumpHex(const void* data, size_t size);
void Resolver::ParseResponse(void *data, size_t size)
{
    uint8_t *_data = (uint8_t *)data;

    DnsPacketHeader packetHeader;
    packetHeader.Unserialize(_data);

    printf("The response contains: ");
    printf("\n %u Questions.", packetHeader.QuestionCount);
    printf("\n %u Answers.", packetHeader.AnswerCount);
    printf("\n %u Authoritative Servers.", packetHeader.AuthorityCount);
    printf("\n %u Additional records.\n", packetHeader.AdditionalCount);

    std::string qname;
    uint8_t *_data2 = _data  + sizeof(DnsPacketHeader);
    for (;*_data2!='\0';_data2++) {
        qname.push_back(*_data2);
    }

    // QuestionNameConvertBack(qname);
    // printf("\n %s", qname.c_str());

    _data2 += 1 + sizeof(Request);

    // For each answer
    for (int i = 0; i < packetHeader.AnswerCount; ++i) {
        printf("\n>>\n");
        for (;*_data2!='\0';_data2++) {
            qname.push_back(*_data2);
        }

        printf("\n %s", qname.c_str());

        ResourceRecord rr;
        rr.Unserialize(_data2);

        printf("\n %u Query type.", rr.QueryType);
        printf("\n %u Query class.", rr.QueryClass);
        printf("\n %u TTL.", rr.TTL);
        printf("\n %u RdataLenth.\n", rr.RdataLenth);

        _data2 += 10;

        uint8_t *rdata = new uint8_t[rr.RdataLenth];
        for (int j = 0; j<rr.RdataLenth; ++j) {
            rdata[j] = _data2[j];
        }
        DumpHex(rdata, rr.RdataLenth);
        delete[] rdata;

        _data2 += rr.RdataLenth;

        // break;
    }

    // ResourceRecord rr;
    // rr.Unserialize(_data2);

    // printf("\n %u Query type.", rr.QueryType);
    // printf("\n %u Query class.", rr.QueryClass);
    // printf("\n %u TTL.", rr.TTL);
    // printf("\n %u RdataLenth.", rr.RdataLenth);
    // printf("\n %u Rdata.", rr.Rdata);
}

void DumpHex(const void* data, size_t size) {
	char ascii[17];
	size_t i, j;
	ascii[16] = '\0';
	for (i = 0; i < size; ++i) {
		printf("%02X ", ((unsigned char*)data)[i]);
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char*)data)[i];
		} else {
			ascii[i % 16] = '.';
		}
		if ((i+1) % 8 == 0 || i+1 == size) {
			printf(" ");
			if ((i+1) % 16 == 0) {
				printf("|  %s \n", ascii);
			} else if (i+1 == size) {
				ascii[(i+1) % 16] = '\0';
				if ((i+1) % 16 <= 8) {
					printf(" ");
				}
				for (j = (i+1) % 16; j < 16; ++j) {
					printf("   ");
				}
				printf("|  %s \n", ascii);
			}
		}
	}
}

void Resolver::PerformRequest(std::pair<void *, size_t> data)
{
    try {
		// Boost io service
		boost::asio::io_service io_service;

		// Create udp socket
		udp::socket socket(io_service, udp::endpoint(udp::v4(), 0));

		// Create the endpoint
		udp::endpoint endpoint = udp::endpoint(NameserverAddress(SelectNextServer()), DEFAULT_SERVICE_PORT);

		// Send to endpoint
		socket.send_to(boost::asio::buffer(data.first, data.second), endpoint);

		uint8_t reply[data.second * 2];
		udp::endpoint sender_endpoint;
		size_t reply_length = socket.receive_from(boost::asio::buffer(reply, data.second * 2), sender_endpoint);
        DumpHex(reply, reply_length);
		ParseResponse(reply, reply_length);
	}
	catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << "\n";
	}
}

// Convert query name into DNS request format
void Resolver::QuestionNameConvertBack(std::string& name)
{
    // std::ostringstream oss;
    // std::istringstream iss(name);
    // std::string token;
    // while (std::getline(iss, token, '.')) {
    //     if (!token.empty()) {
    //         size_t sz = token.size();
    //         oss.write((const char *)&sz, sizeof(uint8_t));
    //         oss << token;
    //     }
    // }

    // name = oss.str();
}

// Convert query name into DNS request format
void Resolver::QuestionNameConvert(std::string& name)
{
    std::ostringstream oss;
    std::istringstream iss(name);
    std::string token;
    while (std::getline(iss, token, '.')) {
        if (!token.empty()) {
            size_t sz = token.size();
            oss.write((const char *)&sz, sizeof(uint8_t));
            oss << token;
        }
    }

    name = oss.str();
}
