#include "stdafx.h"
#include "Win32Project1.h"
#include <windows.h>
#include <commctrl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <shellapi.h>
#include <time.h>
#include <math.h>
#include "AR002\AR002\AR002Alg.h"
#include "FIN\FIN\FinAlg.h"
#include "HUF\HUF\Huf.h"
#include "SPLAY\SPLAY\SplayAlg.h"

using namespace AR002;
using namespace Fin;
using namespace Huf;
using namespace Splay;
using namespace std;

struct Information
{
	TCHAR name[MAX_PATH];
	TCHAR type[MAX_PATH];
	double size;    
	BOOL flag;     //state
	FILETIME crtd; //created
	FILETIME last; //last access
}info;

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND,UINT,UINT,LONG);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK InfoWindow(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK Archivation(HWND, UINT, WPARAM, LPARAM);

TCHAR szWindowClass[MAX_LOADSTRING],szTitle[MAX_LOADSTRING],dir[MAX_PATH],
	dir1[MAX_PATH],copy_buf1[MAX_PATH],clearString[MAX_PATH];
HINSTANCE hInst;
HWND hWndChild = NULL;
HIMAGELIST g_hImageList = NULL;

static HWND hListView_1, hListView_2,hComboBox_1, hComboBox_2,hLabel_1, 
	hLabel_2, hLabel_3, hLabel_4, hToolBar;

//создание панели инстурментов
const int ImageListID = 0;
const int numButtons = 6;
const DWORD buttonStyles = BTNS_AUTOSIZE;
const int bitmapSize = 16;

DWORD Drivers, sum1=0, sum2=0;
int sel,k=0,y=9;
TCHAR c,*ls;
TCHAR buf1[MAX_PATH], cm_dir_from[MAX_PATH], cm_dir_to[MAX_PATH], 
	cm_dir_to_[MAX_PATH],cm_dir_from_[MAX_PATH],path[MAX_PATH],
	_dir[MAX_PATH],_dir1[MAX_PATH],buff[MAX_PATH],tempdir[MAX_PATH],copyBuffer[MAX_PATH];
LPCTSTR s;
bool isCutting = FALSE;

TBBUTTON tbButtons[numButtons] = { { MAKELONG(STD_UNDO, ImageListID), IDM_UP, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)(_T("Back")) },
								   { MAKELONG(STD_COPY, ImageListID), IDM_COPY, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)(_T("Copy")) },
								   { MAKELONG(STD_DELETE, ImageListID), IDM_DEL, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)(_T("Delete")) },
								   { MAKELONG(STD_CUT, ImageListID), IDM_CUT, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)(_T("Cut")) },
								   { MAKELONG(STD_PASTE, ImageListID), IDM_PASTE, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)(_T("Paste")) },
								   { MAKELONG(STD_HELP, ImageListID), IDM_INFO, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)(_T("Info")) } };

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,_In_opt_ HINSTANCE hPrevInstance,_In_ LPTSTR lpCmdLine,_In_ int nCmdShow)
{
	MSG Msg;
	HWND hWnd;
	HACCEL hAccelTable;

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	wcscpy(dir,_T("C:\\*"));
	wcscpy(dir1,_T("C:\\*"));
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WIN32PROJECT1, szWindowClass, MAX_LOADSTRING);
	SendMessage(hToolBar, TB_SETSTATE, (WPARAM)IDM_PASTE, (LPARAM)MAKELONG(0,0));
	MyRegisterClass(hInstance);

	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT1));
	InitCommonControls();

	while(GetMessage(&Msg,NULL,0,0))
	{
		DispatchMessage(&Msg);
		TranslateMessage(&Msg);
	}
	return Msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT1));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_WIN32PROJECT1);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    HWND hWnd;
	int hh=640, ww=916;
    int xx = (GetSystemMetrics(SM_CXSCREEN) - ww) / 2;
	int yy = (GetSystemMetrics(SM_CYSCREEN) - hh) / 2;
    hInst = hInstance;
	hWnd=CreateWindow(szWindowClass, szTitle, WS_BORDER|WS_SYSMENU|WS_MINIMIZEBOX|WS_VISIBLE,
	   xx, yy, ww, hh, NULL, NULL, hInstance, NULL);

    if (!hWnd)
    {
       return FALSE;
    }

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    return TRUE;
}

HWND CreateSimpleToolbar(HWND hWndParent)
{
    //создание тулбара
    HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | TBSTYLE_WRAPABLE,
        0, 0, 0, 0, hWndParent, NULL, hInst, NULL);
    
	if (hWndToolbar == NULL)
    {
        return NULL;
    }

    //создание списка картинок, для кнопок
    g_hImageList = ImageList_Create(bitmapSize, bitmapSize, ILC_COLOR16 | ILC_MASK, numButtons, 0);

    SendMessage(hWndToolbar, TB_SETIMAGELIST, (WPARAM)ImageListID,
        (LPARAM)g_hImageList);

    SendMessage(hWndToolbar, TB_LOADIMAGES, (WPARAM)IDB_STD_SMALL_COLOR,
        (LPARAM)HINST_COMMCTRL);

    SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
    SendMessage(hWndToolbar, TB_ADDBUTTONS, (WPARAM)numButtons, (LPARAM)&tbButtons);
    SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);

    ShowWindow(hWndToolbar, TRUE);

    return hWndToolbar;
}

void AddColToListView(TCHAR *st, int sub, int size)
{
	LVCOLUMN lvc;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = sub;
	lvc.pszText = st;
	lvc.cx = size;
	lvc.fmt = LVCFMT_LEFT;

	ListView_InsertColumn(hListView_1, sub, &lvc);
}
 
int i;
BOOL InitListViewImageLists(HWND hWndListView,int size, TCHAR c_dir[MAX_PATH])
{
	HIMAGELIST hSmall;
	SHFILEINFO lp;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	TCHAR buf1[MAX_PATH];
	DWORD num;

	hSmall = ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_MASK|ILC_COLOR32, size+2, 1);
	hFind = FindFirstFile(c_dir, &FindFileData);

	if (hFind == INVALID_HANDLE_VALUE)
		MessageBox(0,_T("Not found"), _T("Error"),  MB_OK |MB_ICONWARNING);
	else
	{
		//присваеваем атрибуты
		do 
		{
			if(wcscmp(FindFileData.cFileName, _T("."))==0) //если диск
			{		 
				wcscpy(buf1, c_dir);
				wcscat(buf1, FindFileData.cFileName);
				SHGetFileInfo(_T(""),FILE_ATTRIBUTE_DEVICE,&lp,sizeof(lp),SHGFI_ICONLOCATION|SHGFI_ICON|SHGFI_SMALLICON);
				ImageList_AddIcon(hSmall,lp.hIcon);
				DestroyIcon(lp.hIcon);
			}
			if(wcscmp(FindFileData.cFileName,_T(".."))==0)//если фаилы,папки
			{
				wcscpy(buf1, c_dir);
				wcscat(buf1,FindFileData.cFileName);
				SHGetFileInfo(_T(""),FILE_ATTRIBUTE_DIRECTORY,&lp,sizeof(lp),SHGFI_ICONLOCATION|SHGFI_ICON|SHGFI_SMALLICON);
				ImageList_AddIcon(hSmall,lp.hIcon);
				DestroyIcon(lp.hIcon);
			}
			//присваеваем иконки
			wcscpy(buf1, c_dir);
			buf1[wcslen(buf1)-1] = 0;
			wcscat(buf1,FindFileData.cFileName);		 
			num=GetFileAttributes(buf1);
			SHGetFileInfo(buf1,num,&lp,sizeof(lp),SHGFI_ICONLOCATION|SHGFI_ICON|SHGFI_SMALLICON);
			ImageList_AddIcon(hSmall,lp.hIcon);
			DestroyIcon(lp.hIcon);
		} while (FindNextFile(hFind, &FindFileData) != 0);
		
		FindClose(hFind);
	}
	ListView_SetImageList(hWndListView, hSmall, LVSIL_SMALL);

	return TRUE;
}
 
void View_List(TCHAR *buf,HWND hList,int i, int j)
{
	LVITEM lvItem;
	
	lvItem.mask = LVIF_IMAGE|LVIF_TEXT;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iItem = i;
	lvItem.iImage=i;
	lvItem.iSubItem = j;
	lvItem.pszText = buf;
	lvItem.cchTextMax = sizeof(buf);
	
	ListView_InsertItem(hList, &lvItem);
}
 
void FindFile(HWND hList, TCHAR c_dir[MAX_PATH])
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	SendMessage(hList, LVM_DELETEALLITEMS, (WPARAM)0,(LPARAM)0);
	
	i=0;	
	hFind = FindFirstFile(c_dir, &FindFileData);
	
	if(hFind == INVALID_HANDLE_VALUE)
		MessageBox(0,_T("Not found"), _T("Error"),  MB_OK |MB_ICONERROR);
	else
	{
		do
		{
			if((FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(FindFileData.cFileName)) != FILE_ATTRIBUTE_DIRECTORY)
				View_List(_T("file"),hList,i,1); 
			else 
				View_List(_T("directory"),hList,i,1);
			View_List(FindFileData.cFileName,hList,i,0);//выз. ф-ция Viev_List передаем туда наиденый фаил ,и HWND ListBox и итератор i++,
			++i;
		} while (FindNextFile(hFind, &FindFileData) != 0);
		
		FindClose(hFind); //закрываем работу поиска фаилов
		InitListViewImageLists(hList,i, c_dir);//тут уже передаем HWND ListBox, и кол-во фаилов
	}
}

void Copy_File(TCHAR from[MAX_PATH], TCHAR directory[MAX_PATH], TCHAR buf[MAX_PATH])
{
	wcscpy(from, directory);
    from[wcslen(from)-1] = 0;
    wcscat(from, buf);
}

void Delete_File(TCHAR from[MAX_PATH], TCHAR directory[MAX_PATH], TCHAR buf[MAX_PATH])
{
	wcscpy(from, directory);
	from[wcslen(from)-1] = 0;
	wcscat(from, buf);
	DeleteFile(from);
}

Information GetFileInform(TCHAR file[MAX_PATH])
{
	WIN32_FIND_DATA fd;
	Information fileInfo;

	wcscpy(fileInfo.type,_T(""));

	FindFirstFile(file, &fd);
	
	fileInfo.crtd=fd.ftCreationTime;
	fileInfo.last=fd.ftLastAccessTime;
	
	wcscpy(fileInfo.name,fd.cFileName);

	fileInfo.size=(fd.nFileSizeHigh * (MAXDWORD64+1)) + fd.nFileSizeLow;

	if(fd.dwFileAttributes == FILE_ATTRIBUTE_DEVICE)
		wcscpy(fileInfo.type,_T("Device"));
	else if(fd.dwFileAttributes == FILE_ATTRIBUTE_NORMAL)
		wcscpy(fileInfo.type,_T("File"));
	else if(fd.dwFileAttributes == FILE_ATTRIBUTE_SYSTEM)
		wcscpy(fileInfo.type,_T("System file"));
	else if(fd.dwFileAttributes == FILE_ATTRIBUTE_VIRTUAL)
		wcscpy(fileInfo.type,_T("Virtual file"));
	else if(fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		wcscpy(fileInfo.type,_T("Directory")); 

	if (wcscmp(fileInfo.type,clearString)==0)
		wcscpy(fileInfo.type,_T("File"));
	
	return fileInfo;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	LPNMLISTVIEW pnmLV = (LPNMLISTVIEW)lParam;
				
	LPNMHDR lpnmHdr = (LPNMHDR)lParam;
	
	GetLogicalDrives();
	GetLogicalDriveStrings(128, buff);
	
	switch (msg)
	{
    	case WM_COMMAND:
    	{
    		switch (LOWORD(wParam))
            {
    			case IDM_COPY:
    			{
					Copy_File(cm_dir_from, dir, copy_buf1);
					isCutting = FALSE;
					wcscpy(copyBuffer,copy_buf1);
    				return 0;
    			}
    			case IDM_DEL:
    			{
					Delete_File(cm_dir_from, dir, copy_buf1);
					SendMessage(hListView_1,LVM_REDRAWITEMS,0,0);
    				return 0;
    			}
				case IDM_CUT:
				{
					Copy_File(cm_dir_from, dir, copy_buf1);
					isCutting = TRUE;
					wcscpy(copyBuffer,copy_buf1);
					return 0;
				}
				case IDM_PASTE:
				{
					if (!isCutting)
					{
						wcscpy(cm_dir_to, dir);
    					cm_dir_to[wcslen(cm_dir_to)-1] = 0;
    					wcscat(cm_dir_to, copyBuffer);
    					CopyFile(cm_dir_from, cm_dir_to, FALSE);
						SendMessage(hListView_1,LVM_REDRAWITEMS,0,0);
					}
					else
					{
						wcscpy(cm_dir_to, dir);
    					cm_dir_to[wcslen(cm_dir_to)-1] = 0;
    					wcscat(cm_dir_to, copyBuffer);
    					CopyFile(cm_dir_from, cm_dir_to, FALSE);
						DeleteFile(cm_dir_from);
						wcscpy(cm_dir_from,cm_dir_to);
						isCutting=FALSE;
						SendMessage(hListView_1,LVM_REDRAWITEMS,0,0);
					}
					return 0;
				}
				case IDM_UP:
				{
					bool isFind = false;
					ls = buff;
					dir[wcslen(dir)-1]=0;
					while (*ls)
					{
						if (wcscmp(dir,ls)==0)
						{
							isFind = true;
							break;
						}
						ls += wcslen(ls)+1;
					}
					wcscat(dir, _T("*"));
					if (!isFind)
					{
						wcscpy(tempdir,_T(".."));
						if (wcscmp(tempdir, _T(".."))==0)
						{
							dir[wcslen(dir)-2]=0;

							for (i=0; i< wcslen(dir); i++)
							{
								string s;
								s = dir[i];

								if (s == "\\") 
									k = i;
							}

							dir[k+1]=0;
							wcscat(dir, _T("*"));
						}
					
						SetWindowText(hLabel_3, dir);
						FindFile(hListView_1, dir);
					}
					return 0;
				}
				case IDM_INFO:
				{
					DialogBox(hInst, MAKEINTRESOURCE(IDM_INFO), hwnd, InfoWindow);
					return 0;
				}					
				case IDM_ARCH:
				{
					DialogBox(hInst, MAKEINTRESOURCE(IDM_ARCH), hwnd, Archivation);
					return 0;
				}
				case IDM_ABOUT:
					DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
					return 0;
				case IDM_EXIT:
					DestroyWindow(hwnd);
					return 0;
            	case ID_COMBOBOX_1:
                {
                	switch(HIWORD(wParam))
                    {
                		case CBN_SELENDOK:
                        {
                            wcscpy(path, _T(""));
                            sel= SendMessage(hComboBox_1,CB_GETCURSEL,0,0);
                            SendMessage(hComboBox_1,CB_GETLBTEXT,sel,(LPARAM)path);
                            wcscat(path, _T("\*"));
                            wcscpy(dir, path);
                            SetWindowText(hLabel_3, dir);
                            FindFile(hListView_1, dir);
                            return 0;
                        }
                		default: 
							return 0;
                    }
                }
                return 0;
            }
    	}
		case WM_NOTIFY:
		{
			if (((lpnmHdr->idFrom == ID_LISTVIEW_1)) && (lpnmHdr->code ==NM_CLICK))
			{
				ListView_GetItemText(lpnmHdr->hwndFrom, pnmLV->iItem, pnmLV->iSubItem, buf1, MAX_PATH);
				
				wcscpy(copy_buf1, buf1);
				wcscpy(_dir, dir);
				_dir[wcslen(_dir)-1] = 0;
				wcscat(_dir, buf1);

				if((FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(_dir)) == FILE_ATTRIBUTE_DIRECTORY)
				{
				    SendMessage(hToolBar, TB_SETSTATE, (WPARAM)IDM_COPY, (LPARAM)MAKELONG(0,0));
				    SendMessage(hToolBar, TB_SETSTATE, (WPARAM)IDM_DEL, (LPARAM)MAKELONG(0,0));
					SendMessage(hToolBar, TB_SETSTATE, (WPARAM)IDM_CUT, (LPARAM)MAKELONG(0,0));
				}
				else
				{
				    SendMessage(hToolBar, TB_SETSTATE, (WPARAM)IDM_COPY, (LPARAM)MAKELONG(TBSTATE_ENABLED,0));
				    SendMessage(hToolBar, TB_SETSTATE, (WPARAM)IDM_DEL, (LPARAM)MAKELONG(TBSTATE_ENABLED,0));
					SendMessage(hToolBar, TB_SETSTATE, (WPARAM)IDM_CUT, (LPARAM)MAKELONG(TBSTATE_ENABLED,0));
				}
			}

			if ((lpnmHdr->idFrom == ID_LISTVIEW_1) && (lpnmHdr->code ==NM_DBLCLK))
			{
				// Копируем строку в буфер из ячейки ListView (pnmLV->iItem - номер строки;
				// pnmLV->iSubItem - номер столбца)
				ListView_GetItemText(lpnmHdr->hwndFrom, pnmLV->iItem, pnmLV->iSubItem, buf1, MAX_PATH);

				if (lpnmHdr->idFrom == ID_LISTVIEW_1)
				{			
					k=0;
					wcscpy(_dir, dir);
					_dir[wcslen(_dir)-1] = 0;
					wcscat(_dir, buf1);

					for (int i = 0; i<wcslen(buf1); i++)
					{
						string s;
						s = buf1[i];

						if (s == ".") 
							k = i;
					}

					if ((k != 0) && (k != 1))
					{
 						if((FILE_ATTRIBUTE_DIRECTORY & GetFileAttributes(_dir)) != FILE_ATTRIBUTE_DIRECTORY)
							ShellExecute(0, _T("open"), _dir, NULL, NULL, SW_SHOWNORMAL);
						else
							FindFile(hListView_1,_dir);
					}
					else
						if (wcscmp(buf1, _T(".."))==0)
						{
							k=0;
							dir[wcslen(dir)-2]=0;

							for (i=0; i< wcslen(dir); i++)
							{
								string s;
								s = dir[i];

								if (s == "\\") 
									k = i;
							}

							dir[k+1]=0;
							wcscat(dir, _T("*"));
						}
						else 
							if (wcscmp(buf1, _T("."))==0)
							{
								dir[3] = 0;
								wcscat(dir, _T("*"));
							}
							else
							{
								if (wcscmp(buf1, _T(""))==0)
									break;
								else
								{
									wcscat(buf1, _T("\\*"));
									dir[wcslen(dir)-1]=0;
									wcscat(dir, buf1);
								}
							}
					
						SetWindowText(hLabel_3, dir);
						FindFile(hListView_1, dir);
					}
				}
			break;
		}

		case WM_CREATE:
		{
			hToolBar = CreateSimpleToolbar(hwnd);
			hLabel_1 =CreateWindow(_T("static"), _T(""),WS_CHILD | WS_VISIBLE | WS_TABSTOP,
						  0, 35+y, 900, 16,hwnd, (HMENU)ID_LABEL_1, hInst, NULL);
			hLabel_2 =CreateWindow(_T("static"), _T(""),WS_CHILD | WS_VISIBLE | WS_TABSTOP,
						  0, 45+y, 900, 16,hwnd, (HMENU)ID_LABEL_2, hInst, NULL);
			hLabel_3 =CreateWindow(_T("static"), _T("way1"), WS_CHILD | WS_VISIBLE | WS_TABSTOP, 3, 57+y, 900, 16,
						  hwnd, (HMENU)ID_LABEL_3, hInst, NULL);
			hComboBox_1 =CreateWindow(_T("ComboBox"), NULL,	WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWN|CBS_SORT,
							3, 33+y, 50, 110, hwnd, (HMENU) ID_COMBOBOX_1, hInst, NULL);
			hListView_1 =CreateWindow(WC_LISTVIEW, NULL,LVS_REPORT|WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|LVS_AUTOARRANGE,
							0, 60+y+12, 900, 500, hwnd,	(HMENU) ID_LISTVIEW_1, hInst, NULL);
            ls = buff;

            while (*ls)
            {
                    SendMessage(hComboBox_1, CB_ADDSTRING, 0, (LPARAM)ls);
                    ls += wcslen(ls)+1;
            }

			SendMessage(hComboBox_1, CB_SETCURSEL, 0, 0);

			AddColToListView(_T("Name"), 1, 650);
			AddColToListView(_T("Type"), 2, 103);
			AddColToListView(_T("Size"), 3, 130);

			FindFile(hListView_1, dir);
			SetWindowText(hLabel_3, dir);

			return 0;
		}

		case WM_DESTROY:
		{
			DestroyWindow(hListView_1);
			DestroyWindow(hListView_2);
			DestroyWindow(hComboBox_1);
			DestroyWindow(hComboBox_2);
			DestroyWindow(hLabel_1);
			DestroyWindow(hLabel_2);
			DestroyWindow(hLabel_3);
			DestroyWindow(hLabel_4);
			DestroyWindow(hToolBar);
			PostQuitMessage(0);
			return 0;
		}
		default:
			return DefWindowProc(hwnd, msg, wParam, lParam);
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void FileTimeToString(FILETIME ft,TCHAR str[255])
{
	SYSTEMTIME st;
	TCHAR szLocalDate[255], szLocalTime[255];

	FileTimeToLocalFileTime(&ft, &ft);
	FileTimeToSystemTime(&ft, &st);
	GetDateFormat(LOCALE_USER_DEFAULT, DATE_LONGDATE, &st, NULL,szLocalDate, 255);
	GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, szLocalTime, 255);
	wcscpy(str,szLocalDate);
	wcscat(str,_T(" "));
	wcscat(str,szLocalTime);
}

void ShortSize(double size, wchar_t str[255])
{
	int power=0;
	double shortsize=size;

	while (floor(shortsize / 1024) != 0)
	{
		shortsize /= 1024;
		power++;
	}

	wchar_t buffer[MAX_PATH];
	swprintf(buffer, L"%f", shortsize);
	buffer[wcslen(buffer)-5] = 0;
	switch(power)
	{
		case 0:
			break;
		case 1:
			wcscat(str,_T(" ("));
			wcscat(str,buffer);
			wcscat(str,_T(" Kb)"));
			break;
		case 2:
			wcscat(str,_T(" ("));
			wcscat(str,buffer);
			wcscat(str,_T(" Mb)"));
			break;
		case 3:
			wcscat(str,_T(" ("));
			wcscat(str,buffer);
			wcscat(str,_T(" Gb)"));
			break;
		case 4:
			wcscat(str,_T(" ("));
			wcscat(str,buffer);
			wcscat(str,_T(" Tb)"));
			break;
	}
}

INT_PTR CALLBACK InfoWindow(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		static HWND LabelName, LabelSize, LabelType,LabelState,LabelCrtd,LabelLast,LabelAtr;
		TCHAR stringCrtd[255], stringLast[255];
		wchar_t istr[255];
		if (wcscmp(_dir,_T(""))==0)
			info=GetFileInform(dir);
		else
			info=GetFileInform(_dir);
		LabelName =CreateWindow(_T("static"), info.name, WS_CHILD | WS_VISIBLE | WS_TABSTOP,
						  50, 10, 215, 16, hDlg, (HMENU)ID_LABELNAME, hInst, NULL);
		swprintf(istr, L"%f", info.size);
		istr[wcslen(istr)-7] = 0;
		wcscat(istr,_T(" bytes"));
		ShortSize(info.size,istr);
		LabelSize =CreateWindow(_T("static"), istr, WS_CHILD | WS_VISIBLE | WS_TABSTOP,
						  50, 50, 500, 16, hDlg, (HMENU)ID_LABELSIZE, hInst, NULL);
		LabelType =CreateWindow(_T("static"), (LPCWSTR)info.type, WS_CHILD | WS_VISIBLE | WS_TABSTOP,
						  50, 30, 215, 16, hDlg, (HMENU)ID_LABELTYPE, hInst, NULL);
		FileTimeToString(info.crtd,stringCrtd);
		LabelCrtd =CreateWindow(_T("static"), stringCrtd, WS_CHILD | WS_VISIBLE | WS_TABSTOP,
						  80, 70, 215, 16, hDlg, (HMENU)ID_LABELCRTD, hInst, NULL);
		FileTimeToString(info.last,stringLast);
		LabelLast =CreateWindow(_T("static"), stringLast, WS_CHILD | WS_VISIBLE | WS_TABSTOP,
						  80, 90, 215, 16, hDlg, (HMENU)ID_LABELLAST, hInst, NULL);
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Archivation(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
