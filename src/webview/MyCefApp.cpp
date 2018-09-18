#include "MyCefApp.h"

MyCefApp::MyCefApp()
{

}

CefRefPtr<CefBrowserProcessHandler> MyCefApp::GetBrowserProcessHandler()
{
    return this;
}

CefRefPtr<CefRenderProcessHandler> MyCefApp::GetRenderProcessHandler()
{
    return this;
}
void MyCefApp::OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context)
{
    // The var type can accept all object or variable
    CefRefPtr<CefV8Value> window = context->GetGlobal();

    for (size_t i = 0; i < sizeof(CEFFuncName)/sizeof(char*); i++)
    {
        RegistrationCFunc(browser, window, CEFFuncName[i]);
    }
}
//js要调用的C++函数的注册
void MyCefApp::RegistrationCFunc(CefRefPtr<CefBrowser> browser, CefRefPtr<CefV8Value> window, const CefString& FuncName)
{
    CefRefPtr<CV8JsHandler> pJsHandler(new CV8JsHandler(browser));
    CefRefPtr<CefV8Value> FastReplyFunc = CefV8Value::CreateFunction(FuncName, pJsHandler);
    bool createStat = window->SetValue(FuncName, FastReplyFunc, V8_PROPERTY_ATTRIBUTE_NONE);
    if (!createStat)
    {
        //LOG(ERROR) << "CreateStat:" << createStat << " funcName:" << FuncName.ToString();
    }
}
void MyCefApp::OnWebKitInitialized()
{
    std::string extensionCode =
        "var g_value=\"global value here\";"
        "var test;"
        "if (!test)"
        "  test = {};"
        "(function() {"
        "  test.myfunc = function() {"
        "    native function hehe(int,int);"
        "    return hehe(10, 50);"
        "  };"
        "})();";

    // 声明本地函数 native function hehe();" 如果有参数列表需要写具体的类型,而不能写var类型！与本地声明一直
    // 调用本地函数    return hehe();"

    // Create an instance of my CefV8Handler object.
    CefRefPtr<CefV8Handler> handler = new CV8JsHandler();

    // Register the extension.
    CefRegisterExtension("v8/mycode", extensionCode, handler);
}

void MyCefApp::OnContextInitialized()
{

}
void MyCefApp::OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line)
{
    command_line->AppendSwitch("process-pre-site");
}
void MyCefApp::OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info)
{

}

void MyCefApp::OnBeforeCommandLineProcessing(
                                             const CefString& process_type,
                                             CefRefPtr<CefCommandLine> command_line)
{
    //command_line->AppendSwitch("process-per-site");
    //command_line->AppendSwitch("disable-web-security");
    ////command_line->AppendSwitch("enable-npapi");
    //command_line->AppendSwitchWithValue("ppapi-flash-path",  "PepperFlash/pepflashplayer.dll");
    //command_line->AppendSwitchWithValue("ppapi-flash-version", "29.0.0.140");
    //command_line->AppendSwitchWithValue("plugin-policy", "allow");
    //command_line->AppendSwitchWithValue("enable-system-flash", "1");
    
    /*if (process_type.empty())
     {
     if (!command_line->HasSwitch("disable-extensions") &&
     !command_line->HasSwitch("disable-pdf-extension")) {
     command_line->AppendSwitch("disable-surfaces");
     }
     command_line->AppendSwitch("enable-begin-frame-scheduling");
     command_line->AppendSwitch("process-per-site");
     command_line->AppendSwitch("ppapi-out-of-process");
     command_line->AppendSwitchWithValue("plugin-policy", "allow");
     command_line->AppendSwitchWithValue("ppapi-flash-path", "PepperFlash/pepflashplayer.dll");
     }*/
    if (process_type.empty())
    {
        command_line->AppendSwitchWithValue("ppapi-flash-path", "PepperFlash/pepflashplayer.dll");
    }
}
#if (CHROME_VERSION_BUILD > 2623)
void MyCefApp::OnScheduleMessagePumpWork(int64 delay)
{

}
#endif
