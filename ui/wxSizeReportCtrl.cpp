#include "wxSizeReportCtrl.h"

void wxSizeReportCtrl::OnPaint(wxPaintEvent& WXUNUSED(evt))
{
	wxPaintDC dc(this);
	wxSize size = GetClientSize();
	wxString s;
	int h, w, height;

	s.Printf(wxT("Size: %d x %d"), size.x, size.y);

	dc.SetFont(*wxNORMAL_FONT);
	dc.GetTextExtent(s, &w, &height);
	height += 3;
	dc.SetBrush(*wxWHITE_BRUSH);
	dc.SetPen(*wxWHITE_PEN);
	dc.DrawRectangle(0, 0, size.x, size.y);
	dc.SetPen(*wxLIGHT_GREY_PEN);
	dc.DrawLine(0, 0, size.x, size.y);
	dc.DrawLine(0, size.y, size.x, 0);
	dc.DrawText(s, (size.x - w) / 2, ((size.y - (height * 5)) / 2));

	if (m_mgr)
	{
		wxAuiPaneInfo pi = m_mgr->GetPane(this);

		s.Printf(wxT("Layer: %d"), pi.dock_layer);
		dc.GetTextExtent(s, &w, &h);
		dc.DrawText(s, (size.x - w) / 2, ((size.y - (height * 5)) / 2) + (height * 1));

		s.Printf(wxT("Dock: %d Row: %d"), pi.dock_direction, pi.dock_row);
		dc.GetTextExtent(s, &w, &h);
		dc.DrawText(s, (size.x - w) / 2, ((size.y - (height * 5)) / 2) + (height * 2));

		s.Printf(wxT("Position: %d"), pi.dock_pos);
		dc.GetTextExtent(s, &w, &h);
		dc.DrawText(s, (size.x - w) / 2, ((size.y - (height * 5)) / 2) + (height * 3));

		s.Printf(wxT("Proportion: %d"), pi.dock_proportion);
		dc.GetTextExtent(s, &w, &h);
		dc.DrawText(s, (size.x - w) / 2, ((size.y - (height * 5)) / 2) + (height * 4));
	}
}

wxBEGIN_EVENT_TABLE(wxSizeReportCtrl, wxControl)
	EVT_PAINT(wxSizeReportCtrl::OnPaint)
	EVT_SIZE(wxSizeReportCtrl::OnSize)
	EVT_ERASE_BACKGROUND(wxSizeReportCtrl::OnEraseBackground)
wxEND_EVENT_TABLE()
