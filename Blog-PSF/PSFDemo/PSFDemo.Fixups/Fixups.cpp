//-------------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft Corporation. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//-------------------------------------------------------------------------------------------------------
//
// Fixups.cpp : Defines the PackageSupportFramework fixup behavior for the PSFSample Centennial app
//

#include "stdafx.h"
#define PSF_DEFINE_EXPORTS
#include <psf_framework.h>
#include <winrt/Windows.Foundation.h>
#include <string_view>

using namespace std::literals;

std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}

// Intercept and customize MessageBox calls
auto MessageBoxWImpl = &::MessageBoxW;
int WINAPI MessageBoxWShim(
	_In_opt_ HWND hwnd,
	_In_opt_ LPCWSTR message,
	_In_opt_ LPCWSTR /*caption*/,
	_In_ UINT type)
{

	if (auto configRoot = ::PSFQueryCurrentDllConfig())
	{
		auto& config = configRoot->as_object();

		if (auto titleValue = config.try_get("title"))
		{
			//parse the title
			auto title = titleValue->as_string().wstring();
			std::string myTitle = winrt::to_string(title);

			std::wstring temp = s2ws(myTitle);
			LPCWSTR result = temp.c_str();
			return MessageBoxWImpl(hwnd, message, result, type);
		}
	}

	return MessageBoxWImpl(hwnd, message, L"Package Support Framework", type);
}
DECLARE_FIXUP(MessageBoxWImpl, MessageBoxWShim);

