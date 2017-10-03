#pragma once

#include "Reader.h"

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
	virtual std::string FetchNextChunk(size_t sizeHint)
	{
		auto part = cText.substr(offset, sizeHint);
		offset += part.size();

		return part;
	}

private:
	size_t offset = 0;
};