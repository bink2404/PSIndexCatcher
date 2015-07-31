/************************************************************************/
/*																		*/
/* emDEX																*/
/*																		*/
/* FrameMaker indexing plugin executable.								*/
/*																		*/
/* 17/09/02		    Version 1.00 -	Creation.							*/
/* 03/06/03		    Version 2.00 -	Added emDEX marker window.			*/
/*																		*/
/************************************************************************/

#include <windows.h>
#include <ddeml.h>
#include <stdarg.h>
#include <windef.h>
#include <stdio.h>
#include <process.h>
#include <mmsystem.h>

#include "PSIndexCatcher.h"
#include "PSIndexCommon.h"

static HINSTANCE appInstance;
static HCONV convId;
static DWORD ddeInst;

#define _UNICODE

CHAR g_szWindowTitle[MAXLEN] ;
CHAR g_szSaveString[MAXLEN] ;
CHAR g_FileName[MAXLEN] ;
CHAR g_UnformattedText[MAXLEN] ;
CHAR g_szMarkerString[MAXLEN] ;
LONG g_lMarkerId ;

#define ITEMLIST_DELETE	WM_USER+98
#define ITEMLIST_UPDATE	WM_USER+99
#define LISTBOX_UPDATE	WM_USER+100
#define ITEMLIST_CLEAR	WM_USER+101
#define CATCHER_OPEN	WM_USER+102
#define CHANGEFONT		WM_USER+103
#define CHANGEOPTION	WM_USER+104
#define SETFOCUS		WM_USER+105
#define SETPOS			WM_USER+106
#define EDIT_BUTTONS	WM_USER+107

#define ITEMDELETE		1
#define ITEMFIND		2
#define	ITEMEDIT		3
#define	ITEMREPLACE		4
#define	ITEMFINDPAGE	5
//#define ORPHANRANGE		6
#define DOITEMUPDATE	7
#define REALTIME		8
#define ITEMCHANGE		9

#define UDEFAULT		100
#define DEFAULT			104
#define UBOLD			101
#define BOLD			105
#define	UTALIC			102
#define	ITALIC			106
#define UBOLDITALIC		103
#define BOLDITALIC		107

#define	RANGESTART		1
#define RANGEEND		2

#define SORT_WORD_BY_WORD		1
#define	SORT_LETTER_BY_LETTER	2

typedef struct ITEMSTRUCT {
	CHAR	itemText[MAXLEN] ;
	INT		itemPage[MAXLEN] ;
	CHAR	itemReference[MAXLEN] [REFLEN] ;
} ITEMSTRUCT;

typedef struct EDITORBUTTONS {
	CHAR	text[MAXLEN] ;
	CHAR	action[MAXLEN] ;
} EDITORBUTTONS ;

ITEMSTRUCT itemList[MAXITEMS+1] ;
ITEMSTRUCT inItemList[MAXITEMS+1] ;
ITEMSTRUCT saveItemList[MAXITEMS+1] ;
CHAR szAddString[MAXITEMS+1] [MAXLEN] ;
CHAR szPageString[MAXITEMS+1] [MAXLEN] ;
INT iPageFormat[MAXITEMS+1] [MAXLEN] ;
INT iSavePageFormat[MAXITEMS+1] [MAXLEN] ;
INT iPageNumbering[MAXITEMS+1] [MAXLEN] ;
INT iSavePageNumbering[MAXITEMS+1] [MAXLEN] ;
INT iRange[MAXITEMS+1] [MAXLEN] ;
INT iSaveRange[MAXITEMS+1] [MAXLEN] ;
INT szBracketsDisplayed[MAXITEMS+1] [MAXLEN] ;
CHAR szIgnoreChars[MAXLEN] ;
CHAR szSeparatorChars[MAXLEN], szSaveSeparatorChars[MAXLEN] ;
HICON	winIcon ;
HWND	hAboutBox, hFindWindow = NULL, hReplaceWindow, hIgnoreWindow = NULL, hButtonWindow = NULL, hSeparatorWindow = NULL ;
BOOL	bAboutBoxDisplayed ;
HFONT	defaultFont, italicFont, boldFont, bolditalicFont ;
BOOL	g_bOrphanAudio, g_bOrphanVisual ;
BOOL	g_bRealTimeUpdates, g_bAlwaysOnTop, g_bUseEditor ;
INT		g_iSortOption ;
HWND	LBox ;
INT		fontHeight ;
HMENU	ContextMenu, pContextMenu ;
//CRITICAL_SECTION Critical_Section ;
BOOL	isThreadProcessing = FALSE ;
UINT		itemLimit ;
CHAR	executePath[MAXLEN] ;
EDITORBUTTONS buttons[10] ;
HHOOK	hHook ;

COLORREF g_rgbText = RGB(0, 0, 0);
COLORREF chColor = RGB(0, 0, 0); 
static COLORREF acrCustClr[16] = {	RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), 
									RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255),
									RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255),
									RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255), RGB(255, 255, 255) } ;

							// Received    ANSI
int specialChars[160] [2] = {	-11,		-128,
								-30,		-126,
								-29,		-124,
								-55,		-123,
								-96,		-122,
								-32,		-121,
								-28,		-119,
								-77,		-118,
								-36,		-117,
								-50,		-116,
								-6,			-114,
								-48,		-106,
								-7,			-115,
								-9,			-104,
								-86,		-103,
								-16,		-102,
								-35,		-101,
								-2,			-98,
								-37,		-92,
								-92,		-89,
								-84,		-88,
								-88,		-82,
								-8,			-81,
								-72,		-77,
								-85,		-76,
								-4,			-72,
								-74,		-71,
								-67,		-66,
								-53,		-64,
								-25,		-63,
								-27,		-62,
								-52,		-61,
								-23,		-56,
								-26,		-54,
								-24,		-53,
								-19,		-52,
								-22,		-51,
								-21,		-50,
								-20,		-49,
								-61,		-48,
								-15,		-46,
								-18,		-45,
								-17,		-44,
								-51,		-43,
								-80,		-41,
								-81,		-40,
								-12,		-39,
								-14,		-38,
								-13,		-37,
								-59,		-35,
								-41,		-34,
								-117,		-29,
								-78,		-16,
								-101,		-11,
								-65,		-8,
								-58,		-3,
								-54,		-2,
								16,			32,
								17,			32,
								18,			32,
								19,			32,
								20,			32,
								9,			32,
								-45,		34,
								-46,		34,
								-3,			34,
								-43,		39,
								-44,		39,
								21,			45,
								-38,		47,
								-83,		124,
								-60,		131,
								-91,		149,
								-63,		161,
								-76,		165,
								-69,		170,
								-57,		171,
								-47,		173,
								-5,			176,
								-95,		176,
								-73,		178,
								-90,		182,
								-31,		183,
								-68,		186,
								-56,		187,
								-71,		188,
								-70,		189,
								-64,		191,
								-128,		196,
								-127,		197,
								-82,		198,
								-126,		199,
								-125,		201,
								-124,		209,
								-123,		214,
								-122,		220,
								-89,		223,
								-120,		224,
								-121,		225,
								-119,		226,
								-118,		228,
								-116,		229,
								-66,		230,
								-115,		231,
								-113,		232,
								-114,		233,
								-112,		234,
								-111,		235,
								-109,		236,
								-110,		237,
								-108,		238,
								-107,		239,
								-106,		241,
								-104,		242,
								-105,		243,
								-103,		244,
								-102,		246,
								-42,		247,
								-99,		249,
								-100,		250,
								-98,		251,
								-97,		252,
								-40,		255,
								0,			0	} ;

#define UDLGID 2				// id for the tray icon.

FILE *tempHandle ;

void addNewItem(int insertAt, int itemCount) ;
void updateListItems(INT inCount) ;
void startThreads(PVOID in) ;
void sendDDEMessage(HWND dlg, int command) ;
void sendDDEMessage(HWND dlg, int command, CHAR *findText, CHAR *replaceText, BOOL isCaseSensitive) ;
void sendDDEMessage(HWND dlg, int command, INT pageNum, CHAR *referenceText) ;
void sendDDEMessage(HWND dlg, int command, LONG idNum, CHAR *referenceText) ;
void getUnformattedText(CHAR *inText, CHAR *outText) ;
INT  compareSortText(CHAR *text1, CHAR *text2, INT len) ;
BOOL	 getCompareText(CHAR *inText, CHAR *outText) ;
void	 getAddText(CHAR *inText, CHAR *outText) ;
void	 removeTagText(CHAR *inText, CHAR *outText) ;
void	stripSpaces(CHAR *text) ;
void	stripAllSpaces(CHAR *inText, CHAR *outText) ;
INT		noTagCompare(CHAR *inText1, CHAR *inText2) ;
BOOL	APIENTRY AboutProc(HWND, UINT, WPARAM, LPARAM) ;
BOOL	APIENTRY FindProc(HWND, UINT, WPARAM, LPARAM) ;
BOOL	APIENTRY ReplaceProc(HWND, UINT, WPARAM, LPARAM) ;
BOOL	APIENTRY IgnoreCharsProc(HWND, UINT, WPARAM, LPARAM) ;
BOOL	APIENTRY RangeSeparatorsProc(HWND, UINT, WPARAM, LPARAM) ;
BOOL	APIENTRY ButtonProc(HWND, UINT, WPARAM, LPARAM) ;
LRESULT FAR PASCAL GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam) ;
INT	getNumberFormatting(CHAR *inString) ;
void getFullItemText(INT itemStart, CHAR *itemText) ;
void getFullItemTextArray(INT itemStart, CHAR *text1[]) ;
void getTabItemText(CHAR *inText, CHAR *outText, INT numTabs) ;
INT  getTabCount(CHAR *itemText) ;
BOOL	isEscapeSequence(CHAR *text, INT pos) ;
INT		sortFormattedText(CHAR *inText1, CHAR *inText2, CHAR *inCase1, CHAR *inCase2) ;
VOID	stripIgnoreChars(CHAR *text) ;
void stripAllSpecialChars(CHAR *inText, CHAR *outText) ;
VOID stripPageFormatting(CHAR *inText, CHAR *outText) ;
void convertSpecialCharacters(CHAR *inText) ;
void toRoman(int inNum, int inNumbering, CHAR *outString) ;
void toAlpha(int inNum, int inNumbering, CHAR *outString) ;
void writeToDebugFile(CHAR *text) ;
UINT APIENTRY PrintHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, 
                                                LPARAM lParam); 

int errMsg(HWND owner, UINT style, char *fmt, ... )
{
	char text[500];
	va_list args;

	va_start(args, fmt);
	wvsprintf(text, fmt, args);
	va_end(args);

	return MessageBox(owner, text, "emDEX server", style|MB_APPLMODAL);
}  	 

BOOL CALLBACK emDEXProc(HWND dlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RECT	pRect ;
	CHAR	szTemp[MAXLEN] ;
	char	szAtom[MAXLEN], szAtomNum[MAXLEN], szNumString[80], szTempString1[MAXLEN], szTempString2[MAXLEN] ;
	char	szExecString[MAXLEN] ;
	int		iCount, inCount, i, j ;
	static INT		selected, iTempSort ;
	INT		iStart ;
	static	BOOL isProcessing = FALSE ;
	static	BOOL isFirstUpdate = TRUE ;
//	SHORT	RetCode ;
	NOTIFYICONDATA tnib;
	INT numTabs = 0 ;
	INT maxLen = 0, len = 0 ;
	LPDRAWITEMSTRUCT    lpDIS;
	SIZE	sze ;
	INT		formatOffset, menuSelection ;
	CHOOSEFONT cf = {sizeof(CHOOSEFONT)};
	CHOOSECOLOR cc = {sizeof(CHOOSECOLOR)};
	LOGFONT lf;
	HWND	nextWindow, closeWindows[80] ;
	CHAR	szSaveOption[800] ;
	HMENU	popupMenu, subMenu ;
	POINT	CursorPos ;
	INT		formatCount = 0, currentFormat ;
	BOOL	anyFound = FALSE ;
	CHAR	*temp ;
	INT		XPos, YPos, XSize, YSize ;
	RECT	windowRect ;
	LONG	RegValueSize, lRetCode ;
	HWND	hwndWinPos[80] ;
	ATOM	tempAtom ;
	PRINTDLG pd; 
	INT		exeRetCode ;

	if (msg == WM_USER+200)
	{
		POINT	CursorPos ;

		switch (lParam)
		{
			case WM_RBUTTONDOWN:
				/* This allows the popup menu to be cancelled by pressing Esc. etc. */
				SetForegroundWindow(dlg);

				if (bAboutBoxDisplayed == TRUE)
				{
					/* If the about box is already displayed, bring this to the foreground. */
					SetFocus(hAboutBox) ;
					SetForegroundWindow(hAboutBox);

					break ;
				}

				/* Display the context menu. */
				GetCursorPos(&CursorPos) ;

				/* Bring up the popup menu and send messages to the main dialog window (this). */
				TrackPopupMenu(pContextMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON, CursorPos.x, 
													  CursorPos.y, 0, dlg, NULL) ;

				/* This fixes a problem where the second call doesn't show the popup. */
				PostMessage(dlg, WM_USER, 0, 0);

				break;
		}

		return TRUE;
	}

	switch ( msg )
	{
		case WM_CONTEXTMENU :
			// Bring up the popup window to display available pages.
			popupMenu = LoadMenu(appInstance, "IDR_MENU3") ;
			subMenu = GetSubMenu(popupMenu, 0) ;

			RemoveMenu(subMenu, 0, MF_BYPOSITION) ;

			selected = SendMessage(LBox, LB_GETCURSEL, 0, 0) ;

			for (i = 0 ; itemList[selected].itemPage[i] != -1 ; i++)
			{
//				itoa(itemList[selected].itemPage[i], szNumString, 10) ;

//				AppendMenu(subMenu, MF_STRING, 44+i, szNumString) ;
				AppendMenu(subMenu, MF_STRING, 44+i, itemList[selected].itemReference[i]) ;
			}

			/* This allows the popup menu to be cancelled by pressing Esc. etc. */
			SetForegroundWindow(dlg) ;

			GetCursorPos(&CursorPos) ;

			/* Bring up the popup menu and send messages to the main dialog window. */
			menuSelection = TrackPopupMenu(subMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_RETURNCMD, 
											CursorPos.x, CursorPos.y, 0, dlg, NULL) ;

			if (menuSelection != 0)
			{
				GetMenuString(subMenu, menuSelection, szNumString, 80, MF_BYCOMMAND) ;

				for (i = 0 ; itemList[selected].itemPage[i] != -1 ; i++)
					if (strcmp(szNumString, itemList[selected].itemReference[i]) == 0)
						break ;

				sendDDEMessage(dlg, ITEMFINDPAGE, itemList[selected].itemPage[i], itemList[selected].itemReference[i]) ;
//				sendDDEMessage(dlg, ITEMFINDPAGE, atoi(szNumString)) ;

/*				SetFocus(dlg) ;
				SetForegroundWindow(dlg); */
			}

			/* This fixes a problem where the second call doesn't show the popup. */
			PostMessage(dlg, WM_USER, 0, 0);

			break ;

		case WM_INITDIALOG:
			ContextMenu = LoadMenu(appInstance, MAKEINTRESOURCE(IDR_MENU1)) ;
			pContextMenu = GetSubMenu(ContextMenu, 0) ;

			GetPrivateProfileString("Options", "FontColor", "0", szSaveOption, 800, "emDEX.ini") ;
			g_rgbText = atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfHeight", "-15", szSaveOption, 800, "emDEX.ini") ;
			lf.lfHeight = atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfWidth", "0", szSaveOption, 800, "emDEX.ini") ;
			lf.lfWidth = atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfEscapement", "0", szSaveOption, 800, "emDEX.ini") ;
			lf.lfEscapement = atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfOrientation", "0", szSaveOption, 800, "emDEX.ini") ;
			lf.lfOrientation = atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfWeight", "400", szSaveOption, 800, "emDEX.ini") ;
			lf.lfWeight = atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfItalic", "0", szSaveOption, 800, "emDEX.ini") ;
			lf.lfItalic = (UCHAR)atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfUnderline", "0", szSaveOption, 800, "emDEX.ini") ;
			lf.lfUnderline = (UCHAR)atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfStrikeOut", "0", szSaveOption, 800, "emDEX.ini") ;
			lf.lfStrikeOut = (UCHAR)atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfCharSet", "0", szSaveOption, 800, "emDEX.ini") ;
			lf.lfCharSet = (UCHAR)atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfOutPrecision", "3", szSaveOption, 800, "emDEX.ini") ;
			lf.lfOutPrecision = (UCHAR)atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfQuality", "1", szSaveOption, 800, "emDEX.ini") ;
			lf.lfQuality = (UCHAR)atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfPitchAndFamily", "18", szSaveOption, 800, "emDEX.ini") ;
			lf.lfPitchAndFamily = (UCHAR)atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfFaceName", "Times New Roman", szSaveOption, 800, "emDEX.ini") ;
			strcpy(lf.lfFaceName, szSaveOption) ;

			GetPrivateProfileString("Options", "Color", "16777215", szSaveOption, 800, "emDEX.ini") ;
			chColor = atol(LPCTSTR(szSaveOption)) ;

			defaultFont = CreateFontIndirect(&lf) ;

			lf.lfItalic = 1 ;
			italicFont = CreateFontIndirect(&lf) ;

			lf.lfItalic = 0 ;
			lf.lfWeight = 700 ;
			boldFont = CreateFontIndirect(&lf) ;

			lf.lfItalic = 1 ;
			lf.lfWeight = 700 ;
			bolditalicFont = CreateFontIndirect(&lf) ;

			GetPrivateProfileString("Options", "OrphansAudio", "1", szSaveOption, 800, "emDEX.ini") ;
			g_bOrphanAudio = atoi(LPCTSTR(szSaveOption)) ;

			if (g_bOrphanAudio == TRUE)
				CheckMenuItem(pContextMenu, ID_ORPHANSAUDIO, MF_CHECKED) ;
			else
				CheckMenuItem(pContextMenu, ID_ORPHANSAUDIO, MF_UNCHECKED) ;

			GetPrivateProfileString("Options", "OrphansVisual", "1", szSaveOption, 800, "emDEX.ini") ;
			g_bOrphanVisual = atoi(LPCTSTR(szSaveOption)) ;

			if (g_bOrphanVisual == TRUE)
				CheckMenuItem(pContextMenu, ID_ORPHANSVISUAL, MF_CHECKED) ;
			else
				CheckMenuItem(pContextMenu, ID_ORPHANSVISUAL, MF_UNCHECKED) ;

			GetPrivateProfileString("Options", "RealTimeUpdates", "1", szSaveOption, 800, "emDEX.ini") ;
			g_bRealTimeUpdates = atoi(LPCTSTR(szSaveOption)) ;

			if (g_bRealTimeUpdates == TRUE)
			{
				CheckMenuItem(pContextMenu, ID_REALTIMEUPDATE, MF_CHECKED) ;
				EnableMenuItem(GetMenu(dlg), ID_UPDATEWINDOW, MF_GRAYED) ;
			}
			else
			{
				CheckMenuItem(pContextMenu, ID_REALTIMEUPDATE, MF_UNCHECKED) ;
				EnableMenuItem(GetMenu(dlg), ID_UPDATEWINDOW, MF_ENABLED) ;
			}

			GetPrivateProfileString("Options", "AlwaysOnTop", "1", szSaveOption, 800, "emDEX.ini") ;
			g_bAlwaysOnTop = atoi(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "SortOption", "1", szSaveOption, 800, "emDEX.ini") ;
			g_iSortOption = atoi(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "UseEditor", "1", szSaveOption, 800, "emDEX.ini") ;
			g_bUseEditor = atoi(LPCTSTR(szSaveOption)) ;

			if (g_iSortOption == SORT_WORD_BY_WORD)
			{
				CheckMenuItem(pContextMenu, ID_SORT_WORDBYWORD, MF_CHECKED) ;
				CheckMenuItem(pContextMenu, ID_SORT_LETTERBYLETTER, MF_UNCHECKED) ;
			}
			else
			{
				CheckMenuItem(pContextMenu, ID_SORT_LETTERBYLETTER, MF_CHECKED) ;
				CheckMenuItem(pContextMenu, ID_SORT_WORDBYWORD, MF_UNCHECKED) ;
			}

			GetPrivateProfileString("Options", "SeparatorChars", "-", szSaveOption, 800, "emDEX.ini") ;
			strcpy(szSeparatorChars, szSaveOption) ;

			if (szSeparatorChars[0] == '\0')
				strcpy(szSeparatorChars, "-") ;

			szIgnoreChars[0] = '-' ;
			szIgnoreChars[1] = '\0' ;

			GetPrivateProfileString("Options", "IgnoreChars", "", szSaveOption, 800, "emDEX.ini") ;
			strcat(szIgnoreChars, szSaveOption) ;
			
			if (strlen(g_FileName) == 0)
			{
				strcpy(g_szWindowTitle, "emDEX") ;
				
				/* This fixes window flashing when starting up. */
				SetWindowPos(dlg, NULL, 10, 10, 0, 0, SWP_NOREDRAW) ;
				ShowWindow(dlg, SW_HIDE) ;
			}
			else
			{
				sprintf(szTemp, "emDEX - %s", g_FileName) ;
				strcpy(g_szWindowTitle, szTemp) ;
			}

//			InitializeCriticalSection(&Critical_Section) ;

//			SetTimer(dlg, 1, 3000, NULL);

//			SendMessage(dlg, WM_TIMER, 0, 0);

			if (VERSION == DEMO)
				itemLimit = 100 ;
			else
				itemLimit = 5000 ;

			// Set the initial size of the window.
			GetPrivateProfileString("Options", "XPos", "10", szSaveOption, 800, "emDEX.ini") ;
			XPos = atol(LPCTSTR(szSaveOption)) ;
			GetPrivateProfileString("Options", "YPos", "10", szSaveOption, 800, "emDEX.ini") ;
			YPos = atol(LPCTSTR(szSaveOption)) ;
			GetPrivateProfileString("Options", "XSize", "400", szSaveOption, 800, "emDEX.ini") ;
			XSize = atol(LPCTSTR(szSaveOption)) ;
			GetPrivateProfileString("Options", "YSize", "300", szSaveOption, 800, "emDEX.ini") ;
			YSize = atol(LPCTSTR(szSaveOption)) ;

			if (strlen(g_FileName) != 0)
				SetWindowPos(dlg, HWND_TOP, XPos, YPos, XSize, YSize, SWP_SHOWWINDOW) ;

			if (g_bAlwaysOnTop == TRUE)
			{
				CheckMenuItem(pContextMenu, ID_ALWAYSONTOP, MF_CHECKED) ;
				SetWindowPos(dlg, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE) ;
			}
			else
			{
				CheckMenuItem(pContextMenu, ID_ALWAYSONTOP, MF_UNCHECKED) ;
				SetWindowPos(dlg, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE) ;
			}

			if (g_bUseEditor == TRUE)
				CheckMenuItem(pContextMenu, ID_USEEDITOR, MF_CHECKED) ;
			else
				CheckMenuItem(pContextMenu, ID_USEEDITOR, MF_UNCHECKED) ;

			/* Form the window title from the argument passed. */
			SetWindowText(dlg, g_szWindowTitle) ;

			GetClientRect(dlg, &pRect) ;

			LBox = GetDlgItem(dlg, IDC_PSINDEXLIST) ;

			fontHeight = MAKELPARAM((-MulDiv(lf.lfHeight, GetDeviceCaps(GetDC(dlg), LOGPIXELSY), 72) - 1), 0) ;

			// Set the item height depending on the font.
			SendMessage(LBox, LB_SETITEMHEIGHT, 0, fontHeight) ;

			fontHeight = (INT)((FLOAT)fontHeight*0.50) ;

			/* Size the list box to fill the window. */
			if (LBox)
				SetWindowPos(LBox, HWND_TOP, 0, 0, 
										pRect.right - pRect.left, pRect.bottom - pRect.top, SWP_NOZORDER) ;

			// Set the dialog icon.
			winIcon = (HICON)LoadImage(appInstance, "IDI_PSINDEXCATCHER", IMAGE_ICON, 16, 16, LR_DEFAULTSIZE) ;

			SendMessage(dlg, WM_SETICON, ICON_BIG, (LPARAM)winIcon) ;

			if (strlen(g_FileName) == 0)
			{
				/* Add the window to the tray. */
				memset(&tnib, 0, sizeof(NOTIFYICONDATA));
				tnib.cbSize           = sizeof(NOTIFYICONDATA); 
				tnib.uFlags           = NIF_ICON | NIF_TIP | NIF_MESSAGE;
				tnib.uID    = UDLGID ;
				tnib.hWnd = dlg;
				tnib.hIcon = winIcon;
				strcpy(tnib.szTip, "emDEX server");
				tnib.uCallbackMessage    = WM_USER+200;
				Shell_NotifyIcon(NIM_ADD, &tnib) ;
			}

			// Get the installation directory from the registry
			RegValueSize = _MAX_PATH * 2 ;

			lRetCode = RegQueryValue(HKEY_LOCAL_MACHINE, "SOFTWARE\\emDEX\\Directory", executePath, &RegValueSize) ;

			strcat(executePath, "\\emDEX.exe") ;

			bAboutBoxDisplayed = FALSE ;

			return TRUE;

		case WM_PAINT :
			if (strcmp(g_szWindowTitle, "emDEX") == 0)
				ShowWindow(dlg, SW_HIDE) ;

			break ;

		case WM_TIMER :
//			_beginthread(startThreads, 16384, (PVOID)NULL) ;
			break ;

		case WM_CTLCOLORLISTBOX:
			// Set the listbox background colour.
			LOGBRUSH LogBrush;        
			LogBrush.lbStyle = BS_SOLID;       
			LogBrush.lbColor = chColor;       
			LogBrush.lbHatch = (LONG) NULL;        
			return( (LPARAM) CreateBrushIndirect( &LogBrush ) ); 

		case WM_DRAWITEM: 
            lpDIS = (LPDRAWITEMSTRUCT)lParam; 

/*			if (lpDIS->itemID > itemLimit)
				break ; */

			if ((szAddString[lpDIS->itemID][0] == '\0') &&
				(strcmp(szPageString[lpDIS->itemID], szSeparatorChars) == 0))
			{
				// This takes care of lpDIS->itemID being way out of range.
				break ;
			}

			SelectObject(lpDIS->hDC, defaultFont) ;

			// Set the text and text background colour.
			SetBkColor(lpDIS->hDC, chColor); 
			SetTextColor(lpDIS->hDC, g_rgbText);

			if (lpDIS->itemState & ODS_SELECTED)
			{
				SetBkColor (lpDIS->hDC, GetSysColor(COLOR_HIGHLIGHT));     
				SetTextColor (lpDIS->hDC, GetSysColor(COLOR_HIGHLIGHTTEXT)); 
			}

			sze.cx = 0 ;
			formatOffset = 0 ;
			currentFormat = DEFAULT ;

			// Draw the item text one character at a time to cope with formatting.
			for (i = 0 ; i < (INT)strlen(szAddString[lpDIS->itemID]) ; i++)
			{
				while ((szAddString[lpDIS->itemID] [i] == '<') && (szBracketsDisplayed[lpDIS->itemID] [i] == 0))
				{
					if (strncmp(&szAddString[lpDIS->itemID] [i], "<Bold>", 6) == 0)
					{
						if ((currentFormat == ITALIC) || (currentFormat == BOLDITALIC))
						{
							SelectObject(lpDIS->hDC, bolditalicFont) ;
							i += 6 ;
							currentFormat = BOLDITALIC ;
						}
						else
						{
							SelectObject(lpDIS->hDC, boldFont) ;
							i += 6 ;
							currentFormat = BOLD ;
						}
					}
					else
					{
						if (strncmp(&szAddString[lpDIS->itemID] [i], "<Emphasis>", 10) == 0)
						{
							if ((currentFormat == BOLD) || (currentFormat == BOLDITALIC))
							{
								SelectObject(lpDIS->hDC, bolditalicFont) ;
								i += 10 ;
								currentFormat = BOLDITALIC ;
							}
							else
							{
								SelectObject(lpDIS->hDC, italicFont) ;
								i += 10 ;
								currentFormat = ITALIC ;
							}
						}
						else
						{
							if (strnicmp(&szAddString[lpDIS->itemID] [i], "<default para font>", 19) == 0)
							{
								SelectObject(lpDIS->hDC, defaultFont) ;
								i += 19 ;
								currentFormat = DEFAULT ;
							}
							else
							{
								if (strnicmp(&szAddString[lpDIS->itemID] [i], "</>", 3) == 0)
								{
									SelectObject(lpDIS->hDC, defaultFont) ;
									i += 3 ;
									currentFormat = DEFAULT ;
								}
								else
								{
									// Ignore all other formatting.
									for ( ; i < (INT)strlen(szAddString[lpDIS->itemID]) ; i++)
									{
										if (szAddString[lpDIS->itemID] [i] == '>')
										{
											i++ ;
											break ;
										}
									}
								}
							}
						}
					}
				}

				while ((szAddString[lpDIS->itemID] [i] == '[') && ((szBracketsDisplayed[lpDIS->itemID] [i] == 0)))
				{
					for ( ; i < (INT)strlen(szAddString[lpDIS->itemID]) ; i++)
					{
						if (szAddString[lpDIS->itemID] [i] == ']')
						{
							i++ ;
							break ;
						}
					}
				}

				if (i >= (INT)strlen(szAddString[lpDIS->itemID]))
					break ;

				szTemp[0] = szAddString[lpDIS->itemID] [i] ;
				szTemp[1] = '\0' ;

				TextOut(lpDIS->hDC, lpDIS->rcItem.left + formatOffset, lpDIS->rcItem.top, 
						szTemp, strlen(szTemp)) ;

				GetTextExtentPoint(lpDIS->hDC, szTemp, 1, &sze) ;
				formatOffset += sze.cx ;
			}

			formatCount = 0 ;
			currentFormat = DEFAULT ;

			// Now draw the page numbers.
			for (i = 0 ; i < (INT)strlen(szPageString[lpDIS->itemID]) ; i++)
			{
				if ((szPageString[lpDIS->itemID][i] == ',') || /*(szPageString[lpDIS->itemID][i] == '-') ||*/
					(strncmp(&szPageString[lpDIS->itemID][i], szSeparatorChars, strlen(szSeparatorChars)) == 0) ||
					(szPageString[lpDIS->itemID][i] == '?') || (szPageString[lpDIS->itemID][i] == ' '))
				{
					if ((szPageString[lpDIS->itemID][i] == ',') || 
/*						((szPageString[lpDIS->itemID][i] == '-') && 
						((szPageString[lpDIS->itemID][i-1] != '?') && (szPageString[lpDIS->itemID][i+1] != '?')))) */
						((strncmp(&szPageString[lpDIS->itemID][i], szSeparatorChars, strlen(szSeparatorChars)) == 0) &&
						((szPageString[lpDIS->itemID][i-strlen(szSeparatorChars)] != '?') && (szPageString[lpDIS->itemID][i+strlen(szSeparatorChars)] != '?'))))
					{
						formatCount++ ;
					}

					SelectObject(lpDIS->hDC, defaultFont) ;
				}
				else
				{
					switch (iPageFormat[lpDIS->itemID] [formatCount])
					{
						case DEFAULT :
							SelectObject(lpDIS->hDC, defaultFont) ;
							currentFormat = DEFAULT ;
							break ;

						case BOLD :
							SelectObject(lpDIS->hDC, boldFont) ;
							currentFormat = BOLD ;
							break ;

						case ITALIC :
							SelectObject(lpDIS->hDC, italicFont) ;
							currentFormat = ITALIC ;
							break ;

						case BOLDITALIC :
							SelectObject(lpDIS->hDC, bolditalicFont) ;
							currentFormat = BOLDITALIC ;
							break ;

						default :
							SelectObject(lpDIS->hDC, defaultFont) ;
							currentFormat = DEFAULT ;
							break ;
					}
				}

				szTemp[0] = szPageString[lpDIS->itemID][i] ;
				szTemp[1] = '\0' ;

				TextOut(lpDIS->hDC, lpDIS->rcItem.left + formatOffset, lpDIS->rcItem.top, 
						szTemp, strlen(szTemp)) ;

				GetTextExtentPoint32(lpDIS->hDC, szTemp, 1, &sze) ;
				formatOffset += sze.cx ;
			}

			if (lpDIS->itemAction & ODA_FOCUS)         
				DrawFocusRect(lpDIS->hDC, &lpDIS->rcItem);

			break; 

		case WM_SIZE:
			GetClientRect(dlg, &pRect) ;

			/* Size the list box to fill the window. */
			if (LBox)
				SetWindowPos(LBox, HWND_TOP, 0, 0, 
										pRect.right - pRect.left, pRect.bottom - pRect.top, SWP_NOZORDER) ;

			break ;

		case WM_CLOSE :
			if (strlen(g_FileName) == 0)
			{
				/* Remove the icon from the tray. */
				memset(&tnib, 0, sizeof(NOTIFYICONDATA));
				tnib.cbSize           = sizeof(NOTIFYICONDATA); 
				tnib.uFlags           = NIF_ICON | NIF_TIP | NIF_MESSAGE;
				tnib.uID    = UDLGID ;
				tnib.hWnd = dlg;
				tnib.hIcon = winIcon;
				strcpy(tnib.szTip, "emDEX server");
				tnib.uCallbackMessage    = WM_USER+200;
				Shell_NotifyIcon(NIM_DELETE, &tnib) ;
			}

			EndDialog(hAboutBox,TRUE);
			EndDialog(hFindWindow,TRUE);
			EndDialog(hReplaceWindow,TRUE);
			EndDialog(hIgnoreWindow,TRUE);
			EndDialog(hSeparatorWindow,TRUE);
			EndDialog(hButtonWindow,TRUE);
			EndDialog(dlg,TRUE);
			break ;

		case WM_VKEYTOITEM :
			if (LOWORD(wParam) == VK_DELETE)
			{
				if (isProcessing == TRUE)
					return (-1) ;
					
				isProcessing = TRUE ;

				sendDDEMessage(dlg, ITEMDELETE) ;

				selected = SendMessage(LBox, LB_GETCURSEL, 0, 0) ;
				sendDDEMessage(dlg, ITEMFINDPAGE, itemList[selected].itemPage[0], itemList[selected].itemReference[0]) ;

				isProcessing = FALSE ;
			}

			if (LOWORD(wParam) == VK_F5)
			{
				if (isProcessing == TRUE)
					return (-1) ;
					
				isProcessing = TRUE ;

				if (g_bRealTimeUpdates == FALSE)
					sendDDEMessage(dlg, DOITEMUPDATE) ;

/*				SetFocus(dlg) ;
				SetForegroundWindow(dlg); */

				isProcessing = FALSE ;
			}

			return (-1) ;

		case DM_GETDEFID:
			// Cope with the enter key pressed from the listbox.
			if (0x8000 & GetKeyState(VK_RETURN))
			{
				if (GetFocus() == LBox)
				{
					iCount = 0 ;

					while ((isProcessing == TRUE) && (iCount < 50))
					{
						Sleep(10) ;
						iCount++ ;
					}

					if (isProcessing == TRUE)
						return (-1) ;
				
					isProcessing = TRUE ;

					sendDDEMessage(dlg, ITEMEDIT) ;

					isProcessing = FALSE ;

					return TRUE;
				}
			}

            break;

		case WM_COMMAND:
			switch (HIWORD(wParam))
			{
				case LBN_SELCHANGE :
					if (isProcessing == TRUE)
						return (-1) ;
						
					isProcessing = TRUE ;

					selected = SendMessage(LBox, LB_GETCURSEL, 0, 0) ;

					// Find the first occurrence
					sendDDEMessage(dlg, ITEMFINDPAGE, itemList[selected].itemPage[0], itemList[selected].itemReference[0]) ;

					isProcessing = FALSE ;

					// Make sure that the catcher retains the focus.
/*					SetFocus(dlg) ;
					SetForegroundWindow(dlg) ; */

					break ;

				case LBN_DBLCLK :
					iCount = 0 ;

					while ((isProcessing == TRUE) && (iCount < 50))
					{
						Sleep(10) ;
						iCount++ ;
					}

					if (isProcessing == TRUE)
						return (-1) ;
					
					isProcessing = TRUE ;

					// Find the first occurrence
//					sendDDEMessage(dlg, ITEMFINDPAGE, itemList[selected].itemPage[0]) ;
					sendDDEMessage(dlg, ITEMEDIT) ;

					isProcessing = FALSE ;

					break ;

				case ID_EXIT :
					SendMessage(dlg, WM_CLOSE, 0, 0) ;
					break ;

				default :
					break ;
			}

			switch (wParam)
			{
				case ID_MARKERFIND :
					isProcessing = TRUE ;
					DialogBox(appInstance, MAKEINTRESOURCE(IDD_FIND), dlg, (DLGPROC)FindProc) ; 
					isProcessing = FALSE ;

					break ;

				case ID_MARKERREPLACE :
					isProcessing = TRUE ;
					DialogBox(appInstance, MAKEINTRESOURCE(IDD_REPLACE), dlg, (DLGPROC)ReplaceProc) ; 
					isProcessing = FALSE ;

					break ;

				case ID_MARKERDELETE :
					if (isProcessing == TRUE)
						return (-1) ;

					isProcessing = TRUE ;

					sendDDEMessage(dlg, ITEMDELETE) ;

					selected = SendMessage(LBox, LB_GETCURSEL, 0, 0) ;
					sendDDEMessage(dlg, ITEMFINDPAGE, itemList[selected].itemPage[0], itemList[selected].itemReference[0]) ;

					isProcessing = FALSE ;

					break ;

				case ID_MARKEREDIT :
					iCount = 0 ;

					while ((isProcessing == TRUE) && (iCount < 50))
					{
						Sleep(10) ;
						iCount++ ;
					}

					if (isProcessing == TRUE)
						return (-1) ;
				
					isProcessing = TRUE ;

					sendDDEMessage(dlg, ITEMEDIT) ;

					isProcessing = FALSE ;

					break ;

				case ID_FINDORPHANS :
					isProcessing = TRUE ;

					iCount = SendMessage(LBox, LB_GETCOUNT, 0, 0) ;
					iStart = SendMessage(LBox, LB_GETCURSEL, 0, 0) ;

					if (iCount == 0)
					{
						MessageBox(dlg, "No orphan ranges found.", "emDEX", MB_OK) ;
						isProcessing = FALSE ;
						break ;
					}

					if (iStart == (iCount-1))
						iStart = -1 ;

					// Find any orphan range text.
					for (i = (iStart+1) ; i != iStart ; i++)
					{
						if (strstr(szPageString[i], "??") != NULL)
						{
							SendMessage(LBox, LB_SETCURSEL, i, 0) ;

							for (j = 0 ; itemList[i].itemPage[j] != -1 ; j++)
							{
								if (iRange[i][j] == RANGEEND)
									break ;

								if (iRange[i][j] == RANGESTART)
								{
									if (iRange[i][j+1] != RANGEEND)
									{
										break ;
									}
									else
									{
										j++ ;
									}
								}
							}

							sendDDEMessage(dlg, ITEMFINDPAGE, itemList[i].itemPage[j], itemList[i].itemReference[j]) ;

							isProcessing = FALSE ;
							return (-1) ;
						}

						if (i == (iCount-1))
							i = -1 ;

						if (i == iStart)
							break ;
					}

					if (strstr(szPageString[i], "??") != NULL)
					{
						// Set the current selection and find the marker in the document.
						SendMessage(LBox, LB_SETCURSEL, i, 0) ;

						for (j = 0 ; itemList[i].itemPage[j] != -1 ; j++)
						{
							if (iRange[i][j] == RANGEEND)
								break ;

							if (iRange[i][j] == RANGESTART)
							{
								if (iRange[i][j+1] != RANGEEND)
								{
									break ;
								}
								else
								{
									j++ ;
								}
							}
						}

						sendDDEMessage(dlg, ITEMFINDPAGE, itemList[i].itemPage[j], itemList[i].itemReference[j]) ;

						isProcessing = FALSE ;
						return (-1) ;
					}

					MessageBox(dlg, "No orphan ranges found.", "emDEX", MB_OK) ;

					isProcessing = FALSE ;

					break ;

				case ID_CHECKCROSSREFERENCES :
					isProcessing = TRUE ;
					iCount = SendMessage(LBox, LB_GETCOUNT, 0, 0) ;
					iStart = SendMessage(LBox, LB_GETCURSEL, 0, 0) ;

					if (iCount == 0)
					{
						MessageBox(dlg, "Cross-references are OK", "emDEX", MB_OK) ;
						isProcessing = FALSE ;
						break ;
					}

					if (iStart == (iCount-1))
						iStart = -1 ;

					// Find any cross-references.
					for (i = (iStart+1) ; ; i++)
					{
						szTempString1[0] = '\0' ;

						getUnformattedText(szAddString[i], szTempString2) ;
						stripSpaces(szTempString2) ;

						if (strnicmp(szTempString2, "see also", 8) == 0)
						{
							strcpy(szTempString1, (CHAR *)&szTempString2+8) ;
						}
						else
						{
							if (strnicmp(szTempString2, "see", 3) == 0)
							{
								strcpy(szTempString1, (CHAR *)&szTempString2+3) ;
							}
							else
							{
								if ((temp = strstr(strupr(szTempString2), "SEE ALSO")) != NULL)
								{
									strcpy(szTempString1, temp+8) ;
								}
								else
								{
									if ((temp = strstr(strupr(szTempString2), "SEE")) != NULL)
									{
										strcpy(szTempString1, temp+3) ;
									}
								}
							}
						}

						if (szTempString1[0] != '\0')
						{
							// Cross reference found, now try to find it.
							anyFound = TRUE ;

							stripSpaces(szTempString1) ;
	
							for (j = 0 ; j < iCount ; j++)
							{
								getUnformattedText(szAddString[j], szTempString2) ;
								stripSpaces(szTempString2) ;

								if ((stricmp(szTempString2, szTempString1) == 0) && (j != i))
									break ;
							}

							if (j >= iCount)
							{
								SendMessage(LBox, LB_SETCURSEL, i, 0) ;

								sendDDEMessage(LBox, ITEMFIND) ;

								isProcessing = FALSE ;
								return (-1) ;
							}
						}

						if (i == iCount)
							i = -1 ;

						if (i == iStart)
							break ;
					}

					if (anyFound == TRUE)
					{
						MessageBox(dlg, "Cross-references are OK", "emDEX", MB_OK) ;
					}
					else
					{
						MessageBox(dlg, "No cross-references found", "emDEX", MB_OK) ;
					}

					isProcessing = FALSE ;

					break ;

				case ID_UPDATEWINDOW :
					isProcessing = TRUE ;

					if (g_bRealTimeUpdates == FALSE)
						sendDDEMessage(dlg, DOITEMUPDATE) ;

					isProcessing = FALSE ;

					break ;

				case ID_SAVEWINDOWSIZE :
					isProcessing = TRUE ;

					// Save the window size and position.
					GetWindowRect(dlg, &windowRect) ;

					ltoa(windowRect.left, szSaveOption, 10) ;
					WritePrivateProfileString("Options", "XPos", szSaveOption, "emDEX.ini");
					ltoa(windowRect.top, szSaveOption, 10) ;
					WritePrivateProfileString("Options", "YPos", szSaveOption, "emDEX.ini");
					ltoa(windowRect.right - windowRect.left, szSaveOption, 10) ;
					WritePrivateProfileString("Options", "XSize", szSaveOption, "emDEX.ini");
					ltoa(windowRect.bottom - windowRect.top, szSaveOption, 10) ;
					WritePrivateProfileString("Options", "YSize", szSaveOption, "emDEX.ini");

					isProcessing = FALSE ;

					break ;

				case ID_MARKEREXIT :
					EndDialog(dlg, 0) ;
					break ;

				case ID_PRINTMARKERS :
                    ZeroMemory(&pd, sizeof(PRINTDLG)); 

                    // initialize PRINTDLG 
                    pd.lStructSize = sizeof(PRINTDLG); 
                    pd.hwndOwner = dlg; 
                    pd.nCopies = 1; 
                    pd.nFromPage = 0xFFFF; 
                    pd.nToPage = 0xFFFF; 
                    pd.nMinPage = 1; 
                    pd.nMaxPage = 0xFFFF; 
                    pd.Flags = PD_ENABLEPRINTHOOK; 
                    pd.lpfnPrintHook = PrintHookProc; 

                    if (PrintDlg(&pd)) 
                    { 
                        // do some printing... 
                         
                        if (pd.hDevMode) GlobalFree(pd.hDevMode); 
                        if (pd.hDevNames) GlobalFree(pd.hDevNames); 
                    } 
					
					break ;
  
				case ID_CHANGEFONT :
					GetObject(defaultFont, sizeof(LOGFONT), &lf);

                    cf.Flags = CF_EFFECTS | CF_INITTOLOGFONTSTRUCT | CF_SCREENFONTS;
                    cf.hwndOwner = GetDesktopWindow() ;
                    cf.lpLogFont = &lf;
                    cf.rgbColors = g_rgbText;

                    if(ChooseFont(&cf))
                    {
                        HFONT hf = CreateFontIndirect(&lf);
                        if(hf)
                        {
                            defaultFont = hf;
                        }

                        g_rgbText = cf.rgbColors;

						/* Save the options. */
						ltoa(lf.lfHeight, szSaveOption, 10) ;
						WritePrivateProfileString("Options", "LfHeight", szSaveOption, "emDEX.ini");

						ltoa(lf.lfWidth, szSaveOption, 10) ;
						WritePrivateProfileString("Options", "LfWidth", szSaveOption, "emDEX.ini");

						ltoa(lf.lfEscapement, szSaveOption, 10) ;
						WritePrivateProfileString("Options", "LfEscapement", szSaveOption, "emDEX.ini");

						ltoa(lf.lfOrientation, szSaveOption, 10) ;
						WritePrivateProfileString("Options", "LfOrientation", szSaveOption, "emDEX.ini");

						ltoa(lf.lfWeight, szSaveOption, 10) ;
						WritePrivateProfileString("Options", "LfWeight", szSaveOption, "emDEX.ini");

						ltoa(lf.lfItalic, szSaveOption, 10) ;
						WritePrivateProfileString("Options", "LfItalic", szSaveOption, "emDEX.ini");

						ltoa(lf.lfUnderline, szSaveOption, 10) ;
						WritePrivateProfileString("Options", "LfUnderline", szSaveOption, "emDEX.ini");

						ltoa(lf.lfStrikeOut, szSaveOption, 10) ;
						WritePrivateProfileString("Options", "LfStrikeOut", szSaveOption, "emDEX.ini");

						ltoa(lf.lfCharSet, szSaveOption, 10) ;
						WritePrivateProfileString("Options", "LfCharSet", szSaveOption, "emDEX.ini");

						ltoa(lf.lfOutPrecision, szSaveOption, 10) ;
						WritePrivateProfileString("Options", "LfOutPrecision", szSaveOption, "emDEX.ini");

						ltoa(lf.lfQuality, szSaveOption, 10) ;
						WritePrivateProfileString("Options", "LfQuality", szSaveOption, "emDEX.ini");

						ltoa(lf.lfPitchAndFamily, szSaveOption, 10) ;
						WritePrivateProfileString("Options", "LfPitchAndFamily", szSaveOption, "emDEX.ini");

						WritePrivateProfileString("Options", "LfFaceName", lf.lfFaceName, "emDEX.ini");

						ltoa(g_rgbText, szSaveOption, 10) ;
						WritePrivateProfileString("Options", "FontColor", szSaveOption, "emDEX.ini");

						ltoa(chColor, szSaveOption, 10) ;
						WritePrivateProfileString("Options", "Color", szSaveOption, "emDEX.ini");

						nextWindow = GetWindow(GetDesktopWindow(), GW_CHILD) ;

						// Send the font change message to all catcher windows.
						while(nextWindow != NULL)
						{
							GetWindowText(nextWindow, szTempString1, 80) ;

							if (strncmp(szTempString1, "emDEX - ", 8) == 0)
								SendMessage(nextWindow, CHANGEFONT, (WPARAM)defaultFont, 0) ;

							nextWindow = GetNextWindow(nextWindow, GW_HWNDNEXT) ;
						}
                    }
					
					break ;

				case ID_CHANGBACKGROUNDCOLOR :
					cc.Flags = CC_SOLIDCOLOR | CC_RGBINIT | CC_PREVENTFULLOPEN ;
					cc.rgbResult = chColor ;
					cc.lpCustColors = (LPDWORD) acrCustClr;
					cc.hwndOwner = GetDesktopWindow() ;
					cc.lStructSize = sizeof(CHOOSECOLOR) ;

					ChooseColor(&cc) ;

					chColor = cc.rgbResult ;

					ltoa(chColor, szSaveOption, 10) ;
					WritePrivateProfileString("Options", "Color", szSaveOption, "emDEX.ini");

					nextWindow = GetWindow(GetDesktopWindow(), GW_CHILD) ;

					// Send the font change message to all catcher windows.
					while(nextWindow != NULL)
					{
						GetWindowText(nextWindow, szTempString1, 80) ;

						if (strncmp(szTempString1, "emDEX - ", 8) == 0)
							SendMessage(nextWindow, CHANGEFONT, (WPARAM)defaultFont, 0) ;

						nextWindow = GetNextWindow(nextWindow, GW_HWNDNEXT) ;
					}

					break ;

				case ID_ORPHANSAUDIO :
					if (GetMenuState(pContextMenu, ID_ORPHANSAUDIO, MF_BYCOMMAND) == MF_UNCHECKED)
					{
						g_bOrphanAudio = TRUE ;
						CheckMenuItem(pContextMenu, ID_ORPHANSAUDIO, MF_CHECKED) ;
					}
					else
					{
						g_bOrphanAudio = FALSE ;
						CheckMenuItem(pContextMenu, ID_ORPHANSAUDIO, MF_UNCHECKED) ;
					}

					itoa(g_bOrphanAudio, szSaveOption, 10) ;
					WritePrivateProfileString("Options", "OrphansAudio", szSaveOption, "emDEX.ini");

					// Tell all catcher windows that an option has changed.
					nextWindow = GetWindow(GetDesktopWindow(), GW_CHILD) ;

					// Send the font change message to all catcher windows.
					while(nextWindow != NULL)
					{
						GetWindowText(nextWindow, szTempString1, 80) ;

						if (strncmp(szTempString1, "emDEX - ", 8) == 0)
							SendMessage(nextWindow, CHANGEOPTION, 0, 0) ;

						nextWindow = GetNextWindow(nextWindow, GW_HWNDNEXT) ;
					}

					break ;

				case ID_ORPHANSVISUAL :
					if (GetMenuState(pContextMenu, ID_ORPHANSVISUAL, MF_BYCOMMAND) == MF_UNCHECKED)
					{
						g_bOrphanVisual = TRUE ;
						CheckMenuItem(pContextMenu, ID_ORPHANSVISUAL, MF_CHECKED) ;
					}
					else
					{
						g_bOrphanVisual = FALSE ;
						CheckMenuItem(pContextMenu, ID_ORPHANSVISUAL, MF_UNCHECKED) ;
					}

					itoa(g_bOrphanVisual, szSaveOption, 10) ;
					WritePrivateProfileString("Options", "OrphansVisual", szSaveOption, "emDEX.ini");

					// Tell all catcher windows that an option has changed.
					nextWindow = GetWindow(GetDesktopWindow(), GW_CHILD) ;

					// Send the font change message to all catcher windows.
					while(nextWindow != NULL)
					{
						GetWindowText(nextWindow, szTempString1, 80) ;

						if (strncmp(szTempString1, "emDEX - ", 8) == 0)
							SendMessage(nextWindow, CHANGEOPTION, 0, 0) ;

						nextWindow = GetNextWindow(nextWindow, GW_HWNDNEXT) ;
					}

					break ;

				case ID_REALTIMEUPDATE :
					if (GetMenuState(pContextMenu, ID_REALTIMEUPDATE, MF_BYCOMMAND) == MF_UNCHECKED)
					{
						g_bRealTimeUpdates = TRUE ;
						CheckMenuItem(pContextMenu, ID_REALTIMEUPDATE, MF_CHECKED) ;
					}
					else
					{
						g_bRealTimeUpdates = FALSE ;
						CheckMenuItem(pContextMenu, ID_REALTIMEUPDATE, MF_UNCHECKED) ;
					}

					itoa(g_bRealTimeUpdates, szSaveOption, 10) ;
					WritePrivateProfileString("Options", "RealTimeUpdates", szSaveOption, "emDEX.ini");

					// Tell all catcher windows that an option has changed.
					nextWindow = GetWindow(GetDesktopWindow(), GW_CHILD) ;

					// Send the font change message to all catcher windows.
					while(nextWindow != NULL)
					{
						GetWindowText(nextWindow, szTempString1, 80) ;

						if (strncmp(szTempString1, "emDEX - ", 8) == 0)
							SendMessage(nextWindow, CHANGEOPTION, 0, 0) ;

						nextWindow = GetNextWindow(nextWindow, GW_HWNDNEXT) ;
					}

					break ;

				case ID_ALWAYSONTOP :
					if (GetMenuState(pContextMenu, ID_ALWAYSONTOP, MF_BYCOMMAND) == MF_UNCHECKED)
					{
						g_bAlwaysOnTop = TRUE ;
						CheckMenuItem(pContextMenu, ID_ALWAYSONTOP, MF_CHECKED) ;
					}
					else
					{
						g_bAlwaysOnTop = FALSE ;
						CheckMenuItem(pContextMenu, ID_ALWAYSONTOP, MF_UNCHECKED) ;
					}

					itoa(g_bAlwaysOnTop, szSaveOption, 10) ;
					WritePrivateProfileString("Options", "AlwaysOnTop", szSaveOption, "emDEX.ini");

					// Tell all catcher windows that an option has changed.
					nextWindow = GetWindow(GetDesktopWindow(), GW_CHILD) ;

					while(nextWindow != NULL)
					{
						GetWindowText(nextWindow, szTempString1, 80) ;

						if (strncmp(szTempString1, "emDEX - ", 8) == 0)
							SendMessage(nextWindow, CHANGEOPTION, 0, 0) ;

						nextWindow = GetNextWindow(nextWindow, GW_HWNDNEXT) ;
					}

					for (i = 0 ; i < 80 ; i++)
						hwndWinPos[i] = NULL ;

					i = 0 ;

					// Now set the window positioning.
					nextWindow = GetWindow(GetDesktopWindow(), GW_CHILD) ;

					while(nextWindow != NULL)
					{
						GetWindowText(nextWindow, szTempString1, 80) ;

						if (strncmp(szTempString1, "emDEX - ", 8) == 0)
						{
							// Make sure that this window hasn't already been processed
							for (j = 0 ; j < 80 ; j++)
							{
								if (hwndWinPos[j] == NULL)
									break ;

								if (hwndWinPos[j] == nextWindow)
									break ;
							}

							if (hwndWinPos[j] != nextWindow)
							{
								SendMessage(nextWindow, SETPOS, 0, 0) ;
								hwndWinPos[i] = nextWindow ;
								i++ ;

								// Start again - window positioning has changed.
								nextWindow = GetWindow(GetDesktopWindow(), GW_CHILD) ;
							}
							else
							{
								nextWindow = GetNextWindow(nextWindow, GW_HWNDNEXT) ;
							}
						}
						else
						{
							nextWindow = GetNextWindow(nextWindow, GW_HWNDNEXT) ;
						}
					}

					break ;

				case ID_USEEDITOR :
					if (GetMenuState(pContextMenu, ID_USEEDITOR, MF_BYCOMMAND) == MF_UNCHECKED)
					{
						g_bUseEditor = TRUE ;
						CheckMenuItem(pContextMenu, ID_USEEDITOR, MF_CHECKED) ;
					}
					else
					{
						g_bUseEditor = FALSE ;
						CheckMenuItem(pContextMenu, ID_USEEDITOR, MF_UNCHECKED) ;
					}

					itoa(g_bUseEditor, szSaveOption, 10) ;
					WritePrivateProfileString("Options", "UseEditor", szSaveOption, "emDEX.ini");

					// Tell all catcher windows that an option has changed.
					nextWindow = GetWindow(GetDesktopWindow(), GW_CHILD) ;

					while(nextWindow != NULL)
					{
						GetWindowText(nextWindow, szTempString1, 80) ;

						if (strncmp(szTempString1, "emDEX - ", 8) == 0)
							SendMessage(nextWindow, CHANGEOPTION, 0, 0) ;

						nextWindow = GetNextWindow(nextWindow, GW_HWNDNEXT) ;
					}

					break ;

				case ID_SORT_WORDBYWORD :
					g_iSortOption = SORT_WORD_BY_WORD ;
					CheckMenuItem(pContextMenu, ID_SORT_WORDBYWORD, MF_CHECKED) ;
					CheckMenuItem(pContextMenu, ID_SORT_LETTERBYLETTER, MF_UNCHECKED) ;

					itoa(g_iSortOption, szSaveOption, 10) ;
					WritePrivateProfileString("Options", "SortOption", szSaveOption, "emDEX.ini");

					// Tell all catcher windows that an option has changed.
					nextWindow = GetWindow(GetDesktopWindow(), GW_CHILD) ;

					// Send the font change message to all catcher windows.
					while(nextWindow != NULL)
					{
						GetWindowText(nextWindow, szTempString1, 80) ;

						if (strncmp(szTempString1, "emDEX - ", 8) == 0)
							SendMessage(nextWindow, CHANGEOPTION, 0, 0) ;

						nextWindow = GetNextWindow(nextWindow, GW_HWNDNEXT) ;
					}

					break ;

				case ID_SORT_LETTERBYLETTER :
					g_iSortOption = SORT_LETTER_BY_LETTER ;
					CheckMenuItem(pContextMenu, ID_SORT_LETTERBYLETTER, MF_CHECKED) ;
					CheckMenuItem(pContextMenu, ID_SORT_WORDBYWORD, MF_UNCHECKED) ;

					itoa(g_iSortOption, szSaveOption, 10) ;
					WritePrivateProfileString("Options", "SortOption", szSaveOption, "emDEX.ini");

					// Tell all catcher windows that an option has changed.
					nextWindow = GetWindow(GetDesktopWindow(), GW_CHILD) ;

					// Send the font change message to all catcher windows.
					while(nextWindow != NULL)
					{
						GetWindowText(nextWindow, szTempString1, 80) ;

						if (strncmp(szTempString1, "emDEX - ", 8) == 0)
							SendMessage(nextWindow, CHANGEOPTION, 0, 0) ;

						nextWindow = GetNextWindow(nextWindow, GW_HWNDNEXT) ;
					}

					break ;

				case ID_SETIGNORECHARS :
					if (isProcessing == TRUE)
						break ;

					isProcessing = TRUE ;
					DialogBox(appInstance, MAKEINTRESOURCE(IDD_IGNORECHARS), dlg, (DLGPROC)IgnoreCharsProc) ; 
					isProcessing = FALSE ;

					break ;

				case ID_SETRANGESEPARATORS :
					if (isProcessing == TRUE)
						break ;

					isProcessing = TRUE ;
					DialogBox(appInstance, MAKEINTRESOURCE(IDD_RANGESEPARATORS), dlg, (DLGPROC)RangeSeparatorsProc) ; 
					isProcessing = FALSE ;

					break ;

				case ID_ABOUT :
					if (bAboutBoxDisplayed == FALSE)
					{
						bAboutBoxDisplayed = TRUE ;
						DialogBox(appInstance, MAKEINTRESOURCE(IDD_ABOUT), NULL, (DLGPROC)AboutProc) ; 
					}

					break ;

				case ID_EXIT :
					if (MessageBox(dlg, "Are you sure you want to close the emDEX server ?", "emDEX server", MB_YESNO) == IDYES)
					{
						// Close all catcher windows.
						nextWindow = GetWindow(GetDesktopWindow(), GW_CHILD) ;

						i = 0 ;
						closeWindows[i] = NULL ;

						// Find all windows that need to be closed.
						while(nextWindow != NULL)
						{
							GetWindowText(nextWindow, szTempString1, 80) ;

							if (strncmp(szTempString1, "emDEX - ", 8) == 0)
							{
								closeWindows[i] = nextWindow ;
								closeWindows[i+1] = NULL ;

								i++ ;
							}

							nextWindow = GetNextWindow(nextWindow, GW_HWNDNEXT) ;
						}

						// Close the windows.
						for (i = 0 ; i < 80 ; i++)
						{
							if (closeWindows[i] == NULL)
								break ;

							SendMessage(closeWindows[i], WM_CLOSE, 0, 0) ;
						}
			
						SendMessage(dlg, WM_CLOSE, 0, 0) ;
					}

					break ;

				default :
					break ;
			}

/*			if (isProcessing == FALSE)
			{
				if (GetFocus() != LBox)
					break ;

				RetCode = GetKeyState(VK_RETURN) ;

				if (RetCode < 0)
				{
					iCount = 0 ;

					while ((isProcessing == TRUE) && (iCount < 50))
					{
						Sleep(10) ;
						iCount++ ;
					}

					if (isProcessing == TRUE)
						return (-1) ;
				
					isProcessing = TRUE ;

					sendDDEMessage(dlg, ITEMEDIT) ;

					isProcessing = FALSE ;
		
					break ;
				}
			} */

			break ;

		case CHANGEFONT :
			// Font has been changed.
			GetPrivateProfileString("Options", "FontColor", "0", szSaveOption, 800, "emDEX.ini") ;
			g_rgbText = atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfHeight", "-15", szSaveOption, 800, "emDEX.ini") ;
			lf.lfHeight = atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfWidth", "0", szSaveOption, 800, "emDEX.ini") ;
			lf.lfWidth = atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfEscapement", "0", szSaveOption, 800, "emDEX.ini") ;
			lf.lfEscapement = atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfOrientation", "0", szSaveOption, 800, "emDEX.ini") ;
			lf.lfOrientation = atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfWeight", "400", szSaveOption, 800, "emDEX.ini") ;
			lf.lfWeight = atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfItalic", "0", szSaveOption, 800, "emDEX.ini") ;
			lf.lfItalic = (UCHAR)atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfUnderline", "0", szSaveOption, 800, "emDEX.ini") ;
			lf.lfUnderline = (UCHAR)atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfStrikeOut", "0", szSaveOption, 800, "emDEX.ini") ;
			lf.lfStrikeOut = (UCHAR)atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfCharSet", "0", szSaveOption, 800, "emDEX.ini") ;
			lf.lfCharSet = (UCHAR)atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfOutPrecision", "3", szSaveOption, 800, "emDEX.ini") ;
			lf.lfOutPrecision = (UCHAR)atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfQuality", "1", szSaveOption, 800, "emDEX.ini") ;
			lf.lfQuality = (UCHAR)atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfPitchAndFamily", "18", szSaveOption, 800, "emDEX.ini") ;
			lf.lfPitchAndFamily = (UCHAR)atol(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "LfFaceName", "Times New Roman", szSaveOption, 800, "emDEX.ini") ;
			strcpy(lf.lfFaceName, szSaveOption) ;

			GetPrivateProfileString("Options", "Color", "16777215", szSaveOption, 800, "emDEX.ini") ;
			chColor = atol(LPCTSTR(szSaveOption)) ;

			defaultFont = CreateFontIndirect(&lf) ;

			lf.lfItalic = 1 ;
			italicFont = CreateFontIndirect(&lf) ;

			lf.lfItalic = 0 ;
			lf.lfWeight = 700 ;
			boldFont = CreateFontIndirect(&lf) ;

			lf.lfItalic = 1 ;
			lf.lfWeight = 700 ;
			bolditalicFont = CreateFontIndirect(&lf) ;

			fontHeight = MAKELPARAM((-MulDiv(lf.lfHeight, GetDeviceCaps(GetDC(dlg), LOGPIXELSY), 72) - 1), 0) ;

			// Set the item height depending on the font.
			SendMessage(LBox, LB_SETITEMHEIGHT, 0, fontHeight) ;

			fontHeight = (INT)((FLOAT)fontHeight*0.50) ;

			iCount = SendMessage(LBox, LB_GETCOUNT, 0, 0) ;

			maxLen = 0 ;

			// Reset the horizontal scrollbar.
			for (i = 0 ; i < iCount ; i++)
			{
				SendMessage(LBox, LB_GETTEXT, i, (LPARAM)szTempString1) ;

				len = strlen(szTempString1) ;

				if (len > maxLen)
				{
					maxLen = len ;

					SendMessage(LBox, LB_SETHORIZONTALEXTENT, maxLen*(fontHeight), 0) ;
				}
			}

			InvalidateRect(dlg, NULL, TRUE) ;
			SendMessage(dlg, WM_PAINT, 0, 0) ;

			break ;

		case SETFOCUS :
			// Make sure that the focus is set correctly.
			if (hFindWindow != NULL)
			{
				SetFocus(hFindWindow) ;
				SetForegroundWindow(hFindWindow);
			}
			else
			{
				SetFocus(dlg) ;
				SetForegroundWindow(dlg);
			}

			break ;

		case CHANGEOPTION :
			// An option has been changed.
			iTempSort = g_iSortOption ;
			strcpy(szSaveSeparatorChars, szSeparatorChars) ;

			GetPrivateProfileString("Options", "OrphansAudio", "1", szSaveOption, 800, "emDEX.ini") ;
			g_bOrphanAudio = atoi(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "OrphansVisual", "1", szSaveOption, 800, "emDEX.ini") ;
			g_bOrphanVisual = atoi(LPCTSTR(szSaveOption)) ;

			szIgnoreChars[0] = '-' ;
			szIgnoreChars[1] = '\0' ;

			GetPrivateProfileString("Options", "IgnoreChars", "", szSaveOption, 800, "emDEX.ini") ;
			strcat(szIgnoreChars, szSaveOption) ;

			GetPrivateProfileString("Options", "SeparatorChars", "-", szSaveOption, 800, "emDEX.ini") ;
			strcpy(szSeparatorChars, szSaveOption) ;

			GetPrivateProfileString("Options", "SortOption", "1", szSaveOption, 800, "emDEX.ini") ;
			g_iSortOption = atoi(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "AlwaysOnTop", "1", szSaveOption, 800, "emDEX.ini") ;
			g_bAlwaysOnTop = atoi(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "RealTimeUpdates", "1", szSaveOption, 800, "emDEX.ini") ;
			g_bRealTimeUpdates = atoi(LPCTSTR(szSaveOption)) ;

			GetPrivateProfileString("Options", "UseEditor", "1", szSaveOption, 800, "emDEX.ini") ;
			g_bUseEditor = atoi(LPCTSTR(szSaveOption)) ;

			sendDDEMessage(dlg, REALTIME) ;

			if (g_bRealTimeUpdates == TRUE)
			{
				EnableMenuItem(GetMenu(dlg), ID_UPDATEWINDOW, MF_GRAYED) ;
			}
			else
			{
				EnableMenuItem(GetMenu(dlg), ID_UPDATEWINDOW, MF_ENABLED) ;
			}

			if ((g_bRealTimeUpdates == TRUE) || (iTempSort != g_iSortOption))
			{
				sendDDEMessage(dlg, DOITEMUPDATE) ;
			}

			break ;

		case SETPOS :
			if (g_bAlwaysOnTop == TRUE)
			{
				SetWindowPos(dlg, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE) ;
			}
			else
			{
				SetWindowPos(dlg, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE) ;
			}

			break ;

		case EDIT_BUTTONS :
			if (hButtonWindow == NULL)
			{
				CreateDialog(appInstance, MAKEINTRESOURCE(IDD_BUTTONS), dlg, (DLGPROC)ButtonProc) ;
			}
			else
			{
				SetFocus(hButtonWindow) ;
				SetForegroundWindow(hButtonWindow);
			}

			break ;

		case ITEMLIST_DELETE :
			// Take a copy of the item list for comparison later, and then delete it.
			for (iCount = 0 ; itemList[iCount].itemText[0] != '\0' ; iCount++)
			{
				strcpy(saveItemList[iCount].itemText, itemList[iCount].itemText) ;
				for (i = 0 ; itemList[iCount].itemPage[i] != -1 ; i++)
				{
					saveItemList[iCount].itemPage[i] = itemList[iCount].itemPage[i] ;
					strcpy(saveItemList[iCount].itemReference[i], itemList[iCount].itemReference[i]) ;
					iSaveRange[iCount][i] = iRange[iCount][i] ;
					iSavePageFormat[iCount][i] = iPageFormat[iCount][i] ;
					iSavePageNumbering[iCount][i] = iPageNumbering[iCount][i] ;
				}

				saveItemList[iCount].itemPage[i] = -1 ;

				itemList[iCount].itemText[0] = '\0' ;
				itemList[iCount].itemReference[0][0] = '\0' ;
				itemList[iCount].itemPage[0] = -1 ;

				inItemList[iCount].itemText[0] = '\0' ;
			}

			saveItemList[iCount].itemText[0] = '\0' ;
			saveItemList[iCount].itemReference[0][0] = '\0' ;

			break ;

		case ITEMLIST_CLEAR :
			// Delete the item list and the saved copy.
			for (iCount = 0 ; itemList[iCount].itemText[0] != '\0' ; iCount++)
			{
				itemList[iCount].itemText[0] = '\0' ;
				itemList[iCount].itemReference[0][0] = '\0' ;
				itemList[iCount].itemPage[0] = -1 ;

				saveItemList[iCount].itemText[0] = '\0' ;
				saveItemList[iCount].itemReference[0][0] = '\0' ;
				saveItemList[iCount].itemPage[0] = -1 ;

				inItemList[iCount].itemText[0] = '\0' ;
				inItemList[iCount].itemReference[0][0] = '\0' ;
				inItemList[iCount].itemPage[0] = -1 ;
			}

			break ;

		case ITEMLIST_UPDATE :
			// Copy the input message into inItemList
			for (inCount = 0 ; inItemList[inCount].itemText[0] != '\0' ; inCount++) ;

			GlobalGetAtomName((ATOM)lParam, szAtom, MAXLEN) ;
			GlobalGetAtomName((ATOM)wParam, szAtomNum, MAXLEN) ;

			// Check if the max number of items has been reached.
			if (inCount >= (INT)itemLimit)
			{
				inItemList[itemLimit].itemText[0] = '\0' ;

				while ((tempAtom = GlobalFindAtom(szAtom)) != 0)
					GlobalDeleteAtom(tempAtom) ;

				while ((tempAtom = GlobalFindAtom(szAtomNum)) != 0)
					GlobalDeleteAtom(tempAtom) ;

				break ;
			}

			while ((tempAtom = GlobalFindAtom(szAtom)) != 0)
				GlobalDeleteAtom(tempAtom) ;

			while ((tempAtom = GlobalFindAtom(szAtomNum)) != 0)
				GlobalDeleteAtom(tempAtom) ;

			if (szAtom[0] == '\0')
				// Ignore blank markers.
				return(0) ;

			for (i = 0 ; i < (INT)strlen(szAtomNum) ; i++)
				if (szAtomNum[i] == '@')
					break ;

			strncpy(inItemList[inCount].itemReference[0], szAtomNum, i) ;
			inItemList[inCount].itemReference[0][i] = '\0' ;

			strcpy(inItemList[inCount].itemText, szAtom) ;

			if ((szAtomNum[0] == '\0') || (wParam == -1))
				inItemList[inCount].itemPage[0] = -1 ;
			else
				inItemList[inCount].itemPage[0] = atoi(&szAtomNum[i+1]) ;

			inItemList[inCount+1].itemText[0] = '\0' ;
			inItemList[inCount+1].itemReference[0][0] = '\0' ;

			break ;

		case LISTBOX_UPDATE :
			// Update the item list.
//			_beginthread(startThreads, 16384, (PVOID)NULL) ;
			startThreads(NULL) ;

			if (isFirstUpdate == TRUE)
			{
				// Find the first item if this is the first time.
				sendDDEMessage(dlg, ITEMFINDPAGE, itemList[0].itemPage[0], itemList[0].itemReference[0]) ;

				isFirstUpdate = FALSE ;
			}

			break ;

		case CATCHER_OPEN:
			// Allow a maximum of 5 catcher windows
			iCount = 0 ;
			nextWindow = GetWindow(GetDesktopWindow(), GW_CHILD) ;

			// Send the font change message to all catcher windows.
			while(nextWindow != NULL)
			{
				GetWindowText(nextWindow, szTempString1, 80) ;

				if (strncmp(szTempString1, "emDEX - ", 8) == 0)
					iCount++ ;

				nextWindow = GetNextWindow(nextWindow, GW_HWNDNEXT) ;
			}

			if (iCount == 5)
			{
				MessageBox(NULL, "The maximum number of emDEX windows has been reached.\n\nClose some existing emDEX windows to continue.", "emDEX", MB_ICONHAND | MB_SYSTEMMODAL) ;
				break ;
			}

			GlobalGetAtomName((ATOM)lParam, szAtom, MAXLEN) ;

			while ((tempAtom = GlobalFindAtom(szAtom)) != 0)
				GlobalDeleteAtom(tempAtom) ;

			sprintf(szExecString, "\"%s\"", szAtom) ;

			exeRetCode = (INT)ShellExecute(NULL, "open", executePath, szExecString, NULL, SW_SHOWNORMAL) ;

			if ((exeRetCode == 0) || (exeRetCode == SE_ERR_OOM))
			{
				MessageBox(NULL, "Out of memory opening the emDEX window.\n\nIncrease the system virtual memory setting in your control panel.", "emDEX", MB_ICONHAND | MB_SYSTEMMODAL) ;
				break ;
			}

			if (exeRetCode < 32)
			{
				sprintf(szTempString1, "Error %d opening the emDEX window.\n\nRetry or contact support at support@emdex.ca.", exeRetCode) ;

				MessageBox(NULL, szTempString1, "emDEX", MB_ICONHAND | MB_SYSTEMMODAL) ;
				break ;
			}

//			ShellExecute(NULL, "open", "c:\\cpp\\psindexcatcher\\debug\\emdex.exe", szExecString, NULL, SW_SHOWNORMAL) ;

			break ;
	}
	return FALSE;
}

void startThreads(PVOID in)
{
	CHAR	szTemp[MAXLEN] ;
	char	szNumString[80], szTempString1[MAXLEN] ;
	int		iCount, inCount, i, j ;
	BOOL	needUpdating ;
	INT		selected, topIndex ;
	static	BOOL isProcessing = FALSE ;
	INT numTabs = 0 ;
	INT maxLen = 0, len = 0 ;
	CHOOSEFONT cf = {sizeof(CHOOSEFONT)};
	CHOOSECOLOR cc = {sizeof(CHOOSECOLOR)};
	INT		tempRangeCount ;
	static INT	orphanRangeCount = 0 ;

/*	while (isThreadProcessing == TRUE)
		Sleep(150) ; */

//	EnterCriticalSection(&Critical_Section) ;

	isThreadProcessing = TRUE ;

//	Sleep(50) ;

/*	tempHandle = fopen("c:\\update.txt", "a+") ;
	fputs("Updating\n", tempHandle) ;
	fclose(tempHandle) ;*/

	// Initialize the page numbers.
/*	for (i = 0 ; i < MAXITEMS ; i++)
		for (j = 0 ; j < MAXLEN ; j++)
			itemList[i].itemPage[j] = -1 ; */

	for (inCount = 0 ; inItemList[inCount].itemText[0] != '\0' ; inCount++)
		updateListItems(inCount) ;

//	itemList[inCount].itemText[0] = '\0' ;

	needUpdating = FALSE ;

	// If the saved copy is the same as the updated one, then don't update the listbox.
	for (iCount = 0 ; itemList[iCount].itemText[0] != '\0' ; iCount++)
	{
		if (strcmp(saveItemList[iCount].itemText, itemList[iCount].itemText) != 0)
		{
			needUpdating = TRUE ;
			break ;
		}

		for (i = 0 ; itemList[iCount].itemPage[i] != -1 ; i++)
		{
			if (saveItemList[iCount].itemPage[i] != itemList[iCount].itemPage[i])
			{
				needUpdating = TRUE ;
				break ;
			}

			if (strcmp(saveItemList[iCount].itemReference[i], itemList[iCount].itemReference[i]) != 0)
			{
				needUpdating = TRUE ;
				break ;
			}

			if (iSaveRange[iCount][i] != iRange[iCount][i])
			{
				needUpdating = TRUE ;
				break ;
			}

			if (iSavePageFormat[iCount][i] != iPageFormat[iCount][i])
			{
				needUpdating = TRUE ;
				break ;
			}

			if (iSavePageNumbering[iCount][i] != iPageNumbering[iCount][i])
			{
				needUpdating = TRUE ;
				break ;
			}
		}

		if ((itemList[iCount].itemPage[i] == -1) && (saveItemList[iCount].itemPage[i] != -1))
			needUpdating = TRUE ;

		if (needUpdating == TRUE)
			break ;
	}

	if ((itemList[iCount].itemText[0] == '\0') && (saveItemList[iCount].itemText[0] != '\0'))
		needUpdating = TRUE ;

	if (strcmp(szSeparatorChars, szSaveSeparatorChars) == 0)
	{
		if ((needUpdating == FALSE) && (iCount > 0))
		{
			isThreadProcessing = FALSE ;
			return ;
		}
	}
	else
	{
		strcpy(szSaveSeparatorChars, szSeparatorChars) ;
	}

/*	for (iCount = 0 ; itemList[iCount].itemText[0] != '\0' ; iCount++)
	{
		strcpy(saveItemList[iCount].itemText, itemList[iCount].itemText) ;
		for (i = 0 ; itemList[iCount].itemPage[i] != -1 ; i++)
			saveItemList[iCount].itemPage[i] = itemList[iCount].itemPage[i] ;

		saveItemList[iCount].itemPage[i] = -1 ;
	}

	saveItemList[iCount].itemText[0] = '\0' ; */

	maxLen = 0 ;
	SendMessage(LBox, LB_SETHORIZONTALEXTENT, 0, 0) ;

	tempRangeCount = 0 ;

	selected = SendMessage(LBox, LB_GETCURSEL, 0, 0) ;
	topIndex = SendMessage(LBox, LB_GETTOPINDEX, 0, 0) ;

	// Delete the existing entries.
	SendMessage(LBox, LB_RESETCONTENT, 0, 0) ;

	// Now add in the new item list entries.
	for (iCount = 0 ; itemList[iCount].itemText[0] != '\0' ; iCount++)
	{
		szAddString[iCount] [0] = '\0' ;

		for (j = 0 ; j < (INT)strlen(itemList[iCount].itemText) ; j += 2)
		{
			if ((itemList[iCount].itemText[j] == '@') &&
				(itemList[iCount].itemText[j+1] == '>'))
			{
				strcat(szAddString[iCount], "     ") ;
			}
			else
			{
				break ;
			}
		}

		if ((itemList[iCount].itemText[0] == '@') &&
			(itemList[iCount].itemText[1] == '>'))
		{
			for ( ; j < (INT)strlen(itemList[iCount].itemText) ; j++)
				if (itemList[iCount].itemText[j] != ' ')
					break ;

			strcat(szAddString[iCount], &itemList[iCount].itemText[j]) ;
		}
		else
		{
			strcat(szAddString[iCount], itemList[iCount].itemText) ;
		}

		// Now get the page numbers.
		szPageString[iCount] [0] = '\0' ;

		for (j = 0 ; itemList[iCount].itemPage[j] != -1 ; j++)
		{
			if ((iPageNumbering[iCount][j] == FV_PAGE_NUM_ROMAN_LC) || (iPageNumbering[iCount][j] == FV_PAGE_NUM_ROMAN_UC))
				toRoman(itemList[iCount].itemPage[j], iPageNumbering[iCount][j], szNumString);
			else
				if ((iPageNumbering[iCount][j] == FV_PAGE_NUM_ALPHA_LC) || (iPageNumbering[iCount][j] == FV_PAGE_NUM_ALPHA_UC))
					toAlpha(itemList[iCount].itemPage[j], iPageNumbering[iCount][j], szNumString);
				else 
					itoa((itemList[iCount].itemPage[j]), szNumString, 10) ;

			if (iRange[iCount][j] == RANGEEND)
			{
				if (j > 0)
					strcat (szPageString[iCount], ", ") ;
				else
					strcpy (szPageString[iCount], " ") ;

				strcat(szPageString[iCount], "??") ;
				strcat (szPageString[iCount], szSeparatorChars) ;
				strcat(szPageString[iCount], itemList[iCount].itemReference[j]) ;

				tempRangeCount++ ;	

				continue ;
			}

			if (iRange[iCount][j] == RANGESTART)
			{
				if ((iRange[iCount][j+1] == RANGEEND) && (itemList[iCount].itemPage[j+1] != -1))
				{
					if ((iPageNumbering[iCount][j+1] == FV_PAGE_NUM_ROMAN_LC) || (iPageNumbering[iCount][j+1] == FV_PAGE_NUM_ROMAN_UC))
						toRoman(itemList[iCount].itemPage[j+1], iPageNumbering[iCount][j+1], szTemp);
					else
						if ((iPageNumbering[iCount][j+1] == FV_PAGE_NUM_ALPHA_LC) || (iPageNumbering[iCount][j+1] == FV_PAGE_NUM_ALPHA_UC))
							toAlpha(itemList[iCount].itemPage[j+1], iPageNumbering[iCount][j+1], szTemp);
						else 
							itoa((itemList[iCount].itemPage[j+1]), szTemp, 10) ;

					if (j > 0)
						strcat (szPageString[iCount], ", ") ;
					else
						strcpy (szPageString[iCount], " ") ;

					strcat(szPageString[iCount], itemList[iCount].itemReference[j]) ;
					strcat (szPageString[iCount], szSeparatorChars) ;
					strcat(szPageString[iCount], itemList[iCount].itemReference[j+1]) ;

					j++ ;
				}
				else
				{
					if (j > 0)
						strcat (szPageString[iCount], ", ") ;
					else
						strcpy (szPageString[iCount], " ") ;

					strcat(szPageString[iCount], itemList[iCount].itemReference[j]) ;
					strcat (szPageString[iCount], szSeparatorChars) ;
					strcat(szPageString[iCount], "??") ;

					tempRangeCount++ ;	
				}
			}
			else
			{
				if (j > 0)
					strcat (szPageString[iCount], ", ") ;
				else
					strcpy (szPageString[iCount], " ") ;

				strcat(szPageString[iCount], itemList[iCount].itemReference[j]) ;
			}
		}

		len = strlen(szAddString[iCount]) + strlen(szPageString[iCount]) ;

		if (len > maxLen)
		{
			maxLen = len ;

			SendMessage(LBox, LB_SETHORIZONTALEXTENT, maxLen*fontHeight, 0) ;
		}

		convertSpecialCharacters(szAddString[iCount]) ;

		// Remove any linefeeds.
		for (i = 0 ; i < (INT)strlen(szAddString[iCount]) ; i++)
		{
			if (szAddString[iCount][i] == 10)
			{
				szAddString[iCount][i] = '\0' ;
				break ;
			}
		}

		for (i = ((INT)strlen(szAddString[iCount])-1) ; i >= 0 ; i--)
		{
			if ((szAddString[iCount][i] == '>') && (isEscapeSequence(szAddString[iCount], i) == FALSE))
			{
				while (szAddString[iCount][i] != '<') 
				{
					i-- ;

					if (isEscapeSequence(szAddString[iCount], i) == TRUE)
						continue ;

					if (i < 0)
						break ;
//						return ;
				}

				i-- ;
			}

			if (szAddString[iCount][i] != ' ')
				break ;
		}

		szAddString[iCount][i+1] = '\0' ;

		szTempString1[0] = '\0' ;

		// Cope with escape sequences.
		for (i = 0, j = 0 ; i < (INT)strlen(szAddString[iCount]) ; i++)
		{
			szBracketsDisplayed[iCount] [j] = 0 ;

			while (szAddString[iCount][i] == '\\')
			{
				i++	;

				if (szAddString[iCount][i] == '\\')
					break ;

				if ((szAddString[iCount][i] == 'n') || (szAddString[iCount][i] == 't'))
					i++ ;

				if ((szAddString[iCount][i] == '<') || (szAddString[iCount][i] == '>') ||
					(szAddString[iCount][i] == '[') || (szAddString[iCount][i] == ']'))
					szBracketsDisplayed[iCount] [j] = 1 ;
				else
					szBracketsDisplayed[iCount] [j] = 0 ;
			}

			szTempString1[j] = szAddString[iCount][i] ;
			j++ ;
			szTempString1[j] = '\0' ;
		}

		strcpy((CHAR *)szAddString[iCount], szTempString1) ;

		SendMessage(LBox, LB_ADDSTRING, 0, (LPARAM)szAddString[iCount]) ;
	}

	if (selected == LB_ERR)
		selected = 0 ;

	if (selected > SendMessage(LBox, LB_GETCOUNT, 0, 0) - 1)
		selected = SendMessage(LBox, LB_GETCOUNT, 0, 0) - 1 ;

	while (itemList[selected].itemPage[0] == -1)
		selected++ ;

	// Make sure that the positioning in the listbox is the same 
	// as before it was re-created.
	SendMessage(LBox, LB_SETCURSEL, selected, 0) ;
	SendMessage(LBox, LB_SETTOPINDEX, topIndex, 0) ;

	// New orphaned ranges have been created.
	if (tempRangeCount > orphanRangeCount)
	{
		if (g_bOrphanAudio == TRUE)
		{
			PlaySound("orphan.wav", NULL, SND_ASYNC) ;
		}

		if (g_bOrphanVisual == TRUE)
		{
			MessageBox(NULL, "Orphan range markers have been detected", "emDEX", MB_ICONINFORMATION | MB_SYSTEMMODAL) ;
		}
	}

	orphanRangeCount = tempRangeCount ;

	isThreadProcessing = FALSE ;

//	LeaveCriticalSection(&Critical_Section) ;
}

//void updateListItems(PVOID in)
void updateListItems(INT inCount)
{
//	INT		inCount ;
	char	szTempString1[MAXLEN], szTempString2[MAXLEN], szTempString3[MAXLEN], szTempString4[MAXLEN] ;
	char	szAddTempTabbed[80], szCompareTempTabbed[80] ;
	CHAR	szCompare1[MAXLEN], szCompare2[MAXLEN] ;
	CHAR	szAddSearchString[MAXLEN], szCompareSearchString[MAXLEN] ;
	CHAR	szinCompare[MAXLEN], szinAdd[MAXLEN] ;
	CHAR	szCase1[MAXLEN], szCase2[MAXLEN] ;
	char	szAddTabText[5] [MAXLEN], szCompareTabText[5] [MAXLEN] ;
	int		iCount, iSubString, i, j, k, l, iTabbed ;
	BOOL	bAddNew = FALSE ;
	INT numTabs = 0 ;
	INT maxLen = 0, len = 0 ;
	INT		numFormat = 0, formatCount = 0, tempRange, pageNumbering = 0 ;
	BOOL	sortOverride = FALSE, anyFound = FALSE ;

//	inCount = (INT)in ;

/*	for (inCount = 0 ; inItemList[inCount].itemText[0] != '\0' ; inCount++)
	{ */
		for (iCount = 0 ; itemList[iCount].itemText[0] != '\0' ; iCount++) ;

		for (i = 0 ; i < 5 ; i++)
		{
			szAddTabText[i] [0] = '\0' ;
			szCompareTabText[i] [0] = '\0' ;
		}
		
		tempRange = -1 ;

/*		for (k = 0 ; k < (INT)strlen(inItemList[inCount].itemText) ; k++)
			if (strnicmp(&inItemList[inCount].itemText[k], "<$nopage>", 9) == 0)
				inItemList[inCount].itemPage[0] = -1 ; */

		for (k = 0 ; k < (INT)strlen(inItemList[inCount].itemText) ; k++)
		{
			if (strnicmp(&inItemList[inCount].itemText[k], "<$startrange>", 13) == 0)
			{
				tempRange = RANGESTART ;
			}

			if (strnicmp(&inItemList[inCount].itemText[k], "<$endrange>", 11) == 0)
			{
				tempRange = RANGEEND ;
			}
		}

		szTempString1[0] = '\0' ;

		// Find any number formatting
		numFormat = getNumberFormatting(inItemList[inCount].itemText) ;

		sortOverride = getCompareText(inItemList[inCount].itemText, szinCompare) ;
		getAddText(inItemList[inCount].itemText, szinAdd) ;

		for (k = 0 ; k < (INT)strlen(szinAdd) ; k++)
			if (szinAdd[k] != ' ')
				break ;

		strcpy(szinAdd, &szinAdd[k]) ;
		k = 0 ;

		// Check for ':'
		for (iSubString = 0 ; iSubString < (int)strlen(szinAdd) ; iSubString++)
			if ((szinAdd[iSubString] == ':') && (isEscapeSequence(szinAdd, iSubString) == FALSE))
				break ;

		if (iSubString == (int)strlen(szinAdd))
		{
			strcpy(szAddSearchString, &szinAdd[k]) ;

			for (i = 0 ; i < 5 ; i++)
				szAddTabText[i] [0] = '\0' ;
		}
		else
		{
			strncpy(szAddSearchString, &szinAdd[k], iSubString-k) ;
			szAddSearchString[iSubString-k] = '\0' ;

			iSubString++ ;

			for (i = iSubString, j = 0 ; i < (int)strlen(szinAdd) ; i++)
			{
				if ((szinAdd[i] == ':') && (isEscapeSequence(szinAdd, i) == FALSE))
				{
					strncpy(szAddTabText[j], &szinAdd[iSubString], i-iSubString+1) ;
					szAddTabText[j] [i-iSubString] = '\0' ;

					j++ ;
					iSubString = i+1 ;

					if (j >= 5)
						break ;
				}
			}

			if (j <= 5)
			{
				strncpy(szAddTabText[j], &szinAdd[iSubString], i-iSubString+1) ;
				szAddTabText[j] [i-iSubString] = '\0' ;
			}
		}

		for (k = 0 ; k < (INT)strlen(szinCompare) ; k++)
			if (szinCompare[k] != ' ')
				break ;

		strcpy(szinCompare, &szinCompare[k]) ;
		k = 0 ;

		// Check for ':'
		for (iSubString = 0 ; iSubString < (int)strlen(szinCompare) ; iSubString++)
			if ((szinCompare[iSubString] == ':') && (isEscapeSequence(szinCompare, iSubString) == FALSE))
				break ;

		if (iSubString == (int)strlen(szinCompare))
		{
			strcpy(szCompareSearchString, &szinCompare[k]) ;

			for (i = 0 ; i < 5 ; i++)
				szCompareTabText[i] [0] = '\0' ;
		}
		else
		{
			strncpy(szCompareSearchString, &szinCompare[k], iSubString-k) ;
			szCompareSearchString[iSubString-k] = '\0' ;

			iSubString++ ;

			for (i = iSubString, j = 0 ; i < (int)strlen(szinCompare) ; i++)
			{
				if ((szinCompare[i] == ':') && (isEscapeSequence(szinCompare, i) == FALSE))
				{
					strncpy(szCompareTabText[j], &szinCompare[iSubString], i-iSubString+1) ;
					szCompareTabText[j] [i-iSubString] = '\0' ;

					j++ ;
					iSubString = i+1 ;

					if (j >= 5)
						break ;
				}
			}

			if (j <= 5)
			{
				strncpy(szCompareTabText[j], &szinCompare[iSubString], i-iSubString+1) ;
				szCompareTabText[j] [i-iSubString] = '\0' ;
			}
		}

		stripSpaces(szCompareSearchString) ;

		for (i = 0 ; i < 5 ; i++)
			stripSpaces(szCompareTabText[i]) ;

		stripIgnoreChars(szCompareSearchString) ;
		
		for (k = 0 ; k < 5 ; k++)
			stripIgnoreChars(szCompareTabText[k]) ;

		bAddNew = FALSE ;

		// Check for duplicates.
		for (i = 0 ; i < iCount ; i++)
		{
			getCompareText(itemList[i].itemText, szCompare1) ;
			strcpy(szCompare2, szCompareSearchString) ;

			if ((strncmp(szCompare1, szCompare2, strlen(szCompare2)) == 0) &&
				(strlen(szCompare2) == strlen(szCompare1)) &&
				(noTagCompare(szAddSearchString, itemList[i].itemText) == 0))
			{
				if (szAddTabText[0] [0] == '\0')
				{
					// New main entry.
					for (k = 0 ; k < MAXITEMS ; k++)
					{
						if ((int)inItemList[inCount].itemPage[0] == itemList[i].itemPage[k])
						{
							if ((iRange[i] [k] == RANGESTART) && (tempRange == RANGEEND))
								// Collapse the range onto the page.
								iRange[i] [k] = -1 ;
							
							if (strcmp(inItemList[inCount].itemReference[0], itemList[i].itemReference[k]) == 0)
								return ;
						}

						// Don't add in nopage markers when updating an existing item
						if (inItemList[inCount].itemPage[0] == -1)
							return ;

						if (((int)inItemList[inCount].itemPage[0] <= itemList[i].itemPage[k]) || (itemList[i].itemPage[k] == -1))
						{
							for (l = (MAXLEN-1) ; l > k ; l--)
							{
								itemList[i].itemPage[l] = itemList[i].itemPage[l-1] ;
								strcpy(itemList[i].itemReference[l], itemList[i].itemReference[l-1]) ;
								iPageFormat[i] [l] = iPageFormat[i] [l-1] ;
								iPageNumbering[i] [l] = iPageNumbering[i] [l-1] ;
								iRange[i] [l] = iRange[i] [l-1] ;
							}

							itemList[i].itemPage[k] = (int)inItemList[inCount].itemPage[0] ;
							strcpy(itemList[i].itemReference[k], inItemList[inCount].itemReference[0]) ;
							iPageFormat[i] [k] = numFormat ;
							iPageNumbering[i] [k] = pageNumbering ;
							iRange[i] [k] = tempRange ;

							return ;
						}
					}
				}
				else
				{
					for (iTabbed = 0 ; iTabbed < 5 ; iTabbed++)
					{
						if (szCompareTabText[iTabbed] [0] == '\0')
						{
							return ;
						}

						szAddTempTabbed[0] = '\0' ;
						szCompareTempTabbed[0] = '\0' ;

						for (j = 0 ; j <= iTabbed ; j++)
						{
							strcat(szAddTempTabbed, "@>") ;
							strcat(szCompareTempTabbed, "@>") ;
						}

						strcat(szCompareTempTabbed, szCompareTabText[iTabbed]) ;
						strcat(szAddTempTabbed, szAddTabText[iTabbed]) ;

						// Tabbed entry.
						for (j = i+1 ; j < iCount ; j++) 
						{
							strcpy(szTempString1, szCompareTempTabbed) ;
							strcpy(szTempString2, itemList[j].itemText) ;

							// Get the number of tabs
							for (numTabs = 0 ; numTabs < 10 ; numTabs += 2)
							{
								if (strncmp(&itemList[j].itemText[numTabs], "@>", 2) != 0)
									break ;
							}

							strcpy(szCompare1, szTempString1) ;
							if (getCompareText(szTempString2, szCompare2) == TRUE)
								getTabItemText(szCompare2, szCompare2, (numTabs/2)) ;

							strupr(szCompare1) ;
							strupr(szCompare2) ;

							getCompareText(szAddTempTabbed, szTempString3) ;
							getCompareText(szTempString2, szTempString4) ;

							if (((compareSortText(szCompare1, szCompare2, strlen(szCompare2)) < 0) ||
								((strncmp(strupr(szCompare1), strupr(szCompare2), strlen(szCompare2)) == 0) &&
								(sortFormattedText(szAddTempTabbed, szTempString2, szTempString3, szTempString4) == -1)) &&
								((numTabs / 2) <= (iTabbed + 1))))
							{
								// New tabbed entry.
								addNewItem(j, iCount) ;

								iCount++ ;

								if (sortOverride == FALSE)
									strcpy(itemList[j].itemText, szAddTempTabbed) ;
								else
									sprintf(itemList[j].itemText, "%s[%s]", szAddTempTabbed, szinCompare) ;

								if ((szAddTabText[iTabbed+1] [0] == '\0') || (iTabbed == 4))
								{
									itemList[j].itemPage[0] = (int)inItemList[inCount].itemPage[0] ;
									strcpy(itemList[j].itemReference[0], inItemList[inCount].itemReference[0]) ;
									iPageFormat[j] [0] = numFormat ;
									iPageNumbering[j] [0] = pageNumbering ;
									iRange[j] [0] = tempRange ;
									itemList[j].itemPage[1] = -1 ;
								}
								else
									i = j ;

								break ;
							}

							if ((strncmp(strupr(szCompare1), strupr(szCompare2), strlen(szCompare2)) == 0) &&
								(strlen(szCompare1) == strlen(szCompare2)) &&
								(noTagCompare(szAddTempTabbed, szTempString2) == 0))
							{
								// Updated tabbed entry.
								for (k = 0 ; k < MAXITEMS ; k++)
								{
									if (((int)inItemList[inCount].itemPage[0] == itemList[j].itemPage[k]) || (szAddTabText[iTabbed+1] [0] != '\0'))
									{
										if ((iRange[j] [k] == RANGESTART) && (tempRange == RANGEEND))
										{
											// Collapse the range onto the page.
											iRange[j] [k] = -1 ;

											i = j ;
											break ;
										}

										if ((strcmp(inItemList[inCount].itemReference[0], itemList[j].itemReference[k]) == 0) || (szAddTabText[iTabbed+1] [0] != '\0'))
										{
											i = j ;
											break ;
										}
									}

									// Don't add in nopage markers when updating an existing item
									if (inItemList[inCount].itemPage[0] == -1)
										return ;

									if (((int)inItemList[inCount].itemPage[0] <= itemList[j].itemPage[k]) || (itemList[j].itemPage[k] == -1))
									{
										for (l = (MAXLEN-1) ; l > k ; l--)
										{
											itemList[j].itemPage[l] = itemList[j].itemPage[l-1] ;
											strcpy(itemList[j].itemReference[l], itemList[j].itemReference[l-1]) ;
											iPageFormat[j][l] = iPageFormat[j][l-1] ;
											iPageNumbering[j] [l] = iPageNumbering[j] [l-1] ;
											iRange[j] [l] = iRange[j] [l-1] ;
										}

										itemList[j].itemPage[k] = (int)inItemList[inCount].itemPage[0] ;
										strcpy(itemList[j].itemReference[k], inItemList[inCount].itemReference[0]) ;
										iPageFormat[j][k] = numFormat ;
										iPageNumbering[j] [k] = pageNumbering ;
										iRange[j] [k] = tempRange ;

										return ;
									}
								}

								break ;
							}

							// Get the number of tabs
							for (numTabs = 0 ; numTabs < 10 ; numTabs += 2)
							{
								if (strncmp(&itemList[j].itemText[numTabs], "@>", 2) != 0)
									break ;
							}

							if ((strncmp(itemList[j].itemText, "@>", 2) != 0) || ((numTabs / 2) < (iTabbed + 1)))
							{
								// New tabbed entry.
								addNewItem(j, iCount) ;

								iCount++ ;

								if (sortOverride == FALSE)
									strcpy(itemList[j].itemText, szAddTempTabbed) ;
								else
									sprintf(itemList[j].itemText, "%s[%s]", szAddTempTabbed, szinCompare) ;

								if ((szAddTabText[iTabbed+1] [0] == '\0')  || (iTabbed == 4))
								{
									itemList[j].itemPage[0] = (int)inItemList[inCount].itemPage[0] ;
									strcpy(itemList[j].itemReference[0], inItemList[inCount].itemReference[0]) ;
									itemList[j].itemPage[1] = -1 ;
									iPageFormat[j][0] = numFormat ;
									iPageNumbering[j] [0] = pageNumbering ;
									iRange[j] [0] = tempRange ;
								}
								else
								{
									i = j ;
									itemList[j].itemPage[0] = -1 ;
								}

								break ;
							}
						}

						if (j == iCount)
						{
							// New tabbed entry - add to end.
							addNewItem(j, iCount) ;

							iCount++ ;

							if (sortOverride == FALSE)
								strcpy(itemList[j].itemText, szAddTempTabbed) ;
							else
								sprintf(itemList[j].itemText, "%s[%s]", szAddTempTabbed, szinCompare) ;

							if ((szAddTabText[iTabbed+1] [0] == '\0')  || (iTabbed == 4))
							{
								itemList[j].itemPage[0] = (int)inItemList[inCount].itemPage[0] ;
								strcpy(itemList[j].itemReference[0], inItemList[inCount].itemReference[0]) ;
								itemList[j].itemPage[1] = -1 ;
								iPageFormat[j][0] = numFormat ;
								iPageNumbering[j] [0] = pageNumbering ;
								iRange[j] [0] = tempRange ;
							}
							else
							{
								i = j ;
								itemList[j].itemPage[0] = -1 ;
							}
						}
					}
				}

				return ;
			}

			strcpy(szTempString1, szCompareSearchString) ;
			strcpy(szTempString2, itemList[i].itemText) ;

			strcpy(szCompare1, szTempString1) ;
			getCompareText(szTempString2, szCompare2) ;

			strcpy(szCase1, szCompareSearchString) ;
			strcpy(szCase2, szCompare2) ;

			if (((compareSortText(strupr(szCompare1), strupr(szCompare2), strlen(szCompare2)) < 0) &&
				(strncmp(szCompare2, "@>", 2) != 0) && (strchr(szCompare2, ':') == NULL)) ||
				(((strncmp(strupr(szCompare1), strupr(szCompare2), strlen(szCompare2)) == 0) &&
				((compareSortText(strupr(szTempString1), strupr(szTempString2), strlen(szTempString2)) < 0)))))
			{
				bAddNew = TRUE ;
				break ; 
			}

			// Cope with the same text formatted differently
			if (strncmp(strupr(szCompare1), strupr(szCompare2), strlen(szCompare1)) == 0)
			{
				if (sortFormattedText(szAddSearchString, szTempString2, szCase1, szCase2) == -1)
				{
					bAddNew = TRUE ;
					break ;
				}
			}
		}

		if ((i == iCount) || (bAddNew == TRUE))
		{
			// New string.
			if (szAddTabText[0] [0] == '\0')
			{
				addNewItem(i, iCount) ;

				if (sortOverride == FALSE)
					strcpy(itemList[i].itemText, szinAdd) ;
				else
					sprintf(itemList[i].itemText, "%s[%s]", szinAdd, szinCompare) ;

				itemList[i].itemPage[0] = (int)inItemList[inCount].itemPage[0] ;
				strcpy(itemList[i].itemReference[0], inItemList[inCount].itemReference[0]) ;
				itemList[i].itemPage[1] = -1 ;
				iPageFormat[i][0] = numFormat ;
				iPageNumbering[i] [0] = pageNumbering ;
				iRange[i] [0] = tempRange ;
			}
			else
			{
				szAddTempTabbed[0] = '\0' ;

				addNewItem(i, iCount) ;

				strcpy(itemList[i].itemText, szAddSearchString) ;

				for (j = 0 ; j < 5 ; j++)
				{
					if (szAddTabText[j] [0] != '\0')
					{
						addNewItem(i+j+1, iCount+j+1) ;

						strcat(szAddTempTabbed, "@>") ;

						strcpy(itemList[i+j+1].itemText, szAddTempTabbed) ;
						strcat(itemList[i+j+1].itemText, szAddTabText[j]) ;

						if (sortOverride == TRUE)
						{
							strcat(itemList[i+j+1].itemText, "[") ;
							strcat(itemList[i+j+1].itemText, szinCompare) ;
							strcat(itemList[i+j+1].itemText, "]") ;
						}
					}
					else
					{
						break ;
					}
				}

				itemList[i+j].itemPage[0] = (int)inItemList[inCount].itemPage[0] ;
				itemList[i+j].itemPage[1] = -1 ;
				strcpy(itemList[i+j].itemReference[0], inItemList[inCount].itemReference[0]) ;
				iPageFormat[i+j][0] = numFormat ;
				iPageNumbering[i+j][0] = pageNumbering ;
				iRange[i+j] [0] = tempRange ;
			}
		}
//	}
}

INT  compareSortText(CHAR *text1, CHAR *text2, INT len)
{
	INT i ;
	CHAR temp1[MAXLEN], temp2[MAXLEN], temp3[MAXLEN], temp4[MAXLEN] ;

	convertSpecialCharacters(text1) ;
	convertSpecialCharacters(text2) ;

	// Compare the two strings based on the option currently selected.
	if (g_iSortOption == SORT_LETTER_BY_LETTER)
	{
		stripAllSpaces(text1, temp3) ;
		stripAllSpaces(text2, temp4) ;

		stripAllSpecialChars(temp3, temp1) ;
		stripAllSpecialChars(temp4, temp2) ;

		for (i = 0 ; i < len ; i++)
		{
			if ((i >= (INT)strlen(temp1)) && (i >= (INT)strlen(temp2)))
				break ;

			if (i >= (INT)strlen(temp1))
				return (-1) ;

			if (i >= (INT)strlen(temp2))
				return (1) ;

			if ((temp1[i] < temp2[i]) && (temp1[i] != ' '))
				return (-1) ;

			if (temp1[i] > temp2[i])
				return (1) ;
		}

		return (0) ;
	}
	else
	{
		return (strncmp(text1, text2, len)) ;
	}
}

// Sorts two identical strings whose only differences are in formatting and case.
INT	sortFormattedText(CHAR *inText1, CHAR *inText2, CHAR *inCase1, CHAR *inCase2)
{
	INT	iFormat1[MAXLEN], iFormat2[MAXLEN], currentFormat, i1, j1 ;

	currentFormat = DEFAULT ;

	iFormat1[0] = -1 ;
	iFormat2[0] = -1 ;

	for (i1 = 0, j1 = 0 ; i1 < (INT)strlen(inText1) ; )
	{
		if (strnicmp(&inText1[i1], "<Bold>", 6) == 0)
		{
			if (currentFormat == ITALIC)
				currentFormat = BOLDITALIC ;
			else
				currentFormat = BOLD ;

			i1 += 6 ;
		}
		else
		{
			if (strnicmp(&inText1[i1], "<Emphasis>", 10) == 0)
			{
				if (currentFormat == BOLD)
					currentFormat = BOLDITALIC ;
				else
					currentFormat = ITALIC ;

				i1 += 10 ;
			}
			else
			{
				if (strnicmp(&inText1[i1], "<Default Para Font>", 19) == 0)
				{
					currentFormat = DEFAULT ;

					i1 += 19 ;
				}
				else
				{
					if (strnicmp(&inText1[i1], "</>", 3) == 0)
					{
						currentFormat = DEFAULT ;

						i1 += 3 ;
					}
					else
					{
						i1++ ;

						iFormat1[j1] = currentFormat ;
						iFormat1[j1+1] = -1 ;
						j1++ ;
					}
				}
			}
		}
	}

	currentFormat = DEFAULT ;

	for (i1 = 0, j1 = 0 ; i1 < (INT)strlen(inText2) ; )
	{
		if (strnicmp(&inText2[i1], "<Bold>", 6) == 0)
		{
			if (currentFormat == ITALIC)
				currentFormat = BOLDITALIC ;
			else
				currentFormat = BOLD ;

			i1 += 6 ;
		}
		else
		{
			if (strnicmp(&inText2[i1], "<Emphasis>", 10) == 0)
			{
				if (currentFormat == BOLD)
					currentFormat = BOLDITALIC ;
				else
					currentFormat = ITALIC ;

				i1 += 10 ;
			}
			else
			{
				if (strnicmp(&inText2[i1], "<Default Para Font>", 19) == 0)
				{
					currentFormat = DEFAULT ;

					i1 += 19 ;
				}
				else
				{
					if (strnicmp(&inText2[i1], "</>", 3) == 0)
					{
						currentFormat = DEFAULT ;

						i1 += 3 ;
					}
					else
					{
						i1++ ;

						iFormat2[j1] = currentFormat ;
						iFormat2[j1+1] = -1 ;
						j1++ ;
					}
				}
			}
		}
	}

	// Cope with different cases
	for (i1 = 0 ; (iFormat1[i1] != -1) ; i1++)
		if (isupper(inCase1[i1]))
			iFormat1[i1] -= 4 ;

	for (i1 = 0 ; (iFormat2[i1] != -1) ; i1++)
		if (isupper(inCase2[i1]))
			iFormat2[i1] -= 4 ;

	// Now compare the two formats
	for (i1 = 0 ; (iFormat1[i1] != -1) ; i1++)
	{
		if (iFormat1[i1] > iFormat2[i1])
			break ;

		if (iFormat1[i1] < iFormat2[i1])
		{
			return (-1) ;
		}
	}

	return (0) ;
}

BOOL APIENTRY FindProc( HWND hWnd, UINT message, 
                        WPARAM wParam, LPARAM lParam )
{
	static INT i, lbCount, searchCount ;
	INT j, searchIncrement, searchStop ;
	CHAR	lbText[MAXLEN], findText[MAXLEN], tagText[MAXLEN] ;
	BOOL	caseSensitive, found, searchDown ;

	switch( message )
	{
		case WM_INITDIALOG :
			hFindWindow = hWnd ;

			// Initialize the buttons.
			CheckDlgButton(hWnd, IDC_DOWN, BST_CHECKED) ;
			CheckDlgButton(hWnd, IDC_UP, BST_UNCHECKED) ;

			break ;

		case WM_COMMAND :
			lbCount = SendMessage(LBox, LB_GETCOUNT, 0, 0) ;

			switch( wParam )
			{
				case IDC_DOWN :
					if (IsDlgButtonChecked(hWnd, IDC_DOWN) != BST_CHECKED)
						i += 2 ;

					break ;

				case IDC_UP :
					if (IsDlgButtonChecked(hWnd, IDC_UP) != BST_CHECKED)
						i -= 2 ;

					break ;

				case IDOK :
					// Get the text to search for.
					GetDlgItemText(hWnd, IDC_FINDTEXT, findText, MAXLEN) ;

					if (findText[0] == '\0')
						// Nothing to search for.
						break ;

					// Get the options for the search.
					if (IsDlgButtonChecked(hWnd, IDC_CASE) == BST_CHECKED)
						caseSensitive = TRUE ;
					else
						caseSensitive = FALSE ;

					if (IsDlgButtonChecked(hWnd, IDC_DOWN) == BST_CHECKED)
					{
						searchDown = TRUE ;
						searchIncrement = 1 ;
						searchStop = lbCount + 1 ;
					}
					else
					{
						searchDown = FALSE ;
						searchIncrement = -1 ;
						searchStop = -1 ;
					}

					// If lParam is -1, this is a continuation from the end/beginning of the listbox.
					if (lParam != -1)
					{
						i = SendMessage(LBox, LB_GETCURSEL, 0, 0) ;

						if (searchDown == TRUE)
						{
							i++ ;
	
							if (i >= lbCount)
								i = 0 ;
						}
						else
						{
							i-- ;

							if (i < 0)
								i = (lbCount - 1) ;
						}
					}

					searchCount = 0 ;

					// Loop through the listbox to find the text.
					for ( ; i != searchStop ; i += searchIncrement)
					{
						searchCount++ ;
						found = FALSE ;
						SendMessage(LBox, LB_GETTEXT, i, (LPARAM)lbText) ;

						removeTagText(lbText, tagText) ;

						for (j = 0 ; j < (INT)(strlen(tagText)-strlen(findText)+1) ; j++)
						{
							if (caseSensitive == TRUE)
							{
								if (strncmp(findText, &tagText[j], strlen(findText)) == 0)
								{
									found = TRUE ;
									searchCount = 0 ;
									break ;
								}
							}
							else
							{
								if (strnicmp(findText, &tagText[j], strlen(findText)) == 0)
								{
									found = TRUE ;
									searchCount = 0 ;
									break ;
								}
							}
						}

						if (found == TRUE)
						{
							// Set the current selection and find the marker in the document.
							SendMessage(LBox, LB_SETCURSEL, i, 0) ;
	
							sendDDEMessage(GetParent(hWnd), ITEMFIND) ;

							SetFocus(hWnd) ;
							SetForegroundWindow(hWnd);
	
							break ;
						}
					}

					if (searchCount >= (lbCount-1))
					{
						MessageBox(hWnd, "Text not found", "emDEX", MB_OK) ;
						break ;
					}

					if (i == searchStop)
					{
						// End/beginning of listbox reached, so start again.
						if (searchDown == TRUE)
							i = 0 ;
						else
							i = (lbCount-1) ;

						SendMessage(hWnd, WM_COMMAND, IDOK, -1) ;
					}

					break ;


				case IDCANCEL :
					hFindWindow = NULL ;

					EndDialog(hWnd, 0) ;
		
					break;
			}

			break;

		default :
			break ;
	}

	return (FALSE);
}

BOOL APIENTRY ReplaceProc( HWND hWnd, UINT message, 
                        WPARAM wParam, LPARAM lParam )
{
	static INT i, lbCount, searchCount ;
	CHAR	findText[MAXLEN], replaceText[MAXLEN] ;

	switch( message )
	{
		case WM_INITDIALOG :
			hReplaceWindow = hWnd ;

			SendMessage(GetDlgItem(hWnd, IDC_FINDTEXT), EM_SETLIMITTEXT, MAXSTRING-1, 0) ;
			SendMessage(GetDlgItem(hWnd, IDC_REPLACETEXT), EM_SETLIMITTEXT, MAXSTRING-1, 0) ;

			break ;

		case WM_COMMAND :
			switch( wParam )
			{
				case IDOK :
					// Get the text to search for.
					GetDlgItemText(hWnd, IDC_FINDTEXT, findText, MAXLEN) ;
					GetDlgItemText(hWnd, IDC_REPLACETEXT, replaceText, MAXLEN) ;

					if ((findText[0] == '\0') || (replaceText[0] == '\0'))
						// Nothing to search for.
						break ;

					if ((strlen(findText) >= MAXSTRING) || (strlen(replaceText) >= MAXSTRING))
					{
						// String too long.
						MessageBox(hWnd, "Marker text is too long. Marker text is limited to 255 characters.", "emDEX", MB_ICONHAND | MB_APPLMODAL) ;
						break ;
					}

					if (IsDlgButtonChecked(hWnd, IDC_CASE) == BST_CHECKED)
						sendDDEMessage(GetParent(hWnd), ITEMREPLACE, findText, replaceText, TRUE) ;
					else
						sendDDEMessage(GetParent(hWnd), ITEMREPLACE, findText, replaceText, FALSE) ;

					EndDialog(hWnd, 0) ;
		
					break;

				case IDCANCEL :
					hReplaceWindow = NULL ;
					EndDialog(hWnd, 0) ;
		
					break;

			}
			break;

		default :
			break ;
	}

	return (FALSE);
}

BOOL APIENTRY IgnoreCharsProc( HWND hWnd, UINT message, 
                        WPARAM wParam, LPARAM lParam )
{
	static CHAR temp[MAXLEN] ;
	HWND	nextWindow ;
	static BOOL	firstTime ;

	switch( message )
	{
		case WM_INITDIALOG :
			hIgnoreWindow = hWnd ;

			SendMessage(GetDlgItem(hWnd, IDC_IGNORECHARS), EM_SETLIMITTEXT, MAXLEN-1, 0) ;

			strcpy(temp, &szIgnoreChars[1]) ;

			SetDlgItemText(hWnd, IDC_IGNORECHARS, temp) ;

			firstTime = TRUE ;

			break ;

		case WM_COMMAND :
			switch( wParam )
			{
				case IDOK :
					GetDlgItemText(hWnd, IDC_IGNORECHARS, temp, MAXLEN) ;

					szIgnoreChars[0] = '\0' ;
					strcpy(szIgnoreChars, "-") ;
					strcat(szIgnoreChars, temp) ;

					WritePrivateProfileString("Options", "IgnoreChars", temp, "emDEX.ini");

					// Tell all catcher windows that an option has changed.
					nextWindow = GetWindow(GetDesktopWindow(), GW_CHILD) ;

					// Send the change message to all catcher windows.
					while(nextWindow != NULL)
					{
						GetWindowText(nextWindow, temp, 80) ;

						if (strncmp(temp, "emDEX - ", 8) == 0)
							SendMessage(nextWindow, CHANGEOPTION, 0, 0) ;

						nextWindow = GetNextWindow(nextWindow, GW_HWNDNEXT) ;
					}

					EndDialog(hWnd, 0) ;

					break ;

				case IDCANCEL :
					hIgnoreWindow = NULL ;

					EndDialog(hWnd, 0) ;
		
					break;

				default :
					if (firstTime == TRUE)
					{
						// Remove any selection (it can't be done on init because it doesn't exist yet.
						SendDlgItemMessage(hWnd, IDC_IGNORECHARS, EM_SETSEL, (WPARAM)-1, (LPARAM)0);
						firstTime = FALSE ;
					}

					break ;
			}

			break;

		default :
			break ;
	}

	return (FALSE);
}

BOOL APIENTRY ButtonProc( HWND hWnd, UINT message, 
                        WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
		case WM_INITDIALOG :
			hButtonWindow = hWnd ;

			hHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc,
                                      NULL, GetCurrentThreadId());

			// Get the button text/actions from the ini file
			GetPrivateProfileString("Options", "Button0Text", "&Bold", buttons[0].text, 800, "emDEX.ini") ;
			GetPrivateProfileString("Options", "Button0Action", "<Bold>", buttons[0].action, 800, "emDEX.ini") ;
			GetPrivateProfileString("Options", "Button1Text", "&Emphasis", buttons[1].text, 800, "emDEX.ini") ;
			GetPrivateProfileString("Options", "Button1Action", "<Emphasis>", buttons[1].action, 800, "emDEX.ini") ;
			GetPrivateProfileString("Options", "Button2Text", "&Default", buttons[2].text, 800, "emDEX.ini") ;
			GetPrivateProfileString("Options", "Button2Action", "</>", buttons[2].action, 800, "emDEX.ini") ;
			GetPrivateProfileString("Options", "Button3Text", "&No Page", buttons[3].text, 800, "emDEX.ini") ;
			GetPrivateProfileString("Options", "Button3Action", "<$nopage>", buttons[3].action, 800, "emDEX.ini") ;
			GetPrivateProfileString("Options", "Button4Text", "&Page", buttons[4].text, 800, "emDEX.ini") ;
			GetPrivateProfileString("Options", "Button4Action", "<$singlepage>", buttons[4].action, 800, "emDEX.ini") ;
			GetPrivateProfileString("Options", "Button5Text", "See &Also", buttons[5].text, 800, "emDEX.ini") ;
			GetPrivateProfileString("Options", "Button5Action", "<$nopage><Emphasis>see also </>", buttons[5].action, 800, "emDEX.ini") ;
			GetPrivateProfileString("Options", "Button6Text", "&See", buttons[6].text, 800, "emDEX.ini") ;
			GetPrivateProfileString("Options", "Button6Action", "<$nopage><Emphasis>see </>", buttons[6].action, 800, "emDEX.ini") ;
			GetPrivateProfileString("Options", "Button7Text", "s&tartrange", buttons[7].text, 800, "emDEX.ini") ;
			GetPrivateProfileString("Options", "Button7Action", "<$startrange>", buttons[7].action, 800, "emDEX.ini") ;
			GetPrivateProfileString("Options", "Button8Text", "end&range", buttons[8].text, 800, "emDEX.ini") ;
			GetPrivateProfileString("Options", "Button8Action", "<$endrange>", buttons[8].action, 800, "emDEX.ini") ;
			GetPrivateProfileString("Options", "Button9Text", "", buttons[9].text, 800, "emDEX.ini") ;
			GetPrivateProfileString("Options", "Button9Action", "", buttons[9].action, 800, "emDEX.ini") ;

			SetDlgItemText(hWnd, IDC_DISPLAY1, buttons[0].text) ;
			SetDlgItemText(hWnd, IDC_ACTION1, buttons[0].action) ;
			SetDlgItemText(hWnd, IDC_DISPLAY2, buttons[1].text) ;
			SetDlgItemText(hWnd, IDC_ACTION2, buttons[1].action) ;
			SetDlgItemText(hWnd, IDC_DISPLAY3, buttons[2].text) ;
			SetDlgItemText(hWnd, IDC_ACTION3, buttons[2].action) ;
			SetDlgItemText(hWnd, IDC_DISPLAY4, buttons[3].text) ;
			SetDlgItemText(hWnd, IDC_ACTION4, buttons[3].action) ;
			SetDlgItemText(hWnd, IDC_DISPLAY5, buttons[4].text) ;
			SetDlgItemText(hWnd, IDC_ACTION5, buttons[4].action) ;
			SetDlgItemText(hWnd, IDC_DISPLAY6, buttons[5].text) ;
			SetDlgItemText(hWnd, IDC_ACTION6, buttons[5].action) ;
			SetDlgItemText(hWnd, IDC_DISPLAY7, buttons[6].text) ;
			SetDlgItemText(hWnd, IDC_ACTION7, buttons[6].action) ;
			SetDlgItemText(hWnd, IDC_DISPLAY8, buttons[7].text) ;
			SetDlgItemText(hWnd, IDC_ACTION8, buttons[7].action) ;
			SetDlgItemText(hWnd, IDC_DISPLAY9, buttons[8].text) ;
			SetDlgItemText(hWnd, IDC_ACTION9, buttons[8].action) ;
			SetDlgItemText(hWnd, IDC_DISPLAY10, buttons[9].text) ;
			SetDlgItemText(hWnd, IDC_ACTION10, buttons[9].action) ;

			ShowWindow(hWnd, SW_SHOW) ;
			SetFocus(hWnd) ;
			SetForegroundWindow(hWnd);

			break ;

		case WM_COMMAND :
			switch( wParam )
			{
				case IDOK :
					GetDlgItemText(hWnd, IDC_DISPLAY1, buttons[0].text, MAXLEN) ;
					GetDlgItemText(hWnd, IDC_ACTION1, buttons[0].action, MAXLEN) ;
					GetDlgItemText(hWnd, IDC_DISPLAY2, buttons[1].text, MAXLEN) ;
					GetDlgItemText(hWnd, IDC_ACTION2, buttons[1].action, MAXLEN) ;
					GetDlgItemText(hWnd, IDC_DISPLAY3, buttons[2].text, MAXLEN) ;
					GetDlgItemText(hWnd, IDC_ACTION3, buttons[2].action, MAXLEN) ;
					GetDlgItemText(hWnd, IDC_DISPLAY4, buttons[3].text, MAXLEN) ;
					GetDlgItemText(hWnd, IDC_ACTION4, buttons[3].action, MAXLEN) ;
					GetDlgItemText(hWnd, IDC_DISPLAY5, buttons[4].text, MAXLEN) ;
					GetDlgItemText(hWnd, IDC_ACTION5, buttons[4].action, MAXLEN) ;
					GetDlgItemText(hWnd, IDC_DISPLAY6, buttons[5].text, MAXLEN) ;
					GetDlgItemText(hWnd, IDC_ACTION6, buttons[5].action, MAXLEN) ;
					GetDlgItemText(hWnd, IDC_DISPLAY7, buttons[6].text, MAXLEN) ;
					GetDlgItemText(hWnd, IDC_ACTION7, buttons[6].action, MAXLEN) ;
					GetDlgItemText(hWnd, IDC_DISPLAY8, buttons[7].text, MAXLEN) ;
					GetDlgItemText(hWnd, IDC_ACTION8, buttons[7].action, MAXLEN) ;
					GetDlgItemText(hWnd, IDC_DISPLAY9, buttons[8].text, MAXLEN) ;
					GetDlgItemText(hWnd, IDC_ACTION9, buttons[8].action, MAXLEN) ;
					GetDlgItemText(hWnd, IDC_DISPLAY10, buttons[9].text, MAXLEN) ;
					GetDlgItemText(hWnd, IDC_ACTION10, buttons[9].action, MAXLEN) ;

					WritePrivateProfileString("Options", "Button0Text", buttons[0].text, "emDEX.ini");
					WritePrivateProfileString("Options", "Button0Action", buttons[0].action, "emDEX.ini");
					WritePrivateProfileString("Options", "Button1Text", buttons[1].text, "emDEX.ini");
					WritePrivateProfileString("Options", "Button1Action", buttons[1].action, "emDEX.ini");
					WritePrivateProfileString("Options", "Button2Text", buttons[2].text, "emDEX.ini");
					WritePrivateProfileString("Options", "Button2Action", buttons[2].action, "emDEX.ini");
					WritePrivateProfileString("Options", "Button3Text", buttons[3].text, "emDEX.ini");
					WritePrivateProfileString("Options", "Button3Action", buttons[3].action, "emDEX.ini");
					WritePrivateProfileString("Options", "Button4Text", buttons[4].text, "emDEX.ini");
					WritePrivateProfileString("Options", "Button4Action", buttons[4].action, "emDEX.ini");
					WritePrivateProfileString("Options", "Button5Text", buttons[5].text, "emDEX.ini");
					WritePrivateProfileString("Options", "Button5Action", buttons[5].action, "emDEX.ini");
					WritePrivateProfileString("Options", "Button6Text", buttons[6].text, "emDEX.ini");
					WritePrivateProfileString("Options", "Button6Action", buttons[6].action, "emDEX.ini");
					WritePrivateProfileString("Options", "Button7Text", buttons[7].text, "emDEX.ini");
					WritePrivateProfileString("Options", "Button7Action", buttons[7].action, "emDEX.ini");
					WritePrivateProfileString("Options", "Button8Text", buttons[8].text, "emDEX.ini");
					WritePrivateProfileString("Options", "Button8Action", buttons[8].action, "emDEX.ini");
					WritePrivateProfileString("Options", "Button9Text", buttons[9].text, "emDEX.ini");
					WritePrivateProfileString("Options", "Button9Action", buttons[9].action, "emDEX.ini");

					UnhookWindowsHookEx( hHook );
					DestroyWindow(hWnd) ;

					hButtonWindow = NULL ;

//					EndDialog(hWnd, 0) ;

					break ;

				case IDCANCEL :
					UnhookWindowsHookEx( hHook );
					DestroyWindow(hWnd) ;

					hButtonWindow = NULL ;

//					EndDialog(hWnd, 0) ;
		
					break;

				default :
					break ;
			}

			break;

/*		case WM_DESTROY :
			UnhookWindowsHookEx( hHook );
			DestroyWindow(hWnd) ;
			hButtonWindow = NULL ;
			break ; */

		default :
			break ;
	}

	return (FALSE);
}

LRESULT FAR PASCAL GetMsgProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	LPMSG lpMsg = (LPMSG) lParam ;

	if (nCode >= 0 && PM_REMOVE == wParam)
	{
		// Don't translate non-input events.
		if ((lpMsg->message >= WM_KEYFIRST && lpMsg->message <= WM_KEYLAST))
		{
			if (IsDialogMessage(hButtonWindow, lpMsg))
			{
				// The value returned from this hookproc is ignored, 
				// and it cannot be used to tell Windows the message has been handled.
				// To avoid further processing, convert the message to WM_NULL 
				// before returning.
				lpMsg->message = WM_NULL;
				lpMsg->lParam  = 0;
				lpMsg->wParam  = 0;
			}
		}
	}

	return CallNextHookEx(hHook, nCode, wParam, lParam) ;
}

BOOL APIENTRY RangeSeparatorsProc( HWND hWnd, UINT message, 
                        WPARAM wParam, LPARAM lParam )
{
	static CHAR temp[MAXLEN] ;
	HWND	nextWindow ;
	static BOOL	firstTime ;

	switch( message )
	{
		case WM_INITDIALOG :
			hSeparatorWindow = hWnd ;

			SendMessage(GetDlgItem(hWnd, IDC_SEPARATORCHARS), EM_SETLIMITTEXT, MAXLEN-1, 0) ;

			SetDlgItemText(hWnd, IDC_SEPARATORCHARS, szSeparatorChars) ;

			firstTime = TRUE ;

			break ;

		case WM_COMMAND :
			switch( wParam )
			{
				case IDOK :
					GetDlgItemText(hWnd, IDC_SEPARATORCHARS, temp, MAXLEN) ;

					if (temp[0] == '\0')
					{
						MessageBox(hWnd, "Range separator text must be specified.", "emDEX", MB_ICONHAND | MB_APPLMODAL) ;
						break ;
					}

					szSeparatorChars[0] = '\0' ;
					stripSpaces(temp) ;
					strcpy(szSeparatorChars, temp) ;

					WritePrivateProfileString("Options", "SeparatorChars", szSeparatorChars, "emDEX.ini");

					// Tell all catcher windows that an option has changed.
					nextWindow = GetWindow(GetDesktopWindow(), GW_CHILD) ;

					// Send the change message to all catcher windows.
					while(nextWindow != NULL)
					{
						GetWindowText(nextWindow, temp, 80) ;

						if (strncmp(temp, "emDEX - ", 8) == 0)
							SendMessage(nextWindow, CHANGEOPTION, 0, 0) ;

						nextWindow = GetNextWindow(nextWindow, GW_HWNDNEXT) ;
					}

					EndDialog(hWnd, 0) ;

					break ;

				case IDCANCEL :
					hSeparatorWindow = NULL ;

					EndDialog(hWnd, 0) ;
		
					break;

				default :
					if (firstTime == TRUE)
					{
						// Remove any selection (it can't be done on init because it doesn't exist yet.
						SendDlgItemMessage(hWnd, IDC_SEPARATORCHARS, EM_SETSEL, (WPARAM)-1, (LPARAM)0);
						firstTime = FALSE ;
					}

					break ;
			}

			break;

		default :
			break ;
	}

	return (FALSE);
}

BOOL APIENTRY AboutProc( HWND hWnd, UINT message, 
                            WPARAM wParam, LPARAM lParam )
{
	switch( message )
	{
		case WM_INITDIALOG :
			hAboutBox = hWnd ;

			if (VERSION == DEMO)
			{
				ShowWindow(GetDlgItem(hWnd, IDC_ABOUT_RELEASE), SW_HIDE) ;
				ShowWindow(GetDlgItem(hWnd, IDC_ABOUT_DEMO), SW_SHOW) ;
			}
			else
			{
				ShowWindow(GetDlgItem(hWnd, IDC_ABOUT_RELEASE), SW_SHOW) ;
				ShowWindow(GetDlgItem(hWnd, IDC_ABOUT_DEMO), SW_HIDE) ;
			}

			break ;

		case WM_COMMAND :
			switch( wParam )
			{
				case IDOK :
				case IDCANCEL :
					bAboutBoxDisplayed = FALSE ;

					EndDialog(hWnd, 0) ;
		
					break;
			}

			break;

		default :
			break ;
	}

	return (FALSE);
}

HDDEDATA CALLBACK DdeCallBack(UINT type, UINT format, HCONV hconv,
	HSZ hString1, HSZ hString2, HDDEDATA hData, DWORD dwData1,
	DWORD dwData2)
{
	/* Need a DDE callback, even if there's nothing to do */
	return 0;
}

void addNewItem(int insertAt, int itemCount)
{
	int j, k ;

	// Insert a new item list entry.
	for (j = itemCount ; j > insertAt ; j--)
	{
		strcpy(itemList[j].itemText, itemList[j-1].itemText) ;

		for (k = 0 ; itemList[j-1].itemPage[k] != -1 ; k++)
		{
			itemList[j].itemPage[k] = itemList[j-1].itemPage[k] ;
			strcpy(itemList[j].itemReference[k], itemList[j-1].itemReference[k]) ;
			iPageFormat[j][k] = iPageFormat[j-1][k] ;
			iPageNumbering[j] [k] = iPageNumbering[j-1] [k] ;
			iRange[j][k] = iRange[j-1][k] ;
		}

		itemList[j].itemPage[k] = -1 ;
	}

	itemList[insertAt].itemText[0] = '\0' ;
	itemList[insertAt].itemPage[0] = -1 ;

	itemList[insertAt].itemReference[0] [0] = '\0' ;

	itemList[itemCount+1].itemText[0] = '\0' ;
	itemList[itemCount+1].itemPage[0] = -1 ;
	itemList[itemCount+1].itemReference[0] [0] = '\0' ;
}

void sendDDEMessage(HWND dlg, int command)
{
	CHAR	chCommand ;
	HSZ		appHandle, topicHandle ;
	INT		selected ;
	char	szTempString1[MAXLEN] ;
	CHAR	tempFileName[MAXLEN] ;
	HDDEDATA hData ;
	LONG	lTimeout = 10 ;
	CHAR	str[8] ;

//	writeToDebugFile("13") ;

	// Send a message to the DLL.
	switch (command)
	{
		case ITEMDELETE :
			chCommand = 'D' ;
			lTimeout = 10000 ;
			break ;

		case ITEMEDIT :
			chCommand = 'E' ;
			break ;

		case ITEMFIND :
			chCommand = 'F' ;
			break ;

		case ITEMREPLACE :
			chCommand = 'R' ;
			break ;

		case DOITEMUPDATE :
			chCommand = 'U' ;
			break ;

		case REALTIME :
			chCommand = 'T' ;
			break ;
	}

	str[0] = chCommand ;
	str[1] = '\0' ;
//	writeToDebugFile(str) ;

	if (!convId)
	{
		/* Fetch and tokenize application and topic name */
		appHandle = DdeCreateStringHandle(ddeInst, "emDEX", CP_WINANSI);
		topicHandle = DdeCreateStringHandle(ddeInst, "Session", CP_WINANSI);

		convId = DdeConnect(ddeInst, appHandle, topicHandle, NULL);

		DdeFreeStringHandle(ddeInst, appHandle);
		DdeFreeStringHandle(ddeInst, topicHandle);
	}

	if (convId)
	{
//		char cmd[512];
		char tempText[MAXLEN] ;
		DWORD res;

		selected = SendMessage(LBox, LB_GETCURSEL, 0, 0) ;

		if (selected != LB_ERR)
		{
			strncpy(tempFileName, &g_FileName[1], strlen(g_FileName)-2) ;
			tempFileName[strlen(g_FileName)-2] = '\0' ;

			if (command == REALTIME)
			{
				sprintf(szTempString1, "%c@$", chCommand) ;
			}
			else
			{
				if (command == DOITEMUPDATE)
				{
					sprintf(szTempString1, "%c%s@!@$", chCommand, tempFileName) ;
				}
				else
				{
					if (strncmp(itemList[selected].itemText, "@>", 2) != 0)
					{
						sprintf(szTempString1, "%c%s@!%s@$", chCommand, tempFileName, itemList[selected].itemText) ;
					}
					else
					{
						// If this is a tabbed entry, we need to get the full text for the original entry.
						getFullItemText(selected, tempText) ;
	
						sprintf(szTempString1, "%c%s@!%s@$", chCommand, tempFileName, tempText) ;
					}
				}
			}

			if (szTempString1[0] != '\0')
			{
//				writeToDebugFile("14") ;

				/* Get command text and invoke "execute" transaction */
				hData = DdeClientTransaction((UCHAR *)szTempString1, strlen(szTempString1), convId, 0, 0,
						XTYP_EXECUTE, /*TIMEOUT_ASYNC*/lTimeout, &res) ;

//				writeToDebugFile("15") ;

				if (hData != 0)
					DdeFreeDataHandle(hData) ;

//				writeToDebugFile("16") ;
//				writeToDebugFile("\n") ;

/*				if (!DdeClientTransaction((UCHAR *)szTempString1, strlen(szTempString1), convId, 0, 0,
						XTYP_EXECUTE, 10000, &res))
					errMsg(dlg, MB_OK, "Problem communicating with FrameMaker document: %s", cmd);
				if (LOWORD(res) != DDE_FACK)
					errMsg(dlg, MB_OK, "Problem updating FrameMaker document: %x", res); */
			}
		} 
	}
}

// Overloaded sendDDEMessage() for find and replace command.
void sendDDEMessage(HWND dlg, int command, CHAR *findText, CHAR *replaceText, BOOL isCaseSensitive)
{
	CHAR	chCommand ;
	HSZ		appHandle, topicHandle ;
	char	szTempString1[MAXLEN] ;
	CHAR	tempFileName[MAXLEN] ;
	HDDEDATA hData ;

//	writeToDebugFile("1") ;

	chCommand = 'R' ;

	if (!convId)
	{
		/* Fetch and tokenize application and topic name */
		appHandle = DdeCreateStringHandle(ddeInst, "emDEX", CP_WINANSI);
		topicHandle = DdeCreateStringHandle(ddeInst, "Session", CP_WINANSI);

		convId = DdeConnect(ddeInst, appHandle, topicHandle, NULL);

		DdeFreeStringHandle(ddeInst, appHandle);
		DdeFreeStringHandle(ddeInst, topicHandle);
	}

	if (convId)
	{
//		char cmd[512];
		DWORD res;

		strncpy(tempFileName, &g_FileName[1], strlen(g_FileName)-2) ;
		tempFileName[strlen(g_FileName)-2] = '\0' ;

		sprintf(szTempString1, "%c%s@!%s@!%s@!%d", chCommand, tempFileName, findText, replaceText, isCaseSensitive) ;

		if (szTempString1[0] != '\0')
		{
//			writeToDebugFile("2") ;
				
			/* Get command text and invoke "execute" transaction */
			hData = DdeClientTransaction((UCHAR *)szTempString1, strlen(szTempString1), convId, 0, 0,
					XTYP_EXECUTE, /*10*/TIMEOUT_ASYNC , &res) ;

//			writeToDebugFile("3") ;

			if (hData != 0)
				DdeFreeDataHandle(hData) ;

//			writeToDebugFile("4") ;
//			writeToDebugFile("\n") ;

/*			if (!DdeClientTransaction((UCHAR *)szTempString1, strlen(szTempString1), convId, 0, 0,
					XTYP_EXECUTE, 10000, &res))
				errMsg(dlg, MB_OK, "Problem communicating with FrameMaker document: %s", cmd);
			if (LOWORD(res) != DDE_FACK)
				errMsg(dlg, MB_OK, "Problem updating FrameMaker document: %x", res); */
		}
	}
}

// Overloaded sendDDEMessage() for find with a page number.
void sendDDEMessage(HWND dlg, int command, INT pageNum, CHAR *referenceText)
{
	CHAR	chCommand ;
	HSZ		appHandle, topicHandle ;
	char	szTempString1[MAXLEN] ;
	CHAR	tempFileName[MAXLEN] ;
	INT		selected ;
	HDDEDATA hData ;

//	writeToDebugFile("5") ;

	chCommand = 'P' ;

	if (!convId)
	{
		/* Fetch and tokenize application and topic name */
		appHandle = DdeCreateStringHandle(ddeInst, "emDEX", CP_WINANSI);
		topicHandle = DdeCreateStringHandle(ddeInst, "Session", CP_WINANSI);

		convId = DdeConnect(ddeInst, appHandle, topicHandle, NULL);

		DdeFreeStringHandle(ddeInst, appHandle);
		DdeFreeStringHandle(ddeInst, topicHandle);
	}

	if (convId)
	{
//		char cmd[512];
		DWORD res;
		char tempText[MAXLEN] ;

		selected = SendMessage(LBox, LB_GETCURSEL, 0, 0) ;

		if (selected != LB_ERR)
		{
			strncpy(tempFileName, &g_FileName[1], strlen(g_FileName)-2) ;
			tempFileName[strlen(g_FileName)-2] = '\0' ;

			if (strncmp(itemList[selected].itemText, "@>", 2) != 0)
			{
				sprintf(szTempString1, "%c%s@!%s@!%d@!%s@$", chCommand, tempFileName, itemList[selected].itemText, pageNum, referenceText) ;
			}
			else
			{
				// If this is a tabbed entry, we need to get the full text for the original entry.
				getFullItemText(selected, tempText) ;

				sprintf(szTempString1, "%c%s@!%s@!%d@!%s@$", chCommand, tempFileName, tempText, pageNum, referenceText) ;
			}

			if (szTempString1[0] != '\0')
			{
//				writeToDebugFile("6") ;

				/* Get command text and invoke "execute" transaction */
				hData = DdeClientTransaction((UCHAR *)szTempString1, strlen(szTempString1), convId, 0, 0,
						XTYP_EXECUTE, /*10*/TIMEOUT_ASYNC , &res) ;

//				writeToDebugFile("7") ;

				if (hData != 0)
					DdeFreeDataHandle(hData) ;

//				writeToDebugFile("8") ;
//				writeToDebugFile("\n") ;

/*				if (!DdeClientTransaction((UCHAR *)szTempString1, strlen(szTempString1), convId, 0, 0,
						XTYP_EXECUTE, 10000, &res))
					errMsg(dlg, MB_OK, "Problem communicating with FrameMaker document: %s", cmd);
				if (LOWORD(res) != DDE_FACK)
					errMsg(dlg, MB_OK, "Problem updating FrameMaker document: %x", res); */
			}
		}
	}
}

void sendDDEMessage(HWND dlg, int command, LONG idNum, CHAR *referenceText)
{
	CHAR	chCommand ;
	HSZ		appHandle, topicHandle ;
	CHAR	szTempString1[MAXLEN] ;
	CHAR	tempFileName[MAXLEN] ;
	HDDEDATA hData ;

//	writeToDebugFile("9") ;

	chCommand = 'C' ;

	if (!convId)
	{
		/* Fetch and tokenize application and topic name */
		appHandle = DdeCreateStringHandle(ddeInst, "emDEX", CP_WINANSI);
		topicHandle = DdeCreateStringHandle(ddeInst, "Session", CP_WINANSI);

		convId = DdeConnect(ddeInst, appHandle, topicHandle, NULL);

		DdeFreeStringHandle(ddeInst, appHandle);
		DdeFreeStringHandle(ddeInst, topicHandle);
	}

	if (convId)
	{
		DWORD res;

		strncpy(tempFileName, &g_FileName[1], strlen(g_FileName)-2) ;
		tempFileName[strlen(g_FileName)-2] = '\0' ;

		sprintf(szTempString1, "%c%s@!%d@!%s@$", chCommand, tempFileName, idNum, referenceText) ;

		if (szTempString1[0] != '\0')
		{
//			writeToDebugFile("10") ;

			/* Get command text and invoke "execute" transaction */
			hData = DdeClientTransaction((UCHAR *)szTempString1, strlen(szTempString1), convId, 0, 0,
					XTYP_EXECUTE, /*10*/ TIMEOUT_ASYNC , &res) ;

//			writeToDebugFile("11") ;

			if (hData != 0)
				DdeFreeDataHandle(hData) ;

//			writeToDebugFile("12") ;
//			writeToDebugFile("\n") ;

/*			if (!DdeClientTransaction((UCHAR *)szTempString1, strlen(szTempString1), convId, 0, 0,
					XTYP_EXECUTE, 10000, &res))
				errMsg(dlg, MB_OK, "Problem communicating with FrameMaker document: %s", cmd);
			if (LOWORD(res) != DDE_FACK)
				errMsg(dlg, MB_OK, "Problem updating FrameMaker document: %x", res); */
		}
	}
}

void getUnformattedText(CHAR *inText, CHAR *outText)
{
	INT i, j ;

	// Gets the item text without any tags.
	for (i = 0, j = 0 ; i < (INT)strlen(inText) ; i++)
	{
		if (inText[i] == '<')
		{
			while ((inText[i] != '>') && (i < (INT)strlen(inText)))
			{
				i++ ;
			}
		}
		else
		{
			if (inText[i] == '[')
			{
				while ((inText[i] != ']') && (i < (INT)strlen(inText)))
				{
					i++ ;
				}
			}
			else
			{
				outText[j] = inText[i] ;
				outText[j+1] = '\0' ;
				j++ ;
			}
		}
	}
}

void getFullItemText(INT itemStart, CHAR *itemText)
{
	INT tabCount = 0, tempTabCount = 0, j ;
	CHAR tempText[MAXLEN], t2[MAXLEN] ;

	// Get the tab level.
	tabCount = getTabCount(itemList[itemStart].itemText) ;

	strcpy(itemText, &itemList[itemStart].itemText[tabCount*2]) ;

	// Move up the list, creating the full text for the entry (also remove @>'s).
	for (j = (itemStart-1) ; j >= 0 ; j--)
	{
		tempTabCount = getTabCount(itemList[j].itemText) ;

		if (tempTabCount == (tabCount-1))
		{
			strcpy(tempText, itemText) ;

			itemText[0] = '\0' ;

//			stripPageFormatting(&itemList[j].itemText[tempTabCount*2], t2) ;
			strcpy(t2, &itemList[j].itemText[tempTabCount*2]) ;

//			sprintf(itemText, "%s:%s", &itemList[j].itemText[tempTabCount*2], tempText) ;
			sprintf(itemText, "%s:%s", t2, tempText) ;

			tabCount-- ;
		}

		if (tempTabCount == 0)
			break ;
	}
}

void getFullItemTextArray(INT itemStart, CHAR *text[])
{
	INT tabCount = 0, tempTabCount = 0, j ;
	CHAR t2[MAXLEN] ;

	for (j = 0 ; j < 6 ; j++)
		text[j] [0] = '\0' ;

	// Get the tab level.
	tabCount = getTabCount(itemList[itemStart].itemText) ;

	strcpy(text[tabCount], &itemList[itemStart].itemText[tabCount*2]) ;

	convertSpecialCharacters(text[tabCount]) ;
	stripSpaces(text[tabCount]) ;

	// Move up the list, creating the full text for the entry (also remove @>'s).
	for (j = (itemStart-1) ; j >= 0 ; j--)
	{
		tempTabCount = getTabCount(itemList[j].itemText) ;

		if (tempTabCount == (tabCount-1))
		{
//			stripPageFormatting(&itemList[j].itemText[tempTabCount*2], t2) ;
			strcpy(t2, &itemList[j].itemText[tempTabCount*2]) ;

//			sprintf(itemText, "%s:%s", &itemList[j].itemText[tempTabCount*2], tempText) ;
			tabCount-- ;

			strcpy(text[tabCount], t2) ;
			convertSpecialCharacters(text[tabCount]) ;
			stripSpaces(text[tabCount]) ;
		}

		if (tempTabCount == 0)
			break ;
	}
}

INT  getTabCount(CHAR *itemText)
{
	INT tabCount = 0, i ;

	// Find the number of tabs for the text.
	for (i = 0 ; i < (INT)strlen(itemText) ; i += 2)
	{
		if (strncmp(&itemText[i], "@>", 2) != 0)
			break ;
		else
			tabCount++ ;
	}

	return (tabCount) ;
}

INT	getNumberFormatting(CHAR *inString)
{
	INT		i, iEnd, currentFormat = DEFAULT ;
	CHAR	szFormat[80] ;

	// Get any formatting tags for the page numbers.
	for (i = (strlen(inString)-1) ; i >= 0 ; i--)
	{
		while (inString[i] == ' ')
			i-- ;

		if (inString[i] == ']')
		{
			while (inString[i] != '[')
			{
				i-- ;

				if (i < 0)
					return (currentFormat) ;
			}

			i-- ;
		}

		while (inString[i] == ' ')
			i-- ;

		if ((inString[i] != '>') && (currentFormat == DEFAULT))
			return (currentFormat) ;

		while (inString[i] != '>')
		{
			i-- ;

			if (i < 0)
				return (currentFormat) ;
		}

		iEnd = i ;

		while (inString[i] != '<')
		{
			i-- ;

			if (i < 0)
				return (currentFormat) ;
		}

		strncpy(szFormat, &inString[i], iEnd-i+1) ;
		szFormat[iEnd-i+1] = '\0' ;

		if (strcmp(szFormat, "<Bold>") == 0)
		{
			if ((currentFormat == ITALIC) || (currentFormat == BOLDITALIC))
				currentFormat = BOLDITALIC ;
			else
				currentFormat = BOLD ;
		}

		if (strcmp(szFormat, "<Emphasis>") == 0)
		{
			if ((currentFormat == BOLD) || (currentFormat == BOLDITALIC))
				currentFormat = BOLDITALIC ;
			else
				currentFormat = ITALIC ;
		}

		if ((stricmp(szFormat, "<Default Para Font>") == 0) ||
			(stricmp(szFormat, "</>") == 0))
		{
			return (currentFormat) ;
		}
	}

	return (currentFormat) ;
}

BOOL getCompareText(CHAR *inText, CHAR *outText)
{
	INT i, j, iStart ;

	outText[0] = '\0' ;

	// Strip out formatting to retrieve text to compare with.
	for (i = 0, j = 0 ; i < (INT)strlen(inText) ; i++)
	{
		if ((inText[i] == '<')/* && (inText[i+1] == '$')*/)
		{
			while ((inText[i] != '>') && (i < (INT)strlen(inText)))
			{
				i++ ;
			}
		}
		else
		{
			if ((inText[i] == '[') && (isEscapeSequence(inText, i) == FALSE))
			{
				iStart = i+1 ;

				while (inText[i] != ']')
				{
					i++ ;

					if (i >= (INT)strlen(inText))
						return (FALSE) ;
				}

				strncpy(outText, &inText[iStart], i - iStart) ;
				outText[i-iStart] = '\0' ;

				stripSpaces(outText) ;

				if (outText[strlen(outText)-1] == ':')
					strcat(outText, "aaa") ;

				return (TRUE) ;
			}
			else
			{
				outText[j] = inText[i] ;
				outText[j+1] = '\0' ;
				j++ ;
			}
		}
	}

	stripSpaces(outText) ;

	stripIgnoreChars(outText) ;

	return (FALSE) ;
}

VOID getAddText(CHAR *inText, CHAR *outText)
{
	INT i, j ;

	// Get the text without any [] tags.
	for (i = 0, j = 0 ; i < (INT)strlen(inText) ; i++)
	{
		if ((inText[i] == '[') && (isEscapeSequence(inText, i) == FALSE))
		{
			while ((inText[i] != ']') && (i < (INT)strlen(inText)))
			{
				i++ ;
			}

			i++ ;
		}

		outText[j] = inText[i] ;
		outText[j+1] = '\0' ;
		j++ ;
	}

	return ;
}

void removeTagText(CHAR *inText, CHAR *outText)
{
	INT i, j ;

	// Remove any tags.
	for (i = 0, j = 0 ; i < (INT)strlen(inText) ; i++)
	{
		if ((inText[i] == '<') && (inText[i+1] == '$'))
		{
			while ((inText[i] != '>') && (i < (INT)strlen(inText)))
			{
				i++ ;
			}
		}
		else
		{
			outText[j] = inText[i] ;
			outText[j+1] = '\0' ;
			j++ ;
		}
	}

	return ;
}

void getTabItemText(CHAR *inText, CHAR *outText, INT numTabs)
{
	// Convert xxx: yyy: zzz to "@>@>zzz" etc.
	INT		i, tabCount = 0 ;
	CHAR	tempText[MAXLEN] ;

	tempText[0] = '\0' ;

	for (i = 0 ; i < (INT)strlen(inText) ; i++)
	{
		if ((inText[i] == ':') && (isEscapeSequence(inText, i) == FALSE))
		{
			tabCount ++ ;
			strcat(tempText, "@>") ;
		}

		if (tabCount == numTabs)
		{
			strcat(tempText, &inText[i+1]) ;
			strcpy(outText, tempText) ;

			stripSpaces(outText) ;

			return ;
		}
	}
}

INT noTagCompare(CHAR *inText1, CHAR *inText2)
{
	CHAR s1[MAXLEN], s2[MAXLEN] ;
	INT	i ;

	removeTagText(inText1, (CHAR *)s1) ;
	removeTagText(inText2, (CHAR *)s2) ;

	stripSpaces((CHAR *)s1) ;
	stripSpaces((CHAR *)s2) ;

	// Remove any formatting tags for the page numbers.
	for (i = (strlen(s1)-1) ; i >= 0 ; i--)
	{
		while (s1[i] == ' ')
			i-- ;

		if (s1[i] != '>')
			break ;

		while (s1[i] != '<')
			i-- ;

		if (i < 0)
			break ;
	}

	s1[i+1] = '\0' ;

	for (i = (strlen(s2)-1) ; i >= 0 ; i--)
	{
		while (s2[i] == ' ')
			i-- ;

		if (s2[i] != '>')
			break ;

		while (s2[i] != '<')
			i-- ;

		if (i < 0)
			break ;
	}

	s2[i+1] = '\0' ;
	
	return (strcmp((CHAR *)s1, (CHAR *)s2)) ;
}

VOID stripPageFormatting(CHAR *inText, CHAR *outText)
{
	INT	i ;

	removeTagText(inText, (CHAR *)outText) ;

	stripSpaces((CHAR *)outText) ;

	// Remove any formatting tags for the page numbers.
	for (i = (strlen(outText)-1) ; i >= 0 ; i--)
	{
		while (outText[i] == ' ')
			i-- ;

		if (outText[i] != '>')
			break ;

		while (outText[i] != '<')
			i-- ;

		if (i < 0)
			break ;
	}

	outText[i+1] = '\0' ;
}

void stripSpaces(CHAR *text)
{
	INT iStart, iEnd ;
	CHAR	tempText[MAXLEN] ;

	// Strip all leading and trailing spaces.
	for (iStart = 0 ; iStart < (INT)strlen(text) ; iStart++)
		if (text[iStart] != ' ')
			break ;

	for (iEnd = ((INT)strlen(text) -1) ; iEnd >= 0 ; iEnd--)
	{
		if (text[iEnd] == '>')
		{
			while (text[iEnd] != '<')
			{
				iEnd-- ;

				if (iEnd < 0)
					return ;
			}

			iEnd-- ;
		}

		if (text[iEnd] != ' ')
			break ;
	}

	text[iEnd+1] = '\0' ;

	strcpy(tempText, &text[iStart]) ;

	strcpy(text, tempText) ;

	// If this is some tabbed text (@>@> xxx), strip any spaces after the tabs.
	for (iStart = 0 ; iStart < (INT)strlen(text) ; iStart += 2)
		if ((text[iStart] != '@') || (text[iStart+1] != '>'))
			break ;

	if (iStart == 0)
		// Not tab text.
		return ;

	for (iEnd = iStart ; iEnd < (INT)strlen(text) ; iEnd++)
		if (text[iEnd] != ' ')
			break ;

	strncpy(tempText, text, iStart) ;
	tempText[iStart] = '\0' ;

	strcat(tempText, &text[iEnd]) ;

	strcpy(text, tempText) ;
}

void stripAllSpaces(CHAR *inText, CHAR *outText)
{
	INT i, j ;

	for (i = 0, j = 0 ; i < (INT)strlen(inText) ; i++)
	{
		if (inText[i] != ' ')
		{
			outText[j] = inText[i] ;
			outText[j+1] = '\0' ;
			j++ ;
		}
	}

	return ;
}

void stripAllSpecialChars(CHAR *inText, CHAR *outText)
{
	INT i, j ;

	// Strip out all non-alphabetic characters.
	for (i = 0, j = 0 ; i < (INT)strlen(inText) ; i++)
	{
		if (((inText[i] >= 97) && (inText[i] <= 122)) ||
			((inText[i] >= 65) && (inText[i] <= 90)) ||
			((inText[i] >= 48) && (inText[i] <= 57)))
		{
			outText[j] = inText[i] ;
			outText[j+1] = '\0' ;
			j++ ;
		}
	}

	return ;
}

BOOL isEscapeSequence(CHAR *text, INT pos)
{
	INT count = 0, i ;

	for (i = (pos-1) ; i >= 0 ; i--)
	{
		if (text[i] != '\\')
			break ;
		else
			count++ ;
	}

	if ((count % 2) == 1)
		return (TRUE) ;
	else
		return (FALSE) ;
}

VOID stripIgnoreChars(CHAR *text)
{
	INT i = 0, j = 0, k = 0, iStart = 0, tabPos = 0 ;
	CHAR temp1[MAXLEN], temp2[MAXLEN] ;

	temp1[0] = '\0' ;
	temp2[0] = '\0' ;

	while (strncmp(&text[i], "@>", 2) == 0)
		i += 2 ;

	strncpy(temp1, text, i) ;
	temp1[i] = '\0' ;

	iStart = i ;

	// Get rid of characters to ignore
	for ( ; i < (INT)strlen(text) ; i++)
	{
		for (j = 0 ; szIgnoreChars[j] != '\0' ; j++)
		{
			if (text[i] == szIgnoreChars[j])
			{
				break ;
			}
		}

		if (szIgnoreChars[j] == '\0')
		{
			temp2[k] = text[i] ;
			temp2[k+1] = '\0' ;
			k++ ;
		}
	}

	strcpy(text, temp1) ;
	strcat(text, temp2) ;
}

void convertSpecialCharacters(CHAR *inText)
{
	int i, j ;

	for (i = 0 ; i < (INT)strlen(inText) ; i++)
	{
		j = 0 ;

		if (inText[i] < 22)
		{
			while (specialChars[j] [0] != 0)
			{
				if (inText[i] == specialChars[j] [0])
				{
					inText[i] = specialChars[j] [1] ;
					break ;
				}

				j++ ;
			}
		}
	}
}

void toRoman(int inNum, int inNumbering, CHAR *outString)
{
	outString[0] = '\0' ;

    while( inNum >= 1000 )
    {
        strcat(outString, "m") ;
        inNum -= 1000;
    }

    if( inNum >= 900 )
    {
        strcat(outString, "cm");
        inNum -= 900;
    }

    while( inNum >= 500 )
    {
        strcat(outString, "d") ;
        inNum -= 500;
    }

    if( inNum >= 400 )
    {
        strcat(outString, "cd") ;
        inNum -= 400;
    }

    while( inNum >= 100 )
    {
        strcat(outString, "c");
        inNum -= 100;
    }

    if( inNum >= 90 )
    {
        strcat(outString, "xc") ;
        inNum -= 90;
    }

    while( inNum >= 50 )
    {
        strcat(outString, "l") ;
        inNum -= 50;
    }

    if( inNum >= 40 )
    {
        strcat(outString, "xl");
        inNum -= 40;
    }

    while( inNum >= 10 )
    {
        strcat(outString, "x") ;
        inNum -= 10;
    }

    if( inNum >= 9 )
    {
        strcat(outString, "ix") ;
        inNum -= 9;
    }
    while( inNum >= 5 )
    {
        strcat(outString, "v");
        inNum -= 5;
    }

    if( inNum >= 4 )
    {
        strcat(outString, "iv");
        inNum -= 4;
    }

    while( inNum > 0 )
    {
        strcat(outString, "i") ;
        inNum--;
    }

	if (inNumbering == FV_PAGE_NUM_ROMAN_UC)
		strupr(outString) ;
}

void toAlpha(int inNum, int inNumbering, CHAR *outString)
{
	long i ;
	CHAR strTemp[2] ;

	outString[0] = '\0' ;

	i = 1 ;
	while ((i * 17576) <= (inNum-703))
		i++ ;
	i-- ;

	if (i > 0)
	{
		strTemp[0] = (i + 96) ;
		strTemp[1] = '\0' ;
		strcat(outString, strTemp) ;
	}

	inNum -= (i * 17576) ;

	i = 1 ;
	while ((i * 676) <= (inNum-27))
		i++ ;
	i-- ;

	if (i > 0)
	{
		strTemp[0] = (i + 96) ;
		strTemp[1] = '\0' ;
		strcat(outString, strTemp) ;
	}

	inNum -= (i * 676) ;

	i = 1 ;
	while ((i * 26) <= (inNum-1))
		i++ ;
	i-- ;

	if (i > 0)
	{
		strTemp[0] = (i + 96) ;
		strTemp[1] = '\0' ;
		strcat(outString, strTemp) ;
	}

	inNum -= (i * 26) ;

	i = inNum ;

	strTemp[0] = (i + 96) ;
	strTemp[1] = '\0' ;
	strcat(outString, strTemp) ;

	if (inNumbering == FV_PAGE_NUM_ALPHA_UC)
		strupr(outString) ;
}

void writeToDebugFile(CHAR *text)
{
	FILE *ff ;

	return ;

	ff = fopen("c:\\emdexDebug.txt", "a+") ;

	fwrite(text, strlen(text), strlen(text), ff) ;

	fclose(ff) ;
}

UINT APIENTRY PrintHookProc(HWND hdlg, UINT uiMsg, WPARAM wParam, 
                        LPARAM lParam) 
{ 
    return 0L; 
} 

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmdLine, int cmdShow)
{
	UINT ddeStat;
	HWND PrevWnd ;

	appInstance = hInst;

	if (strlen(cmdLine) == 0)
	{
		PrevWnd = FindWindow(NULL, "emDEX");
		if (PrevWnd)
		{
			/* An instance is already running. */
			return FALSE;
		}
	}

	ddeStat = DdeInitialize(&ddeInst, DdeCallBack,
		APPCLASS_STANDARD|APPCMD_CLIENTONLY, 0L);
	if (ddeStat)
	{
		errMsg(0, MB_OK, "Can't Initialize: %d", ddeStat);
		return 1;
	}

	strcpy(g_FileName, cmdLine) ;

	DialogBox(hInst, "IDD_PSINDEXCATCHER_DIALOG", NULL, emDEXProc);

/*	if (convId)
		DdeDisconnect(convId);

	DdeUninitialize(ddeInst) ; */

//	DeleteCriticalSection(&Critical_Section) ;

	return 1;	
}

// End of File