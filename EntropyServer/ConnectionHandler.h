#pragma once

#include <wx/wx.h>
#include <wx/event.h>
#include <wx/socket.h>

#include "EntropyProtocol.h"

class ConnectionHandler : public wxEvtHandler
{
	wxDECLARE_EVENT_TABLE();

	wxSocketBase *m_socket;
	wxIPV4address m_peer;

	char *m_inbuf;
	int m_infill;
	int m_size;
	char *m_outbuf;
	int m_outfill;
	int m_written;

	void OnSocketEvent(wxSocketEvent& pEvent);
	void DoWrite();
	void DoRead();

public:
	ConnectionHandler(wxSocketBase* socket);
	virtual ~ConnectionHandler();
};
