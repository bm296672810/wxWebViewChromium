#ifndef __MY_CEF_CLIENT_HANDLER_H__
#define __MY_CEF_CLIENT_HANDLER_H__

#include <wx/defs.h>
#include <wx/string.h>
#include <wx/msgdlg.h>
#include <wx/filedlg.h>

#include <include/cef_browser.h>
#include <include/cef_client.h>
#include <include/cef_process_message.h>
#include <include/base/cef_logging.h>
#include <include/cef_version.h>

#include "MyCefMessage.h"
#include "MyCefV8Handler.h"
#include "MyCefApp.h"

#include "../webview_chromium.h"

class ClientHandler;

extern ClientHandler* g_clientHander;

class wxWebViewChromium;

class ClientHandler : public CefClient,
    public CefContextMenuHandler,
    public CefDisplayHandler,
    public CefLifeSpanHandler,
    public CefDownloadHandler,
    public CefJSDialogHandler,
    public CefLoadHandler,

    public CefDragHandler,
    public CefFocusHandler,
    //public CefGeolocationHandler,
    public CefKeyboardHandler,
    public CefDialogHandler,

    public CefRequestHandler {

public:
    ClientHandler() { g_clientHander = this; }
    virtual ~ClientHandler() {}

    static ClientHandler* GetInstance() { return g_clientHander; }

    virtual CefRefPtr<CefContextMenuHandler> GetContextMenuHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefDownloadHandler> GetDownloadHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefJSDialogHandler> GetJSDialogHandler() OVERRIDE { return this; }

    virtual CefRefPtr<CefDragHandler> GetDragHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefFocusHandler> GetFocusHandler() OVERRIDE { return this; }
    //virtual CefRefPtr<CefGeolocationHandler> GetGeolocationHandler() OVERRIDE {return this; }
    virtual CefRefPtr<CefKeyboardHandler> GetKeyboardHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefDialogHandler> GetDialogHandler() OVERRIDE { return this; }
    virtual CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE { return this; }

    virtual bool OnProcessMessageReceived(CefRefPtr<CefBrowser> browser,
        CefProcessId source_process,
        CefRefPtr<CefProcessMessage> message)OVERRIDE;
    // CefDisplayHandler methods
    virtual void OnAddressChange(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& url)OVERRIDE;
    virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
        const CefString& title)OVERRIDE;

#if CHROME_VERSION_BUILD < 3538
    virtual bool OnConsoleMessage(CefRefPtr<CefBrowser> WXUNUSED(browser),
        const CefString& WXUNUSED(message),
        const CefString& WXUNUSED(source),
        int WXUNUSED(line))OVERRIDE;
#endif
    // CefContextMenuHandler methods
    virtual void OnBeforeContextMenu(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefContextMenuParams> params,
        CefRefPtr<CefMenuModel> model)OVERRIDE;
    virtual bool OnContextMenuCommand(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        CefRefPtr<CefContextMenuParams> params,
        int command_id,
        CefContextMenuHandler::EventFlags event_flags)OVERRIDE;
    virtual void OnContextMenuDismissed(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame)OVERRIDE;

    // CefLifeSpanHandler methods
    virtual bool OnBeforePopup(CefRefPtr<CefBrowser> browser,
        CefRefPtr<CefFrame> frame,
        const CefString& target_url,
        const CefString& target_frame_name,
        CefLifeSpanHandler::WindowOpenDisposition target_disposition,
        bool user_gesture,
        const CefPopupFeatures& popupFeatures,
        CefWindowInfo& windowInfo,
        CefRefPtr<CefClient>& client,
        CefBrowserSettings& settings,
        bool* no_javascript_access)OVERRIDE;
    virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser)OVERRIDE;
    virtual bool DoClose(CefRefPtr<CefBrowser> browser)OVERRIDE;
    virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser)OVERRIDE;

    // CefLoadHandler methods
    virtual void OnLoadingStateChange(CefRefPtr<CefBrowser> browser,
        bool isLoading, bool canGoBack,
        bool canGoForward)OVERRIDE;
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

    //// CefGeolocationHandler methods
    //virtual bool OnRequestGeolocationPermission(
    //                                CefRefPtr<CefBrowser> browser,
    //                                const CefString& requesting_url,
    //                                int request_id,
    //                                CefRefPtr<CefGeolocationCallback> callback) OVERRIDE;

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

#endif