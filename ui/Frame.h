#pragma once

#include "wxSizeReportCtrl.h"

#include <wx/wxprec.h>
#include <wx/grid.h>
#include <wx/treectrl.h>
#include <wx/wxhtml.h>
#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/manager.h>
#include <wx/aui/aui.h>

class Frame : public wxFrame
{
	enum
	{
		ID_CreateTree = wxID_HIGHEST + 1,
		ID_CreateGrid,
		ID_CreateText,
		ID_CreateHTML,
		ID_CreateNotebook,
		ID_CreateSizeReport,
		ID_CreatePerspective,
		ID_CopyPerspectiveCode,
		ID_AllowFloating,
		ID_AllowActivePane,
		ID_TransparentHint,
		ID_VenetianBlindsHint,
		ID_RectangleHint,
		ID_NoHint,
		ID_HintFade,
		ID_NoVenetianFade,
		ID_TransparentDrag,
		ID_NoGradient,
		ID_VerticalGradient,
		ID_HorizontalGradient,
		ID_LiveUpdate,
		ID_AllowToolbarResizing,
		ID_Settings,
		ID_RandomGeneratorWindow,
		ID_CustomizeToolbar,
		ID_DropDownToolbarItem,

		ID_SampleItem,

		ID_FirstPerspective = ID_CreatePerspective + 1000
	};

public:
	Frame(wxWindow* parent,
		wxWindowID id,
		const wxString& title,
		const wxPoint& pos = wxDefaultPosition,
		const wxSize& size = wxDefaultSize,
		long style = wxDEFAULT_FRAME_STYLE | wxSUNKEN_BORDER);

	~Frame();

	wxAuiDockArt* GetDockArt();
	void DoUpdate();

private:
	wxMenuBar *CreateMenuBar();
	wxTextCtrl *CreateTextCtrl(const wxString& text = wxEmptyString);
	wxGrid *CreateGrid();
	wxTreeCtrl *CreateTreeCtrl();
	wxPropertyGridManager *CreatePropCtrl();
	wxSizeReportCtrl *CreateSizeReportCtrl(int width = 80, int height = 80);
	wxPoint GetStartPosition();
	wxHtmlWindow *CreateHTMLCtrl(wxWindow *parent = NULL);
	wxAuiNotebook *CreateNotebook();
	void CreatePrimitiveFrame();

	wxString GetIntroText();

private:
	void OnCreateTree(wxCommandEvent& evt);
	void OnCreateGrid(wxCommandEvent& evt);
	void OnCreateHTML(wxCommandEvent& evt);
	void OnCreateNotebook(wxCommandEvent& evt);
	void OnCreateText(wxCommandEvent& evt);
	void OnCreateSizeReport(wxCommandEvent& evt);
	void OnDropDownToolbarItem(wxAuiToolBarEvent& evt);
	void OnCreatePerspective(wxCommandEvent& evt);
	void OnCopyPerspectiveCode(wxCommandEvent& evt);
	void OnRestorePerspective(wxCommandEvent& evt);
	void OnSettings(wxCommandEvent& evt);
	void OnCustomizeToolbar(wxCommandEvent& evt);
	void OnAllowNotebookDnD(wxAuiNotebookEvent& evt);
	void OnNotebookPageClose(wxAuiNotebookEvent& evt);
	void OnNotebookPageClosed(wxAuiNotebookEvent& evt);
	void OnExit(wxCommandEvent& evt);
	void OnAbout(wxCommandEvent& evt);
	void OnItemMenu(wxTreeEvent& evt);
	void OnConsoleEnter(wxCommandEvent& evt);
	void OnMenuPrimitiveRun(wxCommandEvent& WXUNUSED(evt))
	{
		CreatePrimitiveFrame();
	}
	void OnTreeDoubleClick(wxTreeEvent& evt)
	{
		wxTreeCtrl *tree = static_cast<wxTreeCtrl *>(evt.GetEventObject());

		// Skip parents
		wxTreeItemId itemId = evt.GetItem();
		if (tree->ItemHasChildren(itemId))
			return;

		CreatePrimitiveFrame();
	}

	void OnGradient(wxCommandEvent& evt);
	void OnToolbarResizing(wxCommandEvent& evt);
	void OnManagerFlag(wxCommandEvent& evt);
	void OnUpdateUI(wxUpdateUIEvent& evt);

	void OnPaneClose(wxAuiManagerEvent& evt);

private:
	wxAuiManager m_mgr;
	wxArrayString m_perspectives;
	wxMenu *m_perspectives_menu;
	wxTextCtrl *m_output;
	long m_notebook_style;
	long m_notebook_theme;

	wxDECLARE_EVENT_TABLE();
};
