#include"common.h"

#include<iostream>
#include<fstream>

extern HWND hMainWindow;

extern int filever;
extern void GetFileVer(std::ifstream &f);

namespace Edit
{
	extern vector2D coordinates1;
	extern vector2D coordinates2;
}

namespace Interface
{
	extern vector2D AlignedMouseCoordinates;	//Grid::Interval間隔で
	extern vector2D FirstAlignedMouseCoordinates;	//始点
}


namespace Place
{
	extern unsigned int NewBusWidth;	//バス幅
	extern Direction NewDirection;	//新しい記号の方向
	extern bool NewGateOperationisNOT;	//設置される論理ゲートの演算が否定されるか
	extern bool NewGateOperationisOR;	//設置される論理ゲートの演算がORならtrue,ANDならfalse
	extern unsigned int NewGateIn; //新しい論理ゲートの入力数
	extern Module *pModule;
	extern char NewLabelName[SIZE_OF_LABEL_NAME];	//新しいラベルの名前
}

extern Data main_data;
extern Data copy_data;


void Data::CopyRange(bool Erase,bool Copy)
{
	vector2D top_left(		Edit::coordinates1.Getx()<Edit::coordinates2.Getx() ? Edit::coordinates1.Getx() : Edit::coordinates2.Getx(),
							Edit::coordinates1.Gety()<Edit::coordinates2.Gety() ? Edit::coordinates1.Gety() : Edit::coordinates2.Gety());

	vector2D bottom_right(	Edit::coordinates1.Getx()>Edit::coordinates2.Getx() ? Edit::coordinates1.Getx() : Edit::coordinates2.Getx(),
							Edit::coordinates1.Gety()>Edit::coordinates2.Gety() ? Edit::coordinates1.Gety() : Edit::coordinates2.Gety());

	Direction tmpNewDirection = Place::NewDirection;
	int tmpNewBusWidth=Place::NewBusWidth;

	if(Copy)copy_data.Clear();

	for(unsigned int i=0;i<note.size();i++)
	{
		vector2D pos;
		pos=note[i].position();
		if(pos.Getx()>=top_left.Getx() && pos.Getx()<=bottom_right.Getx() && pos.Gety()>=top_left.Gety() && pos.Gety()<=bottom_right.Gety())
		{
			if(Copy)
			{
				Interface::AlignedMouseCoordinates=pos-Edit::coordinates2;
				Place::NewDirection = note[i].direction();
				strcpy_s(Place::NewLabelName,note[i].name());
				copy_data.WriteNote();
			}
			if(Erase) Delete(note,i--);
		}
	}

	for(unsigned int i=0;i<input.size();i++)
	{
		vector2D pos;
		pos=node_data[input[i].nodeindex()].position()-DirectionVector[input[i].direction()];
		if(pos.Getx()>=top_left.Getx() && pos.Getx()<=bottom_right.Getx() && pos.Gety()>=top_left.Gety() && pos.Gety()<=bottom_right.Gety())
		{
			if(Copy)
			{
				Interface::AlignedMouseCoordinates=pos-Edit::coordinates2;
				Place::NewDirection = input[i].direction();
				Place::NewBusWidth=input[i].nWire();
				strcpy_s(Place::NewLabelName,input[i].name());
				copy_data.PlaceInLabel();
			}
			if(Erase) EraseInputLabel(i--);
		}
	}

	for(unsigned int i=0;i<output.size();i++)
	{
		vector2D pos;
		pos=node_data[output[i].nodeindex()].position()-DirectionVector[output[i].direction()];
		if(pos.Getx()>=top_left.Getx() && pos.Getx()<=bottom_right.Getx() && pos.Gety()>=top_left.Gety() && pos.Gety()<=bottom_right.Gety())
		{
			if(Copy)
			{
				Interface::AlignedMouseCoordinates=pos-Edit::coordinates2;
				Place::NewDirection = ReverseDirection(output[i].direction());
				Place::NewBusWidth = node(output[i].nodeindex()).nWire();
				strcpy_s(Place::NewLabelName,output[i].name());
				copy_data.PlaceOutLabel();
			}
			if(Erase)EraseOutputLabel(i--);
		}
	}

	for(unsigned int i=0;i<busin.size();i++)
	{
		vector2D pos0,pos1;
		switch(busin[i].ReverseDirection())
		{
		case CLOCKWISE_000:
			pos0=node_data[busin[i].NodeInIndex()].position()+DirectionVector[CLOCKWISE_180]*3+DirectionVector[CLOCKWISE_270]*busin[i].nOut();
			pos1=node_data[busin[i].NodeInIndex()].position()-DirectionVector[CLOCKWISE_000]+DirectionVector[CLOCKWISE_090]*busin[i].nOut();
			break;
		case CLOCKWISE_090:
			pos0=node_data[busin[i].NodeInIndex()].position()+DirectionVector[CLOCKWISE_180]*busin[i].nOut()+DirectionVector[CLOCKWISE_270]*3;
			pos1=node_data[busin[i].NodeInIndex()].position()+DirectionVector[CLOCKWISE_000]*busin[i].nOut()-DirectionVector[CLOCKWISE_090];
			break;
		case CLOCKWISE_180:
			pos0=node_data[busin[i].NodeInIndex()].position()-DirectionVector[CLOCKWISE_180]+DirectionVector[CLOCKWISE_270]*busin[i].nOut();
			pos1=node_data[busin[i].NodeInIndex()].position()+DirectionVector[CLOCKWISE_000]*3+DirectionVector[CLOCKWISE_090]*busin[i].nOut();
			break;
		case CLOCKWISE_270:
			pos0=node_data[busin[i].NodeInIndex()].position()+DirectionVector[CLOCKWISE_180]*busin[i].nOut()-DirectionVector[CLOCKWISE_270];
			pos1=node_data[busin[i].NodeInIndex()].position()+DirectionVector[CLOCKWISE_000]*busin[i].nOut()+DirectionVector[CLOCKWISE_090]*3;
			break;
		}
		if(pos0.Getx()>=top_left.Getx() && pos0.Getx()<=bottom_right.Getx() && pos0.Gety()>=top_left.Gety() && pos0.Gety()<=bottom_right.Gety()
			&&pos1.Getx()>=top_left.Getx() && pos1.Getx()<=bottom_right.Getx() && pos1.Gety()>=top_left.Gety() && pos1.Gety()<=bottom_right.Gety())
		{
			if(Copy)
			{
				Interface::AlignedMouseCoordinates=node_data[busin[i].NodeInIndex()].position()+DirectionVector[busin[i].direction()]-Edit::coordinates2;
				Place::NewDirection=busin[i].direction();
				Place::NewBusWidth=busin[i].nOut();
				copy_data.PlaceBusIn();
			}
			if(Erase)EraseBusIn(i--);
		}
	}

	for(unsigned int i=0;i<busout.size();i++)
	{
		vector2D pos0,pos1;
		switch(busout[i].direction())
		{
		case CLOCKWISE_000:
			pos0=node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_180]*3+DirectionVector[CLOCKWISE_270]*busout[i].nIn();
			pos1=node_data[busout[i].NodeOutIndex()].position()-DirectionVector[CLOCKWISE_000]+DirectionVector[CLOCKWISE_090]*busout[i].nIn();
			break;
		case CLOCKWISE_090:
			pos0=node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_180]*busout[i].nIn()+DirectionVector[CLOCKWISE_270]*3;
			pos1=node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_000]*busout[i].nIn()-DirectionVector[CLOCKWISE_090];
			break;
		case CLOCKWISE_180:
			pos0=node_data[busout[i].NodeOutIndex()].position()-DirectionVector[CLOCKWISE_180]+DirectionVector[CLOCKWISE_270]*busout[i].nIn();
			pos1=node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_000]*3+DirectionVector[CLOCKWISE_090]*busout[i].nIn();
			break;
		case CLOCKWISE_270:
			pos0=node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_180]*busout[i].nIn()-DirectionVector[CLOCKWISE_270];
			pos1=node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_000]*busout[i].nIn()+DirectionVector[CLOCKWISE_090]*3;
			break;
		}
		if(pos0.Getx()>=top_left.Getx() && pos0.Getx()<=bottom_right.Getx() && pos0.Gety()>=top_left.Gety() && pos0.Gety()<=bottom_right.Gety()
			&&pos1.Getx()>=top_left.Getx() && pos1.Getx()<=bottom_right.Getx() && pos1.Gety()>=top_left.Gety() && pos1.Gety()<=bottom_right.Gety())
		{
			if(Copy)
			{
				Interface::AlignedMouseCoordinates=node_data[busout[i].NodeOutIndex()].position()+DirectionVector[busout[i].ReverseDirection()]-Edit::coordinates2;
				Place::NewDirection=busout[i].direction();
				Place::NewBusWidth=busout[i].nIn();
				copy_data.PlaceBusOut();
			}
			if(Erase)EraseBusOut(i--);
		}
	}

	unsigned int TurnOutOfNewGateIn = Place::NewGateIn;
	for(unsigned int i=0;i<gate.size();i++)
	{
		vector2D pos0,pos1;
		switch(gate[i].direction())
		{
		case CLOCKWISE_000:
			pos0=node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_180]*3+DirectionVector[CLOCKWISE_270]*gate[i].nIn();
			pos1=node_data[gate[i].NodeOutIndex()].position()-DirectionVector[CLOCKWISE_000]+DirectionVector[CLOCKWISE_090]*gate[i].nIn();
			break;
		case CLOCKWISE_090:
			pos0=node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_180]*gate[i].nIn()+DirectionVector[CLOCKWISE_270]*3;
			pos1=node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_000]*gate[i].nIn()-DirectionVector[CLOCKWISE_090];
			break;
		case CLOCKWISE_180:
			pos0=node_data[gate[i].NodeOutIndex()].position()-DirectionVector[CLOCKWISE_180]+DirectionVector[CLOCKWISE_270]*gate[i].nIn();
			pos1=node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_000]*3+DirectionVector[CLOCKWISE_090]*gate[i].nIn();
			break;
		case CLOCKWISE_270:
			pos0=node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_180]*gate[i].nIn()-DirectionVector[CLOCKWISE_270];
			pos1=node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_000]*gate[i].nIn()+DirectionVector[CLOCKWISE_090]*3;
			break;
		}
		if(pos0.Getx()>=top_left.Getx() && pos0.Getx()<=bottom_right.Getx() && pos0.Gety()>=top_left.Gety() && pos0.Gety()<=bottom_right.Gety()
			&&pos1.Getx()>=top_left.Getx() && pos1.Getx()<=bottom_right.Getx() && pos1.Gety()>=top_left.Gety() && pos1.Gety()<=bottom_right.Gety())
		{
			if(Copy)
			{
				Interface::AlignedMouseCoordinates=node_data[gate[i].NodeOutIndex()].position()+DirectionVector[gate[i].ReverseDirection()]-Edit::coordinates2;
				Place::NewDirection=gate[i].direction();
				Place::NewGateOperationisNOT = gate[i].isNOT();
				Place::NewGateOperationisOR = gate[i].isOR();
				Place::NewGateIn=gate[i].nIn();
				Place::NewBusWidth=node(gate[i].NodeOutIndex()).nWire();
				copy_data.PlaceGate();
			}
			if(Erase)EraseGate(i--);
		}
	}
	Place::NewGateIn = TurnOutOfNewGateIn;

	for(unsigned int i=0;i<conductor.size();i++)
	{
		vector2D pos0,pos1;
		pos0=node_data[conductor[i].nodeindex0()].position();
		pos1=node_data[conductor[i].nodeindex1()].position();
		if(pos0.Getx()>=top_left.Getx() && pos0.Getx()<=bottom_right.Getx() && pos0.Gety()>=top_left.Gety() && pos0.Gety()<=bottom_right.Gety()
			&&pos1.Getx()>=top_left.Getx() && pos1.Getx()<=bottom_right.Getx() && pos1.Gety()>=top_left.Gety() && pos1.Gety()<=bottom_right.Gety())
		{
			if(Copy)
			{
				Interface::FirstAlignedMouseCoordinates=pos0-Edit::coordinates2;
				Interface::AlignedMouseCoordinates=pos1-Edit::coordinates2;
				Place::NewBusWidth=conductor[i].nWire();
				copy_data.PlaceConductor();
			}
			if(Erase)EraseConductor(i--);
		}
	}

	for(unsigned int i=0;i<module.size();i++)
	{
		vector2D pos0,pos1;
		pos0=module[i].Center()+DirectionVector[CLOCKWISE_270]*(module[i].InternalHalfHeight()+module[i].Margin(3))+DirectionVector[CLOCKWISE_180]*(module[i].InternalHalfWidth()+module[i].Margin(2));
		pos1=module[i].Center()+DirectionVector[CLOCKWISE_090]*(module[i].InternalHalfHeight()+module[i].Margin(1))+DirectionVector[CLOCKWISE_000]*(module[i].InternalHalfWidth()+module[i].Margin(0));

		if(pos0.Getx()>=top_left.Getx() && pos0.Getx()<=bottom_right.Getx() && pos0.Gety()>=top_left.Gety() && pos0.Gety()<=bottom_right.Gety()
			&&pos1.Getx()>=top_left.Getx() && pos1.Getx()<=bottom_right.Getx() && pos1.Gety()>=top_left.Gety() && pos1.Gety()<=bottom_right.Gety())
		{
			if(Copy)
			{
				Place::pModule=&module[i];//Place::NewModule=module[i];
				Interface::AlignedMouseCoordinates=module[i].Center()-Edit::coordinates2;
				copy_data.PlaceModule();
			}
			if(Erase)EraseModule(i--);
		}
	}

	Place::NewDirection = tmpNewDirection;
	Place::NewBusWidth=tmpNewBusWidth;

	if(Copy)
	{
		char ExePath[MAX_PATH+1];
		char FileEdit[MAX_PATH+1];
		char drive[MAX_PATH+1],dir[MAX_PATH+1];
		GetModuleFileName(NULL,ExePath,MAX_PATH);
		_splitpath_s(ExePath,drive,MAX_PATH+1,dir,MAX_PATH+1,NULL,0,NULL,0);
		_makepath_s(FileEdit,MAX_PATH+1,drive,dir,"FileUsedByEditingOfAst","log");
		std::ofstream f(FileEdit);
		f << "Ast0.85" << std::endl <<std::endl;
		f << copy_data;
		f.close();
	}
}

void LoadCopiedData()
{
	char ExePath[MAX_PATH+1];
	char FileEdit[MAX_PATH+1];
	char drive[MAX_PATH+1],dir[MAX_PATH+1];
	GetModuleFileName(NULL,ExePath,MAX_PATH);
	_splitpath_s(ExePath,drive,MAX_PATH+1,dir,MAX_PATH+1,NULL,0,NULL,0);
	_makepath_s(FileEdit,MAX_PATH+1,drive,dir,"FileUsedByEditingOfAst","log");
	std::ifstream f(FileEdit);

	if(f)
	{
		GetFileVer(f);
		if (filever == -1)
		{
			MessageBox(hMainWindow, "バージョンの異なるファイルです", "エラー", MB_OK);
		}
		else
		{
			copy_data.Clear();
			f >> copy_data;
			if (!f.good())
			{
				copy_data.Clear();
				MessageBox(hMainWindow, "ファイルが壊れています", "エラー", MB_OK);
			}
		}
		f.close();
	}
}

bool Data::AbleToPaste()
{
	unsigned int i;
	vector2D AngleVector = DirectionVector[Place::NewDirection];
	Direction tmpNewDirection=Place::NewDirection;
	int tmpNewBusWidth=Place::NewBusWidth;
	vector2D MouseCoordinates=Interface::AlignedMouseCoordinates;
	for(i=0;i<input.size();i++)
	{
		vector2D pos;
		pos=node_data[input[i].nodeindex()].position()-DirectionVector[input[i].direction()];
		Interface::AlignedMouseCoordinates=pos*AngleVector+MouseCoordinates;
		Place::NewDirection = Direction((input[i].direction()+tmpNewDirection)&7);
		Place::NewBusWidth = input[i].nWire();
		strcpy_s(Place::NewLabelName,input[i].name());
		if(!main_data.AbleToPlaceLabelIn())break;
	}
	Interface::AlignedMouseCoordinates=MouseCoordinates;
	Place::NewDirection = tmpNewDirection;
	Place::NewBusWidth = tmpNewBusWidth;
	if(i<input.size())return false;

	for(i=0;i<output.size();i++)
	{
		vector2D pos;
		pos=node_data[output[i].nodeindex()].position()-DirectionVector[output[i].direction()];
		Interface::AlignedMouseCoordinates=pos*AngleVector+MouseCoordinates;
		Place::NewDirection = ReverseDirection(Direction((output[i].direction()+tmpNewDirection)&7));
		Place::NewBusWidth = node(output[i].nodeindex()).nWire();
		strcpy_s(Place::NewLabelName,output[i].name());
		if(!main_data.AbleToPlaceLabelOut())break;
	}
	Interface::AlignedMouseCoordinates=MouseCoordinates;
	Place::NewDirection = tmpNewDirection;
	Place::NewBusWidth = tmpNewBusWidth;
	if(i<output.size())return false;

	for(i=0;i<busin.size();i++)
	{
		Interface::AlignedMouseCoordinates=(node_data[busin[i].NodeInIndex()].position()+DirectionVector[busin[i].direction()])*AngleVector+MouseCoordinates;
		Place::NewDirection=Direction((busin[i].direction()+tmpNewDirection)&7);
		Place::NewBusWidth=busin[i].nOut();
		if(!main_data.AbleToPlaceBusIn())break;
	}
	Interface::AlignedMouseCoordinates=MouseCoordinates;
	Place::NewDirection = tmpNewDirection;
	Place::NewBusWidth = tmpNewBusWidth;
	if(i<busin.size())return false;

	for(i=0;i<busout.size();i++)
	{
		Interface::AlignedMouseCoordinates=(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[busout[i].ReverseDirection()])*AngleVector+MouseCoordinates;
		Place::NewDirection=Direction((busout[i].direction()+tmpNewDirection)&7);
		Place::NewBusWidth=busout[i].nIn();
		if(!main_data.AbleToPlaceBusOut())break;
	}
	Interface::AlignedMouseCoordinates=MouseCoordinates;
	Place::NewDirection = tmpNewDirection;
	Place::NewBusWidth = tmpNewBusWidth;
	if(i<busout.size())return false;

	unsigned int TurnOutOfNewGateIn = Place::NewGateIn;
	for(i=0;i<gate.size();i++)
	{
		Interface::AlignedMouseCoordinates=(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[gate[i].ReverseDirection()])*AngleVector+MouseCoordinates;
		Place::NewDirection=Direction((gate[i].direction()+tmpNewDirection)&7);
		Place::NewGateOperationisNOT = gate[i].isNOT();
		Place::NewGateOperationisOR = gate[i].isOR();
		Place::NewGateIn=gate[i].nIn();
		Place::NewBusWidth=node(gate[i].NodeOutIndex()).nWire();
		if(!main_data.AbleToPlaceGate())break;
	}
	Place::NewGateIn = TurnOutOfNewGateIn;
	Interface::AlignedMouseCoordinates=MouseCoordinates;
	Place::NewDirection = tmpNewDirection;
	Place::NewBusWidth = tmpNewBusWidth;
	if(i<gate.size())return false;

	for(i=0;i<conductor.size();i++)
	{
		Interface::FirstAlignedMouseCoordinates=node_data[conductor[i].nodeindex0()].position()*AngleVector+MouseCoordinates;
		Interface::AlignedMouseCoordinates=node_data[conductor[i].nodeindex1()].position()*AngleVector+MouseCoordinates;
		Place::NewBusWidth = conductor[i].nWire();
		if(!main_data.AbleToPlaceConductor())break;
	}
	Interface::AlignedMouseCoordinates=MouseCoordinates;
	Place::NewDirection = tmpNewDirection;
	Place::NewBusWidth = tmpNewBusWidth;
	if(i<conductor.size())return false;

	for(i=0;i<module.size();i++)
	{
		Place::pModule=&module[i];
		Place::NewDirection = module[i].direction();
		module[i].SetDirection(Direction((Place::NewDirection+tmpNewDirection)&7));
		Interface::AlignedMouseCoordinates=module[i].Center()*AngleVector+MouseCoordinates;
		bool able = main_data.AbleToPlaceModule();
		module[i].SetDirection(Place::NewDirection);
		if (!able) break;
	}
	Interface::AlignedMouseCoordinates=MouseCoordinates;
	Place::NewDirection = tmpNewDirection;
	Place::NewBusWidth = tmpNewBusWidth;
	if(i<module.size())return false;

	return true;
}

void Data::Paste()
{
	vector2D AngleVector = DirectionVector[Place::NewDirection];
	vector2D MouseCoordinates=Interface::AlignedMouseCoordinates;
	Direction tmpNewDirection=Place::NewDirection;
	int tmpNewBusWidth=Place::NewBusWidth;
	for(unsigned int i=0;i<note.size();i++)
	{
		vector2D pos;
		pos=note[i].position();
		Interface::AlignedMouseCoordinates=pos*AngleVector+MouseCoordinates;
		Place::NewDirection = Direction((note[i].direction()+tmpNewDirection)&7);
		strcpy_s(Place::NewLabelName,note[i].name());
		main_data.WriteNote();
	}
	for(unsigned int i=0;i<input.size();i++)
	{
		vector2D pos;
		pos=node_data[input[i].nodeindex()].position()-DirectionVector[input[i].direction()];
		Interface::AlignedMouseCoordinates=pos*AngleVector+MouseCoordinates;
		Place::NewDirection = Direction((input[i].direction()+tmpNewDirection)&7);
		Place::NewBusWidth = input[i].nWire();
		strcpy_s(Place::NewLabelName,input[i].name());
		main_data.PlaceInLabel();
	}
	for(unsigned int i=0;i<output.size();i++)
	{
		vector2D pos;
		pos=node_data[output[i].nodeindex()].position()-DirectionVector[output[i].direction()];
		Interface::AlignedMouseCoordinates=pos*AngleVector+MouseCoordinates;
		Place::NewDirection = ReverseDirection(Direction((output[i].direction()+tmpNewDirection)&7));
		Place::NewBusWidth = node(output[i].nodeindex()).nWire();
		strcpy_s(Place::NewLabelName,output[i].name());
		main_data.PlaceOutLabel();
	}
	for(unsigned int i=0;i<busin.size();i++)
	{
		Interface::AlignedMouseCoordinates=(node_data[busin[i].NodeInIndex()].position()+DirectionVector[busin[i].direction()])*AngleVector+MouseCoordinates;
		Place::NewDirection=Direction((busin[i].direction()+tmpNewDirection)&7);
		Place::NewBusWidth = busin[i].nOut();
		main_data.PlaceBusIn();
	}
	for(unsigned int i=0;i<busout.size();i++)
	{
		Interface::AlignedMouseCoordinates=(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[busout[i].ReverseDirection()])*AngleVector+MouseCoordinates;
		Place::NewDirection=Direction((busout[i].direction()+tmpNewDirection)&7);
		Place::NewBusWidth = busout[i].nIn();
		main_data.PlaceBusOut();
	}
	unsigned int TurnOutOfNewGateIn = Place::NewGateIn;
	for(unsigned int i=0;i<gate.size();i++)
	{
		Interface::AlignedMouseCoordinates=(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[gate[i].ReverseDirection()])*AngleVector+MouseCoordinates;
		Place::NewDirection=Direction((gate[i].direction()+tmpNewDirection)&7);
		Place::NewGateOperationisNOT = gate[i].isNOT();
		Place::NewGateOperationisOR = gate[i].isOR();
		Place::NewGateIn=gate[i].nIn();
		Place::NewBusWidth = node(gate[i].NodeOutIndex()).nWire();
		main_data.PlaceGate();
	}
	Place::NewGateIn = TurnOutOfNewGateIn;
	for(unsigned int i=0;i<conductor.size();i++)
	{
		Interface::FirstAlignedMouseCoordinates=node_data[conductor[i].nodeindex0()].position()*AngleVector+MouseCoordinates;
		Interface::AlignedMouseCoordinates=node_data[conductor[i].nodeindex1()].position()*AngleVector+MouseCoordinates;
		Place::NewBusWidth = conductor[i].nWire();
		main_data.PlaceConductor();
	}
	for(unsigned int i=0;i<module.size();i++)
	{
		Place::pModule=&module[i];
		Interface::AlignedMouseCoordinates=module[i].Center()*AngleVector+MouseCoordinates;
		Place::NewDirection = module[i].direction();
		module[i].SetDirection(Direction((Place::NewDirection + tmpNewDirection) & 7));
		main_data.PlaceModule();
		module[i].SetDirection(Place::NewDirection);
	}
	Place::NewBusWidth=tmpNewBusWidth;
	Place::NewDirection = tmpNewDirection;
	Interface::AlignedMouseCoordinates=MouseCoordinates;
}