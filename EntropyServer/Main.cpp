///////////////////////////////////////////////////////////////////////////////
// Name:        Main.cpp
// Purpose:     Entropy Daemon
// Author:      Quenza Inc.
// Modified by:	Yorick de Wid
// Created:     2017-03-25
// Copyright:   (C) Copyright 2017, Quenza Inc, All Rights Reserved.
// Licence:     GPL, Version 3
///////////////////////////////////////////////////////////////////////////////

#include <wx/wx.h>
#include <wx/socket.h>
#include <wx/event.h>
#include <wx/list.h>
#include <wx/cmdline.h>
#include <wx/datetime.h>
#include <wx/timer.h>
#include <wx/thread.h>

class Daemon : public wxApp {
	//wxDECLARE_EVENT_TABLE();
public:
	Daemon() {}
	~Daemon() {}

	virtual bool OnInit() wxOVERRIDE {
		if (!wxApp::OnInit())
			return false;

		wxLogMessage("I am here");

		return true;
	}

	virtual int OnExit() wxOVERRIDE {
		wxLogMessage("Bye");

		return 0;
	}

	void OnInitCmdLine(wxCmdLineParser& parser)
	{
		wxApp::OnInitCmdLine(parser);
		parser.SetLogo("Cryptox Entropy Server - Copyright (c) 2017");
		parser.AddSwitch("a", "auth", "request authentication");
		parser.AddOption("p", "port", "listen on given port (default 52630)", wxCMD_LINE_VAL_NUMBER);
		parser.AddSwitch("v", "version", "show version and exit");
	}

	bool OnCmdLineParsed(wxCmdLineParser& pParser)
	{
		if (pParser.Found("verbose"))
		{
			wxLog::AddTraceMask("wxSocket");
			wxLog::AddTraceMask("epolldispatcher");
			wxLog::AddTraceMask("selectdispatcher");
			wxLog::AddTraceMask("thread");
			wxLog::AddTraceMask("events");
			wxLog::AddTraceMask("timer");
		}
#if 0
		if (pParser.Found("m", &m_maxConnections))
		{
			wxLogMessage("%ld connection(s) to exit", m_maxConnections);
		}

		long port;
		if (pParser.Found("p", &port))
		{
			if (port <= 0 || port > USHRT_MAX)
			{
				wxLogError("Invalid port number %ld, must be in 0..%u range.",
					port, USHRT_MAX);
				return false;
			}

			m_port = static_cast<unsigned short>(port);
			wxLogMessage("Will listen on port %u", m_port);
		}

		if (pParser.Found("t"))
			m_workMode = THREADS;
		else if (pParser.Found("e"))
			m_workMode = EVENTS;
		else
			m_workMode = MIXED;
#endif
		return wxApp::OnCmdLineParsed(pParser);
	}
};

wxDECLARE_APP(Daemon);
wxIMPLEMENT_APP_CONSOLE(Daemon);
