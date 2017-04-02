#include "HashFrame.h"

void HashFrame::OnHash(wxCommandEvent& WXUNUSED(evt))
{
	if (m_txtInput->IsEmpty()) {
		m_statusBar->SetStatusText(wxT("Input is empty"));
		return;
	}

	std::string encoded("Magic hash");

	m_statusBar->SetStatusText(wxT("Calculating hash..."));

	// TODO

	m_txtOutput->SetValue(encoded);
	m_statusBar->SetStatusText(wxT("Done"));
}

wxBEGIN_EVENT_TABLE(HashFrame, BaseHashFrame)
EVT_BUTTON(BaseHashFrame::wxID_Hash, HashFrame::OnHash)
wxEND_EVENT_TABLE()
