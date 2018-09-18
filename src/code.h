#ifndef CODE_H_INCLUDED
#define CODE_H_INCLUDED
//#include "../libClient/business/getData.h"
#include <string>
//#include <cstring>
#ifdef _WIN32
#include <Windows.h>

#endif

#include "wx/intl.h"

using namespace std;
class CCodeConverter {
public:
	bool isIncludeChinese(const char* str);
	void UTF_8ToUnicode(wchar_t* pOut, char *pText);
	void UnicodeToUTF_8(char* pOut, wchar_t* pText);
	void UnicodeToGB2312(char* pOut, wchar_t uData);
	void Gb2312ToUnicode(wchar_t* pOut, const char *gbBuffer);
	int GB2312ToUTF_8(std::string& destbuf, const char *sourcebuf, int sourcelen);
	int UTF_8ToGB2312(char *destbuf, int destlen, char*sourcebuf, int sourcelen);
	//gb2312转utf8
	std::string GB2312ToUTF8(const std::string& srcGB2312);
	//utf8转gb2312
	std::string UTF8ToGb2312(const std::string strUtf8);
	std::string UTF8ToGB(const char* str);
	//gbk转gb2312
	std::string GBKToGb2312(const std::string strGbk);
	//gbk转UTF-8  传入的strGbk是GBK编码  
	std::string GbkToUtf8(const std::string& strGbk);
	//UTF-8转gbk 传入的strUtf8是UTF-8编码  
	std::string Utf8ToGbk(const std::string& strUtf8);
	//gbk转unicode 返回值是wstring  
	std::wstring GbkToUnicode(const std::string& strGbk);
	//Unicode转gbk  参数是wstring  
	std::string UnicodeToGbk(const std::wstring& strUnicode);
	//Unicode转GB18030
	std::string UnicodeToGB18030(const std::wstring& strUnicode);
	//Unicode转utf8
	std::string UnicodeToUtf8(const std::wstring& strUnicode);
	//GB18030转Unicode
	std::wstring GB18030ToUnicode(const std::string& strGb18030);
    // utf8 转 unicode
    std::wstring Utf8ToUnicode(const std::string& utf8);
    //UTF-8 转 指定的编码格式
    std::string Utf8Transcoding(const std::string& strUtf8, int fontEncoding);
    //指定的编码格式转utf-8
    std::string TranscodingUtf8(const std::string& str, int fontEncoding);
    std::string transcoding(const string& src, int srcCodePage, int destCodePage);
    std::string transCurrentcoding(const string& src, int codePage);
    std::string utf8ToCurrentcoding(const string& src, int currentFontEncoding);
    std::string utf8To850(const string& src);
    
#ifndef _WIN32
    string transcoingIconv(const string& srcCode,const string& destCode,const string& str);
    int code_convert(const char *from_charset,const char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t outlen);
#endif
};

bool isUtf8(const void* data, size_t size);

#ifdef _WIN32
string ansi2utf8(const string& ansi);

string utf82ansi(const string& utf8);

#endif

#endif // CODE_H_INCLUDED
