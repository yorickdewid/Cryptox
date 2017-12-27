// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "pfbase.h"

#include <cry/config.h>

#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include <iostream>

#define PROGRAM_DESC PRODUCT_BRAND_NAME " " PROGRAM_DESCRIPTION "\n"

#if WIN32
#define COMMAND_LINE_DELIMITER "/"
#else
#define COMMAND_LINE_DELIMITER "-"
#endif // WIN32

namespace pb = ProjectBase;

void GetFiles(pb::Project& p)
{
	auto diastore = p.GetStore<pb::DiagramStore>("diagram");

	std::cout << "Diagram store has " << diastore->Size() << " files" << std::endl;

	pb::File &file = diastore->GetFile("book.dia");

	std::cout << "File name " << file.Name() << std::endl;
	std::cout << "File size " << file.Size() << std::endl;
	std::cout << "File content " << file.Data() << std::endl;

	diastore->DeleteFile("book.dia");
}

void PrintInfo(const std::string& filename)
{
	// Load project from disk
	try {
		auto p = ProjectBase::Project::LoadFile(filename);

		std::cout << "Project name:\t" << p->ProjectName() << std::endl;

		auto created = p->CreateTimestamp();
		auto updated = p->UpdateTimestamp();

		std::cout << "Created at:\t" << ::ctime(&created);
		std::cout << "Updated at:\t" << ::ctime(&updated);

		std::cout << "Author:\t\t" << p->Author() << std::endl;
		std::cout << "Stores:\t\t" << p->StoreCount() << std::endl;

		std::cout << " [*] diagram" << std::endl;

		auto diagramStore = p->GetStore<pb::DiagramStore>("diagram");

		for (auto& file : diagramStore->Inventory()) {
			std::cout << "   [-] " << file.Name() << " <" << file.Size() << "B>" << std::endl;
		}

		std::cout << " [*] material" << std::endl;

		auto materialStore = p->GetStore<pb::MaterialStore>("material");

		for (auto& pair : materialStore->Inventory()) {
			std::cout << "   [-] " << pair.Name() << " = " << pair.Algorithm() << " <" << pair.Size() << "B>" << std::endl;
		}
	}
	catch (std::runtime_error& e) {
		std::cerr << e.what() << std::endl;
	}
}

int main(int argc, const char *argv[])
{
	namespace po = boost::program_options;

	try
	{
		po::options_description desc{ PROGRAM_DESC PRODUCT_COPYRIGHT "\n\n" PROGRAM_ORIGINAL_NAME ": [OPTIONS] FILE ...\n\nOptions" };
		desc.add_options()
			("help", "Help screen")
			("upgrade", po::value<int>(), "Upgrade project to version")
			("input-file", po::value<std::string>(), "Input file");

		po::positional_options_description pd;
		pd.add("input-file", -1);

		po::command_line_parser parser{ argc, argv };
		parser.options(desc)
			.positional(pd)
			.style(po::command_line_style::default_style
				   | po::command_line_style::case_insensitive
				   | po::command_line_style::allow_slash_for_short
				   | po::command_line_style::allow_long_disguise)
			.allow_unregistered();

		po::variables_map vm;
		po::store(parser.run(), vm);

		// Print usage whenever there is an error or the help option is requested. The help
		// shows all sections except for the positional arugments. Based on the system defaults
		// the commandline arguments are displayed in system style.
		auto usage = [=]
		{
			std::stringstream ss;
			ss << desc;

			std::string helpMsg = ss.str();
			boost::algorithm::replace_all(helpMsg, "--", COMMAND_LINE_DELIMITER);
			std::cout << helpMsg << std::endl;
		};

		if (vm.count("help")) {
			std::cout << desc << std::endl;
		}
		else if (vm.count("upgrade")) {
			std::cout << "Upgrade project to : " << vm["upgrade"].as<int>() << std::endl;
		}
		else if (vm.count("input-file")) {
			if (!boost::filesystem::exists(vm["input-file"].as<std::string>())) {
				std::cerr << "Cannot find project file " << vm["input-file"].as<std::string>() << std::endl;
				return 1;
			}

			PrintInfo(vm["input-file"].as<std::string>());
		}
		else {
			usage();
			return 1;
		}
	}
	catch (const boost::program_options::error &e)
	{
		std::cerr << e.what() << std::endl;
		return 1;
	}

	return 0;
}
