#pragma  once


static CString LOGINFIELD[] =
{
	"编号",
	"网站名称",
	"网站地址",
	"行业",
	"评分",
	"用户名",
	"注册日期",
	"注册状态",
};

static int LOGINFIELDSIZE[] =
{
	50,
	200,
	200,
	200,
	50,
	200,
	50,
	200,
};


static CString WEBINFOFIELD[] =
{
		"步骤",
		"入口页面",
		"信息提交页面",
		"成功标志",
		"提交参数",
};

static int WEBINFOFIELDSIZE[] =
{
		50,
		200,
		200,
		100,
		200,
};

enum
{
	IE_CONTROL_TEXT		= 0,
	IE_CONTROL_IMG		= 1,
	IE_CONTROL_SELECT	= 2,
};

const COLORREF g_OutBorder  = RGB(241, 242, 246);
const COLORREF g_InBorder  = RGB(156, 164, 187);

const CString MSGTITLE = "博购信息发布专家";