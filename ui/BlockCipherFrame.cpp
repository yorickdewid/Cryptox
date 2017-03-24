#include "BlockCipherFrame.h"

BlockCipherFrame::BlockCipherFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
	: wxFrame(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	m_menubar = new wxMenuBar(0);
	m_menu1 = new wxMenu();
	wxMenuItem* m_menuItem1;
	m_menuItem1 = new wxMenuItem(m_menu1, wxID_ANY, wxString(wxT("Load file")), wxEmptyString, wxITEM_NORMAL);
	m_menu1->Append(m_menuItem1);

	m_menubar->Append(m_menu1, wxT("Import"));

	m_menu3 = new wxMenu();
	wxMenuItem* m_menuItem2;
	m_menuItem2 = new wxMenuItem(m_menu3, wxID_ANY, wxString(wxT("Export to file")), wxEmptyString, wxITEM_NORMAL);
	m_menu3->Append(m_menuItem2);

	m_menubar->Append(m_menu3, wxT("Export"));

	m_menu5 = new wxMenu();
	wxMenuItem* m_menuItem6;
	m_menuItem6 = new wxMenuItem(m_menu5, wxID_ANY, wxString(wxT("Reseed pool")), wxEmptyString, wxITEM_NORMAL);
	m_menu5->Append(m_menuItem6);

	wxMenuItem* m_menuItem7;
	m_menuItem7 = new wxMenuItem(m_menu5, wxID_ANY, wxString(wxT("New key")), wxEmptyString, wxITEM_NORMAL);
	m_menu5->Append(m_menuItem7);

	wxMenuItem* m_menuItem8;
	m_menuItem8 = new wxMenuItem(m_menu5, wxID_ANY, wxString(wxT("New IV")), wxEmptyString, wxITEM_NORMAL);
	m_menu5->Append(m_menuItem8);

	m_menubar->Append(m_menu5, wxT("Material"));

	m_menu31 = new wxMenu();
	wxMenuItem* m_menuItem5;
	m_menuItem5 = new wxMenuItem(m_menu31, wxID_ANY, wxString(wxT("Autoseed pool")), wxEmptyString, wxITEM_CHECK);
	m_menu31->Append(m_menuItem5);
	m_menuItem5->Check(true);

	wxMenuItem* m_menuItem3;
	m_menuItem3 = new wxMenuItem(m_menu31, wxID_ANY, wxString(wxT("Autopadding")), wxEmptyString, wxITEM_CHECK);
	m_menu31->Append(m_menuItem3);
	m_menuItem3->Check(true);

	wxMenuItem* m_menuItem4;
	m_menuItem4 = new wxMenuItem(m_menu31, wxID_ANY, wxString(wxT("Weak algorithms")), wxEmptyString, wxITEM_CHECK);
	m_menu31->Append(m_menuItem4);

	m_menubar->Append(m_menu31, wxT("Options"));

	this->SetMenuBar(m_menubar);

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer(0, 2, 0, 20);
	fgSizer1->SetFlexibleDirection(wxBOTH);
	fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText1 = new wxStaticText(this, wxID_ANY, wxT("Input"), wxPoint(-1, -1), wxDefaultSize, 0);
	m_staticText1->Wrap(-1);
	fgSizer1->Add(m_staticText1, 0, wxALL, 5);

	m_txtInput = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 75), wxTE_MULTILINE);
	fgSizer1->Add(m_txtInput, 0, wxALL | wxEXPAND, 5);


	fgSizer1->Add(0, 0, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer1;
	bSizer1 = new wxBoxSizer(wxHORIZONTAL);

	m_radioBtn1 = new wxRadioButton(this, wxID_ANY, wxT("Plaintext"), wxDefaultPosition, wxDefaultSize, 0);
	m_radioBtn1->SetValue(true);
	bSizer1->Add(m_radioBtn1, 0, wxALL, 5);

	m_radioBtn2 = new wxRadioButton(this, wxID_ANY, wxT("Hexdecimal"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer1->Add(m_radioBtn2, 0, wxALL, 5);

	m_radioBtn3 = new wxRadioButton(this, wxID_ANY, wxT("Binary"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer1->Add(m_radioBtn3, 0, wxALL, 5);


	bSizer1->Add(200, 0, 1, wxEXPAND, 5);


	fgSizer1->Add(bSizer1, 1, wxEXPAND, 5);

	m_staticText2 = new wxStaticText(this, wxID_ANY, wxT("Cipher"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2->Wrap(-1);
	fgSizer1->Add(m_staticText2, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxString m_choice1Choices[] = { wxT("AES"), wxT("3DES"), wxT("Blowfish"), wxT("Serpent"), wxT("XTEA") };
	int m_choice1NChoices = sizeof(m_choice1Choices) / sizeof(wxString);
	m_choice1 = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(100, -1), m_choice1NChoices, m_choice1Choices, wxCB_SORT);
	m_choice1->SetSelection(0);
	fgSizer1->Add(m_choice1, 0, wxALL | wxEXPAND, 5);

	m_staticText3 = new wxStaticText(this, wxID_ANY, wxT("Mode"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText3->Wrap(-1);
	fgSizer1->Add(m_staticText3, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxString m_choice2Choices[] = { wxT("CCM"), wxT("EAX"), wxT("CBC"), wxT("ECB"), wxT("CTR"), wxT("OFB"), wxT("CFB"), wxT("GCM") };
	int m_choice2NChoices = sizeof(m_choice2Choices) / sizeof(wxString);
	m_choice2 = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(100, -1), m_choice2NChoices, m_choice2Choices, wxCB_SORT);
	m_choice2->SetSelection(0);
	fgSizer1->Add(m_choice2, 0, wxALL | wxEXPAND, 5);

	m_staticText4 = new wxStaticText(this, wxID_ANY, wxT("Key"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText4->Wrap(-1);
	fgSizer1->Add(m_staticText4, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	m_txtKey = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer1->Add(m_txtKey, 0, wxALL | wxEXPAND, 5);

	m_staticText5 = new wxStaticText(this, wxID_ANY, wxT("IV"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText5->Wrap(-1);
	m_staticText5->SetToolTip(wxT("Initialization Vector"));

	fgSizer1->Add(m_staticText5, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	m_txtIV = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer1->Add(m_txtIV, 0, wxALL | wxEXPAND, 5);

	m_staticText6 = new wxStaticText(this, wxID_ANY, wxT("AAD"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText6->Wrap(-1);
	m_staticText6->SetToolTip(wxT("Additional Authenticated Dara"));

	fgSizer1->Add(m_staticText6, 0, wxALL, 5);

	m_txtAAD = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 75), wxTE_MULTILINE);
	m_txtAAD->Enable(false);

	fgSizer1->Add(m_txtAAD, 0, wxALL | wxEXPAND, 5);


	fgSizer1->Add(0, 0, 1, wxEXPAND, 5);

	m_txtOutput = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 150), wxTE_MULTILINE | wxTE_READONLY);
	m_txtOutput->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	fgSizer1->Add(m_txtOutput, 0, wxALL | wxEXPAND, 5);


	fgSizer1->Add(0, 0, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxHORIZONTAL);

	m_chkHex = new wxCheckBox(this, wxID_ANY, wxT("Hexadecimal"), wxDefaultPosition, wxDefaultSize, 0);
	m_chkHex->SetValue(true);
	bSizer2->Add(m_chkHex, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);


	bSizer2->Add(0, 0, 1, wxEXPAND, 5);

	m_btnEncrypt = new wxButton(this, wxID_Encrypt, wxT("Encrypt"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(m_btnEncrypt, 0, wxALL, 2);

	m_btnDecrypt = new wxButton(this, wxID_Decrypt, wxT("Decrypt"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(m_btnDecrypt, 0, wxALL, 2);


	fgSizer1->Add(bSizer2, 1, wxEXPAND, 5);


	this->SetSizer(fgSizer1);
	this->Layout();
	m_statusBar = this->CreateStatusBar(1, 0, wxID_ANY);

	this->Centre(wxBOTH);
}


BlockCipherFrame::~BlockCipherFrame()
{
}

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

wxBEGIN_EVENT_TABLE(BlockCipherFrame, wxFrame)
	EVT_BUTTON(wxID_Encrypt, BlockCipherFrame::OnEncrypt)
wxEND_EVENT_TABLE()
