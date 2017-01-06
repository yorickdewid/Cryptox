#include <cryptopp/sha.h>
#include <cryptopp/hex.h>
#include <cryptopp/filters.h>

#include "HashFrame.h"

HashFrame::HashFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
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

	m_menu31 = new wxMenu();
	wxMenuItem* m_menuItem5;
	m_menuItem5 = new wxMenuItem(m_menu31, wxID_ANY, wxString(wxT("Autoseed pool")), wxEmptyString, wxITEM_CHECK);
	m_menu31->Append(m_menuItem5);
	m_menuItem5->Check(true);

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

	m_staticText2 = new wxStaticText(this, wxID_ANY, wxT("Function"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2->Wrap(-1);
	fgSizer1->Add(m_staticText2, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxString m_choice1Choices[] = { wxT("SHA1"), wxT("SHA2"), wxT("MD5") };
	int m_choice1NChoices = sizeof(m_choice1Choices) / sizeof(wxString);
	m_choice1 = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(100, -1), m_choice1NChoices, m_choice1Choices, wxCB_SORT);
	m_choice1->SetSelection(0);
	fgSizer1->Add(m_choice1, 0, wxALL | wxEXPAND, 5);


	fgSizer1->Add(0, 0, 1, wxEXPAND, 5);

	m_txtOutput = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_READONLY);
	fgSizer1->Add(m_txtOutput, 0, wxALL | wxEXPAND, 5);


	fgSizer1->Add(0, 0, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxHORIZONTAL);

	m_chkHex = new wxCheckBox(this, wxID_ANY, wxT("Hexadecimal"), wxDefaultPosition, wxDefaultSize, 0);
	m_chkHex->SetValue(true);
	bSizer2->Add(m_chkHex, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);


	bSizer2->Add(0, 0, 1, wxEXPAND, 5);

	m_btnHash = new wxButton(this, wxID_Hash, wxT("Hash"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(m_btnHash, 0, wxALL, 2);


	fgSizer1->Add(bSizer2, 1, wxEXPAND, 5);


	this->SetSizer(fgSizer1);
	this->Layout();
	m_statusBar = this->CreateStatusBar(1, 0, wxID_ANY);

	this->Centre(wxBOTH);
}

void HashFrame::OnHash(wxCommandEvent& evt)
{
	if (m_txtInput->IsEmpty()) {
		m_statusBar->SetStatusText(wxT("Input is empty"));
		return;
	}

	CryptoPP::SHA1 hash;
	std::string encoded;

	CryptoPP::StringSource s(m_txtInput->GetValue(), true, new CryptoPP::HashFilter(hash, new CryptoPP::HexEncoder(new CryptoPP::StringSink(encoded), false)));

	m_txtOutput->SetValue(encoded);
	m_statusBar->SetStatusText(wxT("Done"));
}

wxBEGIN_EVENT_TABLE(HashFrame, wxFrame)
	EVT_BUTTON(wxID_Hash, HashFrame::OnHash)
wxEND_EVENT_TABLE()
