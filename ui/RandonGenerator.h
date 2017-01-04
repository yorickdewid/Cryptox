#pragma once

#include <wx/wxprec.h>
#include <wx/statline.h>

class RandonGenerator : public wxDialog
{
private:

protected:
	wxStaticText* m_staticText10;
	wxTextCtrl* m_textCtrl6;
	wxStaticLine* m_staticline1;
	wxTextCtrl* m_textCtrl7;
	wxButton* m_button11;
	wxButton* m_button12;

public:

	RandonGenerator(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Random data generator"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(457, 252), long style = wxDEFAULT_DIALOG_STYLE);
	~RandonGenerator();

};

