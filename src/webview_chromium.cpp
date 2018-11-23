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
#include <include/cef_version.h>
#include "include/cef_sandbox_win.h"


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
//==================================================================================
//== class ClientHandler 
//==
//==================================================================================

//处理进程间的消息
bool ClientHandler::OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
    CefProcessId WXUNUSED(source_process),
    CefRefPtr<CefProcessMessage> message)
{
    MessageName m_MessNames;
    string GetMessageName = message->GetName().ToString();
    wstring MessageContent = message->GetArgumentList()->GetString(0).ToWString();
    wstring MessageNumW = message->GetArgumentList()->GetString(1).ToWString();
    string MessageNum = message->GetArgumentList()->GetString(1).ToString();
    vector<wstring> VecValue;
    VecValue.push_back(MessageContent);
    VecValue.push_back(MessageNumW);

    int cefMessage = xiniu::GetMessageType(GetMessageName);

    switch (cefMessage)
    {
    case CefMessageType::FAST_REPLY_TYPE:
        m_webview->GenerateCefRightEvent(VecValue, CEFFastReply);
        return true;
    case CefMessageType::REPLY_TYPE:
        m_webview->GenerateCefRightEvent(VecValue, CEFReply);
        return true;
    case CefMessageType::REPLY_ALL_TYPE:
        m_webview->GenerateCefRightEvent(VecValue, CEFReplyAll);
        return true;
    case CefMessageType::FORWARD_TYPE:
        m_webview->GenerateCefRightEvent(VecValue, CEFFoward);
        return true;
    case CefMessageType::AS_ATTACHMENT_FORWARD_TYPE:
        m_webview->GenerateCefRightEvent(VecValue, CEFAsAttachFoward);
        return true;
    case CefMessageType::ATTACHMENT_VIEW_TYPE:
        m_webview->GenerateCefRightEvent(VecValue, CEFAttachmentView);
        return true;
    case CefMessageType::ATTACHMENT_VIEW_DBCLICK_TYPE:
        m_webview->GenerateCefRightEvent(VecValue, CEFAttachmentViewDBclick);
        return true;
    case CefMessageType::ATTACHMENT_DOWNLOAD_TYPE:
        m_webview->GenerateCefRightEvent(VecValue, CEFAttachmentDownload);
        return true;
    case CefMessageType::ATTACHMENT_SAVEALL_TYPE:
        m_webview->GenerateCefRightEvent(VecValue, CEFAttachmentSaveall);
        return true;
    case CefMessageType::ATTACHMENT_COPY_TYPE:
        m_webview->GenerateCefRightEvent(VecValue, CEFAttachmentCopy);
        return true;
    case CefMessageType::ATTACHMENT_RELAY_TYPE:
        m_webview->GenerateCefRightEvent(VecValue, CEFAttachmentRelay);
        return true;
    case CefMessageType::COPYTEXT_TYPE:
        m_webview->GenerateCefRightEvent(VecValue, CEFCopyText);
        return true;
    case CefMessageType::SELECTALL_TYPE:
        m_webview->GenerateCefRightEvent(VecValue, CEFSelectALL);
        return true;
    case CefMessageType::GET_APPLICATION_URL_TYPE:
        m_webview->GenerateCefApplicationClickEvent(MessageNum, CEFGetApplicationUrl);
        return true;
    case CefMessageType::APPLICATION_DETAIL_TYPE:
        m_webview->GenerateCefApplicationClickEvent(MessageNum, CEFApplicationDetail);
        return true;
    case CefMessageType::INSTALL_APPLICATION_TYPE:
        m_webview->GenerateCefApplicationClickEvent(MessageNum, CEFInstallApplication);
        return true;
    case CefMessageType::EDIT_ON_INPUT:
        m_webview->GenerateCefApplicationClickEvent(MessageNum, CEFEditOnInput);
        return true;
    case CefMessageType::OPEN_WINDOW_URL:
        m_webview->GenerateCefRightEvent(VecValue, CEFOpenWindowUrl);
        return true;
    case CefMessageType::JUMP_WRITE_MAIL:
        m_webview->GenerateCefRightEvent(VecValue, CEFJumpWriteMail);
        return true;
    case CefMessageType::UNKNOWN_TYPE:
        break;
    default:
        break;
    }
    
    return false;
}
// CefDisplayHandler methods
void ClientHandler::OnLoadingStateChange(CefRefPtr<CefBrowser> WXUNUSED(browser), bool WXUNUSED(isLoading),
    bool WXUNUSED(canGoBack), bool WXUNUSED(canGoForward))
{

}

void ClientHandler::OnAddressChange(CefRefPtr<CefBrowser> WXUNUSED(browser), CefRefPtr<CefFrame> WXUNUSED(frame),
    const CefString& WXUNUSED(url))
{

}

void ClientHandler::OnTitleChange(CefRefPtr<CefBrowser> browser, const CefString& title)
{
    m_webview->m_title = title.ToWString();
    wxString target = browser->GetMainFrame()->GetName().ToString();

    wxWebViewEvent event(wxEVT_COMMAND_WEBVIEW_TITLE_CHANGED, m_webview->GetId(), "", target);
    event.SetString(title.ToWString());
    event.SetEventObject(m_webview);

    m_webview->HandleWindowEvent(event);
}

#if CHROME_VERSION_BUILD < 3538
bool ClientHandler::OnConsoleMessage(CefRefPtr<CefBrowser> WXUNUSED(browser), const CefString& WXUNUSED(message),
    const CefString& WXUNUSED(source), int WXUNUSED(line))
{
    return false;
}
#endif

// CefContextMenuHandler methods
void ClientHandler::OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefContextMenuParams> params,
    CefRefPtr<CefMenuModel> model)
{
    if (!m_webview->IsContextMenuEnabled())
        model->Clear();

    cef_context_menu_type_flags_t flag = params->GetTypeFlags();
    // 编辑框的右键消息
    if (flag & CM_TYPEFLAG_EDITABLE) {  
        model->SetLabel(MENU_ID_UNDO, wxT("撤销"));
        model->SetLabel(MENU_ID_REDO, wxT("重做"));
        model->SetLabel(MENU_ID_CUT, wxT("剪切"));
        model->SetLabel(MENU_ID_COPY, wxT("复制"));
        model->SetLabel(MENU_ID_PASTE, wxT("粘贴"));
        model->SetLabel(MENU_ID_DELETE, wxT("删除"));
        model->SetLabel(MENU_ID_SELECT_ALL, wxT("全选"));
    } else {
        // None
        model->Clear();
    }
}

bool ClientHandler::OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefContextMenuParams> params,
    int WXUNUSED(command_id),
    CefContextMenuHandler::EventFlags WXUNUSED(event_flags))
{
    return false;
}

void ClientHandler::OnContextMenuDismissed(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame)
{}

// CefLifeSpanHandler methods
bool ClientHandler::OnBeforePopup(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    const CefString& target_url,
    const CefString& target_frame_name,
    CefLifeSpanHandler::WindowOpenDisposition target_disposition,
    bool user_gesture,
    const CefPopupFeatures& WXUNUSED(popupFeatures),
    CefWindowInfo& WXUNUSED(windowInfo),
    CefRefPtr<CefClient>& WXUNUSED(client),
    CefBrowserSettings& WXUNUSED(settings),
    bool* WXUNUSED(no_javascript_access))
{
    switch (target_disposition)
    {
    case WOD_NEW_FOREGROUND_TAB:
//#ifdef _WIN32
//        OpenUseringDefaultProcess(utf82ansi(target_url.ToString()), true);
//#endif // _WIN32
//        return true;
    case WOD_NEW_BACKGROUND_TAB:
    case WOD_NEW_POPUP:
    case WOD_NEW_WINDOW:
        browser->GetMainFrame()->LoadURL(target_url);
        return true; //cancel create
    }
    return false;
}

void ClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
    if (!m_browser.get()) {
        m_browser = browser;
        m_browserId = browser->GetIdentifier();
    }
}
bool ClientHandler::DoClose(CefRefPtr<CefBrowser> browser)
{
    return false;
}

void ClientHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser)
{
    if (browser->GetIdentifier() == m_browserId) {
        m_browser = NULL;
    }
}

// CefLoadHandler methods
#if (CHROME_VERSION_BUILD <= 2623)
void ClientHandler::OnLoadStart(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame)
#else
void ClientHandler::OnLoadStart(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    TransitionType transition_type)
#endif
{
    wxString url = frame->GetURL().ToString();
    wxString target = frame->GetName().ToString();

    wxWebViewEvent event(wxEVT_COMMAND_WEBVIEW_NAVIGATING, m_webview->GetId(), url, target);
    event.SetEventObject(m_webview);

    m_webview->HandleWindowEvent(event);

    if (!event.IsAllowed()) {
        // We do not yet have support for vetoing pages
    }
}

void ClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    int httpStatusCode)

{
    // HACK: navigate to pendingURL as soon as possible
    if (!m_pendingURL.empty()) {
        GetBrowser()->GetMainFrame()->LoadURL(m_pendingURL.ToStdString());
        m_pendingURL.clear();
        return;
    }

    wxString url = frame->GetURL().ToString();
    wxString target = frame->GetName().ToString();

    wxWebViewEvent event(wxEVT_COMMAND_WEBVIEW_NAVIGATED, m_webview->GetId(), url, target);
    event.SetEventObject(m_webview);

    m_webview->HandleWindowEvent(event);

    if (frame->IsMain()) {
        //Get source code when main frame loads ended.
        CefRefPtr<CefStringVisitor> source_visitor = new wxStringVisitor(
            m_webview, wxStringVisitor::PAGE_SOURCE);
        frame->GetSource(source_visitor);

        //Get page text when main frame loads ended.
        CefRefPtr<CefStringVisitor> text_visitor = new wxStringVisitor(
            m_webview, wxStringVisitor::PAGE_TEXT);
        frame->GetText(text_visitor);

        //As we are complete we also add to the history list, but not if the
        //page is not the main page, ie it is a subframe
        if (m_webview->m_historyEnabled && !m_webview->m_historyLoadingFromList) {
            //If we are not at the end of the list, then erase everything
            //between us and the end before adding the new page
            if (m_webview->m_historyPosition != static_cast<int>(m_webview->m_historyList.size()) - 1) {
                m_webview->m_historyList.erase(m_webview->m_historyList.begin() + m_webview->m_historyPosition + 1,
                    m_webview->m_historyList.end());
            }
            wxSharedPtr<wxWebViewHistoryItem> item(new wxWebViewHistoryItem(url, m_webview->GetCurrentTitle()));
            m_webview->m_historyList.push_back(item);
            m_webview->m_historyPosition++;
        }
        //Reset as we are done now
        m_webview->m_historyLoadingFromList = false;

        wxWebViewEvent levent(wxEVT_COMMAND_WEBVIEW_LOADED, m_webview->GetId(), url, target);
        levent.SetEventObject(m_webview);

        m_webview->HandleWindowEvent(levent);
    }
}

void ClientHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    ErrorCode errorCode,
    const CefString& errorText,
    const CefString& failedUrl)
{
    //We define a macro for convenience
#define ERROR_TYPE_CASE(error, wxtype) case(error): \
    type = wxtype;\
    break

    wxWebViewNavigationError type = wxWEBVIEW_NAV_ERR_OTHER;
    switch (errorCode) {
        ERROR_TYPE_CASE(ERR_FAILED, wxWEBVIEW_NAV_ERR_OTHER);
        ERROR_TYPE_CASE(ERR_ABORTED, wxWEBVIEW_NAV_ERR_USER_CANCELLED);
        ERROR_TYPE_CASE(ERR_INVALID_ARGUMENT, wxWEBVIEW_NAV_ERR_OTHER);
        ERROR_TYPE_CASE(ERR_INVALID_HANDLE, wxWEBVIEW_NAV_ERR_OTHER);
        ERROR_TYPE_CASE(ERR_FILE_NOT_FOUND, wxWEBVIEW_NAV_ERR_NOT_FOUND);
        ERROR_TYPE_CASE(ERR_TIMED_OUT, wxWEBVIEW_NAV_ERR_CONNECTION);
        ERROR_TYPE_CASE(ERR_FILE_TOO_BIG, wxWEBVIEW_NAV_ERR_OTHER);
        ERROR_TYPE_CASE(ERR_UNEXPECTED, wxWEBVIEW_NAV_ERR_OTHER);
        ERROR_TYPE_CASE(ERR_ACCESS_DENIED, wxWEBVIEW_NAV_ERR_AUTH);
        ERROR_TYPE_CASE(ERR_NOT_IMPLEMENTED, wxWEBVIEW_NAV_ERR_OTHER);
        ERROR_TYPE_CASE(ERR_CONNECTION_CLOSED, wxWEBVIEW_NAV_ERR_CONNECTION);
        ERROR_TYPE_CASE(ERR_CONNECTION_RESET, wxWEBVIEW_NAV_ERR_CONNECTION);
        ERROR_TYPE_CASE(ERR_CONNECTION_REFUSED, wxWEBVIEW_NAV_ERR_CONNECTION);
        ERROR_TYPE_CASE(ERR_CONNECTION_ABORTED, wxWEBVIEW_NAV_ERR_CONNECTION);
        ERROR_TYPE_CASE(ERR_CONNECTION_FAILED, wxWEBVIEW_NAV_ERR_CONNECTION);
        ERROR_TYPE_CASE(ERR_NAME_NOT_RESOLVED, wxWEBVIEW_NAV_ERR_CONNECTION);
        ERROR_TYPE_CASE(ERR_INTERNET_DISCONNECTED, wxWEBVIEW_NAV_ERR_CONNECTION);
        ERROR_TYPE_CASE(ERR_SSL_PROTOCOL_ERROR, wxWEBVIEW_NAV_ERR_SECURITY);
        ERROR_TYPE_CASE(ERR_ADDRESS_INVALID, wxWEBVIEW_NAV_ERR_REQUEST);
        ERROR_TYPE_CASE(ERR_ADDRESS_UNREACHABLE, wxWEBVIEW_NAV_ERR_CONNECTION);
        ERROR_TYPE_CASE(ERR_SSL_CLIENT_AUTH_CERT_NEEDED, wxWEBVIEW_NAV_ERR_AUTH);
        ERROR_TYPE_CASE(ERR_TUNNEL_CONNECTION_FAILED, wxWEBVIEW_NAV_ERR_CONNECTION);
        ERROR_TYPE_CASE(ERR_NO_SSL_VERSIONS_ENABLED, wxWEBVIEW_NAV_ERR_SECURITY);
        ERROR_TYPE_CASE(ERR_SSL_VERSION_OR_CIPHER_MISMATCH, wxWEBVIEW_NAV_ERR_SECURITY);
        ERROR_TYPE_CASE(ERR_SSL_RENEGOTIATION_REQUESTED, wxWEBVIEW_NAV_ERR_REQUEST);
        ERROR_TYPE_CASE(ERR_CERT_COMMON_NAME_INVALID, wxWEBVIEW_NAV_ERR_CERTIFICATE);
        ERROR_TYPE_CASE(ERR_CERT_DATE_INVALID, wxWEBVIEW_NAV_ERR_CERTIFICATE);
        ERROR_TYPE_CASE(ERR_CERT_AUTHORITY_INVALID, wxWEBVIEW_NAV_ERR_CERTIFICATE);
        ERROR_TYPE_CASE(ERR_CERT_CONTAINS_ERRORS, wxWEBVIEW_NAV_ERR_CERTIFICATE);
        ERROR_TYPE_CASE(ERR_CERT_NO_REVOCATION_MECHANISM, wxWEBVIEW_NAV_ERR_CERTIFICATE);
        ERROR_TYPE_CASE(ERR_CERT_UNABLE_TO_CHECK_REVOCATION, wxWEBVIEW_NAV_ERR_CERTIFICATE);
        ERROR_TYPE_CASE(ERR_CERT_REVOKED, wxWEBVIEW_NAV_ERR_CERTIFICATE);
        ERROR_TYPE_CASE(ERR_CERT_INVALID, wxWEBVIEW_NAV_ERR_CERTIFICATE);
        ERROR_TYPE_CASE(ERR_CERT_END, wxWEBVIEW_NAV_ERR_CERTIFICATE);
        ERROR_TYPE_CASE(ERR_INVALID_URL, wxWEBVIEW_NAV_ERR_REQUEST);
        ERROR_TYPE_CASE(ERR_DISALLOWED_URL_SCHEME, wxWEBVIEW_NAV_ERR_REQUEST);
        ERROR_TYPE_CASE(ERR_UNKNOWN_URL_SCHEME, wxWEBVIEW_NAV_ERR_REQUEST);
        ERROR_TYPE_CASE(ERR_TOO_MANY_REDIRECTS, wxWEBVIEW_NAV_ERR_OTHER);
        ERROR_TYPE_CASE(ERR_UNSAFE_REDIRECT, wxWEBVIEW_NAV_ERR_SECURITY);
        ERROR_TYPE_CASE(ERR_UNSAFE_PORT, wxWEBVIEW_NAV_ERR_SECURITY);
        ERROR_TYPE_CASE(ERR_INVALID_RESPONSE, wxWEBVIEW_NAV_ERR_OTHER);
        ERROR_TYPE_CASE(ERR_INVALID_CHUNKED_ENCODING, wxWEBVIEW_NAV_ERR_OTHER);
        ERROR_TYPE_CASE(ERR_METHOD_NOT_SUPPORTED, wxWEBVIEW_NAV_ERR_OTHER);
        ERROR_TYPE_CASE(ERR_UNEXPECTED_PROXY_AUTH, wxWEBVIEW_NAV_ERR_OTHER);
        ERROR_TYPE_CASE(ERR_EMPTY_RESPONSE, wxWEBVIEW_NAV_ERR_OTHER);
        ERROR_TYPE_CASE(ERR_RESPONSE_HEADERS_TOO_BIG, wxWEBVIEW_NAV_ERR_OTHER);
        ERROR_TYPE_CASE(ERR_CACHE_MISS, wxWEBVIEW_NAV_ERR_OTHER);
        ERROR_TYPE_CASE(ERR_INSECURE_RESPONSE, wxWEBVIEW_NAV_ERR_SECURITY);
    }

    wxString url = failedUrl.ToString();
    wxString target = frame->GetName().ToString();
    wxWebViewEvent event(wxEVT_COMMAND_WEBVIEW_ERROR, m_webview->GetId(), url, target);
    event.SetEventObject(m_webview);
    event.SetInt(type);
    event.SetString(errorText.ToString());

    m_webview->HandleWindowEvent(event);
}

// CefDownloadHander
void ClientHandler::OnBeforeDownload(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefDownloadItem> download_item,
    const CefString& suggested_name,
    CefRefPtr<CefBeforeDownloadCallback> callback)
{

    callback->Continue(download_item->GetURL(), true);
}

void ClientHandler::OnDownloadUpdated(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefDownloadItem> download_item,
    CefRefPtr<CefDownloadItemCallback> callback)
{
    if (download_item->IsComplete())
    {
        GenerateDownloadEvent();
        if (browser->IsPopup() && !browser->HasDocument())
        {
            browser->GetHost()->CloseBrowser(true);
        }
    }
}

void ClientHandler::GenerateDownloadEvent()
{
    CEFRightMenuEvent e(wxEVT_COMMAND_CEFRIGHT_MENU, m_webview->GetId());
    
    e.SetEventObject(NULL);
    m_webview->GetEventHandler()->ProcessEvent(e);
}

bool ClientHandler::OnFileDialog(CefRefPtr<CefBrowser> browser,
	FileDialogMode mode,   
	const CefString& title, 
	const CefString& default_file_path,
	const std::vector<CefString>& accept_filters,
	int selected_accept_filter,   
	CefRefPtr<CefFileDialogCallback> callback)
{
    const wxString caption = title.ToWString();
    //const wxString  wildcard = wxT("All files (*.*)|*.*| PNG Image (*.png)|*.png|JPEG Image (*.jpg;*pjpeg;*jfif;*pjp)|*.jpg;*pjpeg;*jfif;*pjp|GIF Image (*.gif)|*.gif");
    wxString wildcard = wxT("All files(*.*)|*.*|");
    for (std::vector<CefString>::const_iterator it = accept_filters.begin(); it != accept_filters.end(); ++it)
    {
        wxString tempString = (*it).ToWString();

        if (tempString.Contains(wxT("/")))
        {
            int pos = tempString.Find("/");
            int size = tempString.size();
            wildcard.append(wxT("*."));
            wildcard.append(tempString.SubString(pos + 1, size - 1));
            wildcard.append(wxT(";"));
        }
        else
        {
            wildcard.append(wxT("*"));
            wildcard.append(tempString);
            wildcard.append(wxT(";"));
        }
    }
    wildcard.append(wxT("|"));
    for (std::vector<CefString>::const_iterator it = accept_filters.begin(); it != accept_filters.end(); ++it)
    {
        wxString tempString2 = (*it).ToWString();
        if (tempString2.Contains(wxT("/")))
        {
            int pos = tempString2.Find("/");
            int size = tempString2.size();
            wildcard.append(wxT("*."));
            wildcard.append(tempString2.SubString(pos + 1, size - 1));
            wildcard.append(wxT(";"));
        }
        else {
            wildcard.append(wxT("*"));
            wildcard.append(tempString2);
            wildcard.append(wxT(";"));
        }
    }
    wxString defaultFileName = wxEmptyString;
#ifdef _WIN32
    const wxString defaultDir = utf82ansi(default_file_path.ToString());
#else
    const wxString defaultDir = default_file_path.ToString();
#endif
    wxFileDialog dialog(NULL, caption, defaultDir, defaultFileName, wildcard, wxFD_MULTIPLE,
                        wxDefaultPosition, wxDefaultSize, wxFileDialogNameStr);
    if (dialog.ShowModal() == wxID_OK)
    {
        wxArrayString path;
        dialog.GetPaths(path);
        dialog.GetFilterIndex();
        vector<CefString> Cef;
        for (size_t i = 0; i < path.size(); i++)
        {
            wstring tmpStr = path[i].ToStdWstring();//shove::encode::Charset::s2ws(ansi2utf8(path[i].ToStdString()));
            Cef.push_back(CefString(tmpStr));
        }
        callback->Continue(selected_accept_filter, Cef);
        return true;
    }

    return true;

}

#if (CHROME_VERSION_BUILD <= 2623)
bool ClientHandler::OnJSDialog(CefRefPtr<CefBrowser> browser,
    const CefString& origin_url,
    const CefString& accept_lang,
    JSDialogType dialog_type,
    const CefString& message_text,
    const CefString& default_prompt_text,
    CefRefPtr<CefJSDialogCallback> callback,
    bool& suppress_message)
#else
bool ClientHandler::OnJSDialog(CefRefPtr<CefBrowser> browser,
    const CefString& origin_url,
    JSDialogType dialog_type,
    const CefString& message_text,
    const CefString& default_prompt_text,
    CefRefPtr<CefJSDialogCallback> callback,
    bool& suppress_message)
#endif

{
#ifdef _WIN32
    wxString message_te = utf82ansi(message_text);
#else
    wxString message_te = message_text.ToString();
#endif
    if (dialog_type == JSDIALOGTYPE_ALERT)
    {
        wxMessageBox(message_te);
        return true;
    }
    else if (dialog_type == JSDIALOGTYPE_CONFIRM)
    {
        wxMessageDialog d(0L, message_te,wxT("来自网页的消息"),wxOK|wxCANCEL);
        int r = d.ShowModal();
        if (wxID_OK == r)
        {
            suppress_message = false;
            callback->Continue(true, "");
            return true;
        }
        else if (wxID_CANCEL == r)
        {
            suppress_message = false;
            callback->Continue(false, "");
            return true;
        }
    }
    return false;
}

bool ClientHandler::OnQuotaRequest(CefRefPtr<CefBrowser> browser,
    const CefString& origin_url,
    int64 new_size,
    CefRefPtr<CefRequestCallback> callback) {
    CEF_REQUIRE_IO_THREAD();
	static const int64 max_size = 1024 * 1024 * 20;  // 20mb.

    // Grant the quota request if the size is reasonable.
    callback->Continue(new_size <= max_size);
    return true;
}

cef_return_value_t ClientHandler::OnBeforeResourceLoad(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefRefPtr<CefRequest> request,
    CefRefPtr<CefRequestCallback> callback)
{
    return RV_CONTINUE;
}

void ClientHandler::OnPluginCrashed(CefRefPtr<CefBrowser> browser,
    const CefString& plugin_path)
{
#ifdef _WIN32
    wxString label = wxT("插件异常:");
    wxString path = plugin_path.ToString16();
    label += path;
    wxMessageBox(label);
#endif
}

bool ClientHandler::OnDragEnter(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefDragData> dragData,
                                CefDragHandler::DragOperationsMask mask) {
#ifdef __WXMAC__
    CEF_REQUIRE_UI_THREAD();
#endif
    // Forbid dragging of URLs and files.
    if ((mask & DRAG_OPERATION_LINK) && !dragData->IsFragment()) {
        //test_runner::Alert(browser, "cefclient blocks dragging of URLs and files");
        return true;
    }
    
    return false;
}

void ClientHandler::OnDraggableRegionsChanged(
                                              CefRefPtr<CefBrowser> browser,
                                              const std::vector<CefDraggableRegion>& regions) {

#ifdef __WXMAC__
    CEF_REQUIRE_UI_THREAD();
#endif
    NotifyDraggableRegions(regions);
}

void ClientHandler::OnTakeFocus(CefRefPtr<CefBrowser> browser, bool next) {

#ifdef __WXMAC__
    CEF_REQUIRE_UI_THREAD();
#endif
    
    NotifyTakeFocus(next);
}

//bool ClientHandler::OnRequestGeolocationPermission(
//                                                   CefRefPtr<CefBrowser> browser,
//                                                   const CefString& requesting_url,
//                                                   int request_id,
//                                                   CefRefPtr<CefGeolocationCallback> callback) {
//
//#ifdef __WXMAC__
//    CEF_REQUIRE_UI_THREAD();
//#endif
//    
//    // Allow geolocation access from all websites.
//    callback->Continue(true);
//    return true;
//}

bool ClientHandler::OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                                  const CefKeyEvent& event,
                                  CefEventHandle os_event,
                                  bool* is_keyboard_shortcut) {

#ifdef __WXMAC__
    CEF_REQUIRE_UI_THREAD();
#endif
    if (!event.focus_on_editable_field && event.windows_key_code == 0x20) {
        // Special handling for the space character when an input element does not
        // have focus. Handling the event in OnPreKeyEvent() keeps the event from
        // being processed in the renderer. If we instead handled the event in the
        // OnKeyEvent() method the space key would cause the window to scroll in
        // addition to showing the alert box.
        if (event.type == KEYEVENT_RAWKEYDOWN)
            //test_runner::Alert(browser, "You pressed the space bar!");
        return true;
    }
    
    return false;
}

void ClientHandler::NotifyDraggableRegions(const std::vector<CefDraggableRegion>& regions) {

#if 0
    if (!CURRENTLY_ON_MAIN_THREAD()) {
        // Execute this method on the main thread.
        MAIN_POST_CLOSURE(base::Bind(&ClientHandler::NotifyDraggableRegions, this, regions));
        return;
    }
#endif
}

void ClientHandler::NotifyTakeFocus(bool next) {

#if 0
    if (!CURRENTLY_ON_MAIN_THREAD()) {
        // Execute this method on the main thread.
        MAIN_POST_CLOSURE(base::Bind(&ClientHandler::NotifyTakeFocus, this, next));
        return;
    }
#endif
}

void ClientHandler::CloseAllBrowsers(bool force_close)
{
    m_browser->GetHost()->CloseBrowser(force_close);
}

//#endif
