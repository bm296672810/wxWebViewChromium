/////////////////////////////////////////////////////////////////////////////
// Author: Steven Lamerton
// Copyright: (c) 2013 Steven Lamerton
// Licence: wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WEBVIEWCHROMIUM3_H_
#define _WX_WEBVIEWCHROMIUM3_H_

//#if CEF_API == 3
#include <iostream>
#include <fstream>
#include <set>
#include <wx/control.h>
#include <wx/webview.h>
#include <wx/sharedptr.h>
#include <wx/vector.h>
#include <wx/webview.h>
#include <wx/timer.h>
#include <wx/msgdlg.h>

#ifdef __VISUALC__
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#include "src/webview/MyCefClientHandler.h"

#ifdef __VISUALC__
#pragma warning(pop)
#endif
#include "MyEvent.h"
#include "code.h"

class ClientHandler;

#define CEF_REQUIRE_IO_THREAD() DCHECK(CefCurrentlyOn(TID_IO));
//static void myWriteLog(const string& log);
using namespace std;
extern const char wxWebViewBackendChromium[];

struct CFuncName;
struct MessageName;
// ClientHandler implementation.


class WXDLLIMPEXP_WEBVIEW wxWebViewChromium : public wxWebView {
public:

    wxWebViewChromium() {}

    wxWebViewChromium(wxWindow* parent,
        wxWindowID id,
        const wxString& url = wxWebViewDefaultURLStr,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxWebViewNameStr)
    {
        Create(parent, id, url, pos, size, style, name);
    }

    ~wxWebViewChromium();

    void OnSize(wxSizeEvent &event);

    void SetPageSource(const wxString& pageSource);

    void SetPageText(const wxString& pageText);

    bool Create(wxWindow* parent,
        wxWindowID id,
        const wxString& url = wxWebViewDefaultURLStr,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxWebViewNameStr);

    virtual void LoadURL(const wxString& url);
    virtual void LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item);
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetBackwardHistory();
    virtual wxVector<wxSharedPtr<wxWebViewHistoryItem> > GetForwardHistory();

    virtual bool CanGoForward() const;
    virtual bool CanGoBack() const;
    virtual void GoBack();
    virtual void GoForward();
    virtual void ClearHistory();
    virtual void EnableHistory(bool enable = true);
    virtual void Stop();
    virtual void Reload(wxWebViewReloadFlags flags = wxWEBVIEW_RELOAD_DEFAULT);

    virtual wxString GetPageSource() const;
    virtual wxString GetPageText() const;

    virtual bool IsBusy() const;
    virtual wxString GetCurrentURL() const;
    virtual wxString GetCurrentTitle() const;

    virtual void SetZoomType(wxWebViewZoomType type);
    virtual wxWebViewZoomType GetZoomType() const;
    virtual bool CanSetZoomType(wxWebViewZoomType type) const;

    virtual void Print();

    virtual wxWebViewZoom GetZoom() const;
    virtual void SetZoom(wxWebViewZoom zoom);

    virtual void* GetNativeBackend() const;

    virtual long Find(const wxString& WXUNUSED(text), int WXUNUSED(flags) = wxWEBVIEW_FIND_DEFAULT) { return wxNOT_FOUND; }

    //Clipboard functions
    virtual bool CanCut() const { return true; }
    virtual bool CanCopy() const { return true; }
    virtual bool CanPaste() const { return true; }
    virtual void Cut();
    virtual void Copy();
    virtual void Paste();

    //Undo / redo functionality
    virtual bool CanUndo() const { return true; }
    virtual bool CanRedo() const { return true; }
    virtual void Undo();
    virtual void Redo();

    //Editing functions
    virtual void SetEditable(bool enable = true);
    virtual bool IsEditable() const { return false; }

    //Selection
    virtual void SelectAll();
    virtual bool HasSelection() const { return false; }
    virtual void DeleteSelection();
    virtual wxString GetSelectedText() const { return ""; }
    virtual wxString GetSelectedSource() const { return ""; }
    virtual void ClearSelection();
    

    //virtual bool RunScript(const wxString& javascript, wxString* output = NULL);
    virtual void RunScript(const wxString& javascript);
    void RunScript(const string& javascript);
    virtual bool IsFocused();

    //Virtual Filesystem Support
    virtual void RegisterHandler(wxSharedPtr<wxWebViewHandler> handler);

#ifdef __WXMSW__
    static bool StartUp(int &code, const wxString &path = "");
#else
    static bool StartUp(int &code, const wxString &path,
                        int argc, char* argv[]);
#endif
    // If using a separate subprocess then return the result of this function
    static int StartUpSubprocess();
    static void Shutdown();
    static void DoCEFWork();
    
    CefRefPtr<ClientHandler> getClientHandler() const { return m_clientHandler; }
    //产生一个右键菜单事件
    void GenerateCefRightEvent(vector<wstring> value, int EventId);
    // 产生一个应用点击事件
    void GenerateCefApplicationClickEvent(const string& value, int EventId);
    // 产生一个应用点击事件
    void GenerateCefApplicationClickEvent(const wstring& value, int EventId);

    void OnNewWindow(wxWebViewEvent& event);
    
protected:
    virtual void DoSetPage(const wxString& html, const wxString& baseUrl);

private:
    //History related variables, we currently use our own implementation
    wxVector<wxSharedPtr<wxWebViewHistoryItem> > m_historyList;
    int m_historyPosition;
    bool m_historyLoadingFromList;
    bool m_historyEnabled;

    //We need to store the title and zoom ourselves
    wxString m_title;
    wxWebViewZoom m_zoomLevel;

    //The timer calls the CEF event loop
    wxTimer *m_timer;

    // Current main frame page source
    wxString m_pageSource;

    // The text of the current page
    wxString m_pageText;

    //We also friend ClientHandler so it can access the history
    friend class ClientHandler;
    CefRefPtr<ClientHandler> m_clientHandler;

    wxDECLARE_DYNAMIC_CLASS(wxWebViewChromium);
};

class wxStringVisitor : public CefStringVisitor {
public:
    enum StringType {
        PAGE_SOURCE,
        PAGE_TEXT,
    };
    wxStringVisitor(wxWebViewChromium* webview, StringType type) : m_webview(webview), m_type(type) {}
    void Visit(const CefString& string) OVERRIDE
    {
        switch (m_type) {
        case PAGE_SOURCE:
            m_webview->SetPageSource(string.ToWString());
            break;
        case PAGE_TEXT:
            m_webview->SetPageText(string.ToWString());
            break;
        }
    }
private:
    wxWebViewChromium *m_webview;
    StringType m_type;
    IMPLEMENT_REFCOUNTING(wxStringVisitor);
};

class WXDLLIMPEXP_WEBVIEW wxWebViewFactoryChromium : public wxWebViewFactory {
public:
    virtual wxWebView* Create() { return new wxWebViewChromium; }
    virtual wxWebView* Create(wxWindow* parent,
        wxWindowID id,
        const wxString& url = wxWebViewDefaultURLStr,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        long style = 0,
        const wxString& name = wxWebViewNameStr)
    {
        return new wxWebViewChromium(parent, id, url, pos, size, style, name);
    }
};

#endif // _WX_WEBVIEWCHROMIUM3_H_
