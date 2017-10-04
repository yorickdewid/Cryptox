#pragma once

#include "Reader.h"

#define SOURCE_NAME "__MEMORY__"

static const std::string cText = R"C(
#include <stdio.h>
#include <stdlib.h>

int factor(int x) {
  return x == 0 ? 1 : x * factor(x-1);
}

int main(int argc, char *argv[]) {
  printf("teststring %d\n", factor(5));
  return 0;
}
)C";

class StringReader : public Reader
{
public:
	StringReader(const std::string& content)
	{
	}

	virtual std::string FetchNextChunk(size_t sizeHint)
	{
		auto part = cText.substr(offset, sizeHint);
		offset += part.size();

		return part;
	}

	virtual std::string FetchMetaInfo()
	{
		return SOURCE_NAME;
	}

private:
	size_t offset = 0;
};