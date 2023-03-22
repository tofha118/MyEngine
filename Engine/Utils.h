#pragma once

#include <string>


class Utils
{
public:
	static vector<string> Split(const string& str, char base)
	{
		vector<string> str_Vec;
		int32 size = static_cast<int32>(str.size());
		string strTemp = "";
		for (int32 i = 0; i < size; ++i) {
			if (str[i] == base) {
				str_Vec.push_back(strTemp);
				strTemp = "";
			}
			else {
				strTemp.push_back(str[i]);
			}
		}

		if (strTemp.empty() == false) {
			str_Vec.push_back(strTemp);
		}

		return str_Vec;
	}

	static wstring StrToWstr(const string& str)
	{
		int32 len;
		int32 slength = static_cast<int32>(str.length()) + 1;
		len = ::MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, 0, 0);
		wchar_t* buf = new wchar_t[len];
		::MultiByteToWideChar(CP_ACP, 0, str.c_str(), slength, buf, len);
		wstring ret(buf);
		delete[] buf;
		return ret;
	}
	static string WstrToStr(const wstring& str)
	{
		int32 len;
		int32 slength = static_cast<int32>(str.length()) + 1;
		len = ::WideCharToMultiByte(CP_ACP, 0, str.c_str(), slength, 0, 0, 0, 0);
		string r(len, '\0');
		::WideCharToMultiByte(CP_ACP, 0, str.c_str(), slength, &r[0], len, 0, 0);
		return r;
	}
};

