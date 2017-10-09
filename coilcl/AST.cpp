#include "AST.h"

#include <iostream>

void ASTNode::Print(int level)
{
	if (level == 0) {
		std::cout << NodeName() << std::endl;
	}
	else {
		for (size_t i = 0; i < static_cast<size_t>(level) - 1; ++i) {
			std::cout << "| ";
		}

		std::cout << "|-" << NodeName() << std::endl;
	}

	for (auto weakChild : children) {
		if (auto delegateChildren = weakChild.lock()) {
			delegateChildren->Print(++level);
		}
	}
}
