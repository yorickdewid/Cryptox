#include "SecretListModel.h"

#define INITIAL_NUMBER_OF_ITEMS 100

static int my_sort_reverse(int *v1, int *v2)
{
	return *v2 - *v1;
}

static int my_sort(int *v1, int *v2)
{
	return *v1 - *v2;
}

SecretListModel::SecretListModel() : wxDataViewVirtualListModel(INITIAL_NUMBER_OF_ITEMS)
{
	// the first 100 items are really stored in this model;
	// all the others are synthesized on request
	static const unsigned NUMBER_REAL_ITEMS = 100;

	m_textColValues.reserve(NUMBER_REAL_ITEMS);
	m_textColValues.push_back("first row with long label to test ellipsization");
	for (unsigned int i = 1; i < NUMBER_REAL_ITEMS; i++) {
		m_textColValues.push_back(wxString::Format("real row %d", i));
	}

	m_iconColValues.assign(NUMBER_REAL_ITEMS, "test");

	//m_icon[0] = wxIcon(null_xpm);
	//m_icon[1] = wxIcon(wx_small_xpm);
}

void SecretListModel::Prepend(const wxString &text)
{
	m_textColValues.Insert(text, 0);
	RowPrepended();
}

void SecretListModel::DeleteItem(const wxDataViewItem &item)
{
	unsigned int row = GetRow(item);

	if (row >= m_textColValues.GetCount())
		return;

	m_textColValues.RemoveAt(row);
	RowDeleted(row);
}

void SecretListModel::DeleteItems(const wxDataViewItemArray &items)
{
	unsigned i;
	wxArrayInt rows;
	for (i = 0; i < items.GetCount(); i++) {
		unsigned int row = GetRow(items[i]);
		if (row < m_textColValues.GetCount())
			rows.Add(row);
	}

	if (rows.GetCount() == 0) {
		// none of the selected items were in the range of the items
		// which we store... for simplicity, don't allow removing them
		wxLogError("Cannot remove rows with an index greater than %u", unsigned(m_textColValues.GetCount()));
		return;
	}

	// Sort in descending order so that the last
	// row will be deleted first. Otherwise the
	// remaining indeces would all be wrong.
	rows.Sort(my_sort_reverse);
	for (i = 0; i < rows.GetCount(); i++)
		m_textColValues.RemoveAt(rows[i]);

	// This is just to test if wxDataViewCtrl can
	// cope with removing rows not sorted in
	// descending order
	rows.Sort(my_sort);
	RowsDeleted(rows);
}

void SecretListModel::AddMany()
{
	Reset(GetCount() + 1000);
}

void SecretListModel::GetValueByRow(wxVariant& variant, unsigned int row, unsigned int col) const
{
	switch (col) {
		case Col_EditableText:
			if (row >= m_textColValues.GetCount())
				variant = wxString::Format("virtual row %d", row);
			else
				variant = m_textColValues[row];
			break;

		case Col_IconText:
		{
			wxString text;
			if (row >= m_iconColValues.GetCount())
				text = "virtual icon";
			else
				text = m_iconColValues[row];

			variant << wxDataViewIconText(text);
		}
		break;

		case Col_Date:
			variant = wxDateTime(1, wxDateTime::Jan, 2000).Add(wxTimeSpan(row));
			break;

		case Col_TextWithAttr:
		{
			static const char *labels[5] =
			{
				"blue", "green", "red", "bold cyan", "default",
			};

			variant = labels[row % 5];
		}
		break;

		/*case Col_Custom:
		{
			IntToStringMap::const_iterator it = m_customColValues.find(row);
			if (it != m_customColValues.end())
				variant = it->second;
			else
				variant = wxString::Format("%d", row % 100);
		}
		break;*/

		case Col_Max:
			wxFAIL_MSG("invalid column");
	}
}

bool SecretListModel::GetAttrByRow(unsigned int row, unsigned int col, wxDataViewItemAttr &attr) const
{
	switch (col) {
	case Col_EditableText:
	case Col_Date:
		return false;

	case Col_IconText:
		if (!(row % 2))
			return false;
		attr.SetColour(*wxLIGHT_GREY);
		break;

	case Col_TextWithAttr:
	case Col_Custom:
		// do what the labels defined in GetValueByRow() hint at
		switch (row % 5) {
		case 0:
			attr.SetColour(*wxBLUE);
			break;

		case 1:
			attr.SetColour(*wxGREEN);
			break;

		case 2:
			attr.SetColour(*wxRED);
			break;

		case 3:
			attr.SetColour(*wxCYAN);
			attr.SetBold(true);
			break;

		case 4:
			return false;
		}
		break;

	case Col_Max:
		wxFAIL_MSG("invalid column");
	}

	return true;
}

bool SecretListModel::SetValueByRow(const wxVariant &variant, unsigned int row, unsigned int col)
{
	switch (col) {
	case Col_EditableText:
	case Col_IconText:
		if (row >= m_textColValues.GetCount()) {
			// the item is not in the range of the items
			// which we store... for simplicity, don't allow editing it
			wxLogError("Cannot edit rows with an index greater than %d",
				m_textColValues.GetCount());
			return false;
		}

		if (col == Col_EditableText) {
			m_textColValues[row] = variant.GetString();
		} else // col == Col_IconText
		{
			wxDataViewIconText iconText;
			iconText << variant;
			m_iconColValues[row] = iconText.GetText();
		}
		return true;

	case Col_Date:
	case Col_TextWithAttr:
		wxLogError("Cannot edit the column %d", col);
		break;

	/*case Col_Custom:
		m_customColValues[row] = variant.GetString();
		break;*/

	case Col_Max:
		wxFAIL_MSG("invalid column");
	}

	return false;
}