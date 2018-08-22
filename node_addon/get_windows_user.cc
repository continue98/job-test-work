#include <node.h>
#include <Windows.h>
#include <lm.h>
#include <unordered_set>
#include <iostream>
#include <locale>
#include <codecvt>

#pragma comment(lib, "netapi32.lib")


namespace winapi {
	int isUserExists(const std::wstring name)
	{
		std::unordered_set <std::wstring> user_name;
		LPUSER_INFO_0 pBuf = NULL;
		LPUSER_INFO_0 pTmpBuf;
		DWORD dwLevel = 0;
		DWORD dwPrefMaxLen = MAX_PREFERRED_LENGTH;
		DWORD dwEntriesRead = 0;
		DWORD dwTotalEntries = 0;
		DWORD dwResumeHandle = 0;
		DWORD i;
		DWORD dwTotalCount = 0;
		NET_API_STATUS nStatus;
		LPTSTR pszServerName = NULL;

		do
		{
			nStatus = NetUserEnum((LPCWSTR)pszServerName,
				dwLevel,
				FILTER_NORMAL_ACCOUNT, // global users
				(LPBYTE*)&pBuf,
				dwPrefMaxLen,
				&dwEntriesRead,
				&dwTotalEntries,
				&dwResumeHandle);

			if ((nStatus == NERR_Success) || (nStatus == ERROR_MORE_DATA))
			{
				if ((pTmpBuf = pBuf) != NULL)
				{
					for (i = 0; (i < dwEntriesRead); i++)
					{
						if (pTmpBuf == NULL)
						{
							break;
						}
						user_name.insert(pTmpBuf->usri0_name);
						pTmpBuf++;
						dwTotalCount++;
					}
				}
			}
			if (pBuf != NULL)
			{
				NetApiBufferFree(pBuf);
				pBuf = NULL;
			}
		} while (nStatus == ERROR_MORE_DATA);


		if (pBuf != NULL)
			NetApiBufferFree(pBuf);

		std::unordered_set<std::wstring>::const_iterator find_it = user_name.find(name);

		if (find_it != user_name.end())
		{
			return 1;
		}
		return 0;
	}
}
namespace addon {

	using v8::FunctionCallbackInfo;
	using v8::Isolate;
	using v8::Local;
	using v8::Object;
	using v8::String;
	using v8::Value;

	using v8::Exception;
	using v8::Number;

	void isUserExistsExport(const FunctionCallbackInfo<Value>& args)
	{
		Isolate* isolate = args.GetIsolate();

		if (args.Length() < 1) {
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, "Wrong number of arguments")));
			return;
		}

		if (!args[0]->IsString()) {
			isolate->ThrowException(Exception::TypeError(
				String::NewFromUtf8(isolate, "Wrong arguments")));
			return;
		}
		std::string name = *v8::String::Utf8Value(args[0]->ToString());

		// fix problem with russian user name
		using convert_type = std::codecvt_utf8<wchar_t>;
		std::wstring_convert<convert_type, wchar_t> converter;

		std::wstring ws = converter.from_bytes(name);

		args.GetReturnValue().Set(winapi::isUserExists(ws));
	}
	void init(Local <Object> exports) 
	{
		NODE_SET_METHOD(exports, "isExistsUserWindows", isUserExistsExport);
	}
	NODE_MODULE(addon, init)

}