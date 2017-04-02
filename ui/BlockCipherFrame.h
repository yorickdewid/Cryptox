#pragma once

#include "ExUIFrame.h"

#include <wx/wxprec.h>

class BlockCipherFrame : public BaseBlockCipherFrame
{
	void OnEncrypt(wxCommandEvent& evt);

	wxDECLARE_EVENT_TABLE();

public:
	BlockCipherFrame(wxWindow *parent,
					 wxWindowID id = wxID_ANY,
					 const wxSize& size = wxSize(520, 600))
		: BaseBlockCipherFrame(parent,
							   id,
							   wxT("Block Cipher Encryption"),
							   wxDefaultPosition,
							   size,
							   wxCAPTION | wxCLOSE_BOX | wxSYSTEM_MENU | wxTAB_TRAVERSAL)
	{
	}
};

