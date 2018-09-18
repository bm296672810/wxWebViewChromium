#ifndef _MYCEF_V8HANDLER_H_
#define _MYCEF_V8HANDLER_H_

#include <include/cef_v8.h>

#include "MyCefMessage.h"

class CV8JsHandler :
    public CefV8Handler {
public:
    CV8JsHandler(void);
    CV8JsHandler(CefRefPtr<CefBrowser> browser);
    virtual ~CV8JsHandler(void);
    
public:
    virtual bool Execute(const CefString& name,
        CefRefPtr<CefV8Value> object,
        const CefV8ValueList& arguments,
        CefRefPtr<CefV8Value>& retval,
        CefString& exception)OVERRIDE;
private:
    CefString NullString = "";
    CefRefPtr<CefBrowser> m_browser;

    void SendMyMessage(const CefString& messageContent, const CefString& Num, const CefString& messageName);

    IMPLEMENT_REFCOUNTING(CV8JsHandler);
};

#endif