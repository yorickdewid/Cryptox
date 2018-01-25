#include "DiagramCanvas.h"

DiagramCanvas::DiagramCanvas(wxSFDiagramManager *manager, wxWindow *parent, wxWindowID id)
	: wxSFShapeCanvas(manager, parent, id, wxDefaultPosition, wxDefaultSize, wxHSCROLL | wxVSCROLL)
{
	//AddStyle(sfsGRID_SHOW);
	//SetGridLineMult(5);
	//SetGridColour(*wxLIGHT_GREY);
	SetCanvasColour(*wxWHITE);

	auto pShape = GetDiagramManager()->AddShape(CLASSINFO(wxSFTextShape), wxPoint(10, 10), sfDONT_SAVE_STATE);
	if (pShape) {
		((wxSFTextShape*)pShape)->SetText("Name: SHA-CUSTOM\nVersion: 1");

		// set alignment
		pShape->SetVAlign(wxSFShapeBase::valignTOP);
		pShape->SetHAlign(wxSFShapeBase::halignCENTER);
		pShape->SetVBorder(10);
		pShape->SetStyle(wxSFShapeBase::STYLE::sfsPARENT_CHANGE);
		pShape->Scale(0.9, 0.9);
	}

	pShape = GetDiagramManager()->AddShape(CLASSINFO(wxSFRectShape), wxPoint(100, 100), sfDONT_SAVE_STATE);
	if (pShape) {
		// set shape policy
		pShape->AcceptChild(wxT("All"));
		((wxSFRectShape*)pShape)->SetFill(*wxCYAN_BRUSH);

		pShape->AcceptConnection(wxT("All"));
		pShape->AcceptSrcNeighbour(wxT("All"));
		pShape->AcceptTrgNeighbour(wxT("All"));

		// child shapes can be locked accordingly to their parent's origin if the parent is resized
		//pShape->AddStyle( wxSFShapeBase::sfsLOCK_CHILDREN );

		// shapes can have fixed connection points defined in the following way:
		pShape->AddConnectionPoint(wxSFConnectionPoint::cpCENTERLEFT);
		pShape->AddConnectionPoint(wxSFConnectionPoint::cpCENTERRIGHT);
		// user can define also any number of CUSTOM connection points placed relatively to the 
		// parent shape's bounding box ("25, 50" here means 25% of width and 50% of height):
		pShape->AddConnectionPoint(wxRealPoint(25, 50));
		pShape->AddConnectionPoint(wxRealPoint(75, 50));
		// in this case the line connection can be assigned to the one of the defined
		// fixed connection points only.
	}
}
