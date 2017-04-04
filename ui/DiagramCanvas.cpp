#include "DiagramCanvas.h"

DiagramCanvas::DiagramCanvas(wxSFDiagramManager *manager, wxWindow *parent, wxWindowID id)
	: wxSFShapeCanvas(manager, parent, id, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL)
{
	//AddStyle(sfsGRID_SHOW);
	//SetGridLineMult(5);
	//SetGridColour(*wxLIGHT_GREY);
	SetCanvasColour(*wxWHITE);
}
