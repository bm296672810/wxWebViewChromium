#include <string>
#ifdef _WIN32
#include <windows.h>
#include <winnls.h>
#else
#include <iconv.h>
#endif // _WIN32
#include "code.h"
using namespace std;

#ifdef _WIN32
int _iFontEncoding = wxFONTENCODING_CP936;
#endif

//using namespace xiniu::business;

bool CCodeConverter::isIncludeChinese(const char* str)
{
	int nRet = false;
	char c = *str;
	while (c) {
        str++;
		//如果字符高位为1且下一字符高位也是1则有中文字符
		if ((c & 0x80) && (*str & 0x80)) {
			nRet = true;
			break;
		}
        c = *str;
	}

    if (nRet)
    {
        return true;
    }
    else
    {
        return false;
    }
}

void CCodeConverter::UTF_8ToUnicode(wchar_t* sourcebuf, char *destbuf)
{
	char* uchar = (char *)sourcebuf;
	uchar[1] = ((destbuf[0] & 0x0F) << 4) + ((destbuf[1] >> 2) & 0x0F);
	uchar[0] = ((destbuf[1] & 0x03) << 6) + (destbuf[2] & 0x3F);
}
void CCodeConverter::UnicodeToUTF_8(char* sourcebuf, wchar_t* pIn)
{
	// 注意 WCHAR高低字的顺序,低字节在前，高字节在后
	char* pchar = (char *)pIn;
	sourcebuf[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
	sourcebuf[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[0] & 0xC0) >> 6);
	sourcebuf[2] = (0x80 | (pchar[0] & 0x3F));
}

void CCodeConverter::UnicodeToGB2312(char* sourcebuf, wchar_t uData)
{
#if defined(_WIN32)
	::WideCharToMultiByte(CP_ACP, NULL, &uData, 1, sourcebuf, sizeof(wchar_t), NULL, NULL);
#endif
}

void CCodeConverter::Gb2312ToUnicode(wchar_t* sourcebuf, const char *gbBuffer)
{
#if defined(_WIN32)
	::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, gbBuffer, 2, sourcebuf, 1);
#endif
}

int CCodeConverter::GB2312ToUTF_8(string& destbuf, const char *sourcebuf, int sourcelen)
{
#if defined(_WIN32)
	char buf[4] = { 0 };
	int nLength = sourcelen * 3;
	char* rst = new char[nLength];
	memset(rst, 0, nLength);
	int i = 0, j = 0;
	while (i < sourcelen)
	{
		//如果是英文直接复制就可以
		if (*(sourcebuf + i) >= 0)
		{
			rst[j++] = sourcebuf[i++];
		}
		else
		{
			wchar_t pbuffer;
			Gb2312ToUnicode(&pbuffer, sourcebuf + i);
			UnicodeToUTF_8(buf, &pbuffer);
			rst[j] = buf[0];
			rst[j + 1] = buf[1];
			rst[j + 2] = buf[2];
			j += 3;
			i += 2;
		}
	}
	rst[j] = '\0';   //返回结果
	destbuf = rst;
	delete[]rst;
#else
    destbuf = transcoingIconv("gbk","utf-8",string(sourcebuf,sourcelen));
#endif
	return 0;
}
//gb2312转utf8
std::string CCodeConverter::GB2312ToUTF8(const std::string& srcGB2312)
{
	string destBuf;
	int srcLen = srcGB2312.size();

	char* sourcebuf = new char[srcLen + 1];
    memset(sourcebuf, 0, srcLen + 1);
    memcpy(sourcebuf, srcGB2312.c_str(), srcLen);
    
#ifdef _WIN32
    GB2312ToUTF_8(destBuf, sourcebuf, srcLen);
	return destBuf;
#else
    return transcoingIconv("GBK","UTF-8", srcGB2312);
#endif
}
int CCodeConverter::UTF_8ToGB2312(char* destbuf, int destlen, char* sourcebuf, int sourcelen)
{
#if defined(_WIN32)
    destlen;
    sourcebuf;
    sourcelen;
	char Ctemp[4];
	memset(Ctemp, 0, 4);
	int i = 0, j = 0;
	while (i < sourcelen)
	{
		if (sourcebuf[i] >= 0)
		{
			destbuf[j++] = sourcebuf[i++];
		}
		else
		{
			WCHAR Wtemp;
			UTF_8ToUnicode(&Wtemp, sourcebuf + i);
			UnicodeToGB2312(Ctemp, Wtemp);
			destbuf[j] = Ctemp[0];
			destbuf[j + 1] = Ctemp[1];
			i += 3;
			j += 2;
		}
	}
	destbuf[j] = '\0';
#else
	iconv_t cd;

	if ((cd = iconv_open("gb2312", "utf-8")) == 0) {
		printf("iconv_open fail\n");
		return -1;
	}
	memset(destbuf, 0, destlen);
	char **source = &sourcebuf;
	char **dest = &destbuf;
	if (-1 == iconv(cd, source, (size_t*)&sourcelen, dest, (size_t*)&destlen)){
		printf("iconv fail\n");
		return -1;
	}
	iconv_close(cd);
#endif
	return 0;
}
std::string CCodeConverter::UTF8ToGB(const char* str)
{
    string result;
#ifdef _WIN32
	WCHAR *strSrc;
	LPSTR szRes;

	//获得临时变量的大小
	int i = MultiByteToWideChar(CP_UTF8, 0, str, -1, NULL, 0);
	strSrc = new WCHAR[i + 1];
	MultiByteToWideChar(CP_UTF8, 0, str, -1, strSrc, i);

	//获得临时变量的大小
	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new CHAR[i + 1];
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

	result = szRes;
	delete[]strSrc;
	delete[]szRes;
#endif
	return result;
}

//utf8转gb2312
std::string CCodeConverter::UTF8ToGb2312(const std::string strUtf8)
{
#ifdef _WIN32
	int sourcelen = strUtf8.size();
	int destlen = sourcelen * 2;
	char * destbuf = new char[destlen];
	char * sourcebuf = new char[sourcelen];
	for (int i = 0; i < sourcelen; i++)
	{
		sourcebuf[i] = strUtf8[i];
	}
	char Ctemp[4];
	memset(Ctemp, 0, 4);
	int i = 0, j = 0;
	while (i < sourcelen)
	{
		if (sourcebuf[i] >= 0)
		{
			destbuf[j++] = sourcebuf[i++];
		}
		else
		{
			WCHAR Wtemp;
			UTF_8ToUnicode(&Wtemp, sourcebuf + i);
			UnicodeToGB2312(Ctemp, Wtemp);
			destbuf[j] = Ctemp[0];
			destbuf[j + 1] = Ctemp[1];
			i += 3;
			j += 2;
		}
	}
	destbuf[j] = '\0';
	string returnStr = destbuf;
	return returnStr;
#endif
}

//gbk转gb2312
std::string CCodeConverter::GBKToGb2312(const std::string strGbk)
{
	string sourceGbk = strGbk;
	string utf8Str = GbkToUtf8(sourceGbk);
	//string gb2312Str = Utf8ToGbk(utf8Str);
	string gb2312Str = UTF8ToGb2312(utf8Str);
	return gb2312Str;
}
//gbk转UTF-8  
std::string CCodeConverter::GbkToUtf8(const std::string& strGbk)//传入的strGbk是GBK编码  
{
#ifdef _WIN32
	//gbk转unicode  
	int len = MultiByteToWideChar(CP_ACP, 0, strGbk.c_str(), -1, NULL, 0);
	wchar_t *strUnicode = new wchar_t[len];
	wmemset(strUnicode, 0, len);
	MultiByteToWideChar(CP_ACP, 0, strGbk.c_str(), -1, strUnicode, len);

	//unicode转UTF-8  
	len = WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, NULL, 0, NULL, NULL);
	char * strUtf8 = new char[len];
	WideCharToMultiByte(CP_UTF8, 0, strUnicode, -1, strUtf8, len, NULL, NULL);

	std::string strTemp(strUtf8,len);//此时的strTemp是UTF-8编码  
	delete[]strUnicode;
	delete[]strUtf8;
	strUnicode = NULL;
	strUtf8 = NULL;
	return strTemp;
#else
    return transcoingIconv("gbk", "utf-8", strGbk);
#endif
}
//Unicode转GB18030
std::string CCodeConverter::UnicodeToGB18030(const std::wstring& strUnicode)
{
#ifdef _WIN32
	int len = WideCharToMultiByte(54936, 0, strUnicode.c_str(), -1,NULL, 0, NULL, NULL);
	char* strGB18 = new char[len];
	WideCharToMultiByte(54936, 0, strUnicode.c_str(), -1, strGB18, len, NULL, NULL);
	std::string strTemp(strGB18);
	delete[]strGB18;
	strGB18 = NULL;
	return strTemp;
#else
    return "";
#endif
}
//Unicode转utf8
std::string CCodeConverter::UnicodeToUtf8(const std::wstring& strUnicode)
{
#ifdef _WIN32
	//unicode转UTF-8  
	int len = WideCharToMultiByte(CP_UTF8, 0, strUnicode.c_str(), -1, NULL, 0, NULL, NULL);
	char * strUtf8 = new char[len];
	WideCharToMultiByte(CP_UTF8, 0, strUnicode.c_str(), -1, strUtf8, len, NULL, NULL);
	string strUtf(strUtf8);
	delete[]strUtf8;
	strUtf8 = NULL;
	return strUtf;
#else
    string curLocale = setlocale(LC_ALL, NULL); // curLocale = "C";
    
    setlocale(LC_ALL, "chs");
    
    const wchar_t* _Source = strUnicode.c_str();
    size_t _Dsize = 2 * strUnicode.size() + 1;
    char *_Dest = new char[_Dsize];
    memset(_Dest,0,_Dsize);
    wcstombs(_Dest,_Source,_Dsize);
    string result = _Dest;
    delete []_Dest;
    
    setlocale(LC_ALL, curLocale.c_str());
    
    return result;
#endif
}
//GB18030转Unicode
std::wstring CCodeConverter::GB18030ToUnicode(const std::string& strGb18030)
{
#ifdef _WIN32
	int len = MultiByteToWideChar(54936, 0, strGb18030.c_str(), -1, NULL, 0);
	wchar_t* strUnicode = new wchar_t[len];
	MultiByteToWideChar(54936, 0, strGb18030.c_str(), -1, strUnicode, len);
	wstring retStr(strUnicode);
	delete[]strUnicode;
	strUnicode = NULL;
	return retStr;
#else
    setlocale(LC_ALL, "chs");
    
    const char* _Source = strGb18030.c_str();
    size_t _Dsize = strGb18030.size() + 1;
    wchar_t *_Dest = new wchar_t[_Dsize];
    wmemset(_Dest, 0, _Dsize);
    mbstowcs(_Dest,_Source,_Dsize);
    wstring result = _Dest;
    delete[] _Dest;
    
    setlocale(LC_ALL, "C");
    
    return result;
#endif // _WIN32

}
//UTF-8转gbk  
std::string CCodeConverter::Utf8ToGbk(const std::string& strUtf8)//传入的strUtf8是UTF-8编码  
{
#ifdef _WIN32
	//UTF-8转unicode  
	int len = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), (int)strUtf8.length(), NULL, 0);
	wchar_t * strUnicode = new wchar_t[len];//len = 2  
	wmemset(strUnicode, 0, len);
	MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), (int)strUtf8.length(), strUnicode, len);

	//unicode转gbk  
	int ulen = WideCharToMultiByte(CP_ACP, 0, strUnicode, len, NULL, 0, NULL, NULL);
    char *strGbk = new char[ulen + 1];//len=3 本来为2，但是char*后面自动加上了\0  
    memset(strGbk, 0, ulen + 1);
    WideCharToMultiByte(CP_ACP, 0, strUnicode, len, strGbk, ulen, NULL, NULL);

	std::string strTemp(strGbk);//此时的strTemp是GBK编码  
	delete[]strUnicode;
	delete[]strGbk;
	strUnicode = NULL;
	strGbk = NULL;
	return strTemp;
#else
    return transcoingIconv("utf-8", "gbk", strUtf8);
#endif
}

//gbk转unicode 
std::wstring CCodeConverter::GbkToUnicode(const std::string& strGbk)//返回值是wstring  
{
#ifdef _WIN32
	int len = MultiByteToWideChar(CP_ACP, 0, strGbk.c_str(), -1, NULL, 0);
	wchar_t *strUnicode = new wchar_t[len];
	wmemset(strUnicode, 0, len);
	MultiByteToWideChar(CP_ACP, 0, strGbk.c_str(), -1, strUnicode, len);

	std::wstring strTemp(strUnicode);//此时的strTemp是Unicode编码  
	delete[]strUnicode;
	strUnicode = NULL;
	return strTemp;
#endif
}

//Unicode转gbk  
std::string CCodeConverter::UnicodeToGbk(const std::wstring& strUnicode)//参数是wstring  
{
#ifdef _WIN32
	int len = WideCharToMultiByte(CP_ACP, 0, strUnicode.c_str(), -1, NULL, 0, NULL, NULL);
	char *strGbk = new char[len];//len=3 本来为2，但是char*后面自动加上了\0  
	memset(strGbk, 0, len);
	WideCharToMultiByte(CP_ACP, 0, strUnicode.c_str(), -1, strGbk, len, NULL, NULL);

	std::string strTemp(strGbk);//此时的strTemp是GBK编码  
	delete[]strGbk;
	strGbk = NULL;
	return strTemp;
#endif
}

// utf8 转 unicode
std::wstring CCodeConverter::Utf8ToUnicode(const std::string& utf8)
{
#ifdef _WIN32
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
    wchar_t* strUnicode = new wchar_t[len + 1];
    wmemset(strUnicode, 0, len + 1);
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, strUnicode, len);

    std::wstring strTemp(strUnicode);
    
    delete[] strUnicode;

    return strTemp;
#endif
}
//UTF-8 转 指定的编码格式、该编码格式是获取的本地的编码格式
std::string CCodeConverter::Utf8Transcoding(const std::string& strUtf8, int fontEncoding)
{
    switch (fontEncoding) {
    case wxFONTENCODING_CP936:
        return UTF8ToGB(strUtf8.c_str());
    case wxFONTENCODING_CP950:
        return Utf8ToGbk(strUtf8);
    default:
        return strUtf8;
    }
}

std::string CCodeConverter::TranscodingUtf8(const std::string& str,int fontEncoding)
{
    switch (fontEncoding) {
    case wxFONTENCODING_CP936:
        return GbkToUtf8(str);
    case wxFONTENCODING_CP950:
        return GbkToUtf8(str);
    default:
        return str;
    }
}

std::string CCodeConverter::transcoding(const string& src, int srcCodePage, int destCodePage)
{
#ifdef _WIN32
    int len = MultiByteToWideChar(srcCodePage, 0, src.c_str(), -1, NULL, 0);
    wchar_t *strUnicode = new wchar_t[len];
    wmemset(strUnicode, 0, len);
    MultiByteToWideChar(srcCodePage, 0, src.c_str(), -1, strUnicode, len);

    len = WideCharToMultiByte(destCodePage, 0, strUnicode, -1, NULL, 0, NULL, NULL);
    char* destStr = new char[len];
    WideCharToMultiByte(destCodePage, 0, strUnicode, -1, destStr, len, NULL, NULL);

    std::string strTemp(destStr, len);
    delete[]strUnicode;
    delete[]destStr;
    strUnicode = NULL;
    destStr = NULL;
    return strTemp;
#else
    return "";
#endif
}

std::string CCodeConverter::transCurrentcoding(const string& src,int codePage)
{
#ifdef _WIN32
    int num = 0;
    switch (_iFontEncoding) {
    case wxFONTENCODING_CP936:
        num = 936;
        break;
    case wxFONTENCODING_CP950:
        num = 950;
        break;
    case wxFONTENCODING_CP850:
        num = 850;
        break;
    }
    return transcoding(src, codePage, num);
#else
    return src;
#endif
}

std::string CCodeConverter::utf8ToCurrentcoding(const string& src, int currentFontEncoding)
{
#ifdef _WIN32
    int num = 0;
    switch (_iFontEncoding) {
    case wxFONTENCODING_CP936:
        num = 936;
        break;
    case wxFONTENCODING_CP950:
        num = 950;
        break;
    case wxFONTENCODING_CP850:
        num = 850;
        break;    }

    return transcoding(src, CP_UTF8, num);
#else
    return src;
#endif
}
std::string CCodeConverter::utf8To850(const string& src)
{
#ifdef _WIN32

    int len = MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, NULL, 0);
    wchar_t *strUnicode = new wchar_t[len];
    wmemset(strUnicode, 0, len);
    MultiByteToWideChar(CP_UTF8, 0, src.c_str(), -1, strUnicode, len);

    len = WideCharToMultiByte(1252, 0, strUnicode, -1, NULL, 0, NULL, NULL);
    char* destStr = new char[len];
    WideCharToMultiByte(1252, 0, strUnicode, -1, destStr, len, NULL, NULL);
    std::string strTemp(destStr, len);
    delete[]strUnicode;
    delete[]destStr;
    strUnicode = NULL;
    destStr = NULL;
    return strTemp;
#else
    return src;
#endif
}

#ifndef _WIN32
string CCodeConverter::transcoingIconv(const string& srcCode,const string& destCode,const string& str)
{
    size_t destlen = str.size() * 2;
    char* destbuf = new char[destlen + 1];
    memset(destbuf, 0, destlen + 1);
    
    size_t sourcelen = str.size() + 1;
    char* srcbuf = new char[sourcelen];
    memset(srcbuf, 0, sourcelen);
    memcpy(srcbuf, str.c_str(), str.size());
    
    int r = code_convert(srcCode.c_str(),destCode.c_str(),srcbuf,str.size(),destbuf,destlen);
    if (-1 == r) {
        printf("transcoingIconv from %s to %s error!",srcCode.c_str(),destCode.c_str());
        return str;
    }
    
    string strRet(destbuf);
    
    return strRet;
}

int CCodeConverter::code_convert(const char *from_charset,const char *to_charset,char *inbuf,size_t inlen,char *outbuf,size_t outlen)
{
    iconv_t cd;
    char **pin = &inbuf;
    char **pout = &outbuf;
    
    cd = iconv_open(to_charset,from_charset);
    if (cd==0)
    {
        printf("iconv_open failed!!!\n");
        return -1;
    }
    memset(outbuf,0,outlen);
    if (-1 == iconv(cd,pin,&inlen,pout,&outlen))
    {
        printf("iconv failed!!!\n");
        return -1;
    }
    iconv_close(cd);
    return 0;
    
}
#else

#endif

bool isUtf8(const void* data, size_t size)
{
    bool ret = true;
    unsigned char* start = (unsigned char*)data;
    unsigned char* end = (unsigned char*)data + size;

    while (start < end)
    {
        if (*start < 0x80)
            start++;
        else if (*start < (0xC0))
        {
            ret = false;
            break;
        }
        else if (*start < (0xE0))
        {
            if (start >= end - 1)
                break;

            if ((start[1] & (0xC0)) != 0x80)
            {
                ret = false;
                break;
            }

            start += 2;
        }
        else if (*start < (0xF0))
        {
            if (start >= end - 2)
                break;

            if ((start[1] & (0xC0)) != 0x80 || (start[2] & (0xC0)) != 0x80)
            {
                ret = false;
                break;
            }

            start += 3;
        }
        else
        {
            ret = false;
            break;
        }
    }

    return ret;
}

#ifdef _WIN32
string ansi2utf8(const string& ansi)
{
    if (isUtf8(ansi.c_str(), (long)ansi.length()))
        return ansi;

    int len = MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), (int)ansi.length(), NULL, 0);

    WCHAR* lpszW = new WCHAR[len];
    wmemset(lpszW, 0, len);
    int afterLen = MultiByteToWideChar(CP_ACP, 0, ansi.c_str(), (int)ansi.length(), lpszW, len);
    assert(afterLen == len);

    int utf8_len = WideCharToMultiByte(CP_UTF8, 0, lpszW, len, NULL, 0, NULL, NULL);
    assert(utf8_len > 0);

    char* utf8_string = new char[utf8_len + 1];
    memset(utf8_string, 0, utf8_len + 1);
    afterLen = WideCharToMultiByte(CP_UTF8, 0, lpszW, len, utf8_string, utf8_len, NULL, NULL);
    assert(afterLen == utf8_len);

    string ret(utf8_string);

    delete[] lpszW;
    delete[] utf8_string;

    return ret;
}

string utf82ansi(const string& utf8)
{
    if (!isUtf8(utf8.c_str(), (long)utf8.length()))
        return utf8;

    int len = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), (int)utf8.length(), NULL, 0);

    WCHAR* lpszW = new WCHAR[len];
    wmemset(lpszW, 0, len);
    int afterLen = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), (int)utf8.length(), lpszW, len);
    assert(afterLen == len);

    int ansi_len = WideCharToMultiByte(CP_ACP, 0, lpszW, len, NULL, 0, NULL, NULL);
    assert(ansi_len > 0);

    char* ansi_string = new char[ansi_len + 1];
    memset(ansi_string, 0, ansi_len + 1);
    afterLen = WideCharToMultiByte(CP_ACP, 0, lpszW, len, ansi_string, ansi_len, NULL, NULL);
    assert(afterLen == ansi_len);

    string ret(ansi_string);

    delete[] lpszW;
    delete[] ansi_string;

    return ret;
}
#endif

