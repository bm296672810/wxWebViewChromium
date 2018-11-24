/////////////////////////////////////////////////////////////////////////////
// Author: Steven Lamerton
// Copyright: (c) 2013 Steven Lamerton
// Licence: wxWindows licence
/////////////////////////////////////////////////////////////////////////////

//#include "log.h"

//#if CEF_API == 3

#include "webview_chromium.h"

#include <wx/webview.h>
#include <wx/filesys.h>
#include "wx/filedlg.h"

#ifdef _WIN32
//#pragma comment (lib, "libiconv.lib")
#include <wx/msw/private.h>
//#include "myWindows.h"
#endif

#include <fstream>

#ifdef __VISUALC__
#pragma warning(push)
#pragma warning(disable:4100)
#endif

#include <include/cef_app.h>
#include <include/cef_browser.h>
#include <include/cef_string_visitor.h>

#include "include/cef_sandbox_win.h"

#include "webview/MyCefExtensionHandler.h"

#ifdef __WXMAC__
#include "include/cef_app.h"
#include "include/cef_application_mac.h"
#include "tests/cefclient/browser/main_context_impl.h"
#include "tests/cefclient/browser/resource.h"
#include "tests/cefclient/browser/root_window.h"
#include "tests/cefclient/browser/test_runner.h"
#include "tests/shared/browser/client_app_browser.h"
#include "tests/shared/browser/main_message_loop_external_pump.h"
#include "tests/shared/browser/main_message_loop_std.h"
#include "tests/shared/common/client_switches.h"
#include "../mac/cefdelegate_mac.h"
#include <thread>

using namespace std;
#endif

#ifdef _WIN32
#include <shellapi.h>
#include <windows.h>
#endif

#ifdef __VISUALC__
#pragma warning(pop)
#endif

extern const char wxWebViewBackendChromium[] = "wxWebViewChromium";
extern wxString g_currentappPath;       // 当前程序的目录

ClientHandler* g_clientHander = NULL;

wxIMPLEMENT_DYNAMIC_CLASS(wxWebViewChromium, wxWebView);

bool wxWebViewChromium::Create(wxWindow* parent,
    wxWindowID id,
    const wxString& url,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxString& name)
{
    if (!wxControl::Create(parent, id, pos, size, style,
        wxDefaultValidator, name)) {
        return false;
    }

    m_historyLoadingFromList = false;
    m_historyEnabled = true;
    m_historyPosition = -1;
    m_zoomLevel = wxWEBVIEW_ZOOM_MEDIUM;

    CefBrowserSettings browsersettings;
    CefWindowInfo info;

    m_clientHandler = new ClientHandler();
    m_clientHandler->SetWebView(this);

#ifdef __WXMSW__
    // Initialize window info to the defaults for a child window
    info.SetAsChild(GetHWND(), wxGetClientRect(this->GetHWND()));
#elif defined (__WXMAC__)
    info.SetAsChild(GetHandle(), pos.x, pos.y, size.GetWidth() + 5, size.GetHeight() - 47);
#endif
    // Creat the new child browser window, we do this async as we use a multi
    // threaded message loop

#if CHROME_VERSION_BUILD >= 1650
    CefBrowserHost::CreateBrowser(info, static_cast<CefRefPtr<CefClient>>(m_clientHandler),
        url.ToStdString(), browsersettings, NULL);
#else
    CefBrowserHost::CreateBrowser(info, static_cast<CefRefPtr<CefClient>>(m_clientHandler),
        url.ToStdString(), browsersettings);
#endif
    this->Bind(wxEVT_SIZE, &wxWebViewChromium::OnSize, this);
    //this->Connect(wxEVT_WEBVIEW_NEWWINDOW,wxWebViewChromium::OnNewWindow,0,this,0)
    this->Bind(wxEVT_WEBVIEW_NEWWINDOW, &wxWebViewChromium::OnNewWindow, this);
    return true;
}

wxWebViewChromium::~wxWebViewChromium()
{
    //Shutdown();
#if CHROME_VERSION_BUILD < 2078
    CefRefPtr<CefBrowser> browser = m_clientHandler->GetBrowser();
    if (browser.get()) {
        // Let the browser window know we are about to destroy it.
        browser->GetHost()->ParentWindowWillClose();
    }
#endif
}

void wxWebViewChromium::OnSize(wxSizeEvent& event)
{
#ifdef __WXMSW__
    wxSize size = GetClientSize();
    
    if (m_clientHandler && m_clientHandler->GetBrowser() && m_clientHandler->GetBrowser()->GetHost()) {
        HWND handle = m_clientHandler->GetBrowser()->GetHost()->GetWindowHandle();

        if (handle) {
            HDWP hdwp = BeginDeferWindowPos(1);
            hdwp = DeferWindowPos(hdwp, handle, NULL, 0, 0,
                size.GetWidth(), size.GetHeight(), SWP_NOZORDER);
            EndDeferWindowPos(hdwp);
        }
    }
#endif

    event.Skip();
}

void wxWebViewChromium::SetPageSource(const wxString& pageSource)
{
    m_pageSource = pageSource;
}

void wxWebViewChromium::SetPageText(const wxString& pageText)
{
    m_pageText = pageText;
}

void* wxWebViewChromium::GetNativeBackend() const
{
    return m_clientHandler->GetBrowser();
}

bool wxWebViewChromium::CanGoForward() const
{
    if (m_historyEnabled)
        return m_historyPosition != static_cast<int>(m_historyList.size()) - 1;
    else
        return false;
}

bool wxWebViewChromium::CanGoBack() const
{
    if (m_historyEnabled)
        return m_historyPosition > 0;
    else
        return false;
}

void wxWebViewChromium::LoadHistoryItem(wxSharedPtr<wxWebViewHistoryItem> item)
{
    int pos = -1;
    for (unsigned int i = 0; i < m_historyList.size(); i++) {
        //We compare the actual pointers to find the correct item
        if (m_historyList[i].get() == item.get())
            pos = i;
    }
    wxASSERT_MSG(pos != static_cast<int>(m_historyList.size()),
        "invalid history item");
    m_historyLoadingFromList = true;
    LoadURL(item->GetUrl());
    m_historyPosition = pos;
}

wxVector<wxSharedPtr<wxWebViewHistoryItem> > wxWebViewChromium::GetBackwardHistory()
{
    wxVector<wxSharedPtr<wxWebViewHistoryItem> > backhist;
    //As we don't have std::copy or an iterator constructor in the wxwidgets
    //native vector we construct it by hand
    for (int i = 0; i < m_historyPosition; i++) {
        backhist.push_back(m_historyList[i]);
    }
    return backhist;
}

wxVector<wxSharedPtr<wxWebViewHistoryItem> > wxWebViewChromium::GetForwardHistory()
{
    wxVector<wxSharedPtr<wxWebViewHistoryItem> > forwardhist;
    //As we don't have std::copy or an iterator constructor in the wxwidgets
    //native vector we construct it by hand
    for (int i = m_historyPosition + 1; i < static_cast<int>(m_historyList.size()); i++) {
        forwardhist.push_back(m_historyList[i]);
    }
    return forwardhist;
}

void wxWebViewChromium::GoBack()
{
    LoadHistoryItem(m_historyList[m_historyPosition - 1]);
}

void wxWebViewChromium::GoForward()
{
    LoadHistoryItem(m_historyList[m_historyPosition + 1]);
}

void wxWebViewChromium::LoadURL(const wxString& url)
{
    // Handle LoadURL calls while the browser is still initializing
    if (!m_clientHandler->GetBrowser())
        m_clientHandler->SetPendingURL(url);
    else
        m_clientHandler->GetBrowser()->GetMainFrame()->LoadURL(url.ToStdString());

}

void wxWebViewChromium::ClearHistory()
{
    m_historyList.clear();
    m_historyPosition = -1;
}

void wxWebViewChromium::EnableHistory(bool enable)
{
    m_historyEnabled = enable;
}

void wxWebViewChromium::Stop()
{
    m_clientHandler->GetBrowser()->StopLoad();
}

void wxWebViewChromium::Reload(wxWebViewReloadFlags flags)
{
    if (flags == wxWEBVIEW_RELOAD_NO_CACHE) {
        m_clientHandler->GetBrowser()->ReloadIgnoreCache();
    } else {
        m_clientHandler->GetBrowser()->Reload();
    }
}

wxString wxWebViewChromium::GetPageSource() const
{
    CefRefPtr<CefStringVisitor> source_visitor = new wxStringVisitor(const_cast<wxWebViewChromium*>(this), wxStringVisitor::PAGE_SOURCE);

    getClientHandler()->GetBrowser()->GetMainFrame()->GetSource(source_visitor);
    return m_pageSource;
}

wxString wxWebViewChromium::GetPageText() const
{
    return m_pageText;
}

wxString wxWebViewChromium::GetCurrentURL() const
{
    return m_clientHandler->GetBrowser()->GetMainFrame()->GetURL().ToString();
}

wxString wxWebViewChromium::GetCurrentTitle() const
{
    return m_title;
}

void wxWebViewChromium::Print()
{
#if CHROME_VERSION_BUILD >= 1650
    m_clientHandler->GetBrowser()->GetHost()->Print();
#endif
}

void wxWebViewChromium::Cut()
{
    m_clientHandler->GetBrowser()->GetMainFrame()->Cut();
}

void wxWebViewChromium::Copy()
{
    m_clientHandler->GetBrowser()->GetMainFrame()->Copy();
}

void wxWebViewChromium::Paste()
{
    m_clientHandler->GetBrowser()->GetMainFrame()->Paste();
}

void wxWebViewChromium::Undo()
{
    m_clientHandler->GetBrowser()->GetMainFrame()->Undo();
}

void wxWebViewChromium::Redo()
{
    m_clientHandler->GetBrowser()->GetMainFrame()->Redo();
}

void wxWebViewChromium::SelectAll()
{
    m_clientHandler->GetBrowser()->GetMainFrame()->SelectAll();
}

void wxWebViewChromium::DeleteSelection()
{
    wxString jsdelete = "if (window.getSelection) { if (window.getSelection().deleteFromDocument) { window.getSelection().deleteFromDocument(); } }";
    RunScript(jsdelete);
}

void wxWebViewChromium::ClearSelection()
{
    wxString jsclear = "if (window.getSelection) { alert('hello'); if (window.getSelection().empty) { window.getSelection().empty(); } }";
    
    RunScript(jsclear);
}

void wxWebViewChromium::RunScript(const wxString& javascript)
{
    CefRefPtr<CefBrowser> browser = m_clientHandler->GetBrowser();
    CefRefPtr<CefFrame> frame = browser->GetMainFrame();
#ifdef _WIN32
    frame->ExecuteJavaScript(ansi2utf8(javascript.ToStdString()), "", 0);
#else
	frame->ExecuteJavaScript(javascript.ToStdString(), "", 0);
#endif
}

bool wxWebViewChromium::IsFocused()
{
    CefRefPtr<CefBrowser> browser = m_clientHandler->GetBrowser();
    CefRefPtr<CefFrame> frame = browser->GetMainFrame();
    return frame->IsFocused();
}

bool wxWebViewChromium::IsBusy() const
{
    if (m_clientHandler->GetBrowser())
        return m_clientHandler->GetBrowser()->IsLoading();
    else
        return false;
}

void wxWebViewChromium::SetEditable(bool enable)
{
    wxString mode = enable ? "\"on\"" : "\"off\"";
    RunScript("document.designMode = " + mode);
}

void wxWebViewChromium::DoSetPage(const wxString& html, const wxString& baseUrl)
{
    m_clientHandler->GetBrowser()->GetMainFrame()->LoadString(html.ToStdString(),
        baseUrl.ToStdString());
}

wxWebViewZoom wxWebViewChromium::GetZoom() const
{
    return m_zoomLevel;
}


void wxWebViewChromium::SetZoom(wxWebViewZoom zoom)
{
    m_zoomLevel = zoom;

    double mapzoom = 0.0;
    // arbitrary way to map our common zoom enum to float zoom
    switch (zoom) {
    case wxWEBVIEW_ZOOM_TINY:
        mapzoom = -1.0;
        break;

    case wxWEBVIEW_ZOOM_SMALL:
        mapzoom = -0.5;
        break;

    case wxWEBVIEW_ZOOM_MEDIUM:
        mapzoom = 0.0;
        break;

    case wxWEBVIEW_ZOOM_LARGE:
        mapzoom = 0.5;
        break;

    case wxWEBVIEW_ZOOM_LARGEST:
        mapzoom = 1.0;
        break;

    default:
        wxASSERT(false);
    }
    m_clientHandler->GetBrowser()->GetHost()->SetZoomLevel(mapzoom);
}

void wxWebViewChromium::SetZoomType(wxWebViewZoomType type)
{
    // there is only one supported zoom type at the moment so this setter
    // does nothing beyond checking sanity
    wxASSERT(type == wxWEBVIEW_ZOOM_TYPE_LAYOUT);
}

wxWebViewZoomType wxWebViewChromium::GetZoomType() const
{
    return wxWEBVIEW_ZOOM_TYPE_LAYOUT;
}

bool wxWebViewChromium::CanSetZoomType(wxWebViewZoomType type) const
{
    return type == wxWEBVIEW_ZOOM_TYPE_LAYOUT;
}

void wxWebViewChromium::RegisterHandler(wxSharedPtr<wxWebViewHandler> handler)
{
    // We currently don't support custom scheme handlers
}

#ifdef __WXMSW__
bool wxWebViewChromium::StartUp(int &code, const wxString &path)
#else
bool wxWebViewChromium::StartUp(int &code, const wxString &path,
                                int argc, char* argv[])
#endif
{
	//LOG(WARNING) << "wxWebViewChromium::StartUp before...";
    CefRefPtr<MyCefApp> myApp(new MyCefApp);
#ifdef __WXMSW__
    CefMainArgs args(wxGetInstance());
#else
    cefinitDelegate_mac();
    CefMainArgs args(argc, argv);
    //cefinitDelegate_mac();
#endif
    // If there is no subprocess then we need to execute on this process
    if ( path == "" )
    {
#if CHROME_VERSION_BUILD >= 2078
        code = CefExecuteProcess(args, myApp, NULL);
#else
        code = CefExecuteProcess(args, NULL);
#endif
        if ( code >= 0 )
            return false;
    }
    
    CefSettings settings;
    // We use a multithreaded message loop so we don't have to integrate
    // with the wx message loop
#ifdef __WXMSW__
    settings.multi_threaded_message_loop = true;
#else
    settings.external_message_pump = true;
    
    settings.windowless_rendering_enabled = true;

#endif
    
#if CHROME_VERSION_BUILD >= 1750
    settings.no_sandbox = true;
#endif
    
#ifdef __WXDEBUG__
    settings.log_severity = LOGSEVERITY_INFO;
    CefString(&settings.log_file).FromASCII("./debug.log");
#endif
    // We use a multithreaded message loop so we don't have to integrate
    // with the wx message loop
    //settings.multi_threaded_message_loop = true;
    CefString(&settings.browser_subprocess_path) = path.ToStdString();
    
    std::string local("zh-CN");
    cef_string_utf8_to_utf16(local.c_str(), local.size(), &settings.locale);
    //CefRefPtr<CefApp> app(new SimpleApp);
    
#ifdef __WXMAC__
    
    bool cefInitRetrun = CefInitialize(args, settings, myApp, NULL);
//    thread t(CefRunMessageLoop);
//    CefRunMessageLoop();
    cefdelegate_mac();
    
#endif
    
#if CHROME_VERSION_BUILD >= 2078
#ifdef __WXMAC__
    return cefInitRetrun;
#else
    return CefInitialize(args, settings, myApp, NULL);
#endif
#else
    return CefInitialize(args, settings, NULL);
#endif
    
}

int wxWebViewChromium::StartUpSubprocess()
{
#ifdef _WIN32
    CefMainArgs args(wxGetInstance());
#else
    CefMainArgs args;
#endif
    CefRefPtr<MyCefApp> myApp(new MyCefApp);
#if CHROME_VERSION_BUILD >= 2078
    return CefExecuteProcess(args, myApp, NULL);
#else
    return CefExecuteProcess(args, NULL);
#endif
}

void wxWebViewChromium::Shutdown()
{
    CefShutdown();
}
void wxWebViewChromium::DoCEFWork()
{
#ifdef _WIN32
#else
    sendEvent01();
#endif
    CefDoMessageLoopWork();
    //CefRunMessageLoop();
}
//产生一个右键菜单事件
void wxWebViewChromium::GenerateCefRightEvent(vector<wstring> value, int EventId)
{
    CCodeConverter code;

    vector<string> vecStr;
    for (vector<wstring>::size_type i = 0; i < value.size(); i++) {
        string tmpStr = code.UnicodeToUtf8(value[i]);
        tmpStr = code.UTF8ToGb2312(tmpStr);
        vecStr.push_back(tmpStr);
       
    }
    wxThreadEvent threadEvent(wxEVT_THREAD, EventId);
    threadEvent.SetPayload(vecStr);
    wxQueueEvent(this->GetParent(), threadEvent.Clone());

}

// 产生一个应用点击事件
void wxWebViewChromium::GenerateCefApplicationClickEvent(const string& value, int EventId)
{
#if (CHROME_VERSION_BUILD != 2623)
    MyApplicationClickedEvent e(wxEVT_COMMAND_MYAPPLICATION_CLICKED, EventId);
    e.SetEventObject(this);
    e.SetContent(value);
    GetEventHandler()->ProcessEvent(e);
#endif
}
void wxWebViewChromium::GenerateCefApplicationClickEvent(const wstring& value, int EventId)
{
#if (CHROME_VERSION_BUILD != 2623)
    MyApplicationClickedEvent e(wxEVT_COMMAND_MYAPPLICATION_CLICKED, EventId);

    CCodeConverter code;
    string tmp = code.UnicodeToUtf8(value);

    e.SetEventObject(this);
    e.SetContent(code.UTF8ToGb2312(tmp));
    GetEventHandler()->ProcessEvent(e);
#endif
}

void wxWebViewChromium::OnNewWindow(wxWebViewEvent& event)
{
    this->LoadURL(event.GetURL());
}


//#endif
