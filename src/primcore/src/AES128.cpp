#include "AES128.h"

#include <openssl/evp.h>
#include <openssl/aes.h>

Primitives::AES128::AES128()
	: BlockCipher("AES-128", AES_BLOCK_SIZE, 128)
{
	SetInfo("Advanced Encryption Standard 128bit", 1998, false, {
		"Vincent Rijmen",
		"Joan Daemen",
	});
}


std::string Primitives::AES128::LocalEncrypt(const std::string& data, const std::string& iv, const std::string& key, bool encrypt)
{
	unsigned blocksize;
	int out_len;
	EVP_CIPHER_CTX ctx;
	std::string output;

	const unsigned char *cdata = reinterpret_cast<const unsigned char *>(data.c_str());
	const unsigned char *civ = reinterpret_cast<const unsigned char *>(iv.c_str());
	const unsigned char *ckey = reinterpret_cast<const unsigned char *>(key.c_str());

	EVP_CipherInit(&ctx, EVP_aes_128_ecb(), ckey, civ, encrypt ? 1 : 0);
	blocksize = EVP_CIPHER_CTX_block_size(&ctx);
	output.resize(data.size());
	out_len = data.size();

	// Encrypt each piece of data as it comes in:
	EVP_CipherUpdate(&ctx, (unsigned char *)&output[0], &out_len, cdata, data.size());

	// When you're done with the data, finalize it:
	EVP_CipherFinal(&ctx, (unsigned char *)&output[0], &out_len);

	return output;
}


std::string Primitives::AES128::Encrypt(const std::string& data, const std::string& iv, const std::string& key)
{
	return Primitives::AES128::LocalEncrypt(data, iv, key);
}


void Primitives::AES128::Encrypt(unsigned char *output,
								 char *data, size_t szdata,
								 char *iv, size_t sziv,
								 char *key, size_t szkey)
{
	auto sdata = std::string(data, szdata);
	auto siv = std::string(iv, sziv);
	auto skey = std::string(key, szkey);
	auto result = Primitives::AES128::LocalEncrypt(sdata, siv, skey);
	result.copy(reinterpret_cast<char *>(output), 16, 0);
}


std::string Primitives::AES128::Decrypt(const std::string& data, const std::string& iv, const std::string& key)
{
	return Primitives::AES128::LocalEncrypt(data, iv, key, false);
}


void Primitives::AES128::Decrypt(unsigned char *output,
								 char *data, size_t szdata,
								 char *iv, size_t sziv,
								 char *key, size_t szkey)
{
	auto sdata = std::string(data, szdata);
	auto siv = std::string(iv, sziv);
	auto skey = std::string(key, szkey);
	auto result = Primitives::AES128::LocalEncrypt(sdata, siv, skey, false);
	result.copy(reinterpret_cast<char *>(output), 16, 0);
}
