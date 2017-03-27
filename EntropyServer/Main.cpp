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

#include "ConnectionHandler.h"

class Daemon : public wxApp
{
	wxDECLARE_EVENT_TABLE();

	unsigned short m_port;

	wxSocketServer *m_listeningSocket = nullptr;

public:
	Daemon() : m_port{ 52630 } {}

	virtual bool OnInit() wxOVERRIDE;
	virtual int OnExit() wxOVERRIDE;

	void OnInitCmdLine(wxCmdLineParser& parser);
	bool OnCmdLineParsed(wxCmdLineParser& pParser);

	void OnSocketEvent(wxSocketEvent& evt);
};

wxDECLARE_APP(Daemon);
wxIMPLEMENT_APP_CONSOLE(Daemon);

wxBEGIN_EVENT_TABLE(Daemon, wxEvtHandler)
	EVT_SOCKET(wxID_ANY, Daemon::OnSocketEvent)
	//EVT_WORKER(Daemon::OnWorkerEvent)
wxEND_EVENT_TABLE()


void Daemon::OnInitCmdLine(wxCmdLineParser& parser)
{
	wxApp::OnInitCmdLine(parser);
	parser.SetLogo("Cryptox Entropy Server - Copyright (c) 2017");
	parser.AddSwitch("a", "auth", "request authentication");
	parser.AddOption("p", "port", "listen on given port (default 52630)", wxCMD_LINE_VAL_NUMBER);
	parser.AddSwitch("v", "version", "show version and exit");
}


bool Daemon::OnCmdLineParsed(wxCmdLineParser& parser)
{
	wxLogMessage("Parsing arguments");

	if (parser.Found("verbose")) {
		wxLog::AddTraceMask("wxSocket");
		wxLog::AddTraceMask("epolldispatcher");
		wxLog::AddTraceMask("selectdispatcher");
		wxLog::AddTraceMask("thread");
		wxLog::AddTraceMask("events");
		wxLog::AddTraceMask("timer");
	}

	return wxApp::OnCmdLineParsed(parser);
}


bool Daemon::OnInit()
{
	wxLogMessage("Initializing application");
	if (!wxApp::OnInit())
		return false;

	wxIPV4address addr;
	addr.Service(m_port);

	wxLogMessage("Accepting connections...");
	m_listeningSocket = new wxSocketServer(addr, wxSOCKET_NOWAIT | wxSOCKET_REUSEADDR);
	m_listeningSocket->SetEventHandler(*this);
	m_listeningSocket->SetNotify(wxSOCKET_CONNECTION_FLAG);
	m_listeningSocket->Notify(true);

	if (!m_listeningSocket->IsOk())
	{
		wxLogError("Cannot bind listening socket");
		return false;
	}

	return true;
}


int Daemon::OnExit() {
	wxLogMessage("Shutdown server");

	m_listeningSocket->Destroy();
	return 0;
}


void Daemon::OnSocketEvent(wxSocketEvent& evt)
{
	switch (evt.GetSocketEvent())
	{
	case wxSOCKET_INPUT:
		wxLogError("Unexpected wxSOCKET_INPUT in wxSocketServer");
		break;
	case wxSOCKET_OUTPUT:
		wxLogError("Unexpected wxSOCKET_OUTPUT in wxSocketServer");
		break;
	case wxSOCKET_CONNECTION:
	{
		auto sock = m_listeningSocket->Accept();
		wxIPV4address addr;
		if (!sock->GetPeer(addr))
		{
			wxLogError("Server: cannot get peer info");
		}
		else {
			wxLogMessage("Got connection from %s:%d", addr.IPAddress().c_str(), addr.Service());
		}

		//ConnectionHandler::ConnectionHandler(wxSocketBase *socket)
		ConnectionHandler *w = new ConnectionHandler(sock);
#if 0
		bool createThread;

		if (m_workMode != MIXED)
			createThread = m_workMode == THREADS;
		else
			createThread = (wxDateTime::Now().GetSecond()) % 2 == 0;

		if (createThread)
		{
			ThreadWorker* c = new ThreadWorker(sock);
			if (c->Create() == wxTHREAD_NO_ERROR)
			{
				m_threadWorkers.Append(c);
				if (m_threadWorkers.GetCount() > m_maxThreadWorkers)
					m_maxThreadWorkers++;
				m_threadWorkersCreated++;
				c->Run();
			}
			else
			{
				wxLogError("Server: cannot create next thread (current threads: %d", m_threadWorkers.size());
			};
		}
		else
		{
			EventWorker* w = new EventWorker(sock);
			m_eventWorkers.Append(w);
			if (m_eventWorkers.GetCount() > m_maxEventWorkers)
				m_maxEventWorkers++;
			m_eventWorkersCreated++;
		}
#endif
	}
	break;
	case wxSOCKET_LOST:
		wxLogError("Unexpected wxSOCKET_LOST in wxSocketServer");
		break;
	}
}
