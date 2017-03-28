#pragma once

#include <stdint.h>

constexpr char protobanner[] = "CRYPTOX_ENTROPY\n";

enum {
	ENTROPY_PROTOCOL_1 = 0x1,
};


// Mandatory fields stay the same after
// the initial release regardless of feature changes
struct EntropyProtocolHdr {

	uint8_t bannerString[16];
	uint8_t protoVersion;
	uint16_t hdrSize;

};

struct EntropyRequest : EntropyProtocolHdr {

	struct {
		uint8_t block : 1;
		uint8_t stream : 1;
		uint8_t diffusion : 1;
		uint8_t number : 1;
		uint8_t halt : 1;
		uint8_t unused : 3;
	} flag;

	int32_t requestSize;
	int32_t requestBlocks;
	int32_t magic;

};

struct EntropyStream : EntropyProtocolHdr {

	int32_t counter;
	int32_t magic;

};
