// EXP5  ScreenShot.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "EXP5  ScreenShot.h"
#include <stdio.h>
#define MAX_LOADSTRING 100

// 全局变量: 
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名
HDC storageDC = NULL;//用来长期保存截图
RECT rectOfShot = RECT();//全局变量,用来保存截图的区域位置


// 此代码模块中包含的函数的前向声明: 
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO:  在此放置代码。
	MSG msg;
	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_EXP5SCREENSHOT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 执行应用程序初始化: 
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_EXP5SCREENSHOT));

	// 主消息循环: 
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  函数:  MyRegisterClass()
//
//  目的:  注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_EXP5SCREENSHOT));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_EXP5SCREENSHOT);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   函数:  InitInstance(HINSTANCE, int)
//
//   目的:  保存实例句柄并创建主窗口
//
//   注释: 
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindow(szWindowClass, szTitle, WS_HSCROLL|WS_VSCROLL|WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, CW_USEDEFAULT,300, 200, NULL, NULL, hInstance, NULL);
   
   
   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数:  WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目的:    处理主窗口的消息。
//
//  WM_COMMAND	- 处理应用程序菜单
//  WM_PAINT	- 绘制主窗口
//  WM_DESTROY	- 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	static PAINTSTRUCT ps;
	static HDC hdc;
	static RECT myClientRect=RECT();//客户区大小
	static HDC tmpDC =NULL;//用来保存截取后缩放的图再显示
	static  HDC shotDC = NULL;//要截图对象的dc
	static bool flag4Stretch = true;//是否要缩放
	static  HBITMAP hBitMap =NULL;
	static int   cyClient,cxClient, iVscrollPos,iHscrollPos;
	static bool flag4Anyarea = false;

	switch (message)
	{
	case WM_CREATE:

		break;
	case WM_SIZE:
		cyClient = HIWORD(lParam);
		cxClient = LOWORD(lParam);
		//相关操作移动到WM_PAINT

		SetScrollRange(hWnd, SB_VERT, 0, max(rectOfShot.bottom - rectOfShot.top - cyClient, 0), TRUE);
		SetScrollRange(hWnd, SB_HORZ, 0, max(rectOfShot.right - rectOfShot.left - cxClient, 0), TRUE);
		break;
	case WM_VSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_LINEUP:
			iVscrollPos -= 15;
			break;

		case SB_LINEDOWN:
			iVscrollPos += 15;
			break;

		case SB_PAGEUP:
			iVscrollPos -= myClientRect.bottom-myClientRect.top;
			break;

		case SB_PAGEDOWN:
			iVscrollPos += myClientRect.bottom-myClientRect.top;
			break;

		case SB_THUMBPOSITION:
			iVscrollPos = HIWORD(wParam);
			//InvalidateRect(hWnd, NULL, TRUE);
			//SendMessage(hWnd, WM_PAINT, 1, 1);
			break;

		default:
			break;
		}
		//保证滚动条不超出屏幕
		iVscrollPos = max(0, min(iVscrollPos, rectOfShot.bottom-rectOfShot.top -myClientRect.bottom+myClientRect.top ));

		if (iVscrollPos != GetScrollPos(hWnd, SB_VERT))
		{
			SetScrollPos(hWnd, SB_VERT, iVscrollPos, TRUE);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;

	case WM_HSCROLL:
		switch (LOWORD(wParam))
		{
		case SB_LINEUP:
			iHscrollPos -= 15;
			break;

		case SB_LINEDOWN:
			iHscrollPos += 15;
			break;

		case SB_PAGEUP:
			iHscrollPos -= myClientRect.right-myClientRect.left;
			break;

		case SB_PAGEDOWN:
			iHscrollPos += myClientRect.right-myClientRect.left;
			break;

		case SB_THUMBPOSITION:
			iHscrollPos = HIWORD(wParam);
			break;

		default:
			break;
		}
		//保证滚动条不超出屏幕
		iHscrollPos = max(0, min(iHscrollPos, rectOfShot.right - rectOfShot.left - myClientRect.right + myClientRect.left));

		if (iHscrollPos != GetScrollPos(hWnd, SB_HORZ))
		{
			SetScrollPos(hWnd, SB_HORZ, iHscrollPos, TRUE);
			InvalidateRect(hWnd, NULL, TRUE);
		}
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// 分析菜单选择: 
		switch (wmId)
		{
		case IDM_ABOUT:
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case IDM_RAW:
			if (flag4Stretch)
			{
				InvalidateRect(hWnd, NULL, TRUE);//从另外的模式过来,就需要无效化客户区
				flag4Stretch = false;
				SendMessage(hWnd, WM_PAINT, 1, 0);
			}
			//GetClientRect(GetDesktopWindow(), &myClientRect);

			//InvalidateRect(hWnd, &myClientRect, TRUE);//系统自动设定无效区域,不要干预
			break;
		case IDM_STRETCH:
			if (!flag4Stretch)
			{
				InvalidateRect(hWnd, NULL, TRUE);//同上理由
				flag4Stretch = true;//改变flag在触发绘图
				SendMessage(hWnd, WM_PAINT, 1, 0);
			}
			//GetClientRect(hWnd, &myClientRect);//系统会自动决定无效区域,无需干预
			//InvalidateRect(hWnd, &myClientRect, TRUE);
			break;
		case IDM_FULLSCREEN:
			shotDC = GetDC(GetDesktopWindow());
			GetClientRect(GetDesktopWindow(), &rectOfShot);

			storageDC = CreateCompatibleDC(shotDC);//得到截图目标的兼容dc

			//hBitMap此时没有内容,只是根据rectOfShot的大小建立了一个与桌面DC兼容的位图 
			hBitMap = (HBITMAP)CreateCompatibleBitmap(shotDC, rectOfShot.right, rectOfShot.bottom);//创建桌面兼容位图(也就是屏幕显示兼容),storageDC就不行,因为storageDC是内存环境,不是屏幕显示
			SelectObject(storageDC, hBitMap);
			
			BitBlt(storageDC, 0, 0, rectOfShot.right, rectOfShot.bottom,shotDC,0,0,SRCCOPY);//保存截图在storage中,这时storageDC中才有内容
			InvalidateRect(hWnd, NULL, TRUE);
			SendMessage(hWnd, WM_SIZE, 1, 0);
			ReleaseDC(GetDesktopWindow(), shotDC);
			if (!flag4Anyarea)
			{
				if (OpenClipboard(NULL))
				{
					EmptyClipboard();
					HDC clipedDC = NULL;//用来暂存要被传给剪贴板的截图
					clipedDC = CreateCompatibleDC(storageDC);
					HBITMAP clipedBM = (HBITMAP)CreateCompatibleBitmap(storageDC, rectOfShot.right - rectOfShot.left,
						rectOfShot.bottom - rectOfShot.top);//创建截图兼容位图.此时这个位图是黑的，只是藉此控制了DC的大小
					SelectObject(clipedDC, clipedBM);
					BitBlt(clipedDC, 0, 0, rectOfShot.right - rectOfShot.left, rectOfShot.bottom - rectOfShot.top,
						storageDC, rectOfShot.left, rectOfShot.top, SRCCOPY);//拷贝选区到clipedDC
					SetClipboardData(CF_BITMAP, clipedBM);
					CloseClipboard();
					DeleteDC(clipedDC);
					DeleteObject(clipedBM);

				}
			}
			flag4Anyarea = false;
			SetScrollRange(hWnd, SB_VERT, 0, max(0, min(iVscrollPos, rectOfShot.bottom - rectOfShot.top - myClientRect.bottom + myClientRect.top)), TRUE);
			//下面这句不知为何使得横滚动条消失了.
			//SetScrollRange(hWnd, SB_HORZ, 0, max(0, min(iHscrollPos, rectOfShot.right - rectOfShot.left - myClientRect.right + myClientRect.left)), TRUE);

			break;
		case IDM_ANYAREA:
			flag4Anyarea = true;
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;

	case WM_SYSKEYDOWN:
		switch (wParam)
		{
		case 0X50:
			if (GetKeyState(VK_MENU) & 0X8000)
				PostMessage(hWnd, WM_COMMAND, IDM_FULLSCREEN, 1);
			break;
		case 0X73:
			if (GetKeyState(VK_MENU) & 0X8000)
				PostMessage(hWnd, WM_DESTROY, 1, 1);
		case 0X41:
			if (GetKeyState(VK_MENU) & 0X8000)
				PostMessage(hWnd, WM_COMMAND, IDM_ANYAREA, 1);
			break;
		default:
			break;
		}
		break;

	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		if (flag4Stretch)
		{
			//SelectObject(tmpDC, hBitMap);
			GetClientRect(hWnd, &myClientRect);
			//StretchBlt(tmpDC, myClientRect.left, myClientRect.top, myClientRect.right, myClientRect.bottom,
				//storageDC, rectOfShot.left, rectOfShot.top, rectOfShot.right, rectOfShot.bottom, SRCCOPY);
			StretchBlt(hdc, myClientRect.left, myClientRect.top, myClientRect.right-myClientRect.left, myClientRect.bottom-myClientRect.top,
				storageDC, rectOfShot.left, rectOfShot.top, rectOfShot.right-rectOfShot.left, rectOfShot.bottom-rectOfShot.top, SRCCOPY);
			//BitBlt(hdc, 0, 0, myClientRect.right, myClientRect.bottom, tmpDC, 0, 0, SRCCOPY);
		}
		else
		{
			GetClientRect(hWnd, &myClientRect);
			BitBlt(hdc, 0, 0, rectOfShot.right-rectOfShot.left, rectOfShot.bottom-rectOfShot.top, storageDC, rectOfShot.left+iHscrollPos,rectOfShot.top+iVscrollPos, SRCCOPY);
		}
		EndPaint(hWnd, &ps);

		break;
	case WM_DESTROY:
		DeleteDC(shotDC);
		DeleteDC(tmpDC);
		DeleteDC(storageDC);
		DeleteObject(hBitMap);
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	static RECT rectOfScreen = RECT();
	static bool flag4LbuttonDown = false;
	static HBRUSH hBrush4Dlg;
	static HPEN hPen4Dlg;
	HDC dlgDC;
	static int startX, startY, endX, endY;
	static RECT rectOfCliped=RECT();
	PAINTSTRUCT ps;
	char showbuf[64] = "";


	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_PAINT:
		dlgDC = BeginPaint(hDlg, &ps);
		BitBlt(dlgDC, 0, 0, rectOfScreen.right, rectOfScreen.bottom
			, storageDC, 0, 0, SRCCOPY);
		EndPaint(hDlg, &ps);
		break;
	case WM_INITDIALOG:
		hPen4Dlg = (HPEN)(CreatePen(PS_DASHDOT, 5, RGB(28, 68, 201)));
		dlgDC = GetDC(hDlg);
		hBrush4Dlg = (HBRUSH)GetStockObject(NULL_BRUSH);//CreateSolidBrush(RGB(30, 30, 30));
		SetWindowLong(hDlg, GWL_STYLE, WS_POPUP);
	
		//把dialog铺满全屏
		GetWindowRect(GetDesktopWindow(), &rectOfScreen);
		SendMessage(GetParent(hDlg), WM_COMMAND, IDM_FULLSCREEN, 1);//截取全屏
		SetWindowPos(hDlg, HWND_TOP, 0, 0,
			rectOfScreen.right,rectOfScreen.bottom,0);
			
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC3), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDC_STATIC1), SW_HIDE);
		ShowWindow(GetDlgItem(hDlg, IDOK), SW_HIDE);
		ReleaseDC(hDlg, dlgDC);
		return (INT_PTR)TRUE;

	case WM_RBUTTONDOWN:
		SendMessage(hDlg, WM_COMMAND, IDCANCEL, 1);
		break; 
	case WM_LBUTTONDBLCLK:
		//点击的点在矩形内
		if ((LOWORD(lParam) > min(startX, endX)) && (LOWORD(lParam) < max(startX, endX)) &&
			(HIWORD(lParam) > min(startY, endY)) && (HIWORD(lParam) < max(startY, endY)))
		{

			if (OpenClipboard(NULL))
			{
				EmptyClipboard();

				HDC clipedDC = NULL;//用来暂存要被传给剪贴板的截图
				clipedDC = CreateCompatibleDC(storageDC);
				HBITMAP clipedBM= (HBITMAP)CreateCompatibleBitmap(storageDC, rectOfCliped.right-rectOfCliped.left,
					rectOfCliped.bottom-rectOfCliped.top);//创建截图兼容位图.此时这个位图是黑的，只是藉此控制了DC的大小
				SelectObject(clipedDC, clipedBM);
				BitBlt(clipedDC, 0, 0, rectOfCliped.right-rectOfCliped.left, rectOfCliped.bottom-rectOfCliped.top, 
					storageDC, rectOfCliped.left, rectOfCliped.top, SRCCOPY);//拷贝选区到clipedDC

			//	HDC storageDCBackUp = NULL;//将storage替换成截取的图
				//storageDCBackUp = CreateCompatibleDC(storageDC);
				
				/*HBITMAP clipedBM2=(HBITMAP)CreateCompatibleBitmap(storageDC, rectOfCliped.right - rectOfCliped.left, 
					rectOfCliped.bottom - rectOfCliped.top);//创建截图兼容位图.此时这个位图是黑的，只是藉此控制了DC的大小
				DeleteDC(storageDC);
				storageDC = CreateCompatibleDC(clipedDC);
				SelectObject(storageDC, clipedBM2);
				BitBlt(storageDC, 0, 0, rectOfCliped.right - rectOfCliped.left, rectOfCliped.bottom - rectOfCliped.top,
					clipedDC,0,0, SRCCOPY);//拷贝新的截图到storageDC
					*/
				rectOfShot = rectOfCliped;
				SetClipboardData(CF_BITMAP, clipedBM);
				CloseClipboard();

				//重绘主窗口,显示新的截图
				InvalidateRect(GetParent(hDlg), NULL, TRUE);
				PostMessage(GetParent(hDlg), WM_SIZE, 1, 0);
				//if (storageDC != NULL)
			//		DeleteDC(storageDC);
			//	storageDC = CreateCompatibleDC(clipedDC);//得到截图目标的兼容dc
				//这是为了通过hbitmap的大小间接设置了、storageDC的大小为 被截图区域大小
				//SelectObject(storageDC, clipedBM);
				//BitBlt(storageDC, 0, 0, rectOfCliped.right, rectOfCliped.bottom, clipedDC, 0, 0, SRCCOPY);
				
				startX = startY = endX = endY = 0;//都归零，防止下次截图点击时候先画个框出来
				DeleteObject(clipedBM);
				DeleteDC(clipedDC);
				SendMessage(hDlg, WM_COMMAND, IDCANCEL, 1);
			}
		}
		break;
	case WM_LBUTTONDOWN:
		//点击的点在矩形内
		if ((LOWORD(lParam) > min(startX, endX)) && (LOWORD(lParam) < max(startX, endX)) &&
			(HIWORD(lParam) > min(startY, endY)) && (HIWORD(lParam) < max(startY, endY)))
		{

		}
		else
		{	
			
			dlgDC= GetDC(hDlg);
			SetROP2(dlgDC, R2_XORPEN);
			SelectObject(dlgDC, hPen4Dlg);//选入mousemove同一个画笔实现反转,否则边框会留下痕迹
			SelectObject(dlgDC, hBrush4Dlg);//选入和mousemove的同一个画刷以实现反转
			Rectangle(dlgDC, startX, startY, endX, endY);//因为是反转模式，所以图像复原
			ReleaseDC(hDlg, dlgDC);
			flag4LbuttonDown = true;
			InvalidateRect(hDlg, NULL, TRUE);
			SendMessage(hDlg, WM_PAINT, 1, 1);
			startX = LOWORD(lParam);
			startY = HIWORD(lParam);
			endX = startX;
			endY = startY;

			//rectOfCliped.left = startX;
			//rectOfCliped.right = endX;
		//	rectOfCliped.top = startY;
		//	rectOfCliped.bottom = endY;

		}
		break;
	case WM_LBUTTONUP:
		flag4LbuttonDown = false;
		
		break;
	case WM_MOUSEMOVE:
		dlgDC = GetDC(hDlg);
		SelectObject(dlgDC, hBrush4Dlg);//画刷两者都不选择没关系，没有闪烁
		SelectObject(dlgDC, hPen4Dlg);//不知为何不自主选择画笔就会闪烁
		//这一句用来取反   两次取反就能恢复图像之前样子
		SetROP2(dlgDC, R2_XORPEN);
		if (flag4LbuttonDown)
		{
			
			Rectangle(dlgDC, startX, startY, endX, endY);
			endX = LOWORD(lParam);
			endY = HIWORD(lParam);
			
			Rectangle(dlgDC, startX, startY, endX, endY);
			sprintf_s(showbuf, "当前矩形坐标(%d,%d)<>(%d,%d) 大小为(%d,%d)", startX,startY,endX, endY, abs(endX - startX), abs(endY - startY));
			SetBkColor(dlgDC, RGB(0,0,0));
			SetTextColor(dlgDC, RGB(255,255,255));
			TextOutA(dlgDC, 0,0, showbuf, lstrlenA(showbuf));
			
			rectOfCliped.left = min(startX,endX);
			rectOfCliped.right = max(startX,endX);
			rectOfCliped.top = min(startY,endY);
			rectOfCliped.bottom = max(endY,startY);
		}
		ReleaseDC(hDlg, dlgDC);

		break;


	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			DeleteObject(hBrush4Dlg);
			DeleteObject(hPen4Dlg);
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
