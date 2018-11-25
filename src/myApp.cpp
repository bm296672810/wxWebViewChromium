#include <wx/sizer.h>
#include <wx/image.h>

#include "myApp.h"
#include "webview_chromium.h"

IMPLEMENT_APP(myApp);

bool myApp::OnInit()
{
    int code = 0;
    wxWebViewChromium::StartUpSubprocess();
    if (!wxWebViewChromium::StartUp(code, ""))
        exit(code);

    wxInitAllImageHandlers();

    myFrame* frame = new myFrame(0, wxID_ANY, wxT("ä¯ÀÀÆ÷"), wxDefaultPosition, wxSize(1024, 800));
    frame->Show();

    return true;
}

int myApp::OnExit()
{
    //CefQuitMessageLoop();
    CefShutdown();
    return wxApp::OnExit();
}

myFrame::myFrame(wxFrame *frame, wxWindowID id, const wxString& title, const wxPoint& pos,
    const wxSize& size, long style,
    const wxString& name): wxFrame(frame,id,title,pos,size,style,name)
{
    wxWebView::RegisterFactory(wxWebViewBackendChromium, wxSharedPtr < wxWebViewFactory >
        (new wxWebViewFactoryChromium));
    m_webview = wxWebView::New(this, wxID_ANY, "www.baidu.com");
    
    wxBoxSizer* boxSizer = new wxBoxSizer(wxHORIZONTAL);

    boxSizer->Add(m_webview, 1, wxALL | wxEXPAND, 0);

    SetSizer(boxSizer);
}

myFrame::~myFrame()
{

}