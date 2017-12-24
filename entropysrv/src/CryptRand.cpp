#include <windows.h>
#include <wx/string.h>

#include "CryptRand.h"

void win32_crypt_random(BYTE pbBuffer[], DWORD dwLength)
{
	HCRYPTPROV hProvider = 0;

	if (!::CryptAcquireContextW(&hProvider, 0, 0, PROV_RSA_FULL, CRYPT_VERIFYCONTEXT | CRYPT_SILENT))
		return;

	//const DWORD dwLength = 8;
	//BYTE pbBuffer[dwLength] = {};

	if (!::CryptGenRandom(hProvider, dwLength, pbBuffer)) {
		::CryptReleaseContext(hProvider, 0);
		return;
	}

	//for (DWORD i = 0; i < dwLength; ++i)
		//std::cout << std::hex << static_cast<unsigned int>(pbBuffer[i]) << std::endl;

	if (!::CryptReleaseContext(hProvider, 0))
		return;
}

char *CryptRand::GenerateRandomBlock(size_t requestSize)
{
	auto tmpBuffer = new unsigned char[requestSize];
#ifdef _WIN32
	win32_crypt_random(tmpBuffer, requestSize);
#endif
	//buffer.assign(tmpBuffer, requestSize);
	//delete[] tmpBuffer;

	return (char *)tmpBuffer;
}

