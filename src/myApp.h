#ifndef __MY_APP_H__
#define __MY_APP_H__
#include <wx/app.h>
#include <wx/frame.h>
#include <wx/webview.h>

class myApp : public wxApp
{
public:
    virtual bool OnInit();

};

class myFrame : public wxFrame
{
public:
    myFrame(wxFrame *frame, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxDEFAULT_FRAME_STYLE,
        const wxString& name = wxFrameNameStr);
    ~myFrame();
private:
    wxWebView * m_webview;
};
#endif