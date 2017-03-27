#pragma once

#include <stdint.h>

constexpr char protobanner[] = "CRYPTOX_ENTROPY\n";

enum {
	ENTROPY_PROTOCOL_1 = 0x1,
};


// Mandatory fields regardless of feature changes
struct EntropyProtocolHdr {

	uint8_t bannerString[16];
	uint8_t protoVersion;
	uint16_t hdrSize;

};

struct EntropyProtocol : EntropyProtocolHdr {

	struct {
		uint8_t stream : 1;
		uint8_t unused : 7;
	} flag;

	int32_t requestSize;
	int32_t magic;

};
