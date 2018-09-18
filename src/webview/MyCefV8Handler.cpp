#include "MyCefV8Handler.h"
#include <iostream>
using namespace std;

/*
**                    0.快捷回复,   1.回复,  2.回复全部, 3.转发
*/
const char * CEFFuncName[] = { "FastReply" , "Reply" ,"ReplyAll" ,"Foward" ,
/*
**                  4.作为附件转发,        5.查看附件,        6.双击查看附件
*/
                    "AsAttachmentForward"  ,"Accessory_view" ,"Accessory_view_DBclick",
/*
**                  7.下载附件              8.保存所有附件      9.复制附件
*/
                     "Accessory_Download" ,"Accessory_Saveall" ,"Accessory_Copy" ,
/*
**                  10.转发附件         11.复制   12.全选
*/
                    "Accessory_Relay" ,"CopyText" ,"SelectAll" ,
/*
**                  13.获取应用的url,    14.应用详情             15.安装应用
*/
                    "Get_Application_Url" ,"Application_Detail" ,"Install_Application" ,
/*
**                  16.邮件内容改变  17.打开应用连接   18.点击邮箱账号
*/
                    "edit_on_input" ,"OpenWindowUrl" ,"jump_write_mail" };

//V8 handler
CV8JsHandler::CV8JsHandler(void)
{
}
CV8JsHandler::CV8JsHandler(CefRefPtr<CefBrowser> browser) :m_browser(browser)
{
}
CV8JsHandler::~CV8JsHandler(void)
{
}

bool CV8JsHandler::Execute(const CefString& func_name,
    CefRefPtr<CefV8Value> object,
    const CefV8ValueList& arguments,
    CefRefPtr<CefV8Value>& retval,
    CefString& exception)
{
    CFuncName m_FuncNames;
    if (arguments.size() == 2)
    {
        SendMyMessage(arguments[0]->GetStringValue(), arguments[1]->GetStringValue(), func_name);
        return true;
    }
    else if(arguments.size() == 1)
    {
        SendMyMessage(NullString, arguments[0]->GetStringValue(), func_name);
        return true;
	}
	else if (arguments.size() == 0)
	{
		SendMyMessage(NullString, NullString, func_name);
		return true;
	}

    return false;
}

void CV8JsHandler::SendMyMessage(const CefString& messageContent, const CefString& Num, const CefString& messageName)
{
    CefRefPtr<CefProcessMessage> msg;
    
    msg = CefProcessMessage::Create(messageName);

    CefRefPtr<CefListValue> args = msg->GetArgumentList();
    
    args->SetString(0, messageContent);
    args->SetString(1, Num);
    bool sendMsg = m_browser->SendProcessMessage(PID_BROWSER, msg);
}

namespace xiniu
{
    int GetMessageType(string& messageName)
    {
        for (size_t i = 0; i < sizeof(CEFFuncName) / sizeof(char*); i++)
        {
            string cefStr = CEFFuncName[i];
            if (cefStr == messageName)
            {
                return i;
            }
        }

        return -1;
    }
}