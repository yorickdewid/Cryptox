#pragma once

#include "ExUIFrame.h"

#include <wx/wxprec.h>

class HashFrame : public BaseHashFrame
{
	void OnHash(wxCommandEvent& evt);

	wxDECLARE_EVENT_TABLE();

public:
	HashFrame(wxWindow *parent,
			  wxWindowID id = wxID_ANY,
			  const wxSize& size = wxSize(533, 290))
		: BaseHashFrame(parent,
						id,
						wxT("Hash Calculation"),
						wxDefaultPosition,
						size,
						wxCAPTION | wxCLOSE_BOX | wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP | wxSYSTEM_MENU | wxTAB_TRAVERSAL)
	{
	}
};

