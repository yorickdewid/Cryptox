#include "ConnectionHandler.h"
#include "CryptRand.h"

ConnectionHandler::ConnectionHandler(wxSocketBase *socket)
	: m_socket{socket}
	, m_inbuf{NULL}
	, m_infill{0}
	, m_outbuf{NULL}
	, m_outfill{0}
{
	m_socket->SetNotify(wxSOCKET_LOST_FLAG | wxSOCKET_INPUT_FLAG | wxSOCKET_OUTPUT_FLAG);
	m_socket->Notify(true);
	m_socket->SetEventHandler(*this);
	m_socket->SetFlags(wxSOCKET_NOWAIT);
	m_socket->GetPeer(m_peer);
}


ConnectionHandler::~ConnectionHandler()
{
	m_socket->Destroy();
	delete[] m_inbuf;
	delete[] m_outbuf;
}


void ConnectionHandler::DoRead()
{
	EntropyRequest protocol;
	unsigned char m_buffer[sizeof(EntropyRequest)];

	m_socket->Read(m_buffer, sizeof(EntropyRequest));
	if (m_socket->Error()) {
		if (m_socket->LastError() != wxSOCKET_WOULDBLOCK && m_socket->LastError() != wxSOCKET_IOERR) {
			this->Log(wxString::Format("Socket error %d", m_socket->LastError()));
			m_socket->Close();
			return;
		}
	}

	::memcpy(&protocol, m_buffer, sizeof(EntropyRequest));
	if (wxStrcmp(protocol.bannerString, protobanner)) {
		this->Log("Malformed request");
		m_socket->Close();
		return;
	}

	this->ParseQuery(protocol);
}


void ConnectionHandler::OnSocketEvent(wxSocketEvent& pEvent)
{
	switch (pEvent.GetSocketEvent()) {
		case wxSOCKET_INPUT:
			DoRead();
			break;

		//TODO: remove?
		case wxSOCKET_OUTPUT:
			if (m_outbuf)
				DoWrite();
			break;

		case wxSOCKET_CONNECTION:
			this->Log("Unexpected wxSOCKET_CONNECTION in EventWorker");
			break;

		case wxSOCKET_LOST: {
			this->Log("Connection lost");
			//WorkerEvent e(this);
			//e.m_workerFailed = m_written != m_size;
			//wxGetApp().AddPendingEvent(e);
		}
		break;
	}
}


void ConnectionHandler::DoWrite()
{
	do {
		if (m_written == m_size) {
			wxDELETEA(m_outbuf);
			m_outfill = 0;
			//LogWorker("All data written");
			return;
		}
		if (m_outfill - m_written == 0)
		{
			return;
		}
		m_socket->Write(m_outbuf + m_written, m_outfill - m_written);
		if (m_socket->Error())
		{
			if (m_socket->LastError() != wxSOCKET_WOULDBLOCK)
			{
				//LogWorker(
				//	wxString::Format("Write error (%d): %s",
				//		m_socket->LastError(),
				//		GetSocketErrorMsg(m_socket->LastError())
				//	)
				//	, wxLOG_Error
				//);
				m_socket->Close();
			}
			else
			{
				//LogWorker("Write would block, waiting for OUTPUT event");
			}
		}
		else
		{
			memmove(m_outbuf, m_outbuf + m_socket->LastCount(), m_outfill - m_socket->LastCount());
			m_written += m_socket->LastCount();
		}
		//LogWorker(wxString::Format("Written %d of %d bytes, todo %d",
		//	m_written, m_size, m_size - m_written));
	} while (!m_socket->Error());
}


void ConnectionHandler::ParseQuery(EntropyRequest& proto)
{
	this->Log(wxString::Format("Request client version: %d", proto.protoVersion));
	this->Log(wxString::Format("Request block size: %d", proto.requestSize));

	if (proto.flag.stream)
		this->Log("Rquested stream");

	m_outbuf = CryptRand::GenerateRandomBlock(32);
}


wxBEGIN_EVENT_TABLE(ConnectionHandler, wxEvtHandler)
	EVT_SOCKET(wxID_ANY, ConnectionHandler::OnSocketEvent)
wxEND_EVENT_TABLE()
