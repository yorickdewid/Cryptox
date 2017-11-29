#include "ASTNode.h"

#include <iostream>

// Implement abstract class dtor
Decl::~Decl() {}
Expr::~Expr() {}
Stmt::~Stmt() {}

void ASTNode::Print(int level, bool last, std::vector<int> ignore) const
{
	if (level == 0) {
		std::cout << NodeName() << std::endl;
	}
	else {
		for (size_t i = 0; i < static_cast<size_t>(level) - 1; ++i) {
			if (std::find(ignore.begin(), ignore.end(), i) == ignore.end()) {
				std::cout << "| ";
			}
			else {
				std::cout << "  ";
			}
		}

		if (last) {
			ignore.push_back(level - 1);
			std::cout << "`-" << NodeName() << std::endl;
		}
		else {
			std::cout << "|-" << NodeName() << std::endl;
		}
	}

	for (auto& weakChild : children) {
		if (auto delegateChildren = weakChild.lock()) {
			delegateChildren->Print(level + 1, &weakChild == &children.back(), ignore);
		}
	}
}
