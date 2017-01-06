#pragma once

#include <wx/wxprec.h>

class HashFrame : public wxFrame
{
	enum
	{
		wxID_Hash,
	};

	wxMenuBar* m_menubar;
	wxMenu* m_menu1;
	wxMenu* m_menu31;
	wxStaticText* m_staticText1;
	wxTextCtrl* m_txtInput;
	wxRadioButton* m_radioBtn1;
	wxRadioButton* m_radioBtn2;
	wxRadioButton* m_radioBtn3;
	wxStaticText* m_staticText2;
	wxChoice* m_choice1;
	wxTextCtrl* m_txtOutput;
	wxCheckBox* m_chkHex;
	wxButton* m_btnHash;
	wxStatusBar* m_statusBar;

	void OnHash(wxCommandEvent& evt);

	wxDECLARE_EVENT_TABLE();

public:
	HashFrame(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Hash Calculation"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(533, 290), long style = wxCAPTION | wxCLOSE_BOX | wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP | wxSYSTEM_MENU | wxTAB_TRAVERSAL);
};

