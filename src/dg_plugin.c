/*********************************************************************/
/*                                                                   */
/* Copyright (C) 2006,  AVIQ Systems AG                              */
/*                                                                   */
/* Project:       dg                                                 */
/* Filename:      dg_plugin.c                                        */
/* Description:   DG Plugin API implementation                       */
/*                                                                   */
/*********************************************************************/

#include <dg/dg_plugin.h>

#ifdef _WIN32
#include <windows.h>
#define LOAD_LIB(libname) LoadLibrary(libname)
#define GET_FUNCTION(hlib, fname) GetProcAddress(hlib, fname)
#define FREE_LIB(hlib) FreeLibrary(hlib)	
#else
#include <dlfcn.h>
#define LOAD_LIB(libname) dlopen(libname, RTLD_NOW)
#define GET_FUNCTION(hlib, fname) dlsym(hlib, fname)
#define FREE_LIB(hlib) dlclose(hlib)
#endif

const char* plugin_init_function="dg_plugin_init";

static dg_plugin_p plugins=0;

void dg_plugin_register(dg_plugin_init_t plugin_init, dg_plugin_info_p plugin_info)
{
	dg_plugin_p plugin = (dg_plugin_p)malloc(sizeof(dg_plugin_t));
	plugin->next=plugins;
	plugin->plugin_info = (dg_plugin_info_p)malloc(sizeof(dg_plugin_info_t));
	plugin->plugin_init = plugin_init;
	memcpy(plugin->plugin_info, plugin_info, sizeof(dg_plugin_info_t));
	plugins=plugin;
}

void dg_plugin_destroy( void )
{
	dg_plugin_p plugin = plugins;
	while (plugin)
	{
		dg_plugin_p p = plugin;
		plugin->plugin_init(plugin->plugin_info, 0);
		free(plugin->plugin_info);
		plugin=plugin->next;
		free(p);
	}
	plugins=0;
}

int dg_plugin_load(const char* library)
{
	void *lib=LOAD_LIB(library);
	if (lib)
	{
		dg_plugin_init_t init=(dg_plugin_init_t) GET_FUNCTION(lib, plugin_init_function);
		if (init)
		{
			dg_plugin_info_t plugin_info;
			memset(&plugin_info, 0, sizeof(dg_plugin_info_t));
			if (init(&plugin_info, 1))
			{
				dg_plugin_register(init, &plugin_info);
			}
			return 1;
		}
		else
		{
			FREE_LIB(lib);
		}
	}
	return 0;
}

int dg_plugin_enum(int request_mask, plugin_enum_callback_t plugin_enum_callback, void* param)
{
	dg_plugin_p plugin = plugins;
	while (plugin)
	{
		if (plugin->plugin_info->provide_mask & request_mask)
		{
			if (plugin_enum_callback(param, plugin->plugin_info))
			{
				return 1;
			}
		}
		plugin=plugin->next;
	}
	return 0;
}
