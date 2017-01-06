#pragma once

#include <wx/wxprec.h>
#include <wx/dataview.h>

class SecretListModel : public wxDataViewVirtualListModel
{
public:
	enum
	{
		Col_EditableText,
		Col_IconText,
		Col_Date,
		Col_TextWithAttr,
		Col_Custom,
		Col_Max
	};

	SecretListModel();

	// helper methods to change the model
	void Prepend(const wxString &text);
	void DeleteItem(const wxDataViewItem &item);
	void DeleteItems(const wxDataViewItemArray &items);
	void AddMany();


	// implementation of base class virtuals to define model
	virtual unsigned int GetColumnCount() const wxOVERRIDE
	{
		return Col_Max;
	}

	virtual wxString GetColumnType(unsigned int col) const wxOVERRIDE
	{
		if (col == Col_IconText)
			return wxT("wxDataViewIconText");

		return wxT("string");
	}

	virtual void GetValueByRow(wxVariant &variant, unsigned int row, unsigned int col) const wxOVERRIDE;
	virtual bool GetAttrByRow(unsigned int row, unsigned int col, wxDataViewItemAttr &attr) const wxOVERRIDE;
	virtual bool SetValueByRow(const wxVariant &variant, unsigned int row, unsigned int col) wxOVERRIDE;

private:
	wxArrayString    m_textColValues;
	wxArrayString    m_iconColValues;
	//IntToStringMap   m_customColValues;
	//wxIcon           m_icon[2];
};

