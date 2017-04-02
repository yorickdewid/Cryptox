#include "BlockCipherFrame.h"

void BlockCipherFrame::OnEncrypt(wxCommandEvent& WXUNUSED(evt))
{
	if (m_txtInput->IsEmpty()) {
		m_statusBar->SetStatusText(wxT("Input is empty"));
		return;
	}
#if 0
	std::string cipher, encoded, recovered;
	try {
		CryptoPP::AutoSeededRandomPool rnd;

		m_statusBar->SetStatusText(wxT("Encrypting message..."));

		// Generate a random key
		CryptoPP::SecByteBlock key(0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
		rnd.GenerateBlock(key, key.size());

		// Generate a random IV
		byte iv[CryptoPP::AES::BLOCKSIZE];
		rnd.GenerateBlock(iv, CryptoPP::AES::BLOCKSIZE);

		// Create Cipher Text
		CryptoPP::CBC_Mode<CryptoPP::AES>::Encryption cbcEncryption(key, key.size(), iv);

		CryptoPP::StringSource ss(m_txtInput->GetValue(), true,
								  new CryptoPP::StreamTransformationFilter(cbcEncryption,
								  new CryptoPP::StringSink(cipher)
		) // StreamTransformationFilter      
		); // StringSource

		// Pretty print cipher text
		CryptoPP::StringSource(cipher, true, new CryptoPP::HexEncoder(new CryptoPP::StringSink(encoded), false));

	} catch (CryptoPP::Exception const& e) {
		std::cerr << e.what() << std::endl;
		m_statusBar->SetStatusText(e.what());
	}
#endif
	m_txtOutput->SetValue("magic encoding");
	m_statusBar->SetStatusText(wxT("Done"));
}

wxBEGIN_EVENT_TABLE(BlockCipherFrame, BaseBlockCipherFrame)
EVT_BUTTON(BaseBlockCipherFrame::wxID_Encrypt, BlockCipherFrame::OnEncrypt)
wxEND_EVENT_TABLE()
