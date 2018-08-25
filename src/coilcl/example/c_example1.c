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

#include <CoilCl/coilcl.h>

#define PROG_VERSION 1

struct file_data
{
	FILE *file_handle;
	const char *filename;
};

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

void file_write(datachunk_t *data, const char *out)
{
	FILE *file = NULL;
	if ((file = fopen(out, "wb")) == NULL) {
		fprintf(stderr, "Error opening file: %d\n", errno);
	}

	fwrite(data->ptr, sizeof(char), data->size, file);
	if (data->unmanaged_res) {
		free((void *)data->ptr);
	}

	fclose(file);
}

/* This callback is invoked when the backend encounters an error */
static void error_handler(void *user_data, const char *message, int fatal)
{
	((void*)user_data);

	if (!fatal) {
		fprintf(stderr, "Compiler warning: %s\n", message);
		return;
	}
	fprintf(stderr, "Compiler error: %s\n", message);
}

static datachunk_t *get_next_source_buffer(void *user_data)
{
	datachunk_t *buffer = (datachunk_t*)malloc(sizeof(datachunk_t));

	FILE *handle = ((struct file_data *)user_data)->file_handle;
	char *chunk_buffer = (char *)calloc(100, 1);
	size_t size = fread(chunk_buffer, 100, 1, handle);

	buffer->size = (unsigned int)(size * 100);
	buffer->ptr = chunk_buffer;
	buffer->unmanaged_res = 1;

	return buffer;
}

/* Load source unit as requested by compiler. This
 * example does not support external source includes.
 */
int load_source(void *user_data, const char *source)
{
	((void*)user_data);
	((void*)source);
	return 0;
}

/* Communicate some meta info about the source to the compiler */
metainfo_t *source_info(void *user_data)
{
	metainfo_t *meta_info = (metainfo_t*)malloc(sizeof(metainfo_t));

	const char *file = ((struct file_data *)user_data)->filename;
	strcpy(meta_info->name, file);
	meta_info->size = sizeof(file);

	return meta_info;
}

/*  */
void run_source_file(const char *file, const char *out) {
	compiler_info_t info;

	if (file_exist(file)) {
		fprintf(stderr, "Cannot open source file %s\n", file);
		return;
	}

	struct file_data *ptr = (struct file_data*)malloc(sizeof(struct file_data));
	ptr->filename = file;
	ptr->file_handle = fopen(file, "r");
	if (!ptr->file_handle) {
		fprintf(stderr, "Cannot open source file %s\n", file);
		return;
	}

	// Supply compiler information
	info.api_ref = COILCLAPIVER;
	info.code_opt.standard = c99;
	info.code_opt.optimization = NONE;
	info.streamReaderVPtr = &get_next_source_buffer;
	info.loadStreamRequestVPtr = &load_source;
	info.streamMetaVPtr = &source_info;
	info.error_handler = &error_handler;
	info.program.program_ptr = NULL;
	info.user_data = ptr;
	Compile(&info);

	// Did the compiler fail
	if (info.program.program_ptr == NULL) {
		fprintf(stderr, "Invalid backend call\n");
	}

	result_t result;
	result.program = info.program;
	result.content.ptr = NULL;
	result.content.size = 0;
	result.content.unmanaged_res = 0;
	result.tag = AIIPX;
	GetResultSection(&result);

	if (!out) {
		out = "a.out";
	}

	if (result.content.ptr) {
		file_write(&result.content, out);
	}

	fclose(((struct file_data *)info.user_data)->file_handle);
}

/* Print usage information on screen */
static int usage(const char *prognam) {
	printf("Usage: %s [FILE]\n", prognam);
	printf(" -o FILE   Output file");
	printf(" -v        Show version");
	return 1;
}

int main(int argc, char *argv[]) {
	int fileidx = 1;
	const char *out = NULL;

	if (argc < 2)
		return usage(argv[0]);

	// Check if version was requested
	if (!strcmp(argv[1], "-v") || !strcmp(argv[1], "-V")) {
		printf("Version %d\n", PROG_VERSION);
		return 0;
	}
	else if (!strcmp(argv[1], "-O") || !strcmp(argv[1], "-O")) {
		if (argc < 4)
			return usage(argv[0]);

		out = argv[2];
		fileidx += 2;
	}

	const char *file = argv[fileidx];
	run_source_file(file, out);

	return 0;
}
