// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#ifdef __cplusplus
# error "Source must be compiled as C"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <coilcl.h>

#define PROG_VERSION 1

/* Check if file exist on disk */
int file_exist(const char *filename)
{
	FILE *file = NULL;
	if ((file = fopen(filename, "r")) == NULL) {
		return errno;
	}
	else {
		fclose(file);
	}
	return 0;
}

/* This callback is invoked when the backend encounters an error */
static void error_handler(void *user_data, const char *message, char fatal)
{
	assert(user_data == NULL);
	if (!fatal) {
		fprintf(stderr, "Compiler warning: %s\n", message);
		return;
	}
	fprintf(stderr, "Compiler error: %s\n", message);
}

static datachunk_t *get_next_source_buffer(void *user_data)
{
	datachunk_t *buffer = (datachunk_t*)malloc(sizeof(datachunk_t));
	assert(user_data == NULL);

	buffer->size = 4;
	buffer->ptr = "main";
	buffer->unmanaged_res = 0; // Backend must not free our structure

	return buffer;
}

/* Load source unit as requested by compiler. This
 * example does not support external source includes.
 */
int load_source(void *user_data, const char *source)
{
	assert(user_data == NULL);
	return 0;
}

/* Communicate some meta info about the source to the compiler */
metainfo_t *source_info(void *user_data)
{
	metainfo_t *meta_info = (metainfo_t*)malloc(sizeof(metainfo_t));
	assert(user_data == NULL);

	//meta_info->

	/*StreamReaderAdapter &adapter = side_cast<StreamReaderAdapter>(user_data);
	auto str = adapter.FetchMetaInfo();

	auto metablock = new metainfo_t;
	std::copy(str.begin(), str.end(), metablock->name);
	metablock->name[str.size()] = '\0';
	metablock->name[sizeof(metainfo_t::name) - 1] = '\0';*/

	return meta_info;
}

void run_source_file(const char *file) {
	compiler_info_t info;

	if (file_exist(file)) {
		fprintf(stderr, "Cannot open source file %s\n", file);
		return;
	}

	// Supply compiler information
	info.apiVer = COILCLAPIVER;
	info.code_opt.standard = c99;
	info.code_opt.optimization = NONE;
	info.streamReaderVPtr = &get_next_source_buffer;
	info.loadStreamRequestVPtr = &load_source;
	info.streamMetaVPtr = &source_info;
	info.errorHandler = &error_handler;
	info.program.program_ptr = NULL;
	info.user_data = NULL;
	Compile(&info);
	assert(info.user_data == NULL);

	// Did the compiler fail
	if (info.program.program_ptr == NULL) {
		fprintf(stderr, "Invalid backend call\n");
	}
}

/* Print usage information on screen */
static int usage(const char *prognam) {
	printf("Usage: %s [FILE]\n", prognam);
	printf(" -o FILE   Output file");
	printf(" -v        Show version");
	return 1;
}

int main(int argc, char *argv[]) {
	if (argc < 2)
		return usage(argv[0]);

	// Check if version was requested
	if (!strcmp(argv[1], "-v") || !strcmp(argv[1], "-V")) {
		printf("Version %d\n", PROG_VERSION);
		return 0;
	}
	else if (!strcmp(argv[1], "-O") || !strcmp(argv[1], "-O")) {
		//
	}

	const char *file = argv[1];
	run_source_file(file);

	return 0;
}
