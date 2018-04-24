#include"common.h"
#include<windowsx.h>
#include"resource.h"

namespace Place
{	
	extern unsigned int NewBusWidth;
}

BOOL CALLBACK InputBusWidthDlgProc(HWND hDlg,UINT msg,WPARAM wp,LPARAM lp)
{
	static HWND hEditNin;
	static int tmpBusWidth;
	BOOL bSuccess;

	static HWND hMain;

	switch(msg)
	{
	case WM_INITDIALOG:
		hMain=GetParent(hDlg);
		hEditNin=GetDlgItem(hDlg,IDC_EDIT_NUM);
		tmpBusWidth=1;
		Edit_SetText(hEditNin,"1");
		return true;
	case WM_COMMAND:
		switch(LOWORD(wp))
		{
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			break;
		case IDOK:
			Place::NewBusWidth=tmpBusWidth;
			EndDialog(hDlg,IDOK);
			break;
		case IDC_EDIT_NUM:
			int tmp;
			tmp=GetDlgItemInt(hDlg,IDC_EDIT_NUM,&bSuccess,false);
			if(bSuccess)tmpBusWidth=tmp;
			if(tmpBusWidth<1)tmpBusWidth=1;
			break;
		}
		return true;
	default:
		return false;
	}
}