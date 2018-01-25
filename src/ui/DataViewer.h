#pragma once

#include "ExUIFrame.h"

#include <wx/wxprec.h>

class DataViewer : public BaseDataViewer
{
	wxDECLARE_EVENT_TABLE();

public:
	DataViewer(wxWindow *parent,
			   wxWindowID id = wxID_ANY,
			   const wxSize& size = wxSize(640, 360))
		: BaseDataViewer(parent,
						 id,
						 wxT("Data Viewer"),
						 wxDefaultPosition,
						 size,
						 wxDEFAULT_DIALOG_STYLE | wxMAXIMIZE_BOX | wxRESIZE_BORDER)
	{
	}
};

