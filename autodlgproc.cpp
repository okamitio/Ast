#include"common.h"
#include<windowsx.h>
#include"resource.h"

extern Data main_data;

namespace Automatic
{
	extern bool Execute;
	extern int ClockInLabel;
	extern int BreakOutLabel;
	extern unsigned int draw_cycle;
}


BOOL CALLBACK AutoDlgProc(HWND hDlg,UINT msg,WPARAM wp,LPARAM lp)
{
	static HWND hListIn;
	static HWND hListOut;
	static HWND hCycle;
	BOOL bSuccess;

	switch(msg)
	{
	case WM_INITDIALOG:
		hListIn=GetDlgItem(hDlg,IDC_CLOCK_LIST);
		hListOut=GetDlgItem(hDlg,IDC_BREAK_LIST);
		hCycle=GetDlgItem(hDlg,IDC_CYCLE);

		for(unsigned int i=0;i<main_data.nInput();i++)
			ListBox_AddString(hListIn,main_data.GetInput(i).name());
		SendMessage(hListIn,LB_SETCURSEL,Automatic::ClockInLabel,0);

		for(unsigned int i=0;i<main_data.nOutput();i++)
			ListBox_AddString(hListOut,main_data.GetOutput(i).name());
		ListBox_AddString(hListOut,"’âŽ~‚µ‚È‚¢");
		SendMessage(hListOut,LB_SETCURSEL,Automatic::BreakOutLabel,0);

		Edit_SetText(hCycle,"100");
		return true;
	case WM_COMMAND:
		switch(LOWORD(wp))
		{
			int tmp;
		case IDOK:
			tmp=SendMessage(hListIn,LB_GETCURSEL,0,0);
			if(tmp==LB_ERR)
			{
				Automatic::Execute=false;
				Automatic::ClockInLabel=-1;
			}
			else
			{
				Automatic::Execute=true;
				Automatic::ClockInLabel=tmp;
			}

			tmp=SendMessage(hListOut,LB_GETCURSEL,0,0);
			if(tmp==LB_ERR || tmp==main_data.nOutput())
				Automatic::BreakOutLabel=-1;
			else
				Automatic::BreakOutLabel=tmp;

			tmp=GetDlgItemInt(hDlg,IDC_CYCLE,&bSuccess,false);
			if(bSuccess && tmp>=USER_TIMER_MINIMUM)
				Automatic::draw_cycle=tmp;
			else
				Automatic::draw_cycle=USER_TIMER_MINIMUM;

			EndDialog(hDlg,IDOK);
			return true;
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			return true;
		}
		return false;
	}
	return false;
}

BOOL CALLBACK FastAutoDlgProc(HWND hDlg,UINT msg,WPARAM wp,LPARAM lp)
{
	static HWND hListIn;
	static HWND hListOut;

	switch(msg)
	{
	case WM_INITDIALOG:
		hListIn=GetDlgItem(hDlg,IDC_CLOCK_LIST);
		hListOut=GetDlgItem(hDlg,IDC_BREAK_LIST);

		for(unsigned int i=0;i<main_data.nInput();i++)
			ListBox_AddString(hListIn,main_data.GetInput(i).name());
		SendMessage(hListIn,LB_SETCURSEL,Automatic::ClockInLabel,0);

		for(unsigned int i=0;i<main_data.nOutput();i++)
			ListBox_AddString(hListOut,main_data.GetOutput(i).name());
		ListBox_AddString(hListOut,"’âŽ~‚µ‚È‚¢");
		SendMessage(hListOut,LB_SETCURSEL,Automatic::BreakOutLabel,0);

		return true;
	case WM_COMMAND:
		switch(LOWORD(wp))
		{
			int tmp;
		case IDOK:
			tmp=SendMessage(hListIn,LB_GETCURSEL,0,0);
			if(tmp==LB_ERR)
			{
				Automatic::Execute=false;
				Automatic::ClockInLabel=-1;
			}
			else
			{
				Automatic::Execute=true;
				Automatic::ClockInLabel=tmp;
			}

			tmp=SendMessage(hListOut,LB_GETCURSEL,0,0);
			if(tmp==LB_ERR || tmp==main_data.nOutput())
				Automatic::BreakOutLabel=-1;
			else
				Automatic::BreakOutLabel=tmp;

			Automatic::draw_cycle=100;

			EndDialog(hDlg,IDOK);
			return true;
		case IDCANCEL:
			EndDialog(hDlg,IDCANCEL);
			return true;
		}
		return false;
	}
	return false;
}