#include "RandonGenerator.h"

RandonGenerator::RandonGenerator(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
	: wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer6;
	bSizer6 = new wxBoxSizer(wxVERTICAL);

	wxFlexGridSizer* fgSizer4;
	fgSizer4 = new wxFlexGridSizer(0, 2, 0, 25);
	fgSizer4->SetFlexibleDirection(wxBOTH);
	fgSizer4->SetNonFlexibleGrowMode(wxFLEX_GROWMODE_SPECIFIED);

	m_staticText9 = new wxStaticText(this, wxID_ANY, wxT("Algorithm"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText9->Wrap(-1);
	fgSizer4->Add(m_staticText9, 0, wxALL, 5);

	wxString m_choice4Choices[] = { wxT("MT19937"), wxT("MT19937ar") };
	int m_choice4NChoices = sizeof(m_choice4Choices) / sizeof(wxString);
	m_choice4 = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice4NChoices, m_choice4Choices, 0);
	m_choice4->SetSelection(0);
	fgSizer4->Add(m_choice4, 0, wxALL, 5);

	m_staticText10 = new wxStaticText(this, wxID_ANY, wxT("Seed"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText10->Wrap(-1);
	fgSizer4->Add(m_staticText10, 0, wxALL, 5);

	m_textCtrl6 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxTE_MULTILINE);
	fgSizer4->Add(m_textCtrl6, 0, wxALL, 5);


	bSizer6->Add(fgSizer4, 1, wxEXPAND, 5);

	m_staticline1 = new wxStaticLine(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxLI_HORIZONTAL);
	bSizer6->Add(m_staticline1, 0, wxEXPAND | wxALL, 5);

	wxBoxSizer* bSizer12;
	bSizer12 = new wxBoxSizer(wxVERTICAL);

	m_textCtrl7 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 60), wxTE_MULTILINE | wxTE_READONLY);
	bSizer12->Add(m_textCtrl7, 0, wxALL | wxEXPAND, 5);

	wxBoxSizer* bSizer13;
	bSizer13 = new wxBoxSizer(wxHORIZONTAL);

	m_button11 = new wxButton(this, wxID_ANY, wxT("Generate"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer13->Add(m_button11, 0, wxALIGN_BOTTOM | wxALL, 5);

	m_sdbSizer3 = new wxStdDialogButtonSizer();
	m_sdbSizer3Cancel = new wxButton(this, wxID_CANCEL);
	m_sdbSizer3->AddButton(m_sdbSizer3Cancel);
	m_sdbSizer3->Realize();

	bSizer13->Add(m_sdbSizer3, 1, wxALIGN_CENTER, 5);


	bSizer12->Add(bSizer13, 1, wxEXPAND, 5);


	bSizer6->Add(bSizer12, 1, wxEXPAND, 5);


	this->SetSizer(bSizer6);
	this->Layout();

	this->Centre(wxBOTH);
}


RandonGenerator::~RandonGenerator()
{
}
