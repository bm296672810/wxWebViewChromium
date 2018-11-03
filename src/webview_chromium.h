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

#include <include/cef_browser.h>
#include <include/cef_client.h>
#include <include/cef_process_message.h>
#include <include/base/cef_logging.h>
#include <include/cef_version.h>

#include "src/webview/MyCefMessage.h"
#include "src/webview/MyCefV8Handler.h"
#include "src/webview/MyCefApp.h"

#ifdef __VISUALC__
#pragma warning(pop)
#endif
#include "MyEvent.h"
#include "code.h"

#define CEF_REQUIRE_IO_THREAD() DCHECK(CefCurrentlyOn(TID_IO));
//static void myWriteLog(const string& log);
using namespace std;
extern const char wxWebViewBackendChromium[];

class wxWebViewChromium;
struct CFuncName;
struct MessageName;
// ClientHandler implementation.
class ClientHandler;

extern ClientHandler* g_clientHander;

class ClientHandler : public CefClient,
    public CefContextMenuHandler,
    public CefDisplayHandler,
    public CefLifeSpanHandler,
    public CefDownloadHandler,
    public CefJSDialogHandler,
    public CefLoadHandler,

    public CefDragHandler,
    public CefFocusHandler,
    public CefGeolocationHandler,
    public CefKeyboardHandler,
	public CefDialogHandler,

    public CefRequestHandler{

public:
    ClientHandler() { g_clientHander = this; }
    virtual ~ClientHandler() {}
        
    static ClientHandler* GetInstance(){ return g_clientHander; }

    virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE{ return this; }
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE{ return this; }
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE{ return this; }
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE{ return this; }
    virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler() OVERRIDE{ return this; }
    virtual CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() OVERRIDE{ return this; }

    virtual CefRefPtr<CefDragHandler> GetDragHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefFocusHandler> GetFocusHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefGeolocationHandler> GetGeolocationHandler() OVERRIDE {return this; }
    virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() OVERRIDE { return this; }
	virtual CefRefPtr<CefDialogHandler> GetDialogHandler() OVERRIDE{ return this; }
    virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE{ return this; }

    virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
        CefProcessId WXUNUSED(source_process),
        CefRefPtr<CefProcessMessage> message)OVERRIDE;
    // CefDisplayHandler methods
    virtual void OnAddressChange(CefRefPtr<CefBrowser> WXUNUSED(browser),
        CefRefPtr<CefFrame> WXUNUSED(frame),
        const CefString& WXUNUSED(url))OVERRIDE;
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
        const CefString& title)OVERRIDE;
    virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> WXUNUSED(browser),
        const CefString& WXUNUSED(message),
        const CefString& WXUNUSED(source),
        int WXUNUSED(line))OVERRIDE;

    // CefContextMenuHandler methods
    virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefContextMenuParams> params,
        CefRefPtr<CefMenuModel> model)OVERRIDE;
    virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefContextMenuParams> params,
        int WXUNUSED(command_id),
        CefContextMenuHandler::EventFlags WXUNUSED(event_flags))OVERRIDE;
    virtual void OnContextMenuDismissed(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame)OVERRIDE;

    // CefLifeSpanHandler methods
    virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& target_url,
        const CefString& target_frame_name,
        CefLifeSpanHandler::WindowOpenDisposition target_disposition,
        bool user_gesture,
        const CefPopupFeatures& WXUNUSED(popupFeatures),
        CefWindowInfo& WXUNUSED(windowInfo),
        CefRefPtr<CefClient>& WXUNUSED(client),
        CefBrowserSettings& WXUNUSED(settings),
        bool* WXUNUSED(no_javascript_access))OVERRIDE;
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser)OVERRIDE;
    virtual bool DoClose(CefRefPtr<CefBrowser> browser)OVERRIDE;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser)OVERRIDE;

    // CefLoadHandler methods
    virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> WXUNUSED(browser),
        bool WXUNUSED(isLoading), bool WXUNUSED(canGoBack),
        bool WXUNUSED(canGoForward))OVERRIDE;
#if (CHROME_VERSION_BUILD <= 2623)
    virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame) OVERRIDE;
#else
    virtual void OnLoadStart(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        TransitionType transition_type) OVERRIDE;
#endif
    virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        int httpStatusCode) OVERRIDE;
    virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        ErrorCode errorCode,
        const CefString& errorText,
        const CefString& failedUrl)OVERRIDE;

    CefRefPtr<CefBrowser> GetBrowser() { return m_browser; }

    void SetWebView(wxWebViewChromium *webview) { m_webview = webview; }

    void SetPendingURL(const wxString& url) { m_pendingURL = url; };

    // CefDownloadHander
    void OnBeforeDownload(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefDownloadItem> download_item,
        const CefString& suggested_name,
        CefRefPtr<CefBeforeDownloadCallback> callback)OVERRIDE;
    void OnDownloadUpdated(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefDownloadItem> download_item,
        CefRefPtr<CefDownloadItemCallback> callback)OVERRIDE;

    void GenerateDownloadEvent();

    // CefJSDialogHandler
#if (CHROME_VERSION_BUILD <= 2623)
    virtual bool OnJSDialog(CefRefPtr<CefBrowser> browser,
        const CefString& origin_url,
        const CefString& accept_lang,
        JSDialogType dialog_type,
        const CefString& message_text,
        const CefString& default_prompt_text,
        CefRefPtr<CefJSDialogCallback> callback,
        bool& suppress_message) OVERRIDE;
#else
    virtual bool OnJSDialog(CefRefPtr<CefBrowser> browser,
        const CefString& origin_url,
        JSDialogType dialog_type,
        const CefString& message_text,
        const CefString& default_prompt_text,
        CefRefPtr<CefJSDialogCallback> callback,
        bool& suppress_message) OVERRIDE;
#endif
	//重写选择文件对话框
	virtual bool OnFileDialog(CefRefPtr<CefBrowser> browser,
		FileDialogMode mode,   //代表对话的类型
		const CefString& title, //对话框的标题默认为空
		const CefString& default_file_path,
		const std::vector<CefString>& accept_filters,
		int selected_accept_filter,
		CefRefPtr<CefFileDialogCallback> callback)OVERRIDE;

    // CefDragHandler methods
    virtual bool OnDragEnter(CefRefPtr<CefBrowser> browser,
                 CefRefPtr<CefDragData> dragData,
                 CefDragHandler::DragOperationsMask mask) OVERRIDE;
    virtual void OnDraggableRegionsChanged(
                               CefRefPtr<CefBrowser> browser,
                               const std::vector<CefDraggableRegion>& regions) OVERRIDE;

    // CefFocusHandler methods
    virtual void OnTakeFocus(CefRefPtr<CefBrowser> browser, bool next) OVERRIDE;

    // CefGeolocationHandler methods
    virtual bool OnRequestGeolocationPermission(
                                    CefRefPtr<CefBrowser> browser,
                                    const CefString& requesting_url,
                                    int request_id,
                                    CefRefPtr<CefGeolocationCallback> callback) OVERRIDE;

    // CefKeyboardHandler methods
    virtual bool OnPreKeyEvent(CefRefPtr<CefBrowser> browser,
                   const CefKeyEvent& event,
                   CefEventHandle os_event,
                   bool* is_keyboard_shortcut) OVERRIDE;



    virtual bool OnQuotaRequest(CefRefPtr<CefBrowser> browser,
        const CefString& origin_url,
        int64 new_size,
        CefRefPtr<CefRequestCallback> callback)OVERRIDE;
    // cef 加载资源
    virtual cef_return_value_t OnBeforeResourceLoad(
        CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefRequest> request,
        CefRefPtr<CefRequestCallback> callback) OVERRIDE;
    virtual void OnPluginCrashed(CefRefPtr<CefBrowser> browser,
        const CefString& plugin_path) OVERRIDE;

    void NotifyDraggableRegions(const std::vector<CefDraggableRegion>& regions);
    void NotifyTakeFocus(bool next);

    void CloseAllBrowsers(bool force_close);
private:
    CefRefPtr<CefBrowser> m_browser;
    wxWebViewChromium *m_webview;
    int m_browserId;
    wxString m_pendingURL;

    IMPLEMENT_REFCOUNTING(ClientHandler);
};

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
