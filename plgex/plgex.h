// Cryptox Plugin Interface
// Copyright (C) 2017, Quenza Inc.

#ifndef CRY_PLGEX_H
#define CRY_PLGEX_H

#ifdef _WIN32
# define PLGEX_API __declspec(dllexport)
#else
# define PLGEX_API
#endif

#ifdef __cplusplus
extern "C" {
#endif

	extern PLGEX_API int plugin_base();

	// Plugin details structure that's exposed to the application.
	struct plugin_info {
		int api_version;
		const char *file_name;
		const char *class_name;
		const char *plugin_name;
		const char *plugin_version;
	};

	PLGEX_API int fnplgex(void);

#ifdef __cplusplus
}
#endif

#endif // CRY_PLGEX_H