#pragma once

#include <wx/wxprec.h>
#include <wx/statline.h>

class RandonGenerator : public wxDialog
{
private:

protected:
	wxStaticText* m_staticText9;
	wxChoice* m_choice4;
	wxStaticText* m_staticText10;
	wxTextCtrl* m_textCtrl6;
	wxStaticLine* m_staticline1;
	wxTextCtrl* m_textCtrl7;
	wxButton* m_button11;
	wxStdDialogButtonSizer* m_sdbSizer3;
	wxButton* m_sdbSizer3Cancel;

public:

	RandonGenerator(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Random data generator"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(457, 235), long style = wxDEFAULT_DIALOG_STYLE);
	~RandonGenerator();
};

