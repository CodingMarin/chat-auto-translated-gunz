#ifndef _ZTRANSLATOR_H
#define _ZTRANSLATOR_H

#include <string>
#include <windows.h>
#include <wininet.h>

#pragma comment(lib, "wininet.lib")

class ZTranslator
{
public:
	ZTranslator();
	~ZTranslator();

	static bool IsTranslationRequest(const char* szMsg, std::string& fromLang, std::string& toLang, std::string& text);

	static bool TranslateWithAPI(const std::string& fromLang, const std::string& toLang,
		const std::string& text, std::string& outTranslated);

	static void FormatTranslatedMessage(const std::string& translated, char* outMsg, int maxLen);

private:
	static bool HttpGet(const std::string& url, std::string& response);
	static std::string UrlEncode(const std::string& str);
	static std::string ExtractTranslation(const std::string& jsonResponse);
	static std::string Utf8ToAnsi(const std::string& utf8);
};

#endif
