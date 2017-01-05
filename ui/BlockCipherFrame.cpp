#include "BlockCipherFrame.h"


BlockCipherFrame::BlockCipherFrame(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style) : wxFrame(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	this->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNFACE));

	m_menubar1 = new wxMenuBar(0);
	m_menu1 = new wxMenu();
	wxMenuItem* m_menuItem1;
	m_menuItem1 = new wxMenuItem(m_menu1, wxID_ANY, wxString(wxT("Load file")), wxEmptyString, wxITEM_NORMAL);
	m_menu1->Append(m_menuItem1);

	m_menubar1->Append(m_menu1, wxT("Import"));

	m_menu3 = new wxMenu();
	wxMenuItem* m_menuItem2;
	m_menuItem2 = new wxMenuItem(m_menu3, wxID_ANY, wxString(wxT("Save result")), wxEmptyString, wxITEM_NORMAL);
	m_menu3->Append(m_menuItem2);

	m_menubar1->Append(m_menu3, wxT("Export"));

	m_menu31 = new wxMenu();
	wxMenuItem* m_menuItem3;
	m_menuItem3 = new wxMenuItem(m_menu31, wxID_ANY, wxString(wxT("Autopadding")), wxEmptyString, wxITEM_CHECK);
	m_menu31->Append(m_menuItem3);
	m_menuItem3->Check(true);

	wxMenuItem* m_menuItem4;
	m_menuItem4 = new wxMenuItem(m_menu31, wxID_ANY, wxString(wxT("Weak algorithms")), wxEmptyString, wxITEM_CHECK);
	m_menu31->Append(m_menuItem4);

	m_menubar1->Append(m_menu31, wxT("Options"));

	this->SetMenuBar(m_menubar1);

	wxFlexGridSizer* fgSizer1;
	fgSizer1 = new wxFlexGridSizer(0, 2, 0, 20);
	fgSizer1->SetFlexibleDirection(wxBOTH);
	fgSizer1->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText1 = new wxStaticText(this, wxID_ANY, wxT("Input"), wxPoint(-1, -1), wxDefaultSize, 0);
	m_staticText1->Wrap(-1);
	fgSizer1->Add(m_staticText1, 0, wxALL, 5);

	m_textCtrl1 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 75), wxTE_MULTILINE);
	fgSizer1->Add(m_textCtrl1, 0, wxALL | wxEXPAND, 5);


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

	wxString m_choice1Choices[] = { wxT("AES"), wxT("3DES"), wxT("XTREA"), wxT("Blowfish"), wxT("Serpent") };
	int m_choice1NChoices = sizeof(m_choice1Choices) / sizeof(wxString);
	m_choice1 = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(100, -1), m_choice1NChoices, m_choice1Choices, wxCB_SORT);
	m_choice1->SetSelection(0);
	fgSizer1->Add(m_choice1, 0, wxALL | wxEXPAND, 5);

	m_staticText3 = new wxStaticText(this, wxID_ANY, wxT("Mode"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText3->Wrap(-1);
	fgSizer1->Add(m_staticText3, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	wxString m_choice2Choices[] = { wxT("CCM"), wxT("EAX"), wxT("CBC"), wxT("ECB"), wxT("EAX") };
	int m_choice2NChoices = sizeof(m_choice2Choices) / sizeof(wxString);
	m_choice2 = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(100, -1), m_choice2NChoices, m_choice2Choices, wxCB_SORT);
	m_choice2->SetSelection(0);
	fgSizer1->Add(m_choice2, 0, wxALL | wxEXPAND, 5);

	m_staticText4 = new wxStaticText(this, wxID_ANY, wxT("Key"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText4->Wrap(-1);
	fgSizer1->Add(m_staticText4, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);

	m_textCtrl2 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	fgSizer1->Add(m_textCtrl2, 0, wxALL | wxEXPAND, 5);


	fgSizer1->Add(0, 0, 1, wxEXPAND, 5);

	m_textCtrl3 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 75), wxTE_MULTILINE | wxTE_READONLY);
	fgSizer1->Add(m_textCtrl3, 0, wxALL | wxEXPAND, 5);


	fgSizer1->Add(0, 0, 1, wxEXPAND, 5);

	wxBoxSizer* bSizer2;
	bSizer2 = new wxBoxSizer(wxHORIZONTAL);

	m_checkBox1 = new wxCheckBox(this, wxID_ANY, wxT("Hexadecimal"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(m_checkBox1, 0, wxALIGN_CENTER_VERTICAL | wxALL, 5);


	bSizer2->Add(0, 0, 1, wxEXPAND, 5);

	m_button1 = new wxButton(this, wxID_ANY, wxT("Encrypt"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(m_button1, 0, wxALL, 2);

	m_button2 = new wxButton(this, wxID_ANY, wxT("Decrypt"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(m_button2, 0, wxALL, 2);


	fgSizer1->Add(bSizer2, 1, wxEXPAND, 5);


	this->SetSizer(fgSizer1);
	this->Layout();
	m_statusBar1 = this->CreateStatusBar(1, 0, wxID_ANY);

	this->Centre(wxBOTH);
}


BlockCipherFrame::~BlockCipherFrame()
{
}
