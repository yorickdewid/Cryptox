///////////////////////////////////////////////////////////////////////////////
// Name:        Main.cpp
// Purpose:     Bootstrap core
// Author:      Quenza Inc.
// Modified by:	Yorick de Wid
// Created:     2017-01-01
// Copyright:   (C) Copyright 2017, Quenza Inc, All Rights Reserved.
// Licence:     GPL, Version 3
///////////////////////////////////////////////////////////////////////////////

#include "Frame.h"

#include <wx/wxprec.h>
#include <wx/app.h>

#define APPNAME "Cryptox IDE 0.2.4"

class CyApp : public wxApp
{
public:
    bool OnInit()
	{
		if (!wxApp::OnInit())
			return false;

		SetVendorName(wxT("Quenza Inc."));
		SetAppName(wxT("Cryptox"));
		SetAppDisplayName(wxT("Cryptox Crypto IDE"));

		wxConfigBase *config = wxConfigBase::Get();
		config->SetRecordDefaults();

		wxFrame *frame = new Frame(nullptr,
			wxID_ANY,
			wxT(APPNAME),
			wxDefaultPosition,
			wxSize(1200, 750));
		frame->Show();

		return true;
	}
};

wxDECLARE_APP(CyApp);
wxIMPLEMENT_APP(CyApp);
