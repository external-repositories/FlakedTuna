/*
* Copyright (c) 2014 Clark Cianfarini
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include <algorithm>
#include "PlatformDefs.h"

namespace FlakedTuna
{

#if defined(_WIN32) || defined(WIN32)
/*******************************************************
* Win32 platform specific
*******************************************************/

	std::pair<std::vector<PLUG_HANDLE>, std::vector<std::pair<int, PluginRegistry*>>> GetPluginHandles(std::string directory, std::string extension)
	{
		std::vector<PLUG_HANDLE> plugHandles;
		registryVector regPointers;

		LPWIN32_FIND_DATAA findData = new WIN32_FIND_DATAA;
		BOOL f_ok = TRUE;

		if (directory.length() == 0)
		{
			directory.append(".");
		}

		if (directory.substr(directory.length() - 1, 1) != "\\")
		{
			directory.append("\\");
		}
		std::string search(directory);
		search.append("*");
		search.append(extension);

		HANDLE findHandle = FindFirstFileA(search.c_str(), findData);

		while ((findHandle != INVALID_HANDLE_VALUE) && (f_ok == TRUE))
		{
			std::string plugPath(directory);
			plugPath.append(findData->cFileName);

			PLUG_HANDLE handle = LoadLibraryA(plugPath.c_str());
			if (handle != NULL)
			{
				// Look for plugin registry in file
				RegFuncPtr PluginRegistryAddr = (RegFuncPtr) GetProcAddress(handle, "GetPluginRegistry");

				// Check if valid plugin
				if (PluginRegistryAddr != NULL)
				{
					// Now get the plugin
					PluginRegistry* pluginRegistry = PluginRegistryAddr();

					int pluginVersion = 0;
					VersionFuncPtr PluginFileVersion = (VersionFuncPtr) GetProcAddress(handle, "GetVersion");
					if (PluginFileVersion != NULL)
					{
						pluginVersion = PluginFileVersion();
					}

					// Save library/registry
					plugHandles.push_back(handle);
					regPointers.push_back(std::make_pair(pluginVersion, pluginRegistry));
				}
				else
				{
					FreeLibrary(handle);
				}
			}

			// Grab next file handle
			f_ok = FindNextFileA(findHandle, findData);
		}

		delete findData;
		return std::make_pair(plugHandles, regPointers);
	}

	void ClosePluginHandles(std::vector<PLUG_HANDLE> handles)
	{
		for (auto iter : handles)
		{
			CloseFuncPtr closeRegistry = (CloseFuncPtr) GetProcAddress(iter, "ClosePluginRegistry");
			if (closeRegistry != NULL)
			{
				closeRegistry();
			}
			FreeLibrary(iter);
		}
	}


#else
/*******************************************************
* *NIX platform specific
*******************************************************/
// TODO: DO THIS!

#endif

} // end namespace