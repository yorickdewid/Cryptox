// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#ifndef DATA_H_
#define DATA_H_

#if defined(_MSC_VER)
#pragma once
#endif

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

	// C compatible string structure.
	typedef struct
	{
		// Size of the data chunk.
		unsigned int size;

		// Pointer to data location.
		const char *ptr;

		// Boolean indicating ptr memory block should be freed by the callee.
		int unmanaged_res;

		// Function pointer to be called when the object is freed by the callee.
		void(*deallocVPtr)(void *);
	} datachunk_t;

	// Release internal memory pointer if requested.
	inline void datachunk_internal_release(datachunk_t *item)
	{
		if (!item) { return; }
		if (item->unmanaged_res && item->ptr) {
			void *ptr = (void *)item->ptr;
			if (item->deallocVPtr) {
				item->deallocVPtr(ptr);
			}
			else { free(ptr); }
			item->size = 0;
			item->ptr = NULL;
			item->unmanaged_res = 0;
			item->deallocVPtr = NULL;
		}
	}

	//
	// Datalist structure.
	//

	// Linked list of data chunks.
	typedef datachunk_t** datalist_t;

	// Create new datalist.
	inline datalist_t datalist_new(size_t size)
	{
		datalist_t list = (datalist_t)malloc((size + 1) * sizeof(datachunk_t *));
		list[size] = NULL;
		return list;
	}

	// Create a new datalist item.
	inline datachunk_t *datalist_item_new()
	{
		datachunk_t *item = (datachunk_t *)malloc(sizeof(datachunk_t));
		item->size = 0;
		item->ptr = NULL;
		item->unmanaged_res = 0;
		item->deallocVPtr = NULL;
		return item;
	}

	// Get the number of items in the list.
	inline size_t datalist_size(datalist_t list)
	{
		if (!list) { return 0; }
		size_t count = 0;
		for (size_t i = 0; list[i]; ++i) {
			count++;
		}
		return count;
	}

	// Cleanup datalist and free resources.
	inline void datalist_release(datalist_t list)
	{
		if (!list) { return; }
		for (size_t i = 0; list[i]; ++i) {
			datachunk_internal_release(list[i]);
			free((void*)list[i]);
		}
		free((void*)list);
	}

#ifdef __cplusplus
}
#endif

#endif // DATA_H_
