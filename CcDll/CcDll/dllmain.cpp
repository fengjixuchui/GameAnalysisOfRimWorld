// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "stdafx.h"
#include <Windows.h>
#include <stdio.h>
#include "resource.h"
#include "CcDll.h"

HMODULE g_hModule;
HINSTANCE  hBmpInstance;
HBITMAP  hBmp; //位图句柄
BITMAP   Bmp; //BITMAP是结构体
HDC hDc, hDcMem; //hDcMen兼容设备
PAINTSTRUCT  Ps;
RECT  WinRect;
TCHAR szPath[] = _T("..\\ShowBmp\\Show.bmp"); //外部位图路径
HWND hList;

//文本控件
HWND g_TextGoodsAddr;
HWND g_TextGoodsNum;
HWND g_TextGoodsNaijiu;
HWND g_TextGoodsZuobiao;
HWND g_TextGoodsName;


CHAR* fastResearchMem;//快速建造指令指针

void 初始化快速建造数据()
{
	fastResearchMem = (CHAR*)malloc(0x9);
	/*
		0x0000025C : 8D 65 F4                                      lea     esp, [ebp - 0Ch]
		0x0000025F : 5E                                            pop     esi
		0x00000260 : 5F                                            pop     edi
		0x00000261 : 5B                                            pop     ebx
		0x00000262 : C9                                            leave
		0x00000263 : C3                                            retn
	*/
	char changeByte[] = { 0x8d,0x65,0xf4,0x5e,0x5f,0x5b,0xc9,0xc3 };
	memcpy(fastResearchMem, changeByte, 0x8);
	DWORD dwOldAttrubet;
	VirtualProtect(fastResearchMem,
		0x8,
		PAGE_EXECUTE_READWRITE,
		&dwOldAttrubet);
}

HWND 初始化列表控件(HWND hwndDlg)
{

	hList = GetDlgItem(g_hwndDlg, IDC_LIST1);

	SetFocus(hList);   //获得焦点

	ListView_SetItemState(hList, 0, LVIS_SELECTED, LVIS_SELECTED);       //使第一行处于选择中状态

	ListView_SetExtendedListViewStyle(hList, LVS_EX_FULLROWSELECT);      //设置整行选择风格
	//3.2 添加列
	LVCOLUMN ln;
	ln.mask = LVCF_WIDTH | LVCF_TEXT;
	ln.cx = 150;//宽度
	ln.pszText = (PTCHAR)_T("名称");//列名 
	ln.cchTextMax = 3;      //列名字符个数
	ListView_InsertColumn(
		hList,
		0,
		&ln
	);
	ln.cx = 50;//宽度
	ln.pszText = (PTCHAR)_T("座标x");//列名 
	ListView_InsertColumn(
		hList,
		1,
		&ln
	);
	ln.pszText = (PTCHAR)_T("座标y");//列名 
	ListView_InsertColumn(
		hList,
		2,
		&ln
	);
	ln.pszText = (PTCHAR)_T("数量");//列名 
	ListView_InsertColumn(
		hList,
		3,
		&ln
	);
	ln.pszText = (PTCHAR)_T("耐久");//列名 
	ListView_InsertColumn(
		hList,
		5,
		&ln
	);
	ln.cx = 80;//宽度
	ln.pszText = (PTCHAR)_T("地址");//列名 
	ListView_InsertColumn(
		hList,
		5,
		&ln
	);


	//3.3 添加行
	/*
	LVITEM li = { 0 };
	li.mask = LVIF_TEXT;
	li.pszText = (PTCHAR)_T("小明");
	li.cchTextMax = 3;
	li.iItem = 0;//行号
	ListView_InsertItem(hList,
		&li);
	ListView_SetItemText(hList, 0, 1, (PTCHAR)_T("18"));

	ListView_SetItemText(hList, 0, 2, (PTCHAR)_T("山西"));
	li.iItem = 1;//行号
	li.pszText = (PTCHAR)_T("小红");
	ListView_InsertItem(hList,
		&li);
	ListView_SetItemText(hList, 1, 1, (PTCHAR)_T("20"));
	ListView_SetItemText(hList, 1, 2, (PTCHAR)_T("陕西"));
	*/
	return hList;
}

VOID 获取列表选中物品信息(int nItem)
{
	//g_TextGoodsAddr = GetDlgItem(g_hwndDlg, IDC_THIS_GOODS_ADDR);
	//g_TextGoodsNum = GetDlgItem(g_hwndDlg, IDC_GOODS_NUM);
	//g_TextGoodsNaijiu = GetDlgItem(g_hwndDlg, IDC_NAIJIU);
	//g_TextGoodsZuobiao = GetDlgItem(g_hwndDlg, IDC_ZUOBIAO);
	//g_TextGoodsName = GetDlgItem(g_hwndDlg, IDC_GOODS_NAME);

	char *pBuf;
	int len = 100;
	pBuf = new char[len];
	memset(pBuf, 0, len);
	ListView_GetItemText(hList, nItem, 5, pBuf, len);
	SetDlgItemText(g_hwndDlg, IDC_THIS_GOODS_ADDR, pBuf);
	memset(pBuf, 0, len);
	ListView_GetItemText(hList, nItem, 3, pBuf, len);
	SetDlgItemText(g_hwndDlg, IDC_GOODS_NUM, pBuf);
	memset(pBuf, 0, len);
	ListView_GetItemText(hList, nItem, 4, pBuf, len);
	SetDlgItemText(g_hwndDlg, IDC_NAIJIU, pBuf);
	memset(pBuf, 0, len);
	ListView_GetItemText(hList, nItem, 2, pBuf, len);
	SetDlgItemText(g_hwndDlg, IDC_ZUOBIAO2, pBuf);
	memset(pBuf, 0, len);
	ListView_GetItemText(hList, nItem, 1, pBuf, len);
	SetDlgItemText(g_hwndDlg, IDC_ZUOBIAO, pBuf);
	memset(pBuf, 0, len);
	ListView_GetItemText(hList, nItem, 0, pBuf, len);
	SetDlgItemText(g_hwndDlg, IDC_GOODS_NAME, pBuf);

	delete[] pBuf;


}

INT_PTR CALLBACK DialogProc(
	__in  HWND hwndDlg,
	__in  UINT uMsg,
	__in  WPARAM wParam,
	__in  LPARAM lParam
);

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
		g_hModule = hModule;
		DialogBox(hModule,MAKEINTRESOURCE(IDD_MAJOR),NULL,&DialogProc);
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

INT_PTR CALLBACK DialogProc(
	__in  HWND hwndDlg,
	__in  UINT uMsg,
	__in  WPARAM wParam,
	__in  LPARAM lParam
)
{

	switch (uMsg)
	{
	case WM_PAINT:
	{
		TCHAR str[] = _T("这是一个修改器测试程序");
		hDc = BeginPaint(hwndDlg, &Ps);//加载设备内容
		hDcMem = CreateCompatibleDC(hDc); //创建兼容设备
		SetTextColor(hDc, RGB(13, 25, 200)); //设置文本颜色
		TextOut(hDc, 0, 0, str, 22); //输出文字	
		break;
	}
	case WM_INITDIALOG:
	{
		
		UpdateWindow(hwndDlg);
		g_hwndDlg = hwndDlg;
		g_hList = 初始化列表控件(hwndDlg);
		EnumProcess(hwndDlg);
		break;
	}
	case WM_CLOSE:
		UnHook(g_funAddress);
		EndDialog(hwndDlg,0);
		break;
	case WM_NOTIFY:
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_CLICK:
			if (((LPNMHDR)lParam)->idFrom == IDC_LIST1)
			{
				HWND hStatic = GetDlgItem(hwndDlg, IDC_STATIC_S);
				//获得位图的句柄
				//HBITMAP hBitMap = LoadBitmap(GetModuleHandle(0),
				//	MAKEINTRESOURCE(IDI_ICON2));
				HICON hIco = LoadIcon(g_hModule, MAKEINTRESOURCE(IDI_BAIYIN));
				SendMessage(hStatic, STM_SETIMAGE, IMAGE_ICON, (LPARAM)hIco);


				LPNMITEMACTIVATE temp = (LPNMITEMACTIVATE)lParam;//将传进来的消息转换成LPNMITEMACTIVAT
				int nItem = temp->iItem;//获得行号
				int nSubItem = temp->iSubItem;//获得列号


				获取列表选中物品信息(nItem);
			

				UpdateWindow(hwndDlg);
				return TRUE;

			}
			// More cases on WM_NOTIFY switch.
			break;
		}
		break;
	case WM_COMMAND:
	{
		SHORT Id = LOWORD(wParam);
		switch (Id)
		{
		case IDCANCEL:
			break;
		case IDOK:
			changeFastResearch(hwndDlg,fastResearchMem);
			break;
		case IDC_BUTTON_ADD10:
			//MessageBox(NULL, "+10", "+10", NULL);
			addGoodsNumber(10, hwndDlg);
			break;
		case IDC_BUTTON_ADD100:
			addGoodsNumber(100, hwndDlg);
			break;
		case IDC_BUTTON_ADD1000:
			addGoodsNumber(1000, hwndDlg);
			break;
		case IDC_BUTTON_ADD_NAIJIU:
			addGoodsDurable(10, hwndDlg);
			break;
		default:
				break;	
		}
		break;
	}
	default:
		break;
	}
	return FALSE;
}

