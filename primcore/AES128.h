#pragma once

#include "BlockCipher.h"

namespace Primitives
{

class PRIMAPI AES128 : public BlockCipher
{
public:
	AES128();

	std::string Encrypt(const std::string& data, const std::string& iv, const std::string& key) override;
	void Encrypt(unsigned char *output,
				 char *data, size_t szdata,
				 char *iv, size_t sziv,
				 char *key, size_t szkey) override;

	std::string Decrypt(const std::string& data, const std::string& iv, const std::string& key) override;
	void Decrypt(unsigned char *output,
				 char *data, size_t szdata,
				 char *iv, size_t sziv,
				 char *key, size_t szkey) override;

	static std::string LocalEncrypt(const std::string& data, const std::string& iv, const std::string& key, bool encrypt = true);
};

}
