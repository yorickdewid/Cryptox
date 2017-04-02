#pragma once

#include <wx/wxprec.h>

class wxOutputConsoleCtrl : public wxTextCtrl
{
public:
	wxOutputConsoleCtrl(wxWindow *parent, wxWindowID id = wxID_ANY,
						const wxString& value = wxEmptyString,
						const wxPoint& pos = wxDefaultPosition,
						const wxSize& size = wxDefaultSize)
		: wxTextCtrl(parent, id, wxEmptyString, pos, size, wxTE_READONLY | wxNO_BORDER | wxTE_MULTILINE)
	{
		WriteConsole(value);
	}

	template <typename T>
	void WriteConsole(T input)
	{
		wxString buffer;
		buffer << "[";
		buffer << GetTimestamp();
		buffer << "] ";
		buffer << input;
		buffer << '\n';

		wxTextCtrl::WriteText(buffer);
	}

	virtual void WriteText(const wxString& text) wxOVERRIDE
	{
		WriteConsole(text);
	}

	virtual void AppendText(const wxString& text) wxOVERRIDE
	{
		WriteConsole(text);
	}

	wxOutputConsoleCtrl& operator<<(const wxString& s)
	{
		WriteConsole(s); return *this;
	}
	wxOutputConsoleCtrl& operator<<(int i)
	{
		WriteConsole(i); return *this;
	}
	wxOutputConsoleCtrl& operator<<(long i)
	{
		WriteConsole(i); return *this;
	}
	wxOutputConsoleCtrl& operator<<(float f)
	{
		return *this << double(f);
	}
	wxOutputConsoleCtrl& operator<<(double d)
	{
		WriteConsole(d); return *this;
	}
	wxOutputConsoleCtrl& operator<<(char c)
	{
		return *this << wxString(c);
	}
	wxOutputConsoleCtrl& operator<<(wchar_t c)
	{
		return *this << wxString(c);
	}

protected:
	wxString GetTimestamp() const
	{
		wxDateTime timestamp(wxDateTime::Now());

		return timestamp.FormatISOTime();
	}
};
