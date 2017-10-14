#include "AST.h"

#include <iostream>

void ASTNode::Print(int level, bool last)
{
	if (level == 0) {
		std::cout << NodeName() << std::endl;
	}
	else {
		for (size_t i = 0; i < static_cast<size_t>(level) - 1; ++i) {
			std::cout << "| ";
		}

		if (last) {
			std::cout << "`-" << NodeName() << std::endl;
		}
		else {
			std::cout << "|-" << NodeName() << std::endl;
		}
	}

	for (auto& weakChild : children) {
		if (auto delegateChildren = weakChild.lock()) {
			delegateChildren->Print(level + 1, &weakChild == &children.back());
		}
	}
}
