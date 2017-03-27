#include "ConnectionHandler.h"


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
	unsigned char m_buffer[sizeof(EntropyProtocol)];

	if (!m_inbuf) {
		// Read message header
		do {
			m_socket->Read(m_buffer, sizeof(EntropyProtocol));
			if (m_socket->Error()) {
				if (m_socket->LastError() != wxSOCKET_WOULDBLOCK) {
					wxLogError("Socket error %d", m_socket->LastError());
					//LogWorker(wxString::Format("Read error (%d): %s", m_socket->LastError(), GetSocketErrorMsg(m_socket->LastError())), wxLOG_Error);
					m_socket->Close();
					return;
				}
			}

			wxLogMessage("We'll see if this is an valid headerst");

			/*m_infill += m_socket->LastCount();
			if (m_infill == 2) {
				unsigned char chunks = m_signature[1];
				unsigned char type = m_signature[0];
				if (type == 0xCE)
				{
					//LogWorker("This server does not support test2 from GUI client", wxLOG_Error);
					m_written = -1; //wxSOCKET_LOST will interpret this as failure
					m_socket->Close();
				}
				else if (type == 0xBE || type == 0xDE)
				{
					m_size = chunks * (type == 0xBE ? 1 : 1024);
					m_inbuf = new char[m_size];
					m_outbuf = new char[m_size];
					m_infill = 0;
					m_outfill = 0;
					m_written = 0;
					//LogWorker(wxString::Format("Message signature: len: %d, type: %s, size: %d (bytes)", chunks, type == 0xBE ? "b" : "kB", m_size));
					break;
				}
				else
				{
					//LogWorker(wxString::Format("Unknown test type %x", type));
					m_socket->Close();
				}
			}*/

		//} while (!m_socket->Error() && (2 - m_infill != 0));
		} while (!m_socket->Error());
	}
#if 0
	if (!m_inbuf == NULL)
		return;

	//read message data
	do
	{
		if (m_size == m_infill)
		{
			m_signature[0] = m_signature[1] = 0x0;
			wxDELETEA(m_inbuf);
			m_infill = 0;
			return;
		}
		m_socket->Read(m_inbuf + m_infill, m_size - m_infill);
		if (m_socket->Error())
		{
			if (m_socket->LastError() != wxSOCKET_WOULDBLOCK)
			{
				//LogWorker(wxString::Format("Read error (%d): %s",
				//	m_socket->LastError(),
				//	GetSocketErrorMsg(m_socket->LastError())),
				//	wxLOG_Error);

				m_socket->Close();
			}
		}
		else
		{
			memcpy(m_outbuf + m_outfill, m_inbuf + m_infill, m_socket->LastCount());
			m_infill += m_socket->LastCount();
			m_outfill += m_socket->LastCount();
			DoWrite();
		}
	} while (!m_socket->Error());
#endif
}


void ConnectionHandler::OnSocketEvent(wxSocketEvent& pEvent)
{
	switch (pEvent.GetSocketEvent())
	{
	case wxSOCKET_INPUT:
		DoRead();
		break;

	case wxSOCKET_OUTPUT:
		if (m_outbuf)
			DoWrite();
		break;

	case wxSOCKET_CONNECTION:
		//LogWorker("Unexpected wxSOCKET_CONNECTION in EventWorker", wxLOG_Error);
		wxLogError("Unexpected wxSOCKET_CONNECTION in EventWorker", wxLOG_Error);
		break;

	case wxSOCKET_LOST:
	{
		//LogWorker("Connection lost");
		wxLogError("Connection lost");
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

wxBEGIN_EVENT_TABLE(ConnectionHandler, wxEvtHandler)
	EVT_SOCKET(wxID_ANY, ConnectionHandler::OnSocketEvent)
wxEND_EVENT_TABLE()
