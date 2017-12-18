#include "ASTNode.h"

#include <iostream>

// Implement abstract class dtor
Operator::~Operator() {}
Literal::~Literal() {}
Decl::~Decl() {}
Expr::~Expr() {}
Stmt::~Stmt() {}

int UniqueObj::_id = 100;

void ASTNode::Print(int version, int level, bool last, std::vector<int> ignore) const
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

	const auto traverse = [=](const std::vector<std::weak_ptr<ASTNode>>& cList)
	{
		for (const auto& weakChild : cList) {
			if (auto delegateChildren = weakChild.lock()) {
				delegateChildren->Print(version, level + 1, &weakChild == &cList.back(), ignore);
			}
		}
	};

	// If original version was requested, print node version zero
	if (version == 0 && m_state.HasAlteration()) {
		const auto& nodeVersion = m_state.front();
		traverse(nodeVersion->children);
		return;
	}

	traverse(children);
}
