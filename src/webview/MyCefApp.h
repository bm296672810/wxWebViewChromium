#ifndef _MYCEF_APP_H_
#define _MYCEF_APP_H_
#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <include/cef_process_message.h>

#include "MyCefMessage.h"
#include "MyCefV8Handler.h"
//#endif
class MyCefApp
    :public CefApp,
    public CefBrowserProcessHandler,
    public CefRenderProcessHandler {

public:
    MyCefApp();
    virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()OVERRIDE;
    virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler()OVERRIDE;
    
    virtual void OnContextCreated(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, CefRefPtr<CefV8Context> context) OVERRIDE;
    virtual void OnWebKitInitialized() OVERRIDE;

    void OnContextInitialized() OVERRIDE;
    void OnBeforeChildProcessLaunch(CefRefPtr<CefCommandLine> command_line) OVERRIDE;
    void OnRenderProcessThreadCreated(CefRefPtr<CefListValue> extra_info) OVERRIDE;

#if (CHROME_VERSION_BUILD > 2623)
    void OnScheduleMessagePumpWork(int64 delay) OVERRIDE;
#endif

    virtual void OnBeforeCommandLineProcessing(
        const CefString& process_type,
        CefRefPtr<CefCommandLine> command_line)OVERRIDE;
protected:
    IMPLEMENT_REFCOUNTING(MyCefApp);
private:
    //js要调用的C++函数的注册
    void RegistrationCFunc(CefRefPtr<CefBrowser> browser, CefRefPtr<CefV8Value> window, const CefString& FuncName);
};

#endif
