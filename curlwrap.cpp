#ifdef _WINDOWS
#	define WIN32_MEAN_AND_LEAN
#	include <winsock2.h>
#	include <Windows.h>
#	include <stdint.h>
#	include <map>
#	undef min
#	undef max
#	define MAP_CLASSNAME std::map
#	define strlwr _strlwr
#else
#	include <signal.h>
#	include <unistd.h>
#	include <fcntl.h>
#	include <sys/types.h>
#	include <sys/socket.h>
#	include <sys/shm.h>
#	include <arpa/inet.h>
#	include <netinet/in.h>
#	include <unordered_map>>
#	define MAP_CLASSNAME std::unordered_map
static inline char* strlwr(char* str)
{
	uint8_t ch;
	char* p = str;
	while ((ch = p[0]) != 0)
	{
		if (ch >= 'A' && ch <= 'Z')
			p[0] = ch - 'A' + 'a';
		p ++;
	}
	return str;
}
#endif

#include <algorithm>
#include <string>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "curlwrap.h"

#include <lua.hpp>

#pragma comment(lib, "lua51")

#define CUDATA_INDEX -1000

#define ENTRY_CHECK()\
	luaL_checktype(L, 1, LUA_TTABLE);\
	lua_rawgeti(L, 1, CUDATA_INDEX);\
	CURLUserData* cu = (CURLUserData*)lua_touserdata(L, -1);\
	if (!cu) {\
		luaL_error(L, "CURL core data was released, please call curl.new() to create a new one", 0);\
		return 0;\
	}

#define CHECK_ENCTYPE(newType)\
	if (cu->kCntType != kContentNotSet && cu->kCntType != newType)\
	{\
		luaL_error(L, "HTTP data mode cannot change", 0);\
		return 0;\
	}


static char url_codes[256][4] = { 0 };
static void init_urlcodes()
{
	if (url_codes['%'][0] == 0)
	{
		//第一次，所以需要初始化
		url_codes['%'][3] = 1;
		url_codes['&'][3] = 1;
		url_codes[' '][3] = 1;
		url_codes['='][3] = 1;
		url_codes['/'][3] = 1;
		url_codes['#'][3] = 1;
		url_codes['+'][3] = 1;
		url_codes['\\'][3] = 1;
		url_codes[':'][3] = 1;
		url_codes[';'][3] = 1;
		url_codes['/'][3] = 1;

		for (uint32_t i = 1; i < 256; ++i)
		{
			uint8_t* p = (uint8_t*)url_codes[i];
			p[0] = '%';

			p[1] = i >> 4;
			if (p[1] > 9)
				p[1] = p[1] - 10 + 'a';
			else
				p[1] += '0';

			p[2] = i & 0xF;
			if (p[2] > 9)
				p[2] = p[2] - 10 + 'a';
			else
				p[2] += '0';
		}
	}
}

size_t urlEncode(const char* str, uint32_t leng, std::string& outbuf)
{
	init_urlcodes();

	char chars[3];
	uint8_t* pstr = (uint8_t*)str;
	for (uint32_t i = 0, l; i < leng; )
	{
		uint8_t ch = pstr[i];
		if (!(ch & 0x80))
		{
			//1 ascii
			outbuf.append(url_codes[ch], 3);
			++ i;
		}
		else
		{
			if ((ch & 0xE0) == 0xC0)
				l = 2;
			else if ((ch & 0xF0) == 0xE0)
				l = 3;
			else if ((ch & 0xF8) == 0xF0)
				l = 4;
			else if ((ch & 0xFC) == 0xF8)
				l = 5;
			else
				l = 6;

			while (l -- > 0)
			{
				const char* src = (const char*)url_codes[pstr[i ++]];
				outbuf.append(src, 3);
			}
		}
	}

	return outbuf.length();
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static int luacurl_new(lua_State* L)
{
	lua_newtable(L);
	luaL_newmetatable(L, "CURL");
	lua_setmetatable(L, -2);

	CURLUserData* cu = (CURLUserData*)lua_newuserdata(L, sizeof(CURLUserData));
	new (cu) CURLUserData();
	lua_rawseti(L, -2, CUDATA_INDEX);

	cu->curl = curlInitOne(cu);

	return 1;
}

static int luacurl_addheader(lua_State* L)
{
	ENTRY_CHECK();

	size_t len = 0, len2 = 0;
	const char* name = luaL_checklstring(L, 2, &len);
	const char* val = lua_tolstring(L, 3, &len2);

	if (!val)
	{
		cu->appendHeaders = p_curl_slist_append(cu->appendHeaders, name);
	}
	else
	{
		std::string text;

		text.append(name, len);
		text += ' ';
		text.append(val, len2);

		cu->appendHeaders = p_curl_slist_append(cu->appendHeaders, text.c_str());
	}

	return 0;
}

static int luacurl_addfile(lua_State* L)
{
	ENTRY_CHECK();

	const char* varname = luaL_checkstring(L, 2);
	const char* filename = luaL_checkstring(L, 3);

	CHECK_ENCTYPE(kContentMultiEncData);

	cu->kCntType = kContentMultiEncData;
	if (curlAddFileContent(cu, filename, NULL, 0, varname))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);

	return 1;
}

static int luacurl_addvar(lua_State* L)
{
	ENTRY_CHECK();

	size_t len = 0;
	const char* varname = luaL_checkstring(L, 2);
	const char* value = luaL_checklstring(L, 3, &len);

	CHECK_ENCTYPE(kContentMultiEncData);

	cu->kCntType = kContentMultiEncData;
	if (curlAddFileContent(cu, NULL, value, len, varname))
		lua_pushboolean(L, 1);
	else
		lua_pushboolean(L, 0);

	return 1;
}

static int luacurl_setpost(lua_State* L)
{
	ENTRY_CHECK();

	size_t len = 0;
	const char* posts = luaL_checklstring(L, 2, &len);

	CHECK_ENCTYPE(kContentURLEncoded);

	cu->strUpload.clear();
	len = urlEncode(posts, len, cu->strUpload);
	curlSetPostData(cu->curl, cu->strUpload.c_str(), len);

	lua_pushinteger(L, len);
	return len;
}

static int luacurl_settext(lua_State* L)
{
	ENTRY_CHECK();

	size_t len = 0;
	const char* posts = luaL_checklstring(L, 2, &len);

	CHECK_ENCTYPE(kContentText);

	curlSetPostData(cu->curl, posts, len);

	lua_pushinteger(L, len);
	return len;
}

static int luacurl_setjson(lua_State* L)
{
	ENTRY_CHECK();

	size_t len = 0;
	const char* posts = luaL_checklstring(L, 2, &len);

	CHECK_ENCTYPE(kContentJson);

	curlSetPostData(cu->curl, posts, len);

	lua_pushinteger(L, len);
	return len;
}

static int luacurl_run(lua_State* L)
{
	ENTRY_CHECK();

	const char* url = luaL_checkstring(L, 2);
	int keepAfterRun = lua_toboolean(L, 3);

	if (!curlSetUrl(cu->curl, url))
	{
		luaL_error(L, "illegal url: %s", url);
		return 0;
	}

	switch (cu->kCntType)
	{
	case kContentURLEncoded:
		cu->appendHeaders = p_curl_slist_append(cu->appendHeaders, "Content-Type: application/x-www-form-urlencoded");
		break;
	case kContentMultiEncData:
		break;
	case kContentText:
		cu->appendHeaders = p_curl_slist_append(cu->appendHeaders, "Content-Type: text/plain");
		break;
	case kContentJson:
		cu->appendHeaders = p_curl_slist_append(cu->appendHeaders, "Content-Type: application/json");
		break;
	}

	CURLcode res = curlExecOne(cu);
	if (res != CURLE_OK)
	{
		// 请求失败返回错误信息字符串
		const char* err = p_curl_easy_strerror(res);
		lua_pushboolean(L, 0);
		lua_pushstring(L, err);
	}
	else
	{
		// 请求成功返回一个Table
		lua_pushboolean(L, 1);
		lua_newtable(L);

		// status
		long httpStatus = 0;
		p_curl_easy_getinfo(cu->curl, CURLINFO_RESPONSE_CODE, &httpStatus);

		lua_pushlstring(L, "status", 6);
		lua_pushinteger(L, httpStatus);
		lua_rawset(L, -3);

		// response text
		lua_pushlstring(L, "response", 8);
		lua_pushlstring(L, cu->strResponse.c_str(), cu->strResponse.length());
		lua_rawset(L, -3);
	}

	if (keepAfterRun)
	{
		cu->strResponse.clear();
		cu->strUpload.clear();
		if (cu->formpost)
		{
			p_curl_formfree(cu->formpost);
			cu->formpost = NULL;
		}
	}
	else
	{
		cu->~CURLUserData();

		lua_pushnil(L);
		lua_rawseti(L, 1, CUDATA_INDEX);
	}

	return 2;
}

static int luacurl_release(lua_State* L)
{
	ENTRY_CHECK();

	cu->~CURLUserData();

	lua_pushnil(L);
	lua_rawseti(L, 1, CUDATA_INDEX);

	return 0;
}

#ifdef _WINDOWS
struct InitLib
{
	HMODULE	hLib;
	InitLib()
	{
		hLib = curlLibInit();
	}
	~InitLib()
	{
		if (hLib)
			curlLibUnload(hLib);
	}
} _g_InitLib;

extern "C" __declspec(dllexport) int luaopen_curlwrap(lua_State* L)
#else
extern "C" __declspec(dllexport) int luaopen_libcurlwrap(lua_State* L)
#endif
{
	int top = lua_gettop(L);

	const luaL_Reg gblProcs[] = {
		// 字符串切分
		{ "new", &luacurl_new },
		{ NULL }
	} ;

	const luaL_Reg memProcs[] = {
		{ "addfile", &luacurl_addfile },
		{ "addvar", &luacurl_addvar },
		{ "setpost", &luacurl_setpost },
		{ "addheader", &luacurl_addheader },
		{ "run", &luacurl_run },
		{ "release", &luacurl_release },
		{ NULL }
	} ;

	luaL_register(L, "CURL", gblProcs);

	luaL_newmetatable(L, "CURL");
		lua_pushlstring(L, "__index", 7);
		lua_newtable(L);
		luaL_register(L, NULL, memProcs);
		lua_rawset(L, -3);

	lua_settop(L, top);
	return 0;
}