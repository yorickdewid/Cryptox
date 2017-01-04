#include "Frame.h"
#include "SettingsPanel.h"
#include "RandonGenerator.h"

#include <wx/artprov.h>
#include <wx/clipbrd.h>
#include <wx/spinctrl.h>

Frame::Frame(wxWindow* parent,
	wxWindowID id,
	const wxString& title,
	const wxPoint& pos,
	const wxSize& size,
	long style)
	: wxFrame(parent, id, title, pos, size, style)
{
	// AUI manage this frame
	m_mgr.SetManagedWindow(this);

	// Set frame min size and icon
	SetIcon(wxIcon(wxString("cryptox.ico"), wxBITMAP_TYPE_ICO));
	SetMinSize(wxSize(800, 700));

	// Set border size
	GetDockArt()->SetMetric(wxAUI_DOCKART_PANE_BORDER_SIZE, 0);

	// Set up default notebook style
	m_notebook_style = wxAUI_NB_DEFAULT_STYLE | wxAUI_NB_WINDOWLIST_BUTTON | wxAUI_NB_TAB_EXTERNAL_MOVE | wxNO_BORDER;
	m_notebook_theme = 0;

	// Create menu
	CreateMenu();
	SetMenuBar(CreateMenuBar());

	// Create statusbar
	CreateStatusBar();
	SetStatusText(wxT("Ready"));

	// prepare a few custom overflow elements for the toolbars' overflow buttons
	wxAuiToolBarItemArray prepend_items;
	wxAuiToolBarItemArray append_items;
	wxAuiToolBarItem item;
	item.SetKind(wxITEM_SEPARATOR);
	append_items.Add(item);
	item.SetKind(wxITEM_NORMAL);
	item.SetId(ID_CustomizeToolbar);
	item.SetLabel(wxT("Customize..."));
	append_items.Add(item);


	// create some toolbars
	wxAuiToolBar *tb2 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxAUI_TB_HORIZONTAL);
	tb2->SetToolBitmapSize(wxSize(16, 16));

	wxBitmap tb2_bmp1 = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_OTHER, wxSize(16, 16));
	tb2->AddTool(ID_SampleItem + 6, wxT("Disabled"), tb2_bmp1);
	tb2->AddTool(ID_SampleItem + 7, wxT("Test"), tb2_bmp1);
	tb2->AddTool(ID_SampleItem + 8, wxT("Test"), tb2_bmp1);
	tb2->AddTool(ID_SampleItem + 9, wxT("Test"), tb2_bmp1);
	tb2->AddSeparator();
	tb2->AddTool(ID_SampleItem + 10, wxT("Test"), tb2_bmp1);
	tb2->AddTool(ID_SampleItem + 11, wxT("Test"), tb2_bmp1);
	tb2->AddSeparator();
	tb2->AddTool(ID_SampleItem + 12, wxT("Test"), tb2_bmp1);
	tb2->AddTool(ID_SampleItem + 13, wxT("Test"), tb2_bmp1);
	tb2->AddTool(ID_SampleItem + 14, wxT("Test"), tb2_bmp1);
	tb2->AddTool(ID_SampleItem + 15, wxT("Test"), tb2_bmp1);
	//tb2->SetCustomOverflowItems(prepend_items, append_items);
	tb2->EnableTool(ID_SampleItem + 6, false);
	tb2->Realize();


	wxAuiToolBar* tb3 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW);
	tb3->SetToolBitmapSize(wxSize(16, 16));
	wxBitmap tb3_bmp1 = wxArtProvider::GetBitmap(wxART_FOLDER, wxART_OTHER, wxSize(16, 16));
	tb3->AddTool(ID_SampleItem + 16, wxT("Check 1"), tb3_bmp1, wxT("Check 1"), wxITEM_CHECK);
	tb3->AddTool(ID_SampleItem + 17, wxT("Check 2"), tb3_bmp1, wxT("Check 2"), wxITEM_CHECK);
	tb3->AddTool(ID_SampleItem + 18, wxT("Check 3"), tb3_bmp1, wxT("Check 3"), wxITEM_CHECK);
	tb3->AddTool(ID_SampleItem + 19, wxT("Check 4"), tb3_bmp1, wxT("Check 4"), wxITEM_CHECK);
	tb3->AddSeparator();
	tb3->AddTool(ID_SampleItem + 20, wxT("Radio 1"), tb3_bmp1, wxT("Radio 1"), wxITEM_RADIO);
	tb3->AddTool(ID_SampleItem + 21, wxT("Radio 2"), tb3_bmp1, wxT("Radio 2"), wxITEM_RADIO);
	tb3->AddTool(ID_SampleItem + 22, wxT("Radio 3"), tb3_bmp1, wxT("Radio 3"), wxITEM_RADIO);
	tb3->AddSeparator();
	tb3->AddTool(ID_SampleItem + 23, wxT("Radio 1 (Group 2)"), tb3_bmp1, wxT("Radio 1 (Group 2)"), wxITEM_RADIO);
	tb3->AddTool(ID_SampleItem + 24, wxT("Radio 2 (Group 2)"), tb3_bmp1, wxT("Radio 2 (Group 2)"), wxITEM_RADIO);
	tb3->AddTool(ID_SampleItem + 25, wxT("Radio 3 (Group 2)"), tb3_bmp1, wxT("Radio 3 (Group 2)"), wxITEM_RADIO);
	tb3->SetCustomOverflowItems(prepend_items, append_items);
	tb3->Realize();


	wxAuiToolBar* tb4 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
		wxAUI_TB_DEFAULT_STYLE |
		wxAUI_TB_OVERFLOW |
		wxAUI_TB_TEXT |
		wxAUI_TB_HORZ_TEXT);
	tb4->SetToolBitmapSize(wxSize(16, 16));
	wxBitmap tb4_bmp1 = wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16, 16));
	tb4->AddTool(ID_DropDownToolbarItem, wxT("Item 1"), tb4_bmp1);
	tb4->AddTool(ID_SampleItem + 23, wxT("Item 2"), tb4_bmp1);
	tb4->AddTool(ID_SampleItem + 24, wxT("Item 3"), tb4_bmp1);
	tb4->AddTool(ID_SampleItem + 25, wxT("Item 4"), tb4_bmp1);
	tb4->AddSeparator();
	tb4->AddTool(ID_SampleItem + 26, wxT("Item 5"), tb4_bmp1);
	tb4->AddTool(ID_SampleItem + 27, wxT("Item 6"), tb4_bmp1);
	tb4->AddTool(ID_SampleItem + 28, wxT("Item 7"), tb4_bmp1);
	tb4->AddTool(ID_SampleItem + 29, wxT("Item 8"), tb4_bmp1);
	tb4->SetToolDropDown(ID_DropDownToolbarItem, true);
	tb4->SetCustomOverflowItems(prepend_items, append_items);
	wxChoice* choice = new wxChoice(tb4, ID_SampleItem + 35);
	choice->AppendString(wxT("One choice"));
	choice->AppendString(wxT("Another choice"));
	tb4->AddControl(choice);
	tb4->Realize();


	/*wxAuiToolBar* tb5 = new wxAuiToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize,
	wxAUI_TB_DEFAULT_STYLE | wxAUI_TB_OVERFLOW | wxAUI_TB_VERTICAL);
	tb5->SetToolBitmapSize(wxSize(48,48));
	tb5->AddTool(ID_SampleItem+30, wxT("Test"), wxArtProvider::GetBitmap(wxART_ERROR));
	tb5->AddSeparator();
	tb5->AddTool(ID_SampleItem+31, wxT("Test"), wxArtProvider::GetBitmap(wxART_QUESTION));
	tb5->AddTool(ID_SampleItem+32, wxT("Test"), wxArtProvider::GetBitmap(wxART_INFORMATION));
	tb5->AddTool(ID_SampleItem+33, wxT("Test"), wxArtProvider::GetBitmap(wxART_WARNING));
	tb5->AddTool(ID_SampleItem+34, wxT("Test"), wxArtProvider::GetBitmap(wxART_MISSING_IMAGE));
	tb5->SetCustomOverflowItems(prepend_items, append_items);
	tb5->Realize();*/

	// add a bunch of panes
	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
		Name(wxT("test1")).Caption(wxT("Pane Caption")).
		Top());

	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
		Name(wxT("test2")).Caption(wxT("Client Size Reporter")).
		Bottom().Position(1).
		CloseButton(true).MaximizeButton(true));

	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
		Name(wxT("test3")).Caption(wxT("Client Size Reporter")).
		Bottom().
		CloseButton(true).MaximizeButton(true));

	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
		Name(wxT("test4")).Caption(wxT("Pane Caption")).
		Left());

	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
		Name(wxT("test5")).Caption(wxT("No Close Button")).
		Right().CloseButton(false));

	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
		Name(wxT("test6")).Caption(wxT("Client Size Reporter")).
		Right().Row(1).
		CloseButton(true).MaximizeButton(true));

	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
		Name(wxT("test7")).Caption(wxT("Client Size Reporter")).
		Left().Layer(1).
		CloseButton(true).MaximizeButton(true));

	m_mgr.AddPane(CreateTreeCtrl(), wxAuiPaneInfo().
		Name(wxT("test8")).Caption(wxT("Algorithms")).
		Left().Layer(1).Position(1).
		CloseButton(false));

	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
		Name(wxT("test9")).Caption(wxT("Min Size 200x100")).
		BestSize(wxSize(200, 100)).MinSize(wxSize(200, 100)).
		Bottom().Layer(1).
		CloseButton(true).MaximizeButton(true));

	m_mgr.AddPane(CreatePropCtrl(), wxAuiPaneInfo().
		Name(wxT("test17")).
		Layer(1).Right().
		CloseButton(false).CaptionVisible(false));

	wxWindow *wnd10 = CreateTextCtrl(wxT("Primitives loaded"));

	// Give this pane an icon, too, just for testing.
	int iconSize = m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_CAPTION_SIZE);

	// Make it even to use 16 pixel icons with default 17 caption height.
	iconSize &= ~1;

	m_mgr.AddPane(wnd10, wxAuiPaneInfo().
		Name(wxT("test10")).Caption(wxT("Output")).
		Bottom().Layer(1).Position(1));
		//Icon(wxArtProvider::GetBitmap(wxART_WARNING,
		//	wxART_OTHER,
		//	wxSize(iconSize, iconSize))));

	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
		Name(wxT("test11")).Caption(wxT("Fixed Pane")).
		Bottom().Layer(1).Position(2).Fixed());


	m_mgr.AddPane(new SettingsPanel(this, this), wxAuiPaneInfo().
		Name(wxT("settings")).Caption(wxT("Dock Manager Settings")).
		Dockable(false).Float().Hide());

	// create some center panes

	m_mgr.AddPane(CreateGrid(), wxAuiPaneInfo().Name(wxT("grid_content")).
		CenterPane().Hide());

	m_mgr.AddPane(CreateTreeCtrl(), wxAuiPaneInfo().Name(wxT("tree_content")).
		CenterPane().Hide());

	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().Name(wxT("sizereport_content")).
		CenterPane().Hide());

	m_mgr.AddPane(CreateTextCtrl(), wxAuiPaneInfo().Name(wxT("text_content")).
		CenterPane().Hide());

	m_mgr.AddPane(CreateHTMLCtrl(), wxAuiPaneInfo().Name(wxT("html_content")).
		CenterPane().Hide());

	m_mgr.AddPane(CreateNotebook(), wxAuiPaneInfo().Name(wxT("notebook_content")).
		CenterPane().PaneBorder(false));

	// add the toolbars to the manager


	m_mgr.AddPane(tb2, wxAuiPaneInfo().
		Name(wxT("tb2")).
		ToolbarPane().Top().Row(1).Floatable(false).Gripper(false));

	m_mgr.AddPane(tb3, wxAuiPaneInfo().
		Name(wxT("tb3")).Caption(wxT("Toolbar 3")).
		ToolbarPane().Top().Row(2).Position(1));

	m_mgr.AddPane(tb4, wxAuiPaneInfo().
		Name(wxT("tb4")).Caption(wxT("Sample Bookmark Toolbar")).
		ToolbarPane().Top().Row(2));

	/*m_mgr.AddPane(tb5, wxAuiPaneInfo().
	Name(wxT("tb5")).Caption(wxT("Sample Vertical Toolbar")).
	ToolbarPane().Left().
	GripperTop());*/

	m_mgr.AddPane(new wxButton(this, wxID_ANY, _("Test Button")),
		wxAuiPaneInfo().Name(wxT("tb6")).
		ToolbarPane().Top().Row(2).Position(1).
		LeftDockable(false).RightDockable(false));

	// make some default perspectives

	wxString perspective_all = m_mgr.SavePerspective();

	int i, count;
	wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
	for (i = 0, count = all_panes.GetCount(); i < count; ++i)
		if (!all_panes.Item(i).IsToolbar())
			all_panes.Item(i).Hide();
	m_mgr.GetPane(wxT("tb6")).Hide();
	m_mgr.GetPane(wxT("test8")).Show().Left().Layer(0).Row(0).Position(0);
	m_mgr.GetPane(wxT("test10")).Show().Bottom().Layer(0).Row(0).Position(0);
	m_mgr.GetPane(wxT("test17")).Show().Layer(1).Right().Position(0);
	m_mgr.GetPane(wxT("notebook_content")).Show();
	wxString perspective_default = m_mgr.SavePerspective();

	m_perspectives.Add(perspective_default);
	m_perspectives.Add(perspective_all);

	// "commit" all changes made to wxAuiManager
	m_mgr.Update();
}

Frame::~Frame()
{
	m_mgr.UnInit();
}

wxAuiDockArt *Frame::GetDockArt()
{
	return m_mgr.GetArtProvider();
}

void Frame::DoUpdate()
{
	m_mgr.Update();
}

void Frame::OnEraseBackground(wxEraseEvent& event)
{
	event.Skip();
}

void Frame::OnSize(wxSizeEvent& event)
{
	event.Skip();
}

void Frame::OnSettings(wxCommandEvent& WXUNUSED(evt))
{
	// show the settings pane, and float it
	wxAuiPaneInfo& floating_pane = m_mgr.GetPane(wxT("settings")).Float().Show();

	if (floating_pane.floating_pos == wxDefaultPosition)
		floating_pane.FloatingPosition(GetStartPosition());

	m_mgr.Update();
}

void Frame::OnRandomGeneratorWindow(wxCommandEvent& WXUNUSED(evt))
{
	RandonGenerator dialog(this);
	dialog.ShowModal();
	//if (dialog.ShowModal() != wxID_OK)
		//dialogText->AppendText(_("The about box was cancelled.\n"));
	//else
		//dialogText->AppendText(dialog.GetText());
}

void Frame::OnCustomizeToolbar(wxCommandEvent& WXUNUSED(evt))
{
	wxMessageBox(_("Customize Toolbar clicked"));
}

void Frame::OnGradient(wxCommandEvent& event)
{
	int gradient = 0;

	switch (event.GetId())
	{
		case ID_NoGradient:         gradient = wxAUI_GRADIENT_NONE; break;
		case ID_VerticalGradient:   gradient = wxAUI_GRADIENT_VERTICAL; break;
		case ID_HorizontalGradient: gradient = wxAUI_GRADIENT_HORIZONTAL; break;
	}

	m_mgr.GetArtProvider()->SetMetric(wxAUI_DOCKART_GRADIENT_TYPE, gradient);
	m_mgr.Update();
}

void Frame::OnToolbarResizing(wxCommandEvent& WXUNUSED(evt))
{
	wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
	const size_t count = all_panes.GetCount();
	for (size_t i = 0; i < count; ++i)
	{
		wxAuiToolBar* toolbar = wxDynamicCast(all_panes[i].window, wxAuiToolBar);
		if (toolbar)
		{
			all_panes[i].Resizable(!all_panes[i].IsResizable());
		}
	}

	m_mgr.Update();
}

void Frame::OnManagerFlag(wxCommandEvent& event)
{
	unsigned int flag = 0;

#if !defined(__WXMSW__) && !defined(__WXMAC__) && !defined(__WXGTK__)
	if (event.GetId() == ID_TransparentDrag ||
		event.GetId() == ID_TransparentHint ||
		event.GetId() == ID_HintFade)
	{
		wxMessageBox(wxT("This option is presently only available on wxGTK, wxMSW and wxMac"));
		return;
	}
#endif

	int id = event.GetId();

	if (id == ID_TransparentHint ||
		id == ID_VenetianBlindsHint ||
		id == ID_RectangleHint ||
		id == ID_NoHint)
	{
		unsigned int flags = m_mgr.GetFlags();
		flags &= ~wxAUI_MGR_TRANSPARENT_HINT;
		flags &= ~wxAUI_MGR_VENETIAN_BLINDS_HINT;
		flags &= ~wxAUI_MGR_RECTANGLE_HINT;
		m_mgr.SetFlags(flags);
	}

	switch (id)
	{
		case ID_AllowFloating: flag = wxAUI_MGR_ALLOW_FLOATING; break;
		case ID_TransparentDrag: flag = wxAUI_MGR_TRANSPARENT_DRAG; break;
		case ID_HintFade: flag = wxAUI_MGR_HINT_FADE; break;
		case ID_NoVenetianFade: flag = wxAUI_MGR_NO_VENETIAN_BLINDS_FADE; break;
		case ID_AllowActivePane: flag = wxAUI_MGR_ALLOW_ACTIVE_PANE; break;
		case ID_TransparentHint: flag = wxAUI_MGR_TRANSPARENT_HINT; break;
		case ID_VenetianBlindsHint: flag = wxAUI_MGR_VENETIAN_BLINDS_HINT; break;
		case ID_RectangleHint: flag = wxAUI_MGR_RECTANGLE_HINT; break;
		case ID_LiveUpdate: flag = wxAUI_MGR_LIVE_RESIZE; break;
	}

	if (flag)
	{
		m_mgr.SetFlags(m_mgr.GetFlags() ^ flag);
	}

	m_mgr.Update();
}


void Frame::OnNotebookFlag(wxCommandEvent& event)
{
	int id = event.GetId();

	if (id == ID_NotebookNoCloseButton ||
		id == ID_NotebookCloseButton ||
		id == ID_NotebookCloseButtonAll ||
		id == ID_NotebookCloseButtonActive)
	{
		m_notebook_style &= ~(wxAUI_NB_CLOSE_BUTTON |
			wxAUI_NB_CLOSE_ON_ACTIVE_TAB |
			wxAUI_NB_CLOSE_ON_ALL_TABS);

		switch (id)
		{
			case ID_NotebookNoCloseButton: break;
			case ID_NotebookCloseButton: m_notebook_style |= wxAUI_NB_CLOSE_BUTTON; break;
			case ID_NotebookCloseButtonAll: m_notebook_style |= wxAUI_NB_CLOSE_ON_ALL_TABS; break;
			case ID_NotebookCloseButtonActive: m_notebook_style |= wxAUI_NB_CLOSE_ON_ACTIVE_TAB; break;
		}
	}

	if (id == ID_NotebookAllowTabMove)
	{
		m_notebook_style ^= wxAUI_NB_TAB_MOVE;
	}
	if (id == ID_NotebookAllowTabExternalMove)
	{
		m_notebook_style ^= wxAUI_NB_TAB_EXTERNAL_MOVE;
	}
	else if (id == ID_NotebookAllowTabSplit)
	{
		m_notebook_style ^= wxAUI_NB_TAB_SPLIT;
	}
	else if (id == ID_NotebookWindowList)
	{
		m_notebook_style ^= wxAUI_NB_WINDOWLIST_BUTTON;
	}
	else if (id == ID_NotebookScrollButtons)
	{
		m_notebook_style ^= wxAUI_NB_SCROLL_BUTTONS;
	}
	else if (id == ID_NotebookTabFixedWidth)
	{
		m_notebook_style ^= wxAUI_NB_TAB_FIXED_WIDTH;
	}


	size_t i, count;
	wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
	for (i = 0, count = all_panes.GetCount(); i < count; ++i)
	{
		wxAuiPaneInfo& pane = all_panes.Item(i);
		if (pane.window->IsKindOf(CLASSINFO(wxAuiNotebook)))
		{
			wxAuiNotebook* nb = (wxAuiNotebook*)pane.window;

			if (id == ID_NotebookArtGloss)
			{
				nb->SetArtProvider(new wxAuiDefaultTabArt);
				m_notebook_theme = 0;
			}
			else if (id == ID_NotebookArtSimple)
			{
				nb->SetArtProvider(new wxAuiSimpleTabArt);
				m_notebook_theme = 1;
			}


			nb->SetWindowStyleFlag(m_notebook_style);
			nb->Refresh();
		}
	}
}


void Frame::OnUpdateUI(wxUpdateUIEvent& event)
{
	unsigned int flags = m_mgr.GetFlags();

	switch (event.GetId())
	{
		case ID_NoGradient:
			event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_NONE);
			break;
		case ID_VerticalGradient:
			event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_VERTICAL);
			break;
		case ID_HorizontalGradient:
			event.Check(m_mgr.GetArtProvider()->GetMetric(wxAUI_DOCKART_GRADIENT_TYPE) == wxAUI_GRADIENT_HORIZONTAL);
			break;
		case ID_AllowToolbarResizing:
		{
			wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
			const size_t count = all_panes.GetCount();
			for (size_t i = 0; i < count; ++i)
			{
				wxAuiToolBar* toolbar = wxDynamicCast(all_panes[i].window, wxAuiToolBar);
				if (toolbar)
				{
					event.Check(all_panes[i].IsResizable());
					break;
				}
			}
			break;
		}
		case ID_AllowFloating:
			event.Check((flags & wxAUI_MGR_ALLOW_FLOATING) != 0);
			break;
		case ID_TransparentDrag:
			event.Check((flags & wxAUI_MGR_TRANSPARENT_DRAG) != 0);
			break;
		case ID_TransparentHint:
			event.Check((flags & wxAUI_MGR_TRANSPARENT_HINT) != 0);
			break;
		case ID_LiveUpdate:
			event.Check((flags & wxAUI_MGR_LIVE_RESIZE) != 0);
			break;
		case ID_VenetianBlindsHint:
			event.Check((flags & wxAUI_MGR_VENETIAN_BLINDS_HINT) != 0);
			break;
		case ID_RectangleHint:
			event.Check((flags & wxAUI_MGR_RECTANGLE_HINT) != 0);
			break;
		case ID_NoHint:
			event.Check(((wxAUI_MGR_TRANSPARENT_HINT |
				wxAUI_MGR_VENETIAN_BLINDS_HINT |
				wxAUI_MGR_RECTANGLE_HINT) & flags) == 0);
			break;
		case ID_HintFade:
			event.Check((flags & wxAUI_MGR_HINT_FADE) != 0);
			break;
		case ID_NoVenetianFade:
			event.Check((flags & wxAUI_MGR_NO_VENETIAN_BLINDS_FADE) != 0);
			break;

		case ID_NotebookNoCloseButton:
			event.Check((m_notebook_style & (wxAUI_NB_CLOSE_BUTTON | wxAUI_NB_CLOSE_ON_ALL_TABS | wxAUI_NB_CLOSE_ON_ACTIVE_TAB)) != 0);
			break;
		case ID_NotebookCloseButton:
			event.Check((m_notebook_style & wxAUI_NB_CLOSE_BUTTON) != 0);
			break;
		case ID_NotebookCloseButtonAll:
			event.Check((m_notebook_style & wxAUI_NB_CLOSE_ON_ALL_TABS) != 0);
			break;
		case ID_NotebookCloseButtonActive:
			event.Check((m_notebook_style & wxAUI_NB_CLOSE_ON_ACTIVE_TAB) != 0);
			break;
		case ID_NotebookAllowTabSplit:
			event.Check((m_notebook_style & wxAUI_NB_TAB_SPLIT) != 0);
			break;
		case ID_NotebookAllowTabMove:
			event.Check((m_notebook_style & wxAUI_NB_TAB_MOVE) != 0);
			break;
		case ID_NotebookAllowTabExternalMove:
			event.Check((m_notebook_style & wxAUI_NB_TAB_EXTERNAL_MOVE) != 0);
			break;
		case ID_NotebookScrollButtons:
			event.Check((m_notebook_style & wxAUI_NB_SCROLL_BUTTONS) != 0);
			break;
		case ID_NotebookWindowList:
			event.Check((m_notebook_style & wxAUI_NB_WINDOWLIST_BUTTON) != 0);
			break;
		case ID_NotebookTabFixedWidth:
			event.Check((m_notebook_style & wxAUI_NB_TAB_FIXED_WIDTH) != 0);
			break;
		case ID_NotebookArtGloss:
			event.Check(m_notebook_style == 0);
			break;
		case ID_NotebookArtSimple:
			event.Check(m_notebook_style == 1);
			break;
	}
}

void Frame::OnPaneClose(wxAuiManagerEvent& evt)
{
	if (evt.pane->name == wxT("test10"))
	{
		int res = wxMessageBox(wxT("Are you sure you want to close/hide this pane?"),
			wxT("wxAUI"),
			wxYES_NO,
			this);
		if (res != wxYES)
			evt.Veto();
	}
}

void Frame::OnCreatePerspective(wxCommandEvent& WXUNUSED(event))
{
	wxTextEntryDialog dlg(this, wxT("Enter a name for the new perspective:"),
		wxT("wxAUI Test"));

	dlg.SetValue(wxString::Format(wxT("Perspective %u"), unsigned(m_perspectives.GetCount() + 1)));
	if (dlg.ShowModal() != wxID_OK)
		return;

	if (m_perspectives.GetCount() == 0)
	{
		m_perspectives_menu->AppendSeparator();
	}

	m_perspectives_menu->Append(ID_FirstPerspective + m_perspectives.GetCount(), dlg.GetValue());
	m_perspectives.Add(m_mgr.SavePerspective());
}

void Frame::OnCopyPerspectiveCode(wxCommandEvent& WXUNUSED(evt))
{
	wxString s = m_mgr.SavePerspective();

#if wxUSE_CLIPBOARD
	if (wxTheClipboard->Open())
	{
		wxTheClipboard->SetData(new wxTextDataObject(s));
		wxTheClipboard->Close();
	}
#endif
}

void Frame::OnRestorePerspective(wxCommandEvent& evt)
{
	m_mgr.LoadPerspective(m_perspectives.Item(evt.GetId() - ID_FirstPerspective));
}

void Frame::OnNotebookPageClose(wxAuiNotebookEvent& evt)
{
	wxAuiNotebook *ctrl = (wxAuiNotebook*)evt.GetEventObject();
	if (ctrl->GetPage(evt.GetSelection())->IsKindOf(CLASSINFO(wxHtmlWindow)))
	{
		int res = wxMessageBox(wxT("Are you sure you want to close/hide this notebook page?"),
			wxT("wxAUI"),
			wxYES_NO,
			this);
		if (res != wxYES)
			evt.Veto();
	}
}

void Frame::OnNotebookPageClosed(wxAuiNotebookEvent& evt)
{
	wxAuiNotebook *ctrl = (wxAuiNotebook *)evt.GetEventObject();

	// selection should always be a valid index
	wxASSERT_MSG(ctrl->GetSelection() < (int)ctrl->GetPageCount(),
		wxString::Format("Invalid selection %d, only %d pages left",
			ctrl->GetSelection(),
			(int)ctrl->GetPageCount()));

	evt.Skip();
}

void Frame::OnAllowNotebookDnD(wxAuiNotebookEvent& evt)
{
	// for the purpose of this test application, explicitly
	// allow all notebook drag and drop events
	evt.Allow();
}

wxPoint Frame::GetStartPosition()
{
	static int x = 0;
	x += 20;
	wxPoint pt = ClientToScreen(wxPoint(0, 0));
	return wxPoint(pt.x + x, pt.y + x);
}

void Frame::OnCreateTree(wxCommandEvent& WXUNUSED(event))
{
	m_mgr.AddPane(CreateTreeCtrl(), wxAuiPaneInfo().
		Caption(wxT("Tree Control")).
		Float().FloatingPosition(GetStartPosition()).
		FloatingSize(wxSize(150, 300)));
	m_mgr.Update();
}

void Frame::OnCreateGrid(wxCommandEvent& WXUNUSED(event))
{
	m_mgr.AddPane(CreateGrid(), wxAuiPaneInfo().
		Caption(wxT("Grid")).
		Float().FloatingPosition(GetStartPosition()).
		FloatingSize(wxSize(300, 200)));
	m_mgr.Update();
}

void Frame::OnCreateHTML(wxCommandEvent& WXUNUSED(event))
{
	m_mgr.AddPane(CreateHTMLCtrl(), wxAuiPaneInfo().
		Caption(wxT("HTML Control")).
		Float().FloatingPosition(GetStartPosition()).
		FloatingSize(wxSize(300, 200)));
	m_mgr.Update();
}

void Frame::OnCreateNotebook(wxCommandEvent& WXUNUSED(event))
{
	m_mgr.AddPane(CreateNotebook(), wxAuiPaneInfo().
		Caption(wxT("Notebook")).
		Float().FloatingPosition(GetStartPosition()).
		//FloatingSize(300,200).
		CloseButton(true).MaximizeButton(true));
	m_mgr.Update();
}

void Frame::OnCreateText(wxCommandEvent& WXUNUSED(event))
{
	m_mgr.AddPane(CreateTextCtrl(), wxAuiPaneInfo().
		Caption(wxT("Text Control")).
		Float().FloatingPosition(GetStartPosition()));
	m_mgr.Update();
}

void Frame::OnCreateSizeReport(wxCommandEvent& WXUNUSED(event))
{
	m_mgr.AddPane(CreateSizeReportCtrl(), wxAuiPaneInfo().
		Caption(wxT("Client Size Reporter")).
		Float().FloatingPosition(GetStartPosition()).
		CloseButton(true).MaximizeButton(true));
	m_mgr.Update();
}

void Frame::OnChangeContentPane(wxCommandEvent& evt)
{
	m_mgr.GetPane(wxT("grid_content")).Show(evt.GetId() == ID_GridContent);
	m_mgr.GetPane(wxT("text_content")).Show(evt.GetId() == ID_TextContent);
	m_mgr.GetPane(wxT("tree_content")).Show(evt.GetId() == ID_TreeContent);
	m_mgr.GetPane(wxT("sizereport_content")).Show(evt.GetId() == ID_SizeReportContent);
	m_mgr.GetPane(wxT("html_content")).Show(evt.GetId() == ID_HTMLContent);
	m_mgr.GetPane(wxT("notebook_content")).Show(evt.GetId() == ID_NotebookContent);
	m_mgr.Update();
}

void Frame::OnDropDownToolbarItem(wxAuiToolBarEvent& evt)
{
	if (evt.IsDropDownClicked())
	{
		wxAuiToolBar *tb = static_cast<wxAuiToolBar*>(evt.GetEventObject());

		tb->SetToolSticky(evt.GetId(), true);

		// create the popup menu
		wxMenu menuPopup;

		wxBitmap bmp = wxArtProvider::GetBitmap(wxART_QUESTION, wxART_OTHER, wxSize(16, 16));

		wxMenuItem* m1 = new wxMenuItem(&menuPopup, 10001, _("Drop Down Item 1"));
		m1->SetBitmap(bmp);
		menuPopup.Append(m1);

		wxMenuItem* m2 = new wxMenuItem(&menuPopup, 10002, _("Drop Down Item 2"));
		m2->SetBitmap(bmp);
		menuPopup.Append(m2);

		wxMenuItem* m3 = new wxMenuItem(&menuPopup, 10003, _("Drop Down Item 3"));
		m3->SetBitmap(bmp);
		menuPopup.Append(m3);

		wxMenuItem* m4 = new wxMenuItem(&menuPopup, 10004, _("Drop Down Item 4"));
		m4->SetBitmap(bmp);
		menuPopup.Append(m4);

		// line up our menu with the button
		wxRect rect = tb->GetToolRect(evt.GetId());
		wxPoint pt = tb->ClientToScreen(rect.GetBottomLeft());
		pt = ScreenToClient(pt);


		PopupMenu(&menuPopup, pt);


		// make sure the button is "un-stuck"
		tb->SetToolSticky(evt.GetId(), false);
	}
}


void Frame::OnTabAlignment(wxCommandEvent &evt)
{
	size_t i, count;
	wxAuiPaneInfoArray& all_panes = m_mgr.GetAllPanes();
	for (i = 0, count = all_panes.GetCount(); i < count; ++i)
	{
		wxAuiPaneInfo& pane = all_panes.Item(i);
		if (pane.window->IsKindOf(CLASSINFO(wxAuiNotebook)))
		{
			wxAuiNotebook* nb = (wxAuiNotebook *)pane.window;

			long style = nb->GetWindowStyleFlag();
			style &= ~(wxAUI_NB_TOP | wxAUI_NB_BOTTOM);
			if (evt.GetId() == ID_NotebookAlignTop)
				style |= wxAUI_NB_TOP;
			else if (evt.GetId() == ID_NotebookAlignBottom)
				style |= wxAUI_NB_BOTTOM;
			nb->SetWindowStyleFlag(style);

			nb->Refresh();
		}
	}
}

void Frame::OnExit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}

void Frame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	wxMessageBox(_("Cryptox\nCryptography management studio\n\nCopyright 2017, Quenza Inc."), _("Cryptox"), wxOK, this);
}

wxMenuBar *Frame::CreateMenuBar()
{
	wxMenuBar *mb = new wxMenuBar;

	wxMenu *file_menu = new wxMenu;
	file_menu->Append(wxID_NEW);
	file_menu->Append(wxID_OPEN);
	file_menu->Append(wxID_CLOSE);
	file_menu->Append(wxID_SAVE);
	file_menu->Append(wxID_SAVEAS);
	file_menu->Append(wxID_REVERT, wxT("Re&vert..."));
	file_menu->AppendSeparator();
	file_menu->Append(wxID_EXIT, wxT("&Exit"));

	wxMenu *view_menu = new wxMenu;
	view_menu->Append(ID_CreateText, wxT("Create Text Control"));
	view_menu->Append(ID_CreateHTML, wxT("Create HTML Control"));
	view_menu->Append(ID_CreateTree, wxT("Create Tree"));
	view_menu->Append(ID_CreateGrid, wxT("Create Grid"));
	view_menu->Append(ID_CreateNotebook, wxT("Create Notebook"));
	view_menu->Append(ID_CreateSizeReport, wxT("Create Size Reporter"));
	view_menu->AppendSeparator();
	view_menu->Append(ID_GridContent, wxT("Use a Grid for the Content Pane"));
	view_menu->Append(ID_TextContent, wxT("Use a Text Control for the Content Pane"));
	view_menu->Append(ID_HTMLContent, wxT("Use an HTML Control for the Content Pane"));
	view_menu->Append(ID_TreeContent, wxT("Use a Tree Control for the Content Pane"));
	view_menu->Append(ID_NotebookContent, wxT("Use a wxAuiNotebook control for the Content Pane"));
	view_menu->Append(ID_SizeReportContent, wxT("Use a Size Reporter for the Content Pane"));

	wxMenu *options_menu = new wxMenu;
	options_menu->AppendRadioItem(ID_TransparentHint, wxT("Transparent Hint"));
	options_menu->AppendRadioItem(ID_VenetianBlindsHint, wxT("Venetian Blinds Hint"));
	options_menu->AppendRadioItem(ID_RectangleHint, _("Rectangle Hint"));
	options_menu->AppendRadioItem(ID_NoHint, _("No Hint"));
	options_menu->AppendSeparator();
	options_menu->AppendCheckItem(ID_HintFade, _("Hint Fade-in"));
	options_menu->AppendCheckItem(ID_AllowFloating, _("Allow Floating"));
	options_menu->AppendCheckItem(ID_NoVenetianFade, _("Disable Venetian Blinds Hint Fade-in"));
	options_menu->AppendCheckItem(ID_TransparentDrag, _("Transparent Drag"));
	options_menu->AppendCheckItem(ID_AllowActivePane, _("Allow Active Pane"));
	options_menu->AppendCheckItem(ID_LiveUpdate, _("Live Resize Update"));
	options_menu->AppendSeparator();
	options_menu->AppendRadioItem(ID_NoGradient, _("No Caption Gradient"));
	options_menu->AppendRadioItem(ID_VerticalGradient, _("Vertical Caption Gradient"));
	options_menu->AppendRadioItem(ID_HorizontalGradient, _("Horizontal Caption Gradient"));
	options_menu->AppendSeparator();
	options_menu->AppendCheckItem(ID_AllowToolbarResizing, _("Allow Toolbar Resizing"));
	options_menu->AppendSeparator();
	options_menu->Append(ID_Settings, _("Settings Pane"));

	wxMenu *tools_menu = new wxMenu;
	tools_menu->Append(wxID_ANY, _("Password Generator"));
	tools_menu->AppendSeparator();
	tools_menu->AppendRadioItem(ID_TransparentHint, _("Transparent Hint"));
	tools_menu->AppendRadioItem(ID_VenetianBlindsHint, _("Venetian Blinds Hint"));
	tools_menu->AppendRadioItem(ID_RectangleHint, _("Rectangle Hint"));
	tools_menu->AppendRadioItem(ID_NoHint, _("No Hint"));
	tools_menu->AppendSeparator();
	tools_menu->AppendCheckItem(ID_HintFade, _("Hint Fade-in"));
	tools_menu->AppendCheckItem(ID_AllowFloating, _("Allow Floating"));
	tools_menu->AppendCheckItem(ID_NoVenetianFade, _("Disable Venetian Blinds Hint Fade-in"));
	tools_menu->AppendCheckItem(ID_TransparentDrag, _("Transparent Drag"));
	tools_menu->AppendCheckItem(ID_AllowActivePane, _("Allow Active Pane"));
	tools_menu->AppendCheckItem(ID_LiveUpdate, _("Live Resize Update"));
	tools_menu->AppendSeparator();
	tools_menu->AppendRadioItem(ID_NoGradient, _("No Caption Gradient"));
	tools_menu->AppendRadioItem(ID_VerticalGradient, _("Vertical Caption Gradient"));
	tools_menu->AppendRadioItem(ID_HorizontalGradient, _("Horizontal Caption Gradient"));
	tools_menu->AppendSeparator();
	tools_menu->AppendCheckItem(ID_AllowToolbarResizing, _("Allow Toolbar Resizing"));
	tools_menu->AppendSeparator();
	tools_menu->Append(ID_Settings, _("Settings Pane"));

	wxMenu *notebook_menu = new wxMenu;
	notebook_menu->AppendRadioItem(ID_NotebookArtGloss, _("Glossy Theme (Default)"));
	notebook_menu->AppendRadioItem(ID_NotebookArtSimple, _("Simple Theme"));
	notebook_menu->AppendSeparator();
	notebook_menu->AppendRadioItem(ID_NotebookNoCloseButton, _("No Close Button"));
	notebook_menu->AppendRadioItem(ID_NotebookCloseButton, _("Close Button at Right"));
	notebook_menu->AppendRadioItem(ID_NotebookCloseButtonAll, _("Close Button on All Tabs"));
	notebook_menu->AppendRadioItem(ID_NotebookCloseButtonActive, _("Close Button on Active Tab"));
	notebook_menu->AppendSeparator();
	notebook_menu->AppendRadioItem(ID_NotebookAlignTop, _("Tab Top Alignment"));
	notebook_menu->AppendRadioItem(ID_NotebookAlignBottom, _("Tab Bottom Alignment"));
	notebook_menu->AppendSeparator();
	notebook_menu->AppendCheckItem(ID_NotebookAllowTabMove, _("Allow Tab Move"));
	notebook_menu->AppendCheckItem(ID_NotebookAllowTabExternalMove, _("Allow External Tab Move"));
	notebook_menu->AppendCheckItem(ID_NotebookAllowTabSplit, wxT("Allow Notebook Split"));
	notebook_menu->AppendCheckItem(ID_NotebookScrollButtons, wxT("Scroll Buttons Visible"));
	notebook_menu->AppendCheckItem(ID_NotebookWindowList, wxT("Window List Button Visible"));
	notebook_menu->AppendCheckItem(ID_NotebookTabFixedWidth, wxT("Fixed-width Tabs"));

	m_perspectives_menu = new wxMenu;
	m_perspectives_menu->Append(ID_CreatePerspective, wxT("Create Perspective"));
	m_perspectives_menu->Append(ID_CopyPerspectiveCode, wxT("Copy Perspective Data To Clipboard"));
	m_perspectives_menu->AppendSeparator();
	m_perspectives_menu->Append(ID_FirstPerspective + 0, wxT("Default Startup"));
	m_perspectives_menu->Append(ID_FirstPerspective + 1, wxT("All Panes"));

	wxMenu *help_menu = new wxMenu;
	help_menu->Append(wxID_ABOUT);

	mb->Append(file_menu, wxT("&File"));
	mb->Append(view_menu, wxT("&View"));
	mb->Append(m_perspectives_menu, wxT("&Perspectives"));
	mb->Append(options_menu, wxT("&Options"));
	mb->Append(tools_menu, wxT("&Tools"));
	mb->Append(notebook_menu, wxT("&Notebook"));
	mb->Append(help_menu, wxT("&Help"));

	return mb;
}

wxTextCtrl *Frame::CreateTextCtrl(const wxString& ctrl_text)
{
	static int n = 0;

	wxString text;
	if (!ctrl_text.empty())
		text = ctrl_text;
	else
		text.Printf(wxT("This is text box %d"), ++n);

	return new wxTextCtrl(this, wxID_ANY, text,
		wxPoint(0, 0), wxSize(150, 90),
		wxNO_BORDER | wxTE_MULTILINE);
}


wxGrid *Frame::CreateGrid()
{
	wxGrid* grid = new wxGrid(this, wxID_ANY,
		wxPoint(0, 0),
		wxSize(150, 250),
		wxNO_BORDER | wxWANTS_CHARS);
	grid->CreateGrid(50, 20);
	return grid;
}

wxTreeCtrl *Frame::CreateTreeCtrl()
{
	wxTreeCtrl *tree = new wxTreeCtrl(this, 10009,
		wxPoint(0, 0), wxSize(190, 250),
		wxTR_DEFAULT_STYLE | wxNO_BORDER);

	tree->SetWindowStyle(wxTR_HIDE_ROOT);

	wxImageList* imglist = new wxImageList(16, 16, true, 2);
	imglist->Add(wxArtProvider::GetBitmap(wxART_FOLDER, wxART_OTHER, wxSize(16, 16)));
	imglist->Add(wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16, 16)));
	tree->AssignImageList(imglist);

	wxTreeItemId root = tree->AddRoot("NoRoot", 0);

	wxArrayTreeItemIds items;
	items.Add(tree->AppendItem(root, wxT("Block ciphers"), 0));
	items.Add(tree->AppendItem(root, wxT("Stream ciphers"), 0));
	items.Add(tree->AppendItem(root, wxT("Random generators"), 0));
	items.Add(tree->AppendItem(root, wxT("Elliptic curves"), 0));
	items.Add(tree->AppendItem(root, wxT("Authentication"), 0));
	items.Add(tree->AppendItem(root, wxT("DH Groups"), 0));
	items.Add(tree->AppendItem(root, wxT("Asymmetric"), 0));

	int i, count;
	for (i = 0, count = items.Count(); i < count; ++i)
	{
		wxTreeItemId id = items.Item(i);
		tree->AppendItem(id, wxT("Subitem 1"), 1);
		tree->AppendItem(id, wxT("Subitem 2"), 1);
		tree->AppendItem(id, wxT("Subitem 3"), 1);
		tree->AppendItem(id, wxT("Subitem 4"), 1);
		tree->AppendItem(id, wxT("Subitem 5"), 1);
	}

	wxTreeItemId id = tree->AppendItem(root, wxT("Random generators"), 0);
	tree->AppendItem(id, wxT("MT19937"), 1);
	tree->AppendItem(id, wxT("MT19937ar"), 1);

	return tree;
}

wxPropertyGridManager *Frame::CreatePropCtrl()
{
	wxPropertyGridManager *pgman = new wxPropertyGridManager(this, wxID_ANY, wxPoint(0, 0), wxSize(250, 250));

	wxPropertyGridPage *page =  pgman->AddPage(wxT("TestItem"));
	page->Append(new wxPropertyCategory(wxT("Appearance"), wxPG_LABEL));
	page->Append(new wxStringProperty(wxT("Label"), wxPG_LABEL, GetTitle()));

	page->Append(new wxPropertyCategory(wxT("Environment"), wxPG_LABEL));
	page->Append(new wxStringProperty(wxT("Operating System"), wxPG_LABEL, ::wxGetOsDescription()));

	page->Append(new wxStringProperty(wxT("User Id"), wxPG_LABEL, ::wxGetUserId()));
	page->Append(new wxDirProperty(wxT("User Home"), wxPG_LABEL, ::wxGetUserHome()));
	page->Append(new wxStringProperty(wxT("User Name"), wxPG_LABEL, ::wxGetUserName()));

	page->DisableProperty(wxT("Operating System"));
	page->DisableProperty(wxT("User Id"));
	page->DisableProperty(wxT("User Name"));

	page->Append(new wxPropertyCategory(wxT("More Examples"), wxPG_LABEL));

	return pgman;
}

wxSizeReportCtrl *Frame::CreateSizeReportCtrl(int width, int height)
{
	wxSizeReportCtrl *ctrl = new wxSizeReportCtrl(this, wxID_ANY,
		wxDefaultPosition,
		wxSize(width, height), &m_mgr);
	return ctrl;
}

wxHtmlWindow *Frame::CreateHTMLCtrl(wxWindow *parent)
{
	if (!parent)
		parent = this;

	wxHtmlWindow *ctrl = new wxHtmlWindow(parent, wxID_ANY,
		wxDefaultPosition,
		wxSize(400, 300));
	ctrl->SetPage(GetIntroText());
	return ctrl;
}

wxAuiNotebook *Frame::CreateNotebook()
{
	// create the notebook off-window to avoid flicker
	wxSize client_size = GetClientSize();

	wxAuiNotebook *ctrl = new wxAuiNotebook(this, wxID_ANY,
		wxPoint(client_size.x, client_size.y),
		wxSize(430, 200),
		m_notebook_style);
	ctrl->Freeze();

	wxBitmap page_bmp = wxArtProvider::GetBitmap(wxART_NORMAL_FILE, wxART_OTHER, wxSize(16, 16));

	ctrl->AddPage(CreateHTMLCtrl(ctrl), wxT("Start page"), false, page_bmp);
	ctrl->SetPageToolTip(0, "Welcome to Cryptox");

	wxPanel *panel = new wxPanel(ctrl, wxID_ANY);
	wxFlexGridSizer *flex = new wxFlexGridSizer(4, 2, 0, 0);
	flex->AddGrowableRow(0);
	flex->AddGrowableRow(3);
	flex->AddGrowableCol(1);
	flex->Add(5, 5);   flex->Add(5, 5);
	flex->Add(new wxStaticText(panel, wxID_ANY, wxT("wxTextCtrl:")), 0, wxALL | wxALIGN_CENTRE, 5);
	flex->Add(new wxTextCtrl(panel, wxID_ANY, wxT(""), wxDefaultPosition, wxSize(100, -1)), 1, wxALL | wxALIGN_CENTRE, 5);
	flex->Add(new wxStaticText(panel, wxID_ANY, wxT("wxSpinCtrl:")), 0, wxALL | wxALIGN_CENTRE, 5);
	flex->Add(new wxSpinCtrl(panel, wxID_ANY, wxT("5"), wxDefaultPosition, wxSize(100, -1),
		wxSP_ARROW_KEYS, 5, 50, 5), 0, wxALL | wxALIGN_CENTRE, 5);
	flex->Add(5, 5);   flex->Add(5, 5);
	panel->SetSizer(flex);
	ctrl->AddPage(panel, wxT("wxPanel"), false, page_bmp);

	wxTextCtrl *console = new wxTextCtrl(ctrl, 10010, wxT("Type 'help' to get started\n\n>> "),
		wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxNO_BORDER | wxTE_PROCESS_ENTER);
	console->SetInsertionPointEnd();

	ctrl->AddPage(console, wxT("Console"), false, page_bmp);

	ctrl->Thaw();
	return ctrl;
}

void Frame::OnItemMenu(wxTreeEvent& event)
{
	wxTreeCtrl *tree = static_cast<wxTreeCtrl *>(event.GetEventObject());

	// Skip parents
	wxTreeItemId itemId = event.GetItem();
	if (tree->ItemHasChildren(itemId))
		return;

	wxPoint pt = tree->ClientToScreen(event.GetPoint());
	pt = ScreenToClient(pt);

	// Build menu
	wxMenu menu;
	menu.Append(ID_RandomGeneratorWindow, wxT("&Run"));
	menu.AppendSeparator();
	menu.Append(wxID_ANY, wxT("&Use as template"));
	menu.Append(wxID_ANY, wxT("&Highlight item"));
	menu.Append(wxID_ANY, wxT("&Dump"));

	PopupMenu(&menu, pt);
	event.Skip();
}

void Frame::OnConsoleEnter(wxCommandEvent& event)
{
	wxTextCtrl *console = static_cast<wxTextCtrl *>(event.GetEventObject());
	console->AppendText(wxT("\nUnrecognized command\n\n>> "));
	event.Skip();
}

wxString Frame::GetIntroText()
{
	static const char *text =
		"<html><body>"
		"<h3>Welcome to Cryptox</h3>"
		"<br/><b>Overview</b><br/>"
		"<p>wxAUI is an Advanced User Interface library for the wxWidgets toolkit "
		"that allows developers to create high-quality, cross-platform user "
		"interfaces quickly and easily.</p>"
		"<p><b>Features</b></p>"
		"<p>With wxAUI, developers can create application frameworks with:</p>"
		"<ul>"
		"<li>Native, dockable floating frames</li>"
		"<li>Perspective saving and loading</li>"
		"<li>Native toolbars incorporating real-time, &quot;spring-loaded&quot; dragging</li>"
		"<li>Customizable floating/docking behaviour</li>"
		"<li>Completely customizable look-and-feel</li>"
		"<li>Optional transparent window effects (while dragging or docking)</li>"
		"<li>Splittable notebook control</li>"
		"</ul>"
		"<p><b>What's new in 0.9.4?</b></p>"
		"<p>wxAUI 0.9.4, which is bundled with wxWidgets, adds the following features:"
		"<ul>"
		"<li>New wxAuiToolBar class, a toolbar control which integrates more "
		"cleanly with wxAuiFrameManager.</li>"
		"<li>Lots of bug fixes</li>"
		"</ul>"
		"<p><b>What's new in 0.9.3?</b></p>"
		"<p>wxAUI 0.9.3, which is now bundled with wxWidgets, adds the following features:"
		"<ul>"
		"<li>New wxAuiNotebook class, a dynamic splittable notebook control</li>"
		"<li>New wxAuiMDI* classes, a tab-based MDI and drop-in replacement for classic MDI</li>"
		"<li>Maximize/Restore buttons implemented</li>"
		"<li>Better hinting with wxGTK</li>"
		"<li>Class rename.  'wxAui' is now the standard class prefix for all wxAUI classes</li>"
		"<li>Lots of bug fixes</li>"
		"</ul>"
		"<p><b>What's new in 0.9.2?</b></p>"
		"<p>The following features/fixes have been added since the last version of wxAUI:</p>"
		"<ul>"
		"<li>Support for wxMac</li>"
		"<li>Updates for wxWidgets 2.6.3</li>"
		"<li>Fix to pass more unused events through</li>"
		"<li>Fix to allow floating windows to receive idle events</li>"
		"<li>Fix for minimizing/maximizing problem with transparent hint pane</li>"
		"<li>Fix to not paint empty hint rectangles</li>"
		"<li>Fix for 64-bit compilation</li>"
		"</ul>"
		"<p><b>What changed in 0.9.1?</b></p>"
		"<p>The following features/fixes were added in wxAUI 0.9.1:</p>"
		"<ul>"
		"<li>Support for MDI frames</li>"
		"<li>Gradient captions option</li>"
		"<li>Active/Inactive panes option</li>"
		"<li>Fix for screen artifacts/paint problems</li>"
		"<li>Fix for hiding/showing floated window problem</li>"
		"<li>Fix for floating pane sizing problem</li>"
		"<li>Fix for drop position problem when dragging around center pane margins</li>"
		"<li>LF-only text file formatting for source code</li>"
		"</ul>"
		"<p>See README.txt for more information.</p>"
		"</body></html>";

	return wxString::FromAscii(text);
}

wxBEGIN_EVENT_TABLE(Frame, wxFrame)
	EVT_ERASE_BACKGROUND(Frame::OnEraseBackground)
	EVT_SIZE(Frame::OnSize)
	EVT_MENU(Frame::ID_CreateTree, Frame::OnCreateTree)
	EVT_MENU(Frame::ID_CreateGrid, Frame::OnCreateGrid)
	EVT_MENU(Frame::ID_CreateText, Frame::OnCreateText)
	EVT_MENU(Frame::ID_CreateHTML, Frame::OnCreateHTML)
	EVT_MENU(Frame::ID_CreateSizeReport, Frame::OnCreateSizeReport)
	EVT_MENU(Frame::ID_CreateNotebook, Frame::OnCreateNotebook)
	EVT_MENU(Frame::ID_CreatePerspective, Frame::OnCreatePerspective)
	EVT_MENU(Frame::ID_CopyPerspectiveCode, Frame::OnCopyPerspectiveCode)
	EVT_MENU(ID_AllowFloating, Frame::OnManagerFlag)
	EVT_MENU(ID_TransparentHint, Frame::OnManagerFlag)
	EVT_MENU(ID_VenetianBlindsHint, Frame::OnManagerFlag)
	EVT_MENU(ID_RectangleHint, Frame::OnManagerFlag)
	EVT_MENU(ID_NoHint, Frame::OnManagerFlag)
	EVT_MENU(ID_HintFade, Frame::OnManagerFlag)
	EVT_MENU(ID_NoVenetianFade, Frame::OnManagerFlag)
	EVT_MENU(ID_TransparentDrag, Frame::OnManagerFlag)
	EVT_MENU(ID_LiveUpdate, Frame::OnManagerFlag)
	EVT_MENU(ID_AllowActivePane, Frame::OnManagerFlag)
	EVT_MENU(ID_NotebookTabFixedWidth, Frame::OnNotebookFlag)
	EVT_MENU(ID_NotebookNoCloseButton, Frame::OnNotebookFlag)
	EVT_MENU(ID_NotebookCloseButton, Frame::OnNotebookFlag)
	EVT_MENU(ID_NotebookCloseButtonAll, Frame::OnNotebookFlag)
	EVT_MENU(ID_NotebookCloseButtonActive, Frame::OnNotebookFlag)
	EVT_MENU(ID_NotebookAllowTabMove, Frame::OnNotebookFlag)
	EVT_MENU(ID_NotebookAllowTabExternalMove, Frame::OnNotebookFlag)
	EVT_MENU(ID_NotebookAllowTabSplit, Frame::OnNotebookFlag)
	EVT_MENU(ID_NotebookScrollButtons, Frame::OnNotebookFlag)
	EVT_MENU(ID_NotebookWindowList, Frame::OnNotebookFlag)
	EVT_MENU(ID_NotebookArtGloss, Frame::OnNotebookFlag)
	EVT_MENU(ID_NotebookArtSimple, Frame::OnNotebookFlag)
	EVT_MENU(ID_NotebookAlignTop, Frame::OnTabAlignment)
	EVT_MENU(ID_NotebookAlignBottom, Frame::OnTabAlignment)
	EVT_MENU(ID_NoGradient, Frame::OnGradient)
	EVT_MENU(ID_VerticalGradient, Frame::OnGradient)
	EVT_MENU(ID_HorizontalGradient, Frame::OnGradient)
	EVT_MENU(ID_AllowToolbarResizing, Frame::OnToolbarResizing)
	EVT_MENU(ID_Settings, Frame::OnSettings)
	EVT_MENU(ID_RandomGeneratorWindow, Frame::OnRandomGeneratorWindow)
	EVT_MENU(ID_CustomizeToolbar, Frame::OnCustomizeToolbar)
	EVT_MENU(ID_GridContent, Frame::OnChangeContentPane)
	EVT_MENU(ID_TreeContent, Frame::OnChangeContentPane)
	EVT_MENU(ID_TextContent, Frame::OnChangeContentPane)
	EVT_MENU(ID_SizeReportContent, Frame::OnChangeContentPane)
	EVT_MENU(ID_HTMLContent, Frame::OnChangeContentPane)
	EVT_MENU(ID_NotebookContent, Frame::OnChangeContentPane)
	EVT_MENU(wxID_EXIT, Frame::OnExit)
	EVT_MENU(wxID_ABOUT, Frame::OnAbout)
	EVT_UPDATE_UI(ID_NotebookTabFixedWidth, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_NotebookNoCloseButton, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_NotebookCloseButton, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_NotebookCloseButtonAll, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_NotebookCloseButtonActive, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_NotebookAllowTabMove, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_NotebookAllowTabExternalMove, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_NotebookAllowTabSplit, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_NotebookScrollButtons, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_NotebookWindowList, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_AllowFloating, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_TransparentHint, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_VenetianBlindsHint, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_RectangleHint, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_NoHint, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_HintFade, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_NoVenetianFade, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_TransparentDrag, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_LiveUpdate, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_NoGradient, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_VerticalGradient, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_HorizontalGradient, Frame::OnUpdateUI)
	EVT_UPDATE_UI(ID_AllowToolbarResizing, Frame::OnUpdateUI)
	EVT_MENU_RANGE(Frame::ID_FirstPerspective, Frame::ID_FirstPerspective + 1000,
		Frame::OnRestorePerspective)
	EVT_AUITOOLBAR_TOOL_DROPDOWN(ID_DropDownToolbarItem, Frame::OnDropDownToolbarItem)
	EVT_AUI_PANE_CLOSE(Frame::OnPaneClose)
	EVT_AUINOTEBOOK_ALLOW_DND(wxID_ANY, Frame::OnAllowNotebookDnD)
	EVT_AUINOTEBOOK_PAGE_CLOSE(wxID_ANY, Frame::OnNotebookPageClose)
	EVT_AUINOTEBOOK_PAGE_CLOSED(wxID_ANY, Frame::OnNotebookPageClosed)
	EVT_TREE_ITEM_RIGHT_CLICK(10009, Frame::OnItemMenu)
	EVT_TEXT_ENTER(10010, Frame::OnConsoleEnter)
wxEND_EVENT_TABLE()
