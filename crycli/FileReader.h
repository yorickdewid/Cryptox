#pragma once

#include "Reader.h"
#include "SourceUnit.h"

#include <boost/filesystem.hpp>

#include <stack>

class FileReader : public Reader
{
public:
	FileReader() = default;

	explicit FileReader(const std::string& filename)
	{
		AppendFileToList(filename);
	}

	~FileReader()
	{
		///
	}

	virtual std::string FetchNextChunk(size_t sizeHint)
	{
		auto content = m_unitList.top()->Read(sizeHint);
		if (content.empty()) {
			m_unitList.pop();

			if (!m_unitList.empty()) {
				return FetchNextChunk(sizeHint);
			}
		}

		return content;
	}

	virtual std::string FetchMetaInfo()
	{
		return m_unitList.top()->Name();
	}

	virtual void SwitchSource(const std::string& source)
	{
		AppendFileToList(source);
	}

protected:
	void AppendFileToList(const std::string& filename)
	{
		//XXX: Switch to C++17
		if (!boost::filesystem::exists(filename)) {
			throw std::system_error{ std::make_error_code(std::errc::no_such_file_or_directory) };
		}

		m_unitList.push(std::make_unique<SourceUnit>(SourceUnit(filename, false)));
	}

	template<typename _Ty>
	void AppendFileToList(_Ty&& unit)
	{
		m_unitList.push(std::make_unique<_Ty>(std::move(unit)));
	}

private:
	std::stack<std::unique_ptr<SourceUnit>> m_unitList;
};

