#include"common.h"
#include"resource.h"
#include<CommCtrl.h>
#include<process.h>


#pragma comment(lib,"ComCtl32.lib")

#define ID_TIMER 100

extern void Proc_Paint(State state);	//描画

extern BOOL CALLBACK InputBusWidthDlgProc(HWND hDlg,UINT msg,WPARAM wp,LPARAM lp); //バス幅の入力
extern BOOL CALLBACK InputNumDlgProc(HWND hDlg,UINT msg,WPARAM wp,LPARAM lp);	//ゲートの入力数を入力
extern BOOL CALLBACK LabelDlgProc(HWND hDlg,UINT msg,WPARAM wp,LPARAM lp);	//ラベル名
extern BOOL CALLBACK AutoDlgProc(HWND hDlg,UINT msg,WPARAM wp,LPARAM lp);	//自動クロック入力機能のデータを入力
extern BOOL CALLBACK FastAutoDlgProc(HWND hDlg,UINT msg,WPARAM wp,LPARAM lp);	//(高速)自動クロック入力機能のデータを入力

extern HINSTANCE hInst;	//インスタンスハンドル

extern Direction AlignedDirection(vector2D center,vector2D end); //最も近い八方位
extern vector2D AlignAngle(vector2D center,vector2D end);	//八方位上の点に変換


extern bool SaveAs(Data d,char *szF,char *szFT);
extern bool Save();
extern bool LoadData();
extern bool Open();
extern bool LoadModule();

extern bool SaveSVG(Data d);

extern void LoadCopiedData();

HWND hMainWindow;	//メインウィンドウのハンドル

Data main_data;

Data copy_data;

bool data_modify=false;	//main_dataが更新されていればtrue

State state=scroll;		//現在選択されているモード

//格子間隔
namespace Grid
{
	int Interval = 16;
	bool Draw = true;
}

namespace Automatic
{
	bool Fast=false;		//高速実行
	bool ExecutingFastOperation=false;	//高速実行のスレッドが生きているならtrue
	HANDLE hFastOperationThread;	//高速実行のスレッド
	bool Execute=false;		//自動実行中ならばtrue
	int ClockInLabel=-1;		//クロックを入力するラベル(-1なら発振なし)
	int ClockInWireNumber=0;	//クロックを入力する導線の番号
	int BreakOutLabel=-1;		//変化したらブレークする出力(-1ならブレークなし)
	int BreakOutWireNumber=0;	//変化したらブレークする出力の導線の番号
	bool BreakLabelOldState;	//旧状態
	unsigned int draw_cycle=100;	//描画周期
}

namespace FileIO
{
	char szFile[MAX_PATH]="";
	char szFileTitle[MAX_PATH]="";
}

namespace Interface
{
	POINT MouseCoordinates={0,0};	//マウス座標
	POINT Scroll_Coordinates={0,0};	//スクロール
	vector2D AlignedMouseCoordinates;	//Grid::Interval間隔で
	vector2D FirstAlignedMouseCoordinates;	//始点
	RECT ClientRect;	//クライアント領域
	bool Mouse_Drag;	//ドラッグ中ならtrue
}

namespace Edit
{
	vector2D coordinates1;
	vector2D coordinates2;
}

namespace Place
{
	Direction NewDirection=CLOCKWISE_000;	//新しい記号の方向
	char NewLabelName[SIZE_OF_LABEL_NAME]="";	//新しいラベルの名前
	unsigned int NewGateIn=2;				//新しい論理ゲートの入力数
	bool NewGateOperationisOR=false;	//新しい論理ゲートの演算がORならtrue,ANDならFALSE
	bool NewGateOperationisNOT = false;	//新しい論理ゲートの演算がNOTならtrue
	Module NewModule;		//新しいモジュール
	Module *pModule=&NewModule;
	unsigned int NewBusWidth=1;	//設置されるバス幅
}

namespace Drawing
{
	HBITMAP hBmpOne,hBmpAnd,hBmpOr;
	HDC hDcOne,hDcAnd,hDcOr;
	HFONT hFontH,hFontV;	//水平と垂直のフォント
}


int Node::nBranch()		//枝の数を返す
{
	int count=0;
	for(int i=0;i<8;i++)count+=(branch[i].GetDeviceClass()!=no_device);
	return count;
}


//四捨五入
double round_d(double x)
{
	if(x>0.0)
		return floor(x + 0.5);
	else
		return -1.0 * floor(fabs(x) + 0.5);
}

vector2D POINT2vector2D(POINT p)	//座標変換
{
	vector2D v=vector2D(
	(int)round_d((p.x+(Interface::ClientRect.left-Interface::ClientRect.right)/2-Interface::Scroll_Coordinates.x)/(double)Grid::Interval),
	(int)round_d((p.y+(Interface::ClientRect.top-Interface::ClientRect.bottom)/2-Interface::Scroll_Coordinates.y)/(double)Grid::Interval));
	return v;
}

void Node::write_branch(Direction d,DeviceClassification dclass,int idx,int sidx)
{
	branch[d].SetDeviceClass(dclass);
	branch[d].SetIndexAndSubIndex(idx,sidx);
}

const int STRSIZE=511;

void Data::status(char *s)
{
	const char *dirstr[] = {"000","090","180","270"};
	sprintf_s(s,STRSIZE,"ノード:%d ゲート:%d 導線:%d 入力:%d 出力:%d バス入力:%d バス出力:%d モジュール:%d 注釈:%d 方向:%s 新規和積の入力数:%d 新規バス幅:%d (%d,%d)",node_data.size(),gate.size(),conductor.size(),input.size(),output.size(),busin.size(),busout.size(),module.size(),note.size(),dirstr[Place::NewDirection>>1],Place::NewGateIn,Place::NewBusWidth,Interface::AlignedMouseCoordinates.MulGridInterval().x/Grid::Interval,Interface::AlignedMouseCoordinates.MulGridInterval().y/Grid::Interval);
}

void Data::Clear()
{
	node_data.clear();
	gate.clear();
	conductor.clear();
	input.clear();
	output.clear();
	busin.clear();
	busout.clear();
	module.clear();
	note.clear();
}

void Module::Clear()
{
	d = Place::NewDirection;
	terminal[3].clear();
	terminal[0].clear();
	terminal[1].clear();
	terminal[2].clear();
	node_out.clear();
	array_in.clear();
	((Data*)this)->Clear();
}

void Data::NegateInIndex(int i,int wirenumber)
{
	DeviceIndex sd;
	input[i].SetState(!input[i].GetState(wirenumber),wirenumber);
	if(node_data[input[i].nodeindex()].isAllRight())
	{
		sd.SetDeviceClass(label_input);
		sd.SetIndexAndSubIndex(i,-1);
		Operate(input[i].nodeindex(),sd,wirenumber);
	}
}

unsigned __stdcall FastOperation(LPVOID lpdata)
{
	main_data.InitModuleParentPointer();
	while(Automatic::Execute)
	{
		main_data.NegateInIndex(Automatic::ClockInLabel,Automatic::ClockInWireNumber);
		if(Automatic::BreakOutLabel>=0)
			if(main_data.GetOutState(Automatic::BreakOutLabel,Automatic::BreakOutWireNumber)!=Automatic::BreakLabelOldState)
				Automatic::Execute=false;
	}
	Automatic::Fast=false;
	InvalidateRect(hMainWindow,NULL,false);
	Automatic::ExecutingFastOperation=false;
	return 0;
}

void Data::NegateInCoordinate()
{
	if(!Automatic::Fast)
	{
		InitModuleParentPointer();
		for(unsigned int i=0;i<input.size();i++)
			if((!Automatic::Execute || i!=Automatic::ClockInLabel))
				for(int j=0;j<node(input[i].nodeindex()).nWire();j++)
				{
					switch(input[i].direction())
					{
					case CLOCKWISE_000:
					case CLOCKWISE_090:
					if(Interface::AlignedMouseCoordinates==node(input[i].nodeindex()).position()-DirectionVector[input[i].direction()]*(2+j))
						NegateInIndex(i,j);
					break;
					case CLOCKWISE_180:
					case CLOCKWISE_270:
					if(Interface::AlignedMouseCoordinates==node(input[i].nodeindex()).position()-DirectionVector[input[i].direction()]*(input[i].nWire()+1-j))
						NegateInIndex(i,j);
					break;
					}
				}
	}
}

void Data::JumpToSource()
{
	for(unsigned int i=0;i<output.size();i++)
		if(Interface::AlignedMouseCoordinates==node(output[i].nodeindex()).position()-DirectionVector[output[i].direction()])
			switch(output[i].GetSourceDevClass())
			{
			case label_input:
				Interface::Scroll_Coordinates=node(input[output[i].GetSourceDevIndex()].nodeindex()).position().MulGridInterval();
				Interface::Scroll_Coordinates.x*=-1;
				Interface::Scroll_Coordinates.y*=-1;
				break;
			case gate_out:
				Interface::Scroll_Coordinates=node(gate[output[i].GetSourceDevIndex()].NodeOutIndex()).position().MulGridInterval();
				Interface::Scroll_Coordinates.x*=-1;
				Interface::Scroll_Coordinates.y*=-1;
				break;
			case module_out:
				Interface::Scroll_Coordinates = node(module[output[i].GetSourceDevIndex()].NodeOut(output[i].GetSourceDevSubIndex())).position().MulGridInterval();
				Interface::Scroll_Coordinates.x*=-1;
				Interface::Scroll_Coordinates.y*=-1;
				break;
			}

	for(unsigned int i=0;i<gate.size();i++)
		for(int j=0;j<gate[i].nIn();j++)
			if(Interface::AlignedMouseCoordinates==node_data[gate[i].NodeInIndex(j)].position())
				switch(gate[i].GetSourceDevClass(j))
				{
				case label_input:
					Interface::Scroll_Coordinates=node(input[gate[i].GetSourceDevIndex(j)].nodeindex()).position().MulGridInterval();
					Interface::Scroll_Coordinates.x*=-1;
					Interface::Scroll_Coordinates.y*=-1;
					break;
				case gate_out:
					Interface::Scroll_Coordinates=node(gate[gate[i].GetSourceDevIndex(j)].NodeOutIndex()).position().MulGridInterval();
					Interface::Scroll_Coordinates.x*=-1;
					Interface::Scroll_Coordinates.y*=-1;
					break;
				case module_out:
					Interface::Scroll_Coordinates = node(module[gate[i].GetSourceDevIndex(j)].NodeOut(gate[i].GetSourceDevSubIndex(j))).position().MulGridInterval();
					Interface::Scroll_Coordinates.x*=-1;
					Interface::Scroll_Coordinates.y*=-1;
					break;
				}
	for(unsigned int i=0;i<module.size();i++)
		for(unsigned int j=0;j<module[i].nInput();j++)
			if(Interface::AlignedMouseCoordinates==node_data[module[i].NodeIn(j)].position())
			{
				switch(module[i].InArray(j).GetDeviceClass())
				{
				case label_input:
					Interface::Scroll_Coordinates=node(input[module[i].InArray(j).GetDeviceIndex()].nodeindex()).position().MulGridInterval();
					Interface::Scroll_Coordinates.x*=-1;
					Interface::Scroll_Coordinates.y*=-1;
					break;
				case gate_out:
					Interface::Scroll_Coordinates=node(gate[module[i].InArray(j).GetDeviceIndex()].NodeOutIndex()).position().MulGridInterval();
					Interface::Scroll_Coordinates.x*=-1;
					Interface::Scroll_Coordinates.y*=-1;
					break;
				case module_out:
					Interface::Scroll_Coordinates = node(module[module[i].InArray(j).GetDeviceIndex()].NodeOut(module[i].InArray(j).GetDeviceSubIndex())).position().MulGridInterval();
					Interface::Scroll_Coordinates.x*=-1;
					Interface::Scroll_Coordinates.y*=-1;
					break;
				}
			}
}


//更新後の保存の確認
bool Confirm()
{
	int id;

	id=MessageBox(hMainWindow,"更新されています。\n保存しますか?","警告",MB_YESNOCANCEL | MB_ICONEXCLAMATION);
	if(id==IDYES)
	{
		return Save();
	}
	if(id==IDNO) return true;
	return false;
}

bool Data::GetOutState(int i,int wirenumber)
{
	switch(output[i].GetSourceDevClass())
	{
	case label_input:
		return input[output[i].GetSourceDevIndex()].GetState(wirenumber);
		break;
	case gate_out:
		return gate[output[i].GetSourceDevIndex()].GetState(wirenumber);
		break;
	case module_out:
		return module[output[i].GetSourceDevIndex()].GetState(output[i].GetSourceDevSubIndex(),wirenumber);
		break;
	default:
		return 0;
	}
}

bool Data::ExpandModule()
{
	for(unsigned int i=0;i<module.size();i++)
	if((Interface::MouseCoordinates.x-(module[i].Center()+DirectionVector[CLOCKWISE_180]*(module[i].Margin(2)+module[i].InternalHalfWidth())).toPOINT().x)>0 &&
		(Interface::MouseCoordinates.y-(module[i].Center()+DirectionVector[CLOCKWISE_270]*(module[i].Margin(3)+module[i].InternalHalfHeight())).toPOINT().y)>0 &&
		(Interface::MouseCoordinates.x-(module[i].Center()+DirectionVector[CLOCKWISE_000]*(module[i].Margin(0)+module[i].InternalHalfWidth())).toPOINT().x)<0 &&
		(Interface::MouseCoordinates.y-(module[i].Center()+DirectionVector[CLOCKWISE_090]*(module[i].Margin(1)+module[i].InternalHalfHeight())).toPOINT().y)<0)
		{
			char szFiletmp[MAX_PATH]="";
			char szFileTitletmp[MAX_PATH]="";
			SaveAs(module[i],szFiletmp,szFileTitletmp);
		}
	return false;
}

void MenuEditStateChange(UINT fState)
{
	HMENU hmenu;
	MENUITEMINFO menuInfo;

	hmenu=GetMenu(hMainWindow);
	menuInfo.cbSize = sizeof (MENUITEMINFO);
	menuInfo.fMask = MIIM_STATE;
	menuInfo.fType = 0;
	menuInfo.fState = fState;
	menuInfo.wID = 0;
	menuInfo.hSubMenu = NULL;
	menuInfo.hbmpChecked = NULL;
	menuInfo.hbmpUnchecked = NULL;
	menuInfo.dwItemData = 0;
	menuInfo.dwTypeData = NULL;
	menuInfo.cch = 0;

	SetMenuItemInfo(hmenu,IDM_DEL,false,&menuInfo);
	SetMenuItemInfo(hmenu,IDM_CUT,false,&menuInfo);
	SetMenuItemInfo(hmenu,IDM_COPY,false,&menuInfo);
}

void MenuDrawGridToggle()
{
	HMENU hmenu;
	MENUITEMINFO menuInfo;

	Grid::Draw = !Grid::Draw;

	hmenu = GetMenu(hMainWindow);
	menuInfo.cbSize = sizeof(MENUITEMINFO);
	menuInfo.fMask = MIIM_STATE;
	menuInfo.fType = 0;
	menuInfo.wID = 0;
	menuInfo.hSubMenu = NULL;
	menuInfo.hbmpChecked = NULL;
	menuInfo.hbmpUnchecked = NULL;
	menuInfo.dwItemData = 0;
	menuInfo.dwTypeData = NULL;
	menuInfo.cch = 0;

	menuInfo.fState = Grid::Draw ? MFS_DISABLED : MFS_ENABLED;
	SetMenuItemInfo(hmenu, IDM_GRID_VISIBLE, false, &menuInfo);

	menuInfo.fState = Grid::Draw ?  MFS_ENABLED : MFS_DISABLED;
	SetMenuItemInfo(hmenu, IDM_GRID_INVISIBLE, false, &menuInfo);
}

void UpdateGridInterval(bool inc)
{
	if (inc)
	{
		if (Grid::Interval < 20)
		{
			Interface::Scroll_Coordinates.x = Interface::Scroll_Coordinates.x*(Grid::Interval + 1) / Grid::Interval;
			Interface::Scroll_Coordinates.y = Interface::Scroll_Coordinates.y*(Grid::Interval + 1) / Grid::Interval;
			Grid::Interval++;
		}
	}
	else
	{
		if (Grid::Interval > 8)
		{
			Interface::Scroll_Coordinates.x = Interface::Scroll_Coordinates.x*(Grid::Interval - 1) / Grid::Interval;
			Interface::Scroll_Coordinates.y = Interface::Scroll_Coordinates.y*(Grid::Interval - 1) / Grid::Interval;
			Grid::Interval--;
		}
	}
	DeleteObject(Drawing::hFontV);
	DeleteObject(Drawing::hFontH);
	Drawing::hFontH = CreateFont(Grid::Interval * 2 - 4, Grid::Interval - 1, 0, 0, FW_THIN, false, false, false, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "ＭＳ 明朝");
	Drawing::hFontV = CreateFont(Grid::Interval * 2 - 4, Grid::Interval - 1, -900, 0, FW_REGULAR, false, false, false, SHIFTJIS_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, PROOF_QUALITY, FIXED_PITCH | FF_MODERN, "ＭＳ 明朝");
}


//ウインドウプロシージャ
LRESULT CALLBACK WindowProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	const UINT MyStatus=100;
	static HWND hStatus;
	INITCOMMONCONTROLSEX ic;
	HDC hdc;
	vector2D OldAlignedMouseCoordinates=vector2D(0,0);
	char str[STRSIZE+1];
	//ウインドウメッセージ
	switch(uMsg)
	{
	case WM_CLOSE:
		state=scroll;
		if(data_modify)
		{
			if(Confirm()) DestroyWindow(hwnd);
		}
		else
			DestroyWindow(hwnd);
		break;
	case WM_COMMAND:
		if(Automatic::ExecutingFastOperation)
		{
			Automatic::Execute=false;
			WaitForSingleObject(Automatic::hFastOperationThread,INFINITE);
			CloseHandle(Automatic::hFastOperationThread);
			//Automatic::ExecutingFastOperation=false;
		}
		Automatic::Execute=false;
		Automatic::Fast=false;
		MenuEditStateChange(MFS_DISABLED);
		copy_data.Clear();
		switch(LOWORD(wParam))
		{
		case IDM_EXIT:
			state=scroll;
			if(data_modify)
			{
				if(Confirm()) DestroyWindow(hwnd);
			}
			else
				DestroyWindow(hwnd);
			break;
		case IDM_SVG:
			SaveSVG(main_data);
			state=scroll;
			break;
		case IDM_NEW:
			if(data_modify)
			{
				if(Confirm())
				{
					main_data.Clear();
					strcpy_s(FileIO::szFile,"");
					strcpy_s(FileIO::szFileTitle,"");
					data_modify=false;
					Interface::Scroll_Coordinates.x=0;
					Interface::Scroll_Coordinates.y=0;
					Automatic::ClockInLabel=-1;
					Automatic::BreakOutLabel=-1;
				}
			}
			else
			{
				main_data.Clear();
				strcpy_s(FileIO::szFile,"");
				strcpy_s(FileIO::szFileTitle,"");
				data_modify=false;
				Interface::Scroll_Coordinates.x=0;
				Interface::Scroll_Coordinates.y=0;
				Automatic::ClockInLabel=-1;
				Automatic::BreakOutLabel=-1;
			}
			state=scroll;
			InvalidateRect(hwnd,NULL,true);
			break;
		case IDM_SAVEAS:
			if(SaveAs(main_data,FileIO::szFile,FileIO::szFileTitle))data_modify=false;
			state=scroll;
			break;
		case IDM_SAVE:
			if(Save())data_modify=false;
			state=scroll;
			break;
		case IDM_OPEN:
			if(data_modify)
			{
				if(Confirm())
					if(Open())
					{
						data_modify=false;
						Automatic::ClockInLabel=-1;
						Automatic::BreakOutLabel=-1;
						Interface::Scroll_Coordinates.x=0;
						Interface::Scroll_Coordinates.y=0;
					}
			}
			else if(Open())
				{
					data_modify=false;
					Automatic::ClockInLabel=-1;
					Automatic::BreakOutLabel=-1;
					Interface::Scroll_Coordinates.x=0;
					Interface::Scroll_Coordinates.y=0;
				}
			state=scroll;
			InvalidateRect(hwnd,NULL,true);
			break;
		case IDM_RANGE:
			state=range;
			InvalidateRect(hwnd,NULL,true);
			break;
		case IDM_DEL:
			main_data.CopyRange(true,false);
			data_modify=true;
			Automatic::ClockInLabel=-1;
			Automatic::BreakOutLabel=-1;
			state=scroll;
			InvalidateRect(hwnd,NULL,true);
			break;
		case IDM_CUT:
			main_data.CopyRange(true,true);
			data_modify=true;
			Automatic::ClockInLabel=-1;
			Automatic::BreakOutLabel=-1;
			state=scroll;
			InvalidateRect(hwnd,NULL,true);
			break;
		case IDM_COPY:
			main_data.CopyRange(false,true);
			state=scroll;
			InvalidateRect(hwnd,NULL,true);
			break;
		case IDM_PASTE:
			LoadCopiedData();
			state=paste;
			break;
		case IDM_GRID_VISIBLE:
		case IDM_GRID_INVISIBLE:
			MenuDrawGridToggle();
			InvalidateRect(hwnd, NULL, true);
			break;
		case IDM_SCROLL:
			state=scroll;
			InvalidateRect(hwnd,NULL,true);
			break;
		case IDM_MODULE_LOAD:
			if(LoadModule())state=placemodule;
			break;
		case IDM_MODULE_SAVE:
			state=expandmodule;
			break;
		case IDM_IN_LABEL:
			state=placein;
			break;
		case IDM_OUT_LABEL:
			state=placeout;
			break;
		case IDM_CONDUCTOR:
			state=placewire;
			break;
		case IDM_BUFFER:
			Place::NewGateIn = 1;
			Place::NewGateOperationisNOT = false;
			Place::NewGateOperationisOR = false;
			state = placegate;
			break;
		case IDM_NOT:
			Place::NewGateIn = 1;
			Place::NewGateOperationisNOT = true;
			Place::NewGateOperationisOR = false;
			state=placegate;
			break;
		case IDM_AND:
			if (Place::NewGateIn == 1)Place::NewGateIn = 2;
			Place::NewGateOperationisNOT = false;
			Place::NewGateOperationisOR = false;
			state = placegate;
			break;
		case IDM_NAND:
			if (Place::NewGateIn == 1)Place::NewGateIn = 2;
			Place::NewGateOperationisNOT = true;
			Place::NewGateOperationisOR = false;
			state = placegate;
			break;
		case IDM_OR:
			if (Place::NewGateIn == 1)Place::NewGateIn = 2;
			Place::NewGateOperationisNOT = false;
			Place::NewGateOperationisOR=true;
			state=placegate;
			break;
		case IDM_NOR:
			if (Place::NewGateIn == 1)Place::NewGateIn = 2;
			Place::NewGateOperationisNOT = true;
			Place::NewGateOperationisOR = true;
			state = placegate;
			break;
		case IDM_N_GATEIN:
			state = DialogBox(hInst,TEXT("INPUTNUMDLG"),hwnd,(DLGPROC)InputNumDlgProc)==IDOK ? placegate : scroll;
			break;
		case IDM_BUS_WIDTH:
			DialogBox(hInst,TEXT("INPUTBUSWIDTHDLG"),hwnd,(DLGPROC)InputBusWidthDlgProc);
			break;
		case IDM_BUS_IN:
			state=placebusin;
			break;
		case IDM_BUS_OUT:
			state=placebusout;
			break;
		case IDM_ERASE:
			state=erase;
			break;
		case IDM_AUTO:
			DialogBox(hInst,TEXT("AUTODLG"),hwnd,(DLGPROC)AutoDlgProc);
			if(Automatic::ClockInLabel>=0 && Automatic::Execute)
			{
				SetTimer(hwnd,ID_TIMER,Automatic::draw_cycle,NULL);
				if(Automatic::BreakOutLabel>=0)
					Automatic::BreakLabelOldState=main_data.GetOutState(Automatic::BreakOutLabel,Automatic::BreakOutWireNumber);
			}
			state=scroll;
			break;
		case IDM_AUTO_FAST:
			DialogBox(hInst,TEXT("FASTAUTODLG"),hwnd,(DLGPROC)FastAutoDlgProc);
			if(Automatic::ClockInLabel>=0 && Automatic::Execute)
			{
				unsigned dwID;
				Automatic::Fast=true;
				Automatic::hFastOperationThread=(HANDLE)_beginthreadex(NULL,0,FastOperation,NULL,0,&dwID);
				Automatic::ExecutingFastOperation=true;
				SetTimer(hwnd,ID_TIMER,Automatic::draw_cycle,NULL);
				if(Automatic::BreakOutLabel>=0)
					Automatic::BreakLabelOldState=main_data.GetOutState(Automatic::BreakOutLabel,Automatic::BreakOutWireNumber);
			}
			state=scroll;
			break;
		case IDM_NOTE:
			state=note;
			break;
		case IDM_VERSION:
			MessageBox(hwnd,"論理回路シミュレータAst Ver0.85\n岡　道雄\nhttp://hp.vector.co.jp/authors/VA060227/\nhttp://www.geocities.jp/oka_mitio/\noka_mitio@yahoo.co.jp","Ast",MB_OK);
			state=scroll;
			break;
		}
		break;
	case WM_MOUSEMOVE:
		if((wParam & MK_LBUTTON)==0)Interface::Mouse_Drag=false;
		if(state==scroll && Interface::Mouse_Drag)
		{
			Interface::Scroll_Coordinates.x+=LOWORD(lParam)-Interface::MouseCoordinates.x;
			Interface::Scroll_Coordinates.y+=HIWORD(lParam)-Interface::MouseCoordinates.y;
			InvalidateRect(hwnd,NULL,true);
		}
		Interface::MouseCoordinates.x=LOWORD(lParam);
		Interface::MouseCoordinates.y=HIWORD(lParam);
		OldAlignedMouseCoordinates=Interface::AlignedMouseCoordinates;
		Interface::AlignedMouseCoordinates=POINT2vector2D(Interface::MouseCoordinates);
		if(Interface::AlignedMouseCoordinates!=OldAlignedMouseCoordinates)InvalidateRect(hwnd,NULL,true);
		break;
	case WM_RBUTTONDOWN:
		switch(Place::NewDirection)
		{
		case CLOCKWISE_270:
			Place::NewDirection=CLOCKWISE_000;
			break;
		case CLOCKWISE_000:
			Place::NewDirection=CLOCKWISE_090;
			break;
		case CLOCKWISE_090:
			Place::NewDirection=CLOCKWISE_180;
			break;
		case CLOCKWISE_180:
			Place::NewDirection=CLOCKWISE_270;
			break;
		}
		Place::NewModule.SetDirection(Place::NewDirection);
		InvalidateRect(hwnd,NULL,false);
		break;
	case WM_LBUTTONDOWN:
		Interface::Mouse_Drag=true;
		Interface::FirstAlignedMouseCoordinates=POINT2vector2D(Interface::MouseCoordinates);
		switch(state)
		{
		case range_selected:
			state=range;
			break;
		case range:
			Interface::MouseCoordinates.x=LOWORD(lParam);
			Interface::MouseCoordinates.y=HIWORD(lParam);
			break;
		case placebusin:
			if(main_data.AbleToPlaceBusIn()) main_data.PlaceBusIn(),data_modify=true;
			break;
		case placebusout:
			if(main_data.AbleToPlaceBusOut()) main_data.PlaceBusOut(),data_modify=true;
			break;
		case placewire:
			Interface::MouseCoordinates.x=LOWORD(lParam);
			Interface::MouseCoordinates.y=HIWORD(lParam);
			break;
		case placegate:
			if(main_data.AbleToPlaceGate()) main_data.PlaceGate(),data_modify=true;
			break;
		case placemodule:
			if(main_data.AbleToPlaceModule())
			{
				Place::pModule=&Place::NewModule;
				main_data.PlaceModule(),data_modify=true;
			}
			break;
		case expandmodule:
			main_data.ExpandModule();
			break;
		case paste:
			if(copy_data.AbleToPaste())copy_data.Paste(),data_modify=true,Automatic::ClockInLabel=-1,Automatic::BreakOutLabel=-1;
			break;
		case placein:
			if(main_data.AbleToPlaceLabelIn())
				if(DialogBox(hInst,TEXT("LABELDLG"),hwnd,(DLGPROC)LabelDlgProc)==IDOK)
					main_data.PlaceInLabel(),data_modify=true,Automatic::ClockInLabel=-1;
			break;
		case placeout:
			if(main_data.AbleToPlaceLabelOut())
				if(DialogBox(hInst,TEXT("LABELDLG"),hwnd,(DLGPROC)LabelDlgProc)==IDOK)
					main_data.PlaceOutLabel(),data_modify=true,Automatic::BreakOutLabel=-1;
			break;
		case note:
			if(DialogBox(hInst,TEXT("LABELDLG"),hwnd,(DLGPROC)LabelDlgProc)==IDOK)
				main_data.WriteNote(),data_modify=true;
			break;
		}
		InvalidateRect(hwnd,NULL,false);
		break;
	case WM_LBUTTONUP:
		Interface::Mouse_Drag=false;
		switch(state)
		{
		case range:
			MenuEditStateChange(MFS_ENABLED);
			state=range_selected;
			Edit::coordinates1=Interface::FirstAlignedMouseCoordinates;
			Edit::coordinates2=Interface::AlignedMouseCoordinates;
			break;
		case placewire:
			if(Interface::FirstAlignedMouseCoordinates!=Interface::AlignedMouseCoordinates)
				if(main_data.AbleToPlaceConductor())
					main_data.PlaceConductor(),data_modify=true;
			break;
		case erase:
			if(main_data.EraseDevice())
			{
				data_modify=true;
				Automatic::ClockInLabel=-1;
				Automatic::BreakOutLabel=-1;
			}
			break;
		case scroll:
			if(Interface::FirstAlignedMouseCoordinates==Interface::AlignedMouseCoordinates)
				main_data.NegateInCoordinate();
			break;
		}
		InvalidateRect(hwnd,NULL,false);
		break;
	case WM_LBUTTONDBLCLK:
			if(Interface::FirstAlignedMouseCoordinates==Interface::AlignedMouseCoordinates)
				main_data.JumpToSource();
		break;
	case WM_MOUSEWHEEL:
		if((short)(wParam >> 16) > 0) UpdateGridInterval(true);
		if((short)(wParam >> 16) < 0) UpdateGridInterval(false);
		InvalidateRect(hwnd, NULL, false);
		break;
	case WM_KEYDOWN:
		switch(wParam)
		{
		case VK_PRIOR:
			UpdateGridInterval(true);
			InvalidateRect(hwnd, NULL, false);
			break;
		case VK_NEXT:
			UpdateGridInterval(false);
			InvalidateRect(hwnd, NULL, false);
			break;
		}
		break;
	case WM_TIMER:
		if(Automatic::Fast)
		{
			if(Automatic::Execute && Automatic::ClockInLabel>=0)
			{
				hdc=GetDC(hwnd);
				main_data.DrawInputState(hdc);
				main_data.DrawOutputState(hdc);
				ReleaseDC(hwnd,hdc);
			}
			else
			{
				if(Automatic::ExecutingFastOperation)
				{
					Automatic::Execute=false;
					WaitForSingleObject(Automatic::hFastOperationThread,INFINITE);
					CloseHandle(Automatic::hFastOperationThread);
					//Automatic::ExecutingFastOperation=false;
				}
				Automatic::Execute=false;
				KillTimer(hwnd,ID_TIMER);
				InvalidateRect(hwnd,NULL,false);
			}
		}
		else
		{
			if(Automatic::BreakOutLabel>=0)
				if(main_data.GetOutState(Automatic::BreakOutLabel,Automatic::BreakOutWireNumber)!=Automatic::BreakLabelOldState) Automatic::Execute=false;
			if(Automatic::Execute && Automatic::ClockInLabel>=0)
			{
				main_data.InitModuleParentPointer();
				main_data.NegateInIndex(Automatic::ClockInLabel,Automatic::ClockInWireNumber);
				hdc=GetDC(hwnd);
				main_data.DrawInputState(hdc);
				main_data.DrawOutputState(hdc);
				ReleaseDC(hwnd,hdc);
			}
			else
				KillTimer(hwnd,ID_TIMER);
		}
		break;
	case WM_PAINT:
		Proc_Paint(state);
		main_data.status(str);
		SendMessage(hStatus,SB_SETTEXT,0,(LPARAM)str);
		if(strlen(FileIO::szFileTitle)>0)
			SetWindowText(hwnd,FileIO::szFileTitle);
		else
			SetWindowText(hwnd,"無題");
		break;
	case WM_SIZE:
		GetClientRect(hwnd,&Interface::ClientRect);
		SendMessage(hStatus,uMsg,wParam,lParam);
		break;
	case WM_CREATE:
		hMainWindow=hwnd;
		state=scroll;
		Interface::Mouse_Drag=false;
		GetClientRect(hwnd,&Interface::ClientRect);

		hdc=GetDC(hwnd);
		Drawing::hDcOne=CreateCompatibleDC(hdc);
		Drawing::hDcAnd=CreateCompatibleDC(hdc);
		Drawing::hDcOr=CreateCompatibleDC(hdc);
		ReleaseDC(hwnd,hdc);

		Drawing::hBmpOne=LoadBitmap(hInst,TEXT("BMP_ONE"));
		Drawing::hBmpAnd=LoadBitmap(hInst,TEXT("BMP_AND"));
		Drawing::hBmpOr =LoadBitmap(hInst,TEXT("BMP_OR"));

		SelectObject(Drawing::hDcOne,Drawing::hBmpOne);
		SelectObject(Drawing::hDcAnd,Drawing::hBmpAnd);
		SelectObject(Drawing::hDcOr,Drawing::hBmpOr);

		Drawing::hFontH=CreateFont(Grid::Interval*2-4,Grid::Interval-1,0  ,0,FW_THIN,false,false,false,SHIFTJIS_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,DEFAULT_PITCH|FF_DONTCARE,"ＭＳ 明朝");
		Drawing::hFontV=CreateFont(Grid::Interval*2-4,Grid::Interval-1,-900,0,FW_REGULAR,false,false,false,SHIFTJIS_CHARSET,OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,PROOF_QUALITY,FIXED_PITCH|FF_MODERN,"ＭＳ 明朝");

		ic.dwICC=ICC_BAR_CLASSES;
		ic.dwSize=sizeof(INITCOMMONCONTROLSEX);
		InitCommonControlsEx(&ic);
		hStatus=CreateStatusWindow(WS_CHILD|SBARS_SIZEGRIP|CCS_BOTTOM|WS_VISIBLE,"OK",hwnd,MyStatus);

		break;
	case WM_DESTROY:
		if(Automatic::ExecutingFastOperation)
		{
			Automatic::Execute=false;
			WaitForSingleObject(Automatic::hFastOperationThread,INFINITE);
			CloseHandle(Automatic::hFastOperationThread);
			//Automatic::ExecutingFastOperation=false;
		}

		DestroyWindow(hStatus);
		DeleteObject(Drawing::hFontV);
		DeleteObject(Drawing::hFontH);
		DeleteDC(Drawing::hDcOr);
		DeleteDC(Drawing::hDcAnd);
		DeleteDC(Drawing::hDcOne);
		DeleteObject(Drawing::hBmpOr);
		DeleteObject(Drawing::hBmpAnd);
		DeleteObject(Drawing::hBmpOne);
		//終了

		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hwnd,uMsg,wParam,lParam);
		break;
	}
	return 0;
}