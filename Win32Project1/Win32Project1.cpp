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
#include <stdint.h>

#include "MownAlg.h"
#include "zip.h"
#include "unzip.h"
#include "BasicFileAlgs.h"
#include "lzw.h"

using namespace std;

struct Information
{
	TCHAR name[MAX_PATH];
	TCHAR type[MAX_PATH];
	double size;    
	FILETIME crtd; //created
	FILETIME last; //last access
}info;

static const uint64_t crc64_tab[256] = {
    UINT64_C(0x0000000000000000), UINT64_C(0x7ad870c830358979), UINT64_C(0xf5b0e190606b12f2), 
	UINT64_C(0x8f689158505e9b8b), UINT64_C(0xc038e5739841b68f), UINT64_C(0xbae095bba8743ff6),
    UINT64_C(0x358804e3f82aa47d), UINT64_C(0x4f50742bc81f2d04), UINT64_C(0xab28ecb46814fe75), 
	UINT64_C(0xd1f09c7c5821770c), UINT64_C(0x5e980d24087fec87), UINT64_C(0x24407dec384a65fe),
    UINT64_C(0x6b1009c7f05548fa), UINT64_C(0x11c8790fc060c183), UINT64_C(0x9ea0e857903e5a08),
	UINT64_C(0xe478989fa00bd371), UINT64_C(0x7d08ff3b88be6f81), UINT64_C(0x07d08ff3b88be6f8),
    UINT64_C(0x88b81eabe8d57d73), UINT64_C(0xf2606e63d8e0f40a), UINT64_C(0xbd301a4810ffd90e),
	UINT64_C(0xc7e86a8020ca5077), UINT64_C(0x4880fbd87094cbfc), UINT64_C(0x32588b1040a14285),
    UINT64_C(0xd620138fe0aa91f4), UINT64_C(0xacf86347d09f188d), UINT64_C(0x2390f21f80c18306), 
	UINT64_C(0x594882d7b0f40a7f), UINT64_C(0x1618f6fc78eb277b), UINT64_C(0x6cc0863448deae02),
    UINT64_C(0xe3a8176c18803589), UINT64_C(0x997067a428b5bcf0), UINT64_C(0xfa11fe77117cdf02), 
	UINT64_C(0x80c98ebf2149567b), UINT64_C(0x0fa11fe77117cdf0), UINT64_C(0x75796f2f41224489),
    UINT64_C(0x3a291b04893d698d), UINT64_C(0x40f16bccb908e0f4), UINT64_C(0xcf99fa94e9567b7f),
	UINT64_C(0xb5418a5cd963f206), UINT64_C(0x513912c379682177), UINT64_C(0x2be1620b495da80e),
    UINT64_C(0xa489f35319033385), UINT64_C(0xde51839b2936bafc), UINT64_C(0x9101f7b0e12997f8),
	UINT64_C(0xebd98778d11c1e81), UINT64_C(0x64b116208142850a), UINT64_C(0x1e6966e8b1770c73),
    UINT64_C(0x8719014c99c2b083), UINT64_C(0xfdc17184a9f739fa), UINT64_C(0x72a9e0dcf9a9a271), 
	UINT64_C(0x08719014c99c2b08), UINT64_C(0x4721e43f0183060c), UINT64_C(0x3df994f731b68f75),
    UINT64_C(0xb29105af61e814fe), UINT64_C(0xc849756751dd9d87), UINT64_C(0x2c31edf8f1d64ef6), 
	UINT64_C(0x56e99d30c1e3c78f), UINT64_C(0xd9810c6891bd5c04), UINT64_C(0xa3597ca0a188d57d),
    UINT64_C(0xec09088b6997f879), UINT64_C(0x96d1784359a27100), UINT64_C(0x19b9e91b09fcea8b), 
	UINT64_C(0x636199d339c963f2), UINT64_C(0xdf7adabd7a6e2d6f), UINT64_C(0xa5a2aa754a5ba416),
    UINT64_C(0x2aca3b2d1a053f9d), UINT64_C(0x50124be52a30b6e4), UINT64_C(0x1f423fcee22f9be0), 
	UINT64_C(0x659a4f06d21a1299), UINT64_C(0xeaf2de5e82448912), UINT64_C(0x902aae96b271006b),
    UINT64_C(0x74523609127ad31a), UINT64_C(0x0e8a46c1224f5a63), UINT64_C(0x81e2d7997211c1e8), 
	UINT64_C(0xfb3aa75142244891), UINT64_C(0xb46ad37a8a3b6595), UINT64_C(0xceb2a3b2ba0eecec),
    UINT64_C(0x41da32eaea507767), UINT64_C(0x3b024222da65fe1e), UINT64_C(0xa2722586f2d042ee), 
	UINT64_C(0xd8aa554ec2e5cb97), UINT64_C(0x57c2c41692bb501c), UINT64_C(0x2d1ab4dea28ed965),
    UINT64_C(0x624ac0f56a91f461), UINT64_C(0x1892b03d5aa47d18), UINT64_C(0x97fa21650afae693), 
	UINT64_C(0xed2251ad3acf6fea), UINT64_C(0x095ac9329ac4bc9b), UINT64_C(0x7382b9faaaf135e2),
    UINT64_C(0xfcea28a2faafae69), UINT64_C(0x8632586aca9a2710), UINT64_C(0xc9622c4102850a14), 
	UINT64_C(0xb3ba5c8932b0836d), UINT64_C(0x3cd2cdd162ee18e6), UINT64_C(0x460abd1952db919f),
    UINT64_C(0x256b24ca6b12f26d), UINT64_C(0x5fb354025b277b14), UINT64_C(0xd0dbc55a0b79e09f), 
	UINT64_C(0xaa03b5923b4c69e6), UINT64_C(0xe553c1b9f35344e2), UINT64_C(0x9f8bb171c366cd9b),
    UINT64_C(0x10e3202993385610), UINT64_C(0x6a3b50e1a30ddf69), UINT64_C(0x8e43c87e03060c18), 
	UINT64_C(0xf49bb8b633338561), UINT64_C(0x7bf329ee636d1eea), UINT64_C(0x012b592653589793),
    UINT64_C(0x4e7b2d0d9b47ba97), UINT64_C(0x34a35dc5ab7233ee), UINT64_C(0xbbcbcc9dfb2ca865), 
	UINT64_C(0xc113bc55cb19211c), UINT64_C(0x5863dbf1e3ac9dec), UINT64_C(0x22bbab39d3991495),
    UINT64_C(0xadd33a6183c78f1e), UINT64_C(0xd70b4aa9b3f20667), UINT64_C(0x985b3e827bed2b63),
	UINT64_C(0xe2834e4a4bd8a21a), UINT64_C(0x6debdf121b863991), UINT64_C(0x1733afda2bb3b0e8),
    UINT64_C(0xf34b37458bb86399), UINT64_C(0x8993478dbb8deae0), UINT64_C(0x06fbd6d5ebd3716b), 
	UINT64_C(0x7c23a61ddbe6f812), UINT64_C(0x3373d23613f9d516), UINT64_C(0x49aba2fe23cc5c6f),
    UINT64_C(0xc6c333a67392c7e4), UINT64_C(0xbc1b436e43a74e9d), UINT64_C(0x95ac9329ac4bc9b5), 
	UINT64_C(0xef74e3e19c7e40cc), UINT64_C(0x601c72b9cc20db47), UINT64_C(0x1ac40271fc15523e),
    UINT64_C(0x5594765a340a7f3a), UINT64_C(0x2f4c0692043ff643), UINT64_C(0xa02497ca54616dc8), 
	UINT64_C(0xdafce7026454e4b1), UINT64_C(0x3e847f9dc45f37c0), UINT64_C(0x445c0f55f46abeb9),
    UINT64_C(0xcb349e0da4342532), UINT64_C(0xb1eceec59401ac4b), UINT64_C(0xfebc9aee5c1e814f), 
	UINT64_C(0x8464ea266c2b0836), UINT64_C(0x0b0c7b7e3c7593bd), UINT64_C(0x71d40bb60c401ac4),
    UINT64_C(0xe8a46c1224f5a634), UINT64_C(0x927c1cda14c02f4d), UINT64_C(0x1d148d82449eb4c6), 
	UINT64_C(0x67ccfd4a74ab3dbf), UINT64_C(0x289c8961bcb410bb), UINT64_C(0x5244f9a98c8199c2),
    UINT64_C(0xdd2c68f1dcdf0249), UINT64_C(0xa7f41839ecea8b30), UINT64_C(0x438c80a64ce15841), 
	UINT64_C(0x3954f06e7cd4d138), UINT64_C(0xb63c61362c8a4ab3), UINT64_C(0xcce411fe1cbfc3ca),
    UINT64_C(0x83b465d5d4a0eece), UINT64_C(0xf96c151de49567b7), UINT64_C(0x76048445b4cbfc3c), 
	UINT64_C(0x0cdcf48d84fe7545), UINT64_C(0x6fbd6d5ebd3716b7), UINT64_C(0x15651d968d029fce),
    UINT64_C(0x9a0d8ccedd5c0445), UINT64_C(0xe0d5fc06ed698d3c), UINT64_C(0xaf85882d2576a038), 
	UINT64_C(0xd55df8e515432941), UINT64_C(0x5a3569bd451db2ca), UINT64_C(0x20ed197575283bb3),
    UINT64_C(0xc49581ead523e8c2), UINT64_C(0xbe4df122e51661bb), UINT64_C(0x3125607ab548fa30), 
	UINT64_C(0x4bfd10b2857d7349), UINT64_C(0x04ad64994d625e4d), UINT64_C(0x7e7514517d57d734),
    UINT64_C(0xf11d85092d094cbf), UINT64_C(0x8bc5f5c11d3cc5c6), UINT64_C(0x12b5926535897936), 
	UINT64_C(0x686de2ad05bcf04f), UINT64_C(0xe70573f555e26bc4), UINT64_C(0x9ddd033d65d7e2bd),
    UINT64_C(0xd28d7716adc8cfb9), UINT64_C(0xa85507de9dfd46c0), UINT64_C(0x273d9686cda3dd4b), 
	UINT64_C(0x5de5e64efd965432), UINT64_C(0xb99d7ed15d9d8743), UINT64_C(0xc3450e196da80e3a),
    UINT64_C(0x4c2d9f413df695b1), UINT64_C(0x36f5ef890dc31cc8), UINT64_C(0x79a59ba2c5dc31cc),
	UINT64_C(0x037deb6af5e9b8b5), UINT64_C(0x8c157a32a5b7233e), UINT64_C(0xf6cd0afa9582aa47),
    UINT64_C(0x4ad64994d625e4da), UINT64_C(0x300e395ce6106da3), UINT64_C(0xbf66a804b64ef628), 
	UINT64_C(0xc5bed8cc867b7f51), UINT64_C(0x8aeeace74e645255), UINT64_C(0xf036dc2f7e51db2c),
    UINT64_C(0x7f5e4d772e0f40a7), UINT64_C(0x05863dbf1e3ac9de), UINT64_C(0xe1fea520be311aaf), 
	UINT64_C(0x9b26d5e88e0493d6), UINT64_C(0x144e44b0de5a085d), UINT64_C(0x6e963478ee6f8124),
    UINT64_C(0x21c640532670ac20), UINT64_C(0x5b1e309b16452559), UINT64_C(0xd476a1c3461bbed2),
	UINT64_C(0xaeaed10b762e37ab), UINT64_C(0x37deb6af5e9b8b5b), UINT64_C(0x4d06c6676eae0222),
    UINT64_C(0xc26e573f3ef099a9), UINT64_C(0xb8b627f70ec510d0), UINT64_C(0xf7e653dcc6da3dd4), 
	UINT64_C(0x8d3e2314f6efb4ad), UINT64_C(0x0256b24ca6b12f26), UINT64_C(0x788ec2849684a65f),
    UINT64_C(0x9cf65a1b368f752e), UINT64_C(0xe62e2ad306bafc57), UINT64_C(0x6946bb8b56e467dc), 
	UINT64_C(0x139ecb4366d1eea5), UINT64_C(0x5ccebf68aecec3a1), UINT64_C(0x2616cfa09efb4ad8),
    UINT64_C(0xa97e5ef8cea5d153), UINT64_C(0xd3a62e30fe90582a), UINT64_C(0xb0c7b7e3c7593bd8), 
	UINT64_C(0xca1fc72bf76cb2a1), UINT64_C(0x45775673a732292a), UINT64_C(0x3faf26bb9707a053),
    UINT64_C(0x70ff52905f188d57), UINT64_C(0x0a2722586f2d042e), UINT64_C(0x854fb3003f739fa5), 
	UINT64_C(0xff97c3c80f4616dc), UINT64_C(0x1bef5b57af4dc5ad), UINT64_C(0x61372b9f9f784cd4),
    UINT64_C(0xee5fbac7cf26d75f), UINT64_C(0x9487ca0fff135e26), UINT64_C(0xdbd7be24370c7322), 
	UINT64_C(0xa10fceec0739fa5b), UINT64_C(0x2e675fb4576761d0), UINT64_C(0x54bf2f7c6752e8a9),
    UINT64_C(0xcdcf48d84fe75459), UINT64_C(0xb71738107fd2dd20), UINT64_C(0x387fa9482f8c46ab),
	UINT64_C(0x42a7d9801fb9cfd2), UINT64_C(0x0df7adabd7a6e2d6), UINT64_C(0x772fdd63e7936baf),
    UINT64_C(0xf8474c3bb7cdf024), UINT64_C(0x829f3cf387f8795d), UINT64_C(0x66e7a46c27f3aa2c), 
	UINT64_C(0x1c3fd4a417c62355), UINT64_C(0x935745fc4798b8de), UINT64_C(0xe98f353477ad31a7),
    UINT64_C(0xa6df411fbfb21ca3), UINT64_C(0xdc0731d78f8795da), UINT64_C(0x536fa08fdfd90e51), 
	UINT64_C(0x29b7d047efec8728), };

ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND,UINT,UINT,LONG);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK InfoWindow(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK Archivation(HWND, UINT, WPARAM, LPARAM);

TCHAR szWindowClass[MAX_LOADSTRING],szTitle[MAX_LOADSTRING],dir[MAX_PATH],dir1[MAX_PATH],copy_buf1[MAX_PATH];
HINSTANCE hInst;
HWND hWndChild = NULL;
HIMAGELIST g_hImageList = NULL;

static HWND hListView_1, hListView_2,hComboBox_1, hComboBox_2,hLabel_1, 
	hLabel_2, hLabel_3, hLabel_4, hToolBar;

//toolbar
const int ImageListID = 0;
const int numButtons = 7;
const DWORD buttonStyles = BTNS_AUTOSIZE;
const int bitmapSize = 16;

DWORD Drivers, sum1=0, sum2=0;
int sel,k=0,y=9,index=-1;
TCHAR c,*ls;

TCHAR buf1[MAX_PATH],cm_dir_from[MAX_PATH],cm_dir_to[MAX_PATH],cm_dir_to_[MAX_PATH],cm_dir_from_[MAX_PATH],
	path[MAX_PATH],_dir[MAX_PATH],_dir1[MAX_PATH],buff[MAX_PATH],tempdir[MAX_PATH],copyBuffer[MAX_PATH];
LPCTSTR s;

bool isCutting = FALSE;

TBBUTTON tbButtons[numButtons] = 
{
	{ MAKELONG(STD_UNDO,   ImageListID), IDM_UP,   TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)(_T("Back"))      },
	{ MAKELONG(STD_COPY,   ImageListID), IDM_COPY, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)(_T("Copy"))      },
	{ MAKELONG(STD_DELETE, ImageListID), IDM_DEL,  TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)(_T("Delete"))    },
	{ MAKELONG(STD_CUT,    ImageListID), IDM_CUT,  TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)(_T("Cut"))       },
	{ MAKELONG(STD_PASTE,  ImageListID), IDM_PASTE,TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)(_T("Paste"))     },
	{ MAKELONG(STD_HELP,   ImageListID), IDM_INFO, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)(_T("Info"))      },
	{ MAKELONG(STD_FILENEW,ImageListID), IDM_ARCH, TBSTATE_ENABLED, buttonStyles, {0}, 0, (INT_PTR)(_T("Arch Menu")) } 
};

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
 
int i,i_dirs,i_files,i_all;
BOOL InitListViewImageLists(HWND hWndListView,int size, TCHAR c_dir[MAX_PATH])
{
	HIMAGELIST hSmall;
	SHFILEINFO lp;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	TCHAR buf1[MAX_PATH],buffer[MAX_PATH];
	DWORD num;

	hSmall = ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_MASK|ILC_COLOR32, size+2, 1);
	
	int LVcounter=0;
	for (int j=0;j<size;j++)
	{
		hFind = FindFirstFile(c_dir, &FindFileData);

		if (hFind != INVALID_HANDLE_VALUE)
		{
			do 
			{
				ListView_GetItemText(hWndListView,LVcounter,0,buffer,MAX_PATH);
				if (wcscmp(FindFileData.cFileName,buffer)==0)
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
					LVcounter++;
					break;
				}
			} while (FindNextFile(hFind, &FindFileData) != 0);
		
			FindClose(hFind);
		}
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
 
void reverseString(wchar_t str1[255], wchar_t str2[255])
{
	int a1,a2,a3;
	a1=wcslen(str1);
	a3=a1;
	for(a2=0;a2<a1;a2++,a3--)
		str2[a2]=str1[a3-1];						
}

void getTypeOfFile(TCHAR nameStr[MAX_PATH], TCHAR res[MAX_PATH])
{
	TCHAR typestr[255],temp[255];
	int ending=0;
	BOOL flag=false;
	wcscpy(typestr,nameStr);
	reverseString(typestr,temp);
	while((ending<wcslen(temp)))
	{
		if(!flag)
		{
			if(temp[ending]=='.')
			{
				temp[ending]=NULL;
				flag=true;
			}
		}
		else
			temp[ending]=NULL;
		ending++;
	}
	wcscpy(typestr,temp);
	reverseString(typestr,temp);
	wcscpy(res,temp);
}

uint64_t crc64(uint64_t crc, const unsigned char *s, uint64_t l) 
{
    uint64_t j;

    for (j = 0; j < l; j++) 
	{
        uint8_t byte = s[j];
        crc = crc64_tab[(uint8_t)crc ^ byte] ^ (crc >> 8);
    }
    return crc;
}

void FindFile(HWND hList, TCHAR c_dir[MAX_PATH])
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;
	BOOL firstSearch=true;

	SendMessage(hList, LVM_DELETEALLITEMS, (WPARAM)0,(LPARAM)0);
	
	i=i_dirs=i_files=i_all=0;
	for (int j=0;j<2;j++)
	{
		hFind = FindFirstFile(c_dir, &FindFileData);
	
		if(hFind == INVALID_HANDLE_VALUE)
		{
			if (firstSearch)
				MessageBox(0,_T("Not found"), _T("Error"),  MB_OK |MB_ICONERROR);
		}
		else
		{
			do
			{
				if(wcscmp(FindFileData.cFileName,_T("."))!=0 && wcscmp(FindFileData.cFileName,_T(".."))!=0)
				{
					if (firstSearch)
					{
						if((FindFileData.dwFileAttributes == 16)||(FindFileData.dwFileAttributes == 17))
						{
							View_List(FindFileData.cFileName,hList,i,0);
							
							ListView_SetItemText(hList,i,1,_T("<directory>"));
							
							i++;
						}
					}
					else
					{
						if(FindFileData.dwFileAttributes == 32)
						{	 
							View_List(FindFileData.cFileName,hList,i,0);
							
							TCHAR temp[255],finaltype[255];
							getTypeOfFile(FindFileData.cFileName,temp);
							wcscpy(finaltype,_T("'"));
							wcscat(finaltype,temp);
							wcscat(finaltype,_T("' file"));
							ListView_SetItemText(hList,i,1,finaltype);
							
							double size=(FindFileData.nFileSizeHigh * (MAXDWORD64+1)) + FindFileData.nFileSizeLow;
							wchar_t sizestr[255];
							swprintf(sizestr, L"%f", size);
							sizestr[wcslen(sizestr)-7] = 0;
							wcscat(sizestr,_T(" bytes"));
							ListView_SetItemText(hList,i,2,sizestr);

							wchar_t crcstr[255];
							uint64_t crc = (unsigned long long)crc64(0,(unsigned char*)FindFileData.cFileName,wcslen(FindFileData.cFileName));
							swprintf(crcstr, L"%016llx", crc);
							ListView_SetItemText(hList,i,3,crcstr);
							
							i++;
						}
					}
				}
			} while (FindNextFile(hFind, &FindFileData) != 0);
		
			FindClose(hFind); 
			InitListViewImageLists(hList,i,c_dir);
		}
		firstSearch=false;
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
	else if(fd.dwFileAttributes == 32)
		wcscpy(fileInfo.type,_T("File"));
	else if(fd.dwFileAttributes == FILE_ATTRIBUTE_SYSTEM)
		wcscpy(fileInfo.type,_T("System file"));
	else if(fd.dwFileAttributes == FILE_ATTRIBUTE_VIRTUAL)
		wcscpy(fileInfo.type,_T("Virtual file"));
	else if(fd.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
		wcscpy(fileInfo.type,_T("Directory")); 

	if (wcscmp(fileInfo.type,_T(""))==0)
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
					FindFile(hListView_1,dir);
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
					}
					FindFile(hListView_1,dir);
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
				wcscpy(buf1,_T(""));
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
			hComboBox_1 =CreateWindow(_T("ComboBox"), NULL,	WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST|CBS_SORT,
							3, 33+y, 50, 110, hwnd, (HMENU) ID_COMBOBOX_1, hInst, NULL);
			hListView_1 =CreateWindow(WC_LISTVIEW, NULL,LVS_REPORT|WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|LVS_AUTOARRANGE,
							0, 60+y+12, 900, 500, hwnd,	(HMENU) ID_LISTVIEW_1, hInst, NULL);
			ListView_SetExtendedListViewStyle(hListView_1,LVS_EX_FULLROWSELECT);

			ls = buff;

            while (*ls)
            {
                    SendMessage(hComboBox_1, CB_ADDSTRING, 0, (LPARAM)ls);
                    ls += wcslen(ls)+1;
            }

			SendMessage(hComboBox_1, CB_SETCURSEL, 0, 0);

			AddColToListView(_T("Name"), 1, 550);
			AddColToListView(_T("Type"), 2, 103);
			AddColToListView(_T("Size"), 3, 130);
			AddColToListView(_T("CRC64"),4, 100);

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
						  80, 72, 215, 16, hDlg, (HMENU)ID_LABELCRTD, hInst, NULL);
		FileTimeToString(info.last,stringLast);
		LabelLast =CreateWindow(_T("static"), stringLast, WS_CHILD | WS_VISIBLE | WS_TABSTOP,
						  80, 95, 215, 16, hDlg, (HMENU)ID_LABELLAST, hInst, NULL);
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

static HWND rb_arch, rb_disarch, edit_path, edit_name, cb_alg;
void UseBasicAlgorithm(HWND hDlg,TCHAR option[MAX_PATH],char *alg,TCHAR formatOfArch[MAX_PATH])
{
	char *args[5];
	char *tempFilePath=(char*)malloc(MAX_PATH);
	char *tempCharStr=(char*)malloc(MAX_PATH);
	char *algStr=(char*)malloc(MAX_PATH);
	TCHAR nameStr[MAX_PATH],pathStr[MAX_PATH],temp[MAX_PATH];

	if(IsDlgButtonChecked(hDlg,ID_RBARCH))
	{
		args[0]="";
		args[1]="c";
		args[2]=alg;

		index=ListView_GetNextItem(hListView_1,-1,LVIS_SELECTED);
		if (index!=-1)
		{
			ListView_GetItemText(hListView_1,index,0,temp,MAX_PATH);
			wcstombs(tempCharStr,temp,MAX_PATH);
			wcstombs(tempFilePath,dir,MAX_PATH);
			tempFilePath[strlen(tempFilePath)-1]=0;
			strcat(tempFilePath,tempCharStr);
			args[3]=tempFilePath;

			wcscpy(nameStr,L"");
			wcscpy(pathStr,L"");
			GetWindowText(edit_name,nameStr,MAX_PATH);	
			GetWindowText(edit_path,pathStr,MAX_PATH);
			if ((nameStr!=L"") && (pathStr!=L""))
			{
				wcscat(pathStr,nameStr);
				wcscat(pathStr,L".");
				wcscat(pathStr,formatOfArch);
				wcstombs(tempCharStr,pathStr,MAX_PATH);
				args[4]=tempCharStr;

				BFA::BasicFileAlgs bfa;
				bfa.BasicFile(5,args);
				MessageBox(0,_T("Done!"), _T(""),MB_OK|MB_ICONASTERISK);
				FindFile(hListView_1,dir);
				SendMessage(hDlg,WM_CLOSE,0,0);
			}
			else
				MessageBox(0,_T("Input name & path to file"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
		}
		else
			MessageBox(0,_T("Choose any file"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
	}
	else
		if(IsDlgButtonChecked(hDlg,ID_RBDISARCH))
		{
			index=ListView_GetNextItem(hListView_1,-1,LVIS_SELECTED);
			if (index!=-1)
			{
				args[1]="d";

				ListView_GetItemText(hListView_1,index,0,temp,MAX_PATH);
				wcstombs(tempCharStr,temp,MAX_PATH);
				wcstombs(tempFilePath,dir,MAX_PATH);
				tempFilePath[strlen(tempFilePath)-1]=0;
				strcat(tempFilePath,tempCharStr);
				args[2]=tempFilePath;

				wcscpy(nameStr,L"");
				wcscpy(pathStr,L"");
				GetWindowText(edit_name,nameStr,MAX_PATH);	
				GetWindowText(edit_path,pathStr,MAX_PATH);
				if (nameStr!=L"" && pathStr!=L"")
				{
					getTypeOfFile(nameStr,temp);
					if(wcscmp(temp,formatOfArch)==0)
					{
						nameStr[wcslen(nameStr)-(wcslen(formatOfArch)+1)]=0;
						wcscat(pathStr,nameStr);
						wcstombs(tempCharStr,pathStr,MAX_PATH);
						args[3]=tempCharStr;

						BFA::BasicFileAlgs bfa;
						bfa.BasicFile(4,args);
						MessageBox(0,_T("Done!"), _T(""),MB_OK|MB_ICONASTERISK);
						FindFile(hListView_1,dir);
						SendMessage(hDlg,WM_CLOSE,0,0);
					}
					else
						MessageBox(0,_T("Wrong type of file"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
				}
				else
					MessageBox(0,_T("Input name & path to file"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
			}
			else
				MessageBox(0,_T("Choose any file"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
		}
		else
			MessageBox(0,_T("Choose method"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
}

void getNameOfDir(TCHAR nameStr[MAX_PATH], TCHAR res[MAX_PATH])
{
	TCHAR typestr[255],temp[255];
	int ending=0;
	BOOL flag=false;
	wcscpy(typestr,nameStr);
	reverseString(typestr,temp);
	while((ending<wcslen(temp)))
	{
		if(!flag)
		{
			if(temp[ending]=='\\')
			{
				temp[ending]=NULL;
				flag=true;
			}
		}
		else
			temp[ending]=NULL;
		ending++;
	}
	wcscpy(typestr,temp);
	reverseString(typestr,temp);
	wcscpy(res,temp);
}

INT_PTR CALLBACK Archivation(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		TCHAR name[MAX_PATH],archive_path[MAX_PATH];

		rb_arch=CreateWindow(_T("button"), _T("Archivate"),WS_CHILD|WS_VISIBLE|BS_RADIOBUTTON,
			50, 8, 80, 20, hDlg, (HMENU)ID_RBARCH, hInst, NULL);
		rb_disarch=CreateWindow(_T("button"), _T("Disarchivate"),WS_CHILD|WS_VISIBLE|BS_RADIOBUTTON,
			135, 8, 100, 20, hDlg, (HMENU)ID_RBDISARCH, hInst, NULL);
		
		wcscpy(archive_path,dir);
		archive_path[wcslen(archive_path)-1]=0;
		edit_path=CreateWindow(_T("edit"), archive_path,WS_CHILD | WS_VISIBLE | ES_LEFT | WS_BORDER,
			50, 39, 400, 20, hDlg, (HMENU)ID_EDITPATH, hInst, NULL);

		wcscpy(name,L"");
		index=ListView_GetNextItem(hListView_1,-1,LVIS_SELECTED);
		ListView_GetItemText(hListView_1,index,0,name,MAX_PATH);
		if (name!=L"")
			edit_name=CreateWindow(_T("edit"), name, WS_CHILD | WS_VISIBLE | ES_CENTER | WS_BORDER,
				70, 72, 380, 20, hDlg, (HMENU)ID_EDITNAME, hInst, NULL);
		else
			edit_name=CreateWindow(_T("edit"), _T(""), WS_CHILD | WS_VISIBLE | ES_CENTER | WS_BORDER,
				70, 72, 380, 20, hDlg, (HMENU)ID_EDITNAME, hInst, NULL);

		cb_alg =CreateWindow(_T("ComboBox"), NULL,	WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST,
							320, 8, 130, 110, hDlg, (HMENU) ID_COMBOBOXALG, hInst, NULL);
		
		SendMessage(cb_alg, CB_ADDSTRING, 0, (LPARAM)_T("ZIP"));
		SendMessage(cb_alg, CB_ADDSTRING, 0, (LPARAM)_T("LZW"));
		SendMessage(cb_alg, CB_ADDSTRING, 0, (LPARAM)_T("LZ77"));
		SendMessage(cb_alg, CB_ADDSTRING, 0, (LPARAM)_T("MOWN"));
		SendMessage(cb_alg, CB_ADDSTRING, 0, (LPARAM)_T("HUFF"));
		SendMessage(cb_alg, CB_ADDSTRING, 0, (LPARAM)_T("SHANNON-FANO"));

		SendMessage(cb_alg, CB_SETCURSEL, 0, 0);
		
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			SendMessage(hListView_1,LVM_REDRAWITEMS,0,0);
			return (INT_PTR)TRUE;
		}

		if (ID_RBARCH<=LOWORD(wParam) && LOWORD(wParam)<=ID_RBDISARCH)
		{
			CheckRadioButton(hDlg, ID_RBARCH, ID_RBDISARCH, LOWORD(wParam));
		}

		if (LOWORD(wParam)==ID_START)
		{
			TCHAR option[MAX_PATH],outFile[MAX_PATH];
			GetDlgItemText(hDlg,ID_COMBOBOXALG,option,MAX_PATH);

			if (wcscmp(option,_T("ZIP"))==0)
			{
				TCHAR *args[2], tempFilePath[MAX_PATH], nameOfDir[MAX_PATH];
				TCHAR nameStr[MAX_PATH],pathStr[MAX_PATH],temp[MAX_PATH];

				if(IsDlgButtonChecked(hDlg,ID_RBARCH))
				{
					index=ListView_GetNextItem(hListView_1,-1,LVIS_SELECTED);
					if (index!=-1)
					{
						wcscpy(nameStr,L"");
						wcscpy(pathStr,L"");
						GetWindowText(edit_name,nameStr,MAX_PATH);	
						GetWindowText(edit_path,pathStr,MAX_PATH);
						if (nameStr!=L"" && pathStr!=L"")
						{
							wcscat(pathStr,nameStr);
							wcscat(pathStr,L".zip");
							wcscpy(temp,pathStr);
							args[1]=temp;

							HZIP hz = CreateZip(args[1],"");
							while(index!=-1)
							{
								ListView_GetItemText(hListView_1,index,0,temp,MAX_PATH);
								wcscpy(tempFilePath,dir);
								tempFilePath[wcslen(tempFilePath)-1]=0;
								wcscat(tempFilePath,temp);
								args[0]=tempFilePath;
								info=GetFileInform(args[0]);
								if (wcscmp(info.type,_T("Directory"))==0)
								{
									getNameOfDir(args[0],nameOfDir);
									ZipAddFolder(hz,nameOfDir);
									index=ListView_GetNextItem(hListView_1,-1,LVNI_ALL);
								}
								else
									ZipAdd(hz,temp, args[0]);
								index=ListView_GetNextItem(hListView_1,index,LVIS_SELECTED|LVNI_BELOW);
								if(index==0)
									index=-1;
							}
							CloseZip(hz);
							MessageBox(0,_T("Done!"), _T(""),MB_OK|MB_ICONASTERISK);
							FindFile(hListView_1,dir);
							SendMessage(hDlg,WM_CLOSE,0,0);
						}
						else
							MessageBox(0,_T("Input name & path to file"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
					}
					else
						MessageBox(0,_T("Choose any file"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
				}
				else
					if(IsDlgButtonChecked(hDlg,ID_RBDISARCH))
					{
						index=ListView_GetNextItem(hListView_1,-1,LVIS_SELECTED);
						if (index!=-1)
						{
							ListView_GetItemText(hListView_1,index,0,temp,MAX_PATH);
							wcscpy(tempFilePath,dir);
							tempFilePath[wcslen(tempFilePath)-1]=0;
							wcscat(tempFilePath,temp);
							args[0]=tempFilePath;

							wcscpy(nameStr,L"");
							wcscpy(pathStr,L"");
							GetWindowText(edit_name,nameStr,MAX_PATH);	
							GetWindowText(edit_path,pathStr,MAX_PATH);
							if (nameStr!=L"" && pathStr!=L"")
							{
								getTypeOfFile(nameStr,temp);
								if(wcscmp(temp,_T("zip"))==0)
								{
									nameStr[wcslen(nameStr)-4]=0;
									wcscpy(temp,pathStr);
									args[1]=temp;

									HZIP hz = OpenZip(args[0],"");
									SetUnzipBaseDir(hz,args[1]);
									ZIPENTRY ze; 
									GetZipItem(hz,-1,&ze); 
									int numitems=ze.index;
									for (int i=0; i<numitems; i++)
									{ 
										GetZipItem(hz,i,&ze);
										UnzipItem(hz,i,ze.name);
									}
									CloseZip(hz);
									MessageBox(0,_T("Done!"), _T(""),MB_OK|MB_ICONASTERISK);
									FindFile(hListView_1,dir);
									SendMessage(hDlg,WM_CLOSE,0,0);
								}
								else
									MessageBox(0,_T("Input file must be .zip"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
							}
							else
								MessageBox(0,_T("Input name & path to file"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
						}
						else
							MessageBox(0,_T("Choose any file"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
					}
					else
						MessageBox(0,_T("Choose method"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
			}
			if (wcscmp(option,_T("LZW"))==0)
			{
				char *args[3];
				char *tempFilePath=(char*)malloc(MAX_PATH);
				char *tempCharStr=(char*)malloc(MAX_PATH);
				TCHAR nameStr[MAX_PATH],pathStr[MAX_PATH],temp[MAX_PATH];

				if(IsDlgButtonChecked(hDlg,ID_RBARCH))
				{
					index=ListView_GetNextItem(hListView_1,-1,LVIS_SELECTED);
					if (index!=-1)
					{
						ListView_GetItemText(hListView_1,index,0,temp,MAX_PATH);
						wcstombs(tempCharStr,temp,MAX_PATH);
						wcstombs(tempFilePath,dir,MAX_PATH);
						tempFilePath[strlen(tempFilePath)-1]=0;
						strcat(tempFilePath,tempCharStr);
						args[1]=tempFilePath;

						wcscpy(nameStr,L"");
						wcscpy(pathStr,L"");
						GetWindowText(edit_name,nameStr,MAX_PATH);	
						GetWindowText(edit_path,pathStr,MAX_PATH);
						if (nameStr!=L"" && pathStr!=L"")
						{
							wcscat(pathStr,nameStr);
							wcscat(pathStr,L".lzw");
							wcstombs(tempCharStr,pathStr,MAX_PATH);
							args[2]=tempCharStr;

							CLZWCompressFile lzw;
							lzw.Compress(args[1],args[2]);
							MessageBox(0,_T("Done!"), _T(""),MB_OK|MB_ICONASTERISK);
							FindFile(hListView_1,dir);
							SendMessage(hDlg,WM_CLOSE,0,0);
						}
						else
							MessageBox(0,_T("Input name & path to file"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
					}
					else
						MessageBox(0,_T("Choose any file"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
				}
				else
					if(IsDlgButtonChecked(hDlg,ID_RBDISARCH))
					{
						index=ListView_GetNextItem(hListView_1,-1,LVIS_SELECTED);
						if (index!=-1)
						{
							ListView_GetItemText(hListView_1,index,0,temp,MAX_PATH);
							wcstombs(tempCharStr,temp,MAX_PATH);
							wcstombs(tempFilePath,dir,MAX_PATH);
							tempFilePath[strlen(tempFilePath)-1]=0;
							strcat(tempFilePath,tempCharStr);
							args[1]=tempFilePath;

							wcscpy(nameStr,L"");
							wcscpy(pathStr,L"");
							GetWindowText(edit_name,nameStr,MAX_PATH);	
							GetWindowText(edit_path,pathStr,MAX_PATH);
							if (nameStr!=L"" && pathStr!=L"")
							{
								getTypeOfFile(nameStr,temp);
								if(wcscmp(temp,_T("lzw"))==0)
								{
									nameStr[wcslen(nameStr)-4]=0;
									wcscat(pathStr,nameStr);
									wcstombs(tempCharStr,pathStr,MAX_PATH);
									args[2]=tempCharStr;

									CLZWCompressFile lzw;
									lzw.Expand(args[1],args[2]);
									MessageBox(0,_T("Done!"), _T(""),MB_OK|MB_ICONASTERISK);
									FindFile(hListView_1,dir);
									SendMessage(hDlg,WM_CLOSE,0,0);
								}
								else
									MessageBox(0,_T("Input file must be .mown"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
							}
							else
								MessageBox(0,_T("Input name & path to file"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
						}
						else
							MessageBox(0,_T("Choose any file"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
					}
					else
						MessageBox(0,_T("Choose method"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
			}
			if (wcscmp(option,_T("LZ77"))==0)
			{
				UseBasicAlgorithm(hDlg,option,"lz",_T("lz"));
			}
			if (wcscmp(option,_T("HUFF"))==0)
			{
				UseBasicAlgorithm(hDlg,option,"huff",_T("huf"));
			}
			if (wcscmp(option,_T("SHANNON-FANO"))==0)
			{
				UseBasicAlgorithm(hDlg,option,"sf",_T("sf"));
			}
			if (wcscmp(option,_T("MOWN"))==0)
			{
				char *args[4];
				char *tempFilePath=(char*)malloc(MAX_PATH);
				char *tempCharStr=(char*)malloc(MAX_PATH);
				TCHAR nameStr[MAX_PATH],pathStr[MAX_PATH],temp[MAX_PATH];

				if(IsDlgButtonChecked(hDlg,ID_RBARCH))
				{
					args[0]="";

					index=ListView_GetNextItem(hListView_1,-1,LVIS_SELECTED);
					if (index!=-1)
					{
						ListView_GetItemText(hListView_1,index,0,temp,MAX_PATH);
						wcstombs(tempCharStr,temp,MAX_PATH);
						wcstombs(tempFilePath,dir,MAX_PATH);
						tempFilePath[strlen(tempFilePath)-1]=0;
						strcat(tempFilePath,tempCharStr);
						args[1]=tempFilePath;

						wcscpy(nameStr,L"");
						wcscpy(pathStr,L"");
						GetWindowText(edit_name,nameStr,MAX_PATH);	
						GetWindowText(edit_path,pathStr,MAX_PATH);
						if (nameStr!=L"" && pathStr!=L"")
						{
							wcscat(pathStr,nameStr);
							wcscat(pathStr,L".mown");
							wcstombs(tempCharStr,pathStr,MAX_PATH);
							args[2]=tempCharStr;

							MowN::MownAlg mAlg;
							mAlg.mainMown(3,args);
							MessageBox(0,_T("Done!"), _T(""),MB_OK|MB_ICONASTERISK);
							FindFile(hListView_1,dir);
							SendMessage(hDlg,WM_CLOSE,0,0);
						}
						else
							MessageBox(0,_T("Input name & path to file"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
					}
					else
						MessageBox(0,_T("Choose any file"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
				}
				else
					if(IsDlgButtonChecked(hDlg,ID_RBDISARCH))
					{
						args[0]="X";
						args[1]="X";

						index=ListView_GetNextItem(hListView_1,-1,LVIS_SELECTED);
						if (index!=-1)
						{
							ListView_GetItemText(hListView_1,index,0,temp,MAX_PATH);
							wcstombs(tempCharStr,temp,MAX_PATH);
							wcstombs(tempFilePath,dir,MAX_PATH);
							tempFilePath[strlen(tempFilePath)-1]=0;
							strcat(tempFilePath,tempCharStr);
							args[2]=tempFilePath;

							wcscpy(nameStr,L"");
							wcscpy(pathStr,L"");
							GetWindowText(edit_name,nameStr,MAX_PATH);	
							GetWindowText(edit_path,pathStr,MAX_PATH);
							if (nameStr!=L"" && pathStr!=L"")
							{
								getTypeOfFile(nameStr,temp);
								if(wcscmp(temp,_T("mown"))==0)
								{
									nameStr[wcslen(nameStr)-5]=0;
									wcscat(pathStr,nameStr);
									wcstombs(tempCharStr,pathStr,MAX_PATH);
									args[3]=tempCharStr;

									MowN::MownAlg mAlg;
									mAlg.mainMown(4,args);
									MessageBox(0,_T("Done!"), _T(""),MB_OK|MB_ICONASTERISK);
									FindFile(hListView_1,dir);
									SendMessage(hDlg,WM_CLOSE,0,0);
								}
								else
									MessageBox(0,_T("Input file must be .mown"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
							}
							else
								MessageBox(0,_T("Input name & path to file"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
						}
						else
							MessageBox(0,_T("Choose any file"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
					}
					else
						MessageBox(0,_T("Choose method"), _T("Info"),  MB_OK|MB_ICONINFORMATION);
			}
		}
		break;
	}
	return (INT_PTR)FALSE;
}
