#include"common.h"
#include"resource.h"

namespace Place
{
	extern char NewLabelName[SIZE_OF_LABEL_NAME];	//êVÇµÇ¢ÉâÉxÉãÇÃñºëO
}

BOOL CALLBACK LabelDlgProc(HWND hDlg,UINT msg,WPARAM wp,LPARAM lp)
{
	static HWND hEdit;

	static HWND hMain;

	switch(msg)
	{
	case WM_INITDIALOG:
		hMain=GetParent(hDlg);
		hEdit=GetDlgItem(hDlg,IDC_EDIT_NAME);
		return true;
	case WM_COMMAND:
		switch(LOWORD(wp))
		{
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			break;
		case IDOK:
			if(strlen(Place::NewLabelName))
				EndDialog(hDlg,IDOK);
			else
				EndDialog(hDlg,IDCANCEL);
			break;
		case IDC_EDIT_NAME:
			GetDlgItemText(hDlg,IDC_EDIT_NAME,Place::NewLabelName,SIZE_OF_LABEL_NAME);
			break;
		}
		return true;
	default:
		return false;
	}
}