#ifndef _MYCEF_MESSAGE_H_
#define _MYCEF_MESSAGE_H_
#include <string.h>

using namespace std;

enum CefMessageType {
    FAST_REPLY_TYPE = 0,		// 0.快捷回复
    REPLY_TYPE,					// 1.回复
    REPLY_ALL_TYPE,				// 2.回复全部
    FORWARD_TYPE,				// 3.转发
    AS_ATTACHMENT_FORWARD_TYPE,	// 4.作为附件转发
    ATTACHMENT_VIEW_TYPE,       // 5.查看附件
    ATTACHMENT_VIEW_DBCLICK_TYPE,// 6.双击查看附件
    ATTACHMENT_DOWNLOAD_TYPE,   // 7.附件下载
    ATTACHMENT_SAVEALL_TYPE,    // 8.保存所有下载
    ATTACHMENT_COPY_TYPE,       // 9.复制附件
    ATTACHMENT_RELAY_TYPE,      // 10.转发附件
    COPYTEXT_TYPE,              // 11.复制
    SELECTALL_TYPE,             // 12.全选
    GET_APPLICATION_URL_TYPE,   // 13.获取应用的url
    APPLICATION_DETAIL_TYPE,    // 14.应用详情
    INSTALL_APPLICATION_TYPE,   // 15.安装应用
    EDIT_ON_INPUT,              // 16.邮件内容经过编辑
    OPEN_WINDOW_URL,            // 17.处理应用内跳链接的问题
    JUMP_WRITE_MAIL,            // 18.跳转到写邮件界面
    UNKNOWN_TYPE,				// 未知类型
};

extern const char *CEFFuncName[CefMessageType::UNKNOWN_TYPE];

struct CFuncName {
    string FastReplyFuncName            = "FastReply";				//快捷回复
    string ReplyFuncName                = "Reply";					//回复
    string ReplyAllFuncName             = "ReplyAll";				//回复全部
    string FowardFuncName               = "Foward";	                //转发
    string AsAttachmentFowardFuncName   = "AsAttachmentForward";    //作为附件转发
    string AccessoryViewFuncName        = "Accessory_view";         //查看附件
    string AccessoryViewDBclickFuncName = "Accessory_view_DBclick"; //双击查看附件
    string AccessoryDownloadFuncName    = "Accessory_Download";     //下载附件
    string AccessorySaveallFuncName     = "Accessory_Saveall";      //保存所有附件
    string AccessoryCopyFuncName        = "Accessory_Copy";         //复制附件
    string AccessoryRelayFuncName       = "Accessory_Relay";        //转发附件
    string CopyTextFuncName             = "CopyText";               //复制
    string SelectAllFuncName            = "SelectAll";              //全选
    string GetApplicationUrlFuncName    = "Get_Application_Url";    //获取应用的url
    string ApplicationDetailFuncName    = "Application_Detail";     // 应用详情点击调用的函数名
    string InstallApplicationFuncName   = "Install_Application";    // 安装应用点击时js调用的函数名
    string EditOnInputFuncName          = "edit_on_input";          //邮件内容改变时触发的js函数名
    string OpenWindowUrlFuncName        = "OpenWindowUrl";          //处理应用链接的问题
    string JumpWriteMailFuncName        = "jump_write_mail";        // 点击邮箱账号要跳转到的写邮件界面
    string UnknownName = "Unknown";
};
struct MessageName :public CFuncName {

};

enum {
    CEFFastReply = 5300,	//快捷回复
    CEFReply,				//回复
    CEFReplyAll,			//回复全部
    CEFFoward,				//转发
    CEFAsAttachFoward,		//作为附件转发
    CEFAttachmentView,      //查看附件
    CEFAttachmentViewDBclick,//双击查看附件
    CEFAttachmentDownload,  //下载附件
    CEFAttachmentSaveall,   //保存所有附件
    CEFAttachmentCopy,      //复制附件
    CEFAttachmentRelay,     //转发附件
    CEFCopyText,            //复制
    CEFSelectALL,           //全选
    CEFGetApplicationUrl,   //应用的url
    CEFApplicationDetail,   // 应用详情
    CEFInstallApplication,  // 安装应用
    CEFEditOnInput,         // 邮件编辑
    CEFOpenWindowUrl,       // 处理应用内跳链接
    CEFJumpWriteMail        // 处理跳转到写邮件界面
};

namespace xiniu
{
    int GetMessageType(string& messageName);
};

#endif