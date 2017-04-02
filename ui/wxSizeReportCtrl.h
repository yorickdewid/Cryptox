#pragma once

#include <wx/wxprec.h>
#include <wx/aui/aui.h>

class wxSizeReportCtrl : public wxControl
{
public:
	wxSizeReportCtrl(wxWindow *parent, wxWindowID id = wxID_ANY,
					 const wxPoint& pos = wxDefaultPosition,
					 const wxSize& size = wxDefaultSize,
					 wxAuiManager *mgr = NULL)
		: wxControl(parent, id, pos, size, wxNO_BORDER)
	{
		m_mgr = mgr;
	}

private:
	void OnPaint(wxPaintEvent& evt);

	void OnEraseBackground(wxEraseEvent& WXUNUSED(evt))
	{
		// intentionally empty
	}

	void OnSize(wxSizeEvent& WXUNUSED(evt))
	{
		Refresh();
	}

private:
	wxAuiManager *m_mgr;

	wxDECLARE_EVENT_TABLE();
};
