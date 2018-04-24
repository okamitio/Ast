#include"common.h"
#include<windowsx.h>
#include"resource.h"

namespace Place
{
//	extern Operation NewGateOperation;
	extern unsigned int NewGateIn;
}

BOOL CALLBACK InputNumDlgProc(HWND hDlg,UINT msg,WPARAM wp,LPARAM lp)
{
	static HWND hEditNin;
	static int tmpGateIn;
	BOOL bSuccess;

	static HWND hMain;

	switch(msg)
	{
	case WM_INITDIALOG:
		hMain=GetParent(hDlg);
		hEditNin=GetDlgItem(hDlg,IDC_EDIT_NUM);
		tmpGateIn=1;
		Edit_SetText(hEditNin,"2");
		return true;
	case WM_COMMAND:
		switch(LOWORD(wp))
		{
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			break;
		case IDOK:
			Place::NewGateIn=tmpGateIn;
			EndDialog(hDlg,IDOK);
			break;
		case IDC_EDIT_NUM:
			int tmp;
			tmp=GetDlgItemInt(hDlg,IDC_EDIT_NUM,&bSuccess,false);
			if(bSuccess)tmpGateIn=tmp;
			if(tmpGateIn<1)tmpGateIn=1;
			break;
		}
		return true;
	default:
		return false;
	}
}