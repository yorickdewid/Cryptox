#pragma once

#include <wx/wxprec.h>

class DataViewer : public wxDialog
{
	wxStaticText* m_staticText19;
	wxChoice* m_choice7;
	wxButton* m_btnClipboard;
	wxButton* m_btnSave;
	wxTextCtrl* m_txtData;

	wxDECLARE_EVENT_TABLE();

public:
	DataViewer(wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Data Viewer"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize(640, 360), long style = wxDEFAULT_DIALOG_STYLE | wxMAXIMIZE_BOX | wxRESIZE_BORDER);
};

