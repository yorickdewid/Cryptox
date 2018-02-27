//
// blocking_udp_echo_client.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <cstring>
#include <iostream>

#include "nameserver.h"
#include "resolver.h"

#define GOOGLE_DNS_V4_1 "8.8.8.8"
#define GOOGLE_DNS_V4_2 "8.8.4.4"

// enum { max_length = 1024 };

int main(int argc, char* argv[]) {

	// Use Google DNS for now
	// CryDNS::Nameserver servers[] = {
	// 	GOOGLE_DNS_V4_1,
	// 	GOOGLE_DNS_V4_2,
	// };

	// Resolve nu.nl to an ipv4 address
	CryDNS::Resolver resolver{ GOOGLE_DNS_V4_1 };
	resolver.Resolve("nu.nl");

#if 0
	CryDNS::Resolver::iterator it = resolver.Resolve("google.com", DNS::RecordType::RT_A);
	
	//
	for	(; it != CryDNS::Resolver.end(); ++it) {
		std::cout << it-> std::endl;
	}
#endif

	return 0;
}
