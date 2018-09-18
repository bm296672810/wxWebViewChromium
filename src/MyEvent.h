#ifndef MYEVENT_H
#define MYEVENT_H
#include <vector>
#include <string>
#include "wx/event.h"
//这个事件是标记所有为已读事件，主要是用于myTreeCtrl里面点击标记为已读菜单项产生的事件在MainWindowMain.cpp里面捕获事件
class MyRemaskEvent : public wxCommandEvent
{
public:
    MyRemaskEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandType, id)
    {}
    MyRemaskEvent(const MyRemaskEvent& event)
        : wxCommandEvent(event)
    {}
    virtual wxEvent* Clone() const
    {
        return new MyRemaskEvent(*this);
    }
    DECLARE_DYNAMIC_CLASS(MyRemaskEvent)
protected:

private:
};
//这个事件只是为了传递myAddressItem被点击之后的信号给myAddressCtrl接收
class MyTreeItemClickedEvent : public wxCommandEvent
{
public:
    MyTreeItemClickedEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandType, id)
    {}
    MyTreeItemClickedEvent(const MyTreeItemClickedEvent& event)
        : wxCommandEvent(event)
    {}
    virtual wxEvent*Clone()const
    {
        return new MyTreeItemClickedEvent(*this);
    }
    DECLARE_DYNAMIC_CLASS(MyTreeItemClickedEvent)
    //item id
    void SetItemId(long id)
    {
        m_itemId = id;
    }
    long GetItemId()
    {
        return m_itemId;
    }
    void SetContant(const wxArrayString& constant)
    {
        m_ArrStrContant = constant;
    }
    wxArrayString& GetContant()
    {
        return m_ArrStrContant;
    }
    //数据库ID
    void SetDataId(long id)
    {
        m_DataId = id;
    }
    long GetDataId()
    {
        return m_DataId;
    }
private:
    long m_itemId = -1;
    long m_DataId = -1;
    wxArrayString m_ArrStrContant;
};

//该事件是由NewAccount.cpp里创建按键点击产生并在MainWindowMain.cpp里进行捕获
//当添加联系人完成之后用该信号通知刷新联系人列表
class NewAccountOkClickedEvent : public wxCommandEvent
{
public:
    NewAccountOkClickedEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandType, id)
    {}
    NewAccountOkClickedEvent(const NewAccountOkClickedEvent& event)
        : wxCommandEvent(event)
    {}
    virtual wxEvent* Clone()const
    {
        return new NewAccountOkClickedEvent(*this);
    }
    DECLARE_DYNAMIC_CLASS(NewAccountOkClickedEvent)

};

class MyNBValueChangeEvent : public wxCommandEvent
{
public:
    MyNBValueChangeEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandType, id)
    {}
    MyNBValueChangeEvent(const MyNBValueChangeEvent& event)
        : wxCommandEvent(event)
    {}
    virtual wxEvent* Clone()const
    {
        return new MyNBValueChangeEvent(*this);
    }
    DECLARE_DYNAMIC_CLASS(MyNBValueChangeEvent)
};

//该事件是邮件预览中，标题的星标被点击之后产生的事件，该事件主要是用于传递邮件的星标状态以同步邮件预览类和邮件列表类的星标状态
class MyStarChangeEvent : public wxCommandEvent
{
public:
    MyStarChangeEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandType, id)
    {}
    MyStarChangeEvent(const MyStarChangeEvent& event)
        : wxCommandEvent(event)
    {}
    virtual wxEvent* Clone()const
    {
        return new MyStarChangeEvent(*this);
    }
    void SetStar(bool isStar){ m_BisStar = isStar; }
    bool GetStar(){ return m_BisStar; }
private:
    bool m_BisStar = false;
    DECLARE_DYNAMIC_CLASS(MyStarChangeEvent)
};

// 处理收取到一封邮件后，通知邮件列表更新UI
class MyCustomEvent : public wxCommandEvent {
public:
    MyCustomEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandType, id)
    {}
    MyCustomEvent(const MyCustomEvent& myReceivMailEvent)
        : wxCommandEvent(myReceivMailEvent)
    {}
    ~MyCustomEvent() {}
    virtual wxEvent* Clone()const
    {
        return new MyCustomEvent(*this);
    }
    DECLARE_DYNAMIC_CLASS(MyCustomEvent)

};

//myDataViewTreeCtrl的change发生改变时产生的事件
class MyDataViewSelectChangeEvent : public wxCommandEvent
{
public:
    MyDataViewSelectChangeEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandType, id)
    {}
    MyDataViewSelectChangeEvent(const MyDataViewSelectChangeEvent& event)
        : wxCommandEvent(event)
    {}
    ~MyDataViewSelectChangeEvent(){}
    virtual wxEvent*Clone()const{ return new MyDataViewSelectChangeEvent(*this); }
    DECLARE_DYNAMIC_CLASS(MyDataViewSelectChangeEvent)

    wxString m_text;
};
//list的未读状态发生改变时产生的事件，用于刷新邮件夹列表上面的未读数量
class MyReadStatChangeEvent : public wxCommandEvent
{
public:
    MyReadStatChangeEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandType, id)
    {}
    MyReadStatChangeEvent(const MyReadStatChangeEvent& event)
        : wxCommandEvent(event)
    {}
    ~MyReadStatChangeEvent(){}
    virtual wxEvent*Clone()const{ return new MyReadStatChangeEvent(*this); }
    DECLARE_DYNAMIC_CLASS(MyReadStatChangeEvent)
};

//删除完通讯录中分组之后,刷新分组控件中的字段
class SetUpGroupEvent : public wxCommandEvent
{
public:
    SetUpGroupEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandType, id)
    {}
    SetUpGroupEvent(const SetUpGroupEvent& event)
        : wxCommandEvent(event)
    {}
    ~SetUpGroupEvent(){}
    virtual wxEvent*Clone()const{ return new SetUpGroupEvent(*this); }
    DECLARE_DYNAMIC_CLASS(SetUpGroupEvent)
};

//编辑完通讯录中分组之后,刷新分组控件中的字段
class EditGroupEvent : public wxCommandEvent
{
public:
    EditGroupEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandType, id)
    {}
    EditGroupEvent(const EditGroupEvent& event)
        : wxCommandEvent(event)
    {}
    ~EditGroupEvent(){}
    virtual wxEvent*Clone()const{ return new EditGroupEvent(*this); }
    DECLARE_DYNAMIC_CLASS(EditGroupEvent)
};

//新建账号完成之后，刷新主窗口中的账号列表
//class AddNewaccountEvent : public wxCommandEvent
//{
//public:
//	AddNewaccountEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
//		: wxCommandEvent(commandType, id)
//	{}
//	AddNewaccountEvent(const EditGroupEvent& event)
//		: wxCommandEvent(event)
//	{}
//	~AddNewaccountEvent(){}
//	virtual wxEvent*Clone()const{ return new AddNewaccountEvent(*this); }
//	DECLARE_DYNAMIC_CLASS(AddNewaccountEvent)
//};

//浏览器中接收到右键菜单时产生的事件，主要用于CEF和主程序的通信
class CEFRightMenuEvent : public wxCommandEvent
{
public:
    CEFRightMenuEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandType, id)
    {}
    CEFRightMenuEvent(const EditGroupEvent& event)
        : wxCommandEvent(event)
    {}
    ~CEFRightMenuEvent(){}
    bool SetVecString(size_t index, std::string str);
    std::vector<std::string> GetVecString()const{ return m_string; }
    virtual wxEvent*Clone()const{ return new CEFRightMenuEvent(*this); }
    DECLARE_DYNAMIC_CLASS(CEFRightMenuEvent)
private:
    std::vector<std::string> m_string;
};

class MyShiftAndCtrlEvent : public wxCommandEvent
{
public:
    MyShiftAndCtrlEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandType, id)
    {}
    MyShiftAndCtrlEvent(const MyShiftAndCtrlEvent& event)
        : wxCommandEvent(event)
    {}
    ~MyShiftAndCtrlEvent(){}
    void SetShiftDown(){ m_bShiftDown = true; }
    bool GetShiftDown(){ return m_bShiftDown; }
    void SetCtrlDown(){ m_bCtrlDown = true; }
    bool GetCtrlDown(){ return m_bCtrlDown; }
    void SetMouseEvent(const wxMouseEvent*mouseEvent){ m_mouseEvent = const_cast<wxMouseEvent*>(mouseEvent); }
    wxMouseEvent* GetMouseEvent(){ return m_mouseEvent; }
    virtual wxEvent*Clone()const{ return new MyShiftAndCtrlEvent(*this); }
    DECLARE_DYNAMIC_CLASS(MyShiftAndCtrlEvent)
private:
    bool m_bShiftDown = false;
    bool m_bCtrlDown = false;
    wxMouseEvent *m_mouseEvent;
};
//-----------------------------
class MySetUpAccountEvent : public wxCommandEvent
{
public:
    MySetUpAccountEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandType, id)
    {}
    MySetUpAccountEvent(const MySetUpAccountEvent& event)
        : wxCommandEvent(event)
    {}
    virtual wxEvent* Clone() const
    {
        return new MySetUpAccountEvent(*this);
    }
    DECLARE_DYNAMIC_CLASS(MySetUpAccountEvent)
protected:

private:
};
//通讯录Item点击中公司、部门、邮箱联系人等item点击之后产生的事件
class MyAddressBookTreeCtrlItemClickEvent : public wxCommandEvent {
public:
    MyAddressBookTreeCtrlItemClickEvent(wxEventType commandType = wxEVT_NULL, int id = 0,std::string companyId = "",std::string departmentId = "")
        : wxCommandEvent(commandType, id), m_companyId(companyId), m_departmentId(departmentId)
    {}
    MyAddressBookTreeCtrlItemClickEvent(const MyAddressBookTreeCtrlItemClickEvent& myReceivMailEvent)
        : wxCommandEvent(myReceivMailEvent)
    {}
    ~MyAddressBookTreeCtrlItemClickEvent() {}
    virtual wxEvent* Clone()const
    {
        return new MyAddressBookTreeCtrlItemClickEvent(*this);
    }
    const std::string& GetCompanyId(){ return m_companyId; }
    const std::string& GetDepartmentId(){ return m_departmentId; }
    void SetCompanyId(std::string companyId){ m_companyId = companyId; }
    void SetDepartmentId(std::string departmentId){ m_departmentId = departmentId; }
    DECLARE_DYNAMIC_CLASS(MyAddressBookTreeCtrlItemClickEvent)
private:
    std::string m_companyId;
    std::string m_departmentId;
};
// 应用点击事件，该事件是应用点击之后 js调用c++代码 触发的事件
class MyApplicationClickedEvent : public wxCommandEvent{
public:
    MyApplicationClickedEvent(wxEventType commandType = wxEVT_NULL, int id = 0, std::string content = "")
        : wxCommandEvent(commandType, id), m_content(content)
    {}
    MyApplicationClickedEvent(const MyApplicationClickedEvent& myAppEvent)
        : wxCommandEvent(myAppEvent)
    {}
    ~MyApplicationClickedEvent(){}
    virtual wxEvent* Clone()const
    {
        return new MyApplicationClickedEvent(*this);
    }
    void SetContent(std::string content){ m_content = content; }
    const std::string& GetContent(){ return m_content; }
    DECLARE_DYNAMIC_CLASS(MyApplicationClickedEvent)
private:
    std::string m_content;
};

class SaveMailEvent : public wxCommandEvent {
public:
    SaveMailEvent(wxEventType commandType = wxEVT_NULL, int id = 0)
        : wxCommandEvent(commandType, id)
    {}
    SaveMailEvent(const SaveMailEvent& event)
        : wxCommandEvent(event)
    {}
    virtual wxEvent* Clone() const
    {
        return new SaveMailEvent(*this);
    }

    DECLARE_DYNAMIC_CLASS(SaveMailEvent)
protected:

private:
};

BEGIN_DECLARE_EVENT_TYPES()
DECLARE_EVENT_TYPE(wxEVT_COMMAND_REMASK_ALL, 801)           //801只是为了兼容更早版本
DECLARE_EVENT_TYPE(wxEVT_COMMAND_MYTREEITEM_CLICKED, 801)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_NEWACCOUNTOK_CLICKED, 801)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_MYNB_VALUE_CHANGE, 801)             //自定义MyNoteBook 里值改变事件
DECLARE_EVENT_TYPE(wxEVT_COMMAND_MYSTAR_CHANGE, 801)					//自定义星标状态改变事件类型
DECLARE_EVENT_TYPE(wxEVT_COMMAND_UPDATE_MAILLSIT, 801)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_MYDATAVIEW_SELECT_CHANGE, 801)		//自定义dataViewitemchange事件
DECLARE_EVENT_TYPE(wxEVT_COMMAND_MYREAD_STAT_CHANGE, 801)			//自定义list的未读状态发生改变的事件
DECLARE_EVENT_TYPE(wxEVT_COMMAND_SETUP_GROUP, 801)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_EDIT_GROUP, 801)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_ADD_NEW_ACCOUNT, 801)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_CEFRIGHT_MENU, 801)				//Cef的右键菜单
DECLARE_EVENT_TYPE(wxEVT_COMMAND_MYSHIFTANDCTRL, 801)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_SETUP_ACCOUNT, 801)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_MYADDRESSBOOK_TREECTRL_ITEMCLICK,801)
DECLARE_EVENT_TYPE(wxEVT_COMMAND_MYAPPLICATION_CLICKED, 801)        // js调用的应用点击事件
DECLARE_EVENT_TYPE(wxEVT_COMMAND_SAVEMAIL, 801)
END_DECLARE_EVENT_TYPES()

// typedef 函数指针
typedef void (wxEvtHandler::*MyRemaskEventFunction) (MyRemaskEvent&);
typedef void (wxEvtHandler::*MyTreeItemClickedFunction) (MyTreeItemClickedEvent&);
typedef void (wxEvtHandler::*NewAccountOkClickedFunction) (NewAccountOkClickedEvent&);
typedef void (wxEvtHandler::*MyNBValueChangeFunction) (MyNBValueChangeEvent&);
typedef void (wxEvtHandler::*MyStarChangeFunction) (MyStarChangeEvent&);			//定义星标改变事件的类型
typedef void (wxEvtHandler::*MyCustomEventFunction)(MyCustomEvent&);
typedef void (wxEvtHandler::*MyDataViewSelectChangeFunction) (MyDataViewSelectChangeEvent&); //自定义dataViewitemchange事件
typedef void (wxEvtHandler::*MyReadStatChangeFunction) (MyReadStatChangeEvent&);			//自定义list的未读状态发生改变的事件
typedef void (wxEvtHandler::*SetUpGroupFunction) (SetUpGroupEvent&);
typedef void (wxEvtHandler::*EditGroupFunction) (EditGroupEvent&);
//typedef void (wxEvtHandler::*AddNewaccountFunction) (AddNewaccountEvent&);
typedef void (wxEvtHandler::*CEFRightMenuFunction) (CEFRightMenuEvent&);				//Cef右键菜单
typedef void (wxEvtHandler::*MyShiftAndCtrlFunction) (MyShiftAndCtrlEvent&);			//shift和ctrl事件
typedef void (wxEvtHandler::*MySetUpAccountFunction) (MySetUpAccountEvent&);
typedef void (wxEvtHandler::*MyAddressBookTreeCtrlItemClickFunction) (MyAddressBookTreeCtrlItemClickEvent&);  //myAddressBookAndDataViewCtrl item点击事件
typedef void (wxEvtHandler::*MyApplicationClickedFunction) (MyApplicationClickedEvent&);  //myAddressBookAndDataViewCtrl item点击事件
typedef void (wxEvtHandler::*SaveMailEventFunction) (SaveMailEvent&);
//定义处理函数
#define  wxMyStarChangeHandler(func) \
	wxEVENT_HANDLER_CAST(MyStarChangeFunction,func)

// 收取邮件，更新邮件列表
#define wxMyCustomEvent (fn)\
wxEVENT_HANDLER_CAST(MyCustomEventFunction,fn)

#define CEFRightMenuEventHandler(func) \
    wxEVENT_HANDLER_CAST(CEFRightMenuFunction, func)

#define EVT_COMMAND_UPDATE_MAILLSIT(id,fn) \
DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_UPDATE_MAILLSIT,\
                        id,wxID_ANY,\
                        (wxObjectEventFunction)(wxEventFunction)\
                        (MyCustomEventFunction) & fn,\
                        (wxObject*)NULL),

//wxDEFINE_EVENT(wxEVT_COMMAND_REMASK_ALL, wxCommandEvent);
#define EVT_COMMAND_REMASK_ALL(id,fn) \
DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_REMASK_ALL,\
                        id,wxID_ANY,\
                        (wxObjectEventFunction)(wxEventFunction)\
                        (MyRemaskEventFunction) & fn,\
                        (wxObject*)NULL),

#define EVT_COMMAND_MYTREEITEM_CLICKED(id,fn) \
DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_MYTREEITEM_CLICKED,\
                          id,wxID_ANY,\
                          (wxObjectEventFunction)(wxEventFunction)\
                          (MyTreeItemClickedFunction) & fn,\
                          (wxObject*)NULL),

#define EVT_COMMAND_NEWACCOUNTOKCLICKED(id,fn)\
DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_NEWACCOUNTOK_CLICKED,\
                          id,wxID_ANY,\
                          (wxObjectEventFunction)(wxEventFunction)\
                          (NewAccountOkClickedFunction) & fn,\
                          (wxObject*)NULL),

#define EVT_COMMAND_MYNB_VALUE_CHANGE(id,fn)\
DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_MYNB_VALUE_CHANGE,\
                          id,wxID_ANY,\
                          (wxObjectEventFunction)(wxEventFunction)\
                          (MyNBValueChangeFunction) & fn,\
                          (wxObject*)NULL),
//定义星标改变事件的入口
#define EVT_COMMAND_MYSTAR_CHANGE(id,fn)\
DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_MYSTAR_CHANGE,\
						  id,wxID_ANY,\
						  (wxObjectEventFunction)(wxEventFunction)\
						  (MyStarChangeFunction) & fn,\
						  (wxObject*)NULL),
////自定义dataViewitemchange事件
#define EVT_COMMAND_MYDATAVIEW_SELECT_CHANGE(id,fn)\
DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_MYDATAVIEW_SELECT_CHANGE,\
						  id,wxID_ANY,\
						  (wxObjectEventFunction)(wxEventFunction)\
						  (MyDataViewSelectChangeFunction) & fn,\
						  (wxObject*)NULL),
//自定义的list未读状态改变的事件
#define EVT_COMMAND_MYREAD_STAT_CHANGE(id,fn)\
DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_MYREAD_STAT_CHANGE,\
						  id,wxID_ANY,\
						  (wxObjectEventFunction)(wxEventFunction)\
						  (MyReadStatChangeFunction) & fn,\
						  (wxObject*)NULL),


#define EVT_COMMAND_SETUP_GROUP(id,fn)\
DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_SETUP_GROUP,\
						  id,wxID_ANY,\
						  (wxObjectEventFunction)(wxEventFunction)\
						  (SetUpGroupFunction) & fn,\
						  (wxObject*)NULL),

#define EVT_COMMAND_EDIT_GROUP(id,fn)\
DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_EDIT_GROUP,\
						  id,wxID_ANY,\
						  (wxObjectEventFunction)(wxEventFunction)\
						  (EditGroupFunction) & fn,\
						  (wxObject*)NULL),

#define EVT_COMMAND_ADD_NEW_ACCOUNT(id,fn)\
DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_ADD_NEW_ACCOUNT,\
						  id,wxID_ANY,\
						  (wxObjectEventFunction)(wxEventFunction)\
						  (AddNewaccountFunction) & fn,\
						  (wxObject*)NULL),
#define EVT_COMMAND_CEFRIGHT_MENU(id,fn)\
DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_CEFRIGHT_MENU,\
						  id,wxID_ANY,\
						  (wxObjectEventFunction)(wxEventFunction)\
						  (CEFRightMenuFunction) & fn,\
						  (wxObject*)NULL),
#define EVT_COMMAND_MYSHIFTANDCTRL(id,fn)\
DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_MYSHIFTANDCTRL,\
						  id,wxID_ANY,\
						  (wxObjectEventFunction)(wxEventFunction)\
						  (MyShiftAndCtrlFunction) & fn,\
						   (wxObject*)NULL),
#define EVT_COMMAND_SETUP_ACCOUNT(id,fn)\
DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_SETUP_ACCOUNT,\
						  id,wxID_ANY,\
						  (wxObjectEventFunction)(wxEventFunction)\
						  (MySetUpAccountFunction) & fn,\
						   (wxObject*)NULL),
//#define EVT_COMMAND_MYADDRESSBOOK_TREECTRL_ITEMCLICK(id,fn)\
//DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_MYADDRESSBOOK_TREECTRL_ITEMCLICK,\
//						  id,wxID_ANY,\
//						  (wxObjectEventFunction)(wxEventFunction)\
//						  (MyAddressBookTreeCtrlItemClickFunction) & fn,\
//						   (wxObject*)NULL),

#define EVT_COMMAND_MYADDRESSBOOK_TREECTRL_ITEMCLICK(id,fn)\
DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_MYADDRESSBOOK_TREECTRL_ITEMCLICK,\
                          id,wxID_ANY,\
(wxObjectEventFunction)(wxEventFunction)\
(MyAddressBookTreeCtrlItemClickFunction) & fn,\
(wxObject*)NULL),

// js调用的app点击事件
#define EVT_COMMAND_MYAPPLICATION_CLICKED(id,fn)\
DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_MYAPPLICATION_CLICKED,\
                          id,wxID_ANY,\
(wxObjectEventFunction)(wxEventFunction)\
(MyApplicationClickedFunction) & fn,\
(wxObject*)NULL),

#define EVT_COMMAND_SAVEMAIL(id,fn) \
DECLARE_EVENT_TABLE_ENTRY(wxEVT_COMMAND_SAVEMAIL,\
                        id,wxID_ANY,\
                        (wxObjectEventFunction)(wxEventFunction)\
                        (SaveMailEventFunction) & fn,\
                        (wxObject*)NULL),

#endif // MYEVENT_H
