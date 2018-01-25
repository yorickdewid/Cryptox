#pragma once

#include <wx/wxsf/wxShapeFramework.h>

class DiagramCanvas : public wxSFShapeCanvas
{

public:
	DiagramCanvas(wxSFDiagramManager *manager, wxWindow *parent, wxWindowID id);
};

