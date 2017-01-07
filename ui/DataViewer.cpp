#include "DataViewer.h"

DataViewer::DataViewer(wxWindow* parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style)
	: wxDialog(parent, id, title, pos, size, style)
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer* bSizer7;
	bSizer7 = new wxBoxSizer(wxVERTICAL);

	wxBoxSizer* bSizer9;
	bSizer9 = new wxBoxSizer(wxHORIZONTAL);

	m_staticText19 = new wxStaticText(this, wxID_ANY, wxT("Show as"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText19->Wrap(-1);
	bSizer9->Add(m_staticText19, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxTOP, 5);

	wxString m_choice7Choices[] = { wxT("Hexdump"), wxT("Text"), wxT("HTML"), wxT("Image") };
	int m_choice7NChoices = sizeof(m_choice7Choices) / sizeof(wxString);
	m_choice7 = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, m_choice7NChoices, m_choice7Choices, 0);
	m_choice7->SetSelection(0);
	bSizer9->Add(m_choice7, 0, wxALIGN_BOTTOM | wxLEFT | wxTOP, 5);


	bSizer9->Add(0, 0, 1, wxEXPAND, 5);

	m_btnClipboard = new wxButton(this, wxID_ANY, wxT("Copy to Clipboard"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer9->Add(m_btnClipboard, 0, wxRIGHT | wxTOP, 5);

	m_btnSave = new wxButton(this, wxID_ANY, wxT("Save as"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer9->Add(m_btnSave, 0, wxRIGHT | wxTOP, 5);


	bSizer7->Add(bSizer9, 0, wxEXPAND, 5);

	m_txtData = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxSTATIC_BORDER);
	m_txtData->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));
	m_txtData->SetValue("000 : 2D2D2D2D 2D424547 494E2052 53412050  -----BEGIN RSA P\n");

	bSizer7->Add(m_txtData, 1, wxALL | wxEXPAND, 5);


	this->SetSizer(bSizer7);
	this->Layout();

	this->Centre(wxBOTH);
}

wxBEGIN_EVENT_TABLE(DataViewer, wxDialog)
wxEND_EVENT_TABLE()
