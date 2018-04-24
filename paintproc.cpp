#define _CRT_SECURE_NO_WARNINGS
#define _CRT_NON_CONFORMING_SWPRINTFS

#include"common.h"
#include"resource.h"

extern HWND hMainWindow;
extern Data main_data;

namespace Grid
{
	extern int Interval;
	extern bool Draw;
}

namespace Automatic
{
	extern int ClockInLabel;
}

namespace Interface
{
	extern vector2D AlignedMouseCoordinates;
	extern vector2D FirstAlignedMouseCoordinates;	//始点
	extern bool Mouse_Drag;	//ドラッグ中ならtrue
	extern POINT Scroll_Coordinates;
	extern RECT ClientRect;
}

namespace Edit
{
	extern vector2D coordinates1;
	extern vector2D coordinates2;
}

namespace Place
{
	extern unsigned int NewBusWidth;
	extern Direction NewDirection;
	extern char NewLabelName[SIZE_OF_LABEL_NAME];
	extern bool NewGateOperationisNOT;
	extern bool NewGateOperationisOR;
	extern unsigned int NewGateIn;
	extern Module NewModule;
}

extern vector2D AlignAngle(vector2D center,vector2D end);

namespace Drawing
{
	extern HDC hDcOne,hDcAnd,hDcOr;
	extern HFONT hFontH,hFontV;
}

extern Data copy_data;

POINT Offset;


void DrawGrid(HDC hdc)
{
	LONG i;
	HPEN hPen,hOldPen;


	hPen=CreatePen(PS_SOLID,0,RGB(192,192,192));
	hOldPen=(HPEN)SelectObject(hdc,hPen);

	for(i=Offset.x;i>0;i-=Grid::Interval)
	{
		MoveToEx(hdc,i,0,NULL);
		LineTo(hdc,i,Interface::ClientRect.bottom);
	}
	for(i=Offset.x+Grid::Interval;i<Interface::ClientRect.right;i+=Grid::Interval)
	{
		MoveToEx(hdc,i,0,NULL);
		LineTo(hdc,i,Interface::ClientRect.bottom);
	}
	for(i=Offset.y;i>0;i-=Grid::Interval)
	{
		MoveToEx(hdc,0,i,NULL);
		LineTo(hdc,Interface::ClientRect.right,i);
	}
	for(i=Offset.y+Grid::Interval;i<Interface::ClientRect.bottom;i+=Grid::Interval)
	{
		MoveToEx(hdc,0,i,NULL);
		LineTo(hdc,Interface::ClientRect.right,i);
	}
	SelectObject(hdc,hOldPen);
	DeleteObject(hPen);
}

void Rectangle(HDC hdc, POINT upper_left_point, POINT lower_right_point)
{
	MoveToEx(hdc, upper_left_point.x, upper_left_point.y, NULL);
	LineTo(hdc, lower_right_point.x, upper_left_point.y);
	LineTo(hdc, lower_right_point.x, lower_right_point.y);
	LineTo(hdc, upper_left_point.x, lower_right_point.y);
	LineTo(hdc, upper_left_point.x, upper_left_point.y);
}

void DrawGateRight(HDC hdc,vector2D p,int nIn)
{
	POINT p1,p2;
	vector2D v;
	v=p+DirectionVector[CLOCKWISE_180]*2+DirectionVector[CLOCKWISE_270]*nIn;
	p1=v.toPOINT();
	v=p+DirectionVector[CLOCKWISE_090]*nIn;
	p2=v.toPOINT();
	Rectangle(hdc,p1,p2);
	v=p+DirectionVector[CLOCKWISE_180]*2+DirectionVector[CLOCKWISE_270]*(nIn-1);
	p1=v.toPOINT();
	v=p+DirectionVector[CLOCKWISE_180]*3+DirectionVector[CLOCKWISE_270]*(nIn-1);
	p2=v.toPOINT();
	for(int i=0;i<nIn;i++,p1.y+=Grid::Interval*2,p2.y+=Grid::Interval*2)
		MoveToEx(hdc,p1.x,p1.y,NULL),LineTo(hdc,p2.x,p2.y);
}

void DrawGateDown(HDC hdc,vector2D p,int nIn)
{
	POINT p1,p2;
	vector2D v;
	v=p+DirectionVector[CLOCKWISE_270]*2+DirectionVector[CLOCKWISE_180]*nIn;
	p1=v.toPOINT();
	v=p+DirectionVector[CLOCKWISE_000]*nIn;
	p2=v.toPOINT();
	Rectangle(hdc,p1,p2);
	v=p+DirectionVector[CLOCKWISE_270]*2+DirectionVector[CLOCKWISE_000]*(nIn-1);
	p1=v.toPOINT();
	v=p+DirectionVector[CLOCKWISE_270]*3+DirectionVector[CLOCKWISE_000]*(nIn-1);
	p2=v.toPOINT();
	for(int i=0;i<nIn;i++,p1.x-=Grid::Interval*2,p2.x-=Grid::Interval*2)
		MoveToEx(hdc,p1.x,p1.y,NULL),LineTo(hdc,p2.x,p2.y);
}

void DrawGateLeft(HDC hdc,vector2D p,int nIn)
{
	POINT p1,p2;
	vector2D v;
	v=p+DirectionVector[CLOCKWISE_270]*nIn;
	p1=v.toPOINT();
	v=p+DirectionVector[CLOCKWISE_000]*2+DirectionVector[CLOCKWISE_090]*nIn;
	p2=v.toPOINT();
	Rectangle(hdc,p1,p2);
	v=p+DirectionVector[CLOCKWISE_000]*2+DirectionVector[CLOCKWISE_090]*(nIn-1);
	p1=v.toPOINT();
	v=p+DirectionVector[CLOCKWISE_000]*3+DirectionVector[CLOCKWISE_090]*(nIn-1);
	p2=v.toPOINT();
	for(int i=0;i<nIn;i++,p1.y-=Grid::Interval*2,p2.y-=Grid::Interval*2)
		MoveToEx(hdc,p1.x,p1.y,NULL),LineTo(hdc,p2.x,p2.y);
}

void DrawGateUp(HDC hdc,vector2D p,int nIn)
{
	POINT p1,p2;
	vector2D v;
	v=p+DirectionVector[CLOCKWISE_180]*nIn;
	p1=v.toPOINT();
	v=p+DirectionVector[CLOCKWISE_000]*nIn+DirectionVector[CLOCKWISE_090]*2;
	p2=v.toPOINT();
	Rectangle(hdc,p1,p2);
	v=p+DirectionVector[CLOCKWISE_090]*2+DirectionVector[CLOCKWISE_180]*(nIn-1);
	p1=v.toPOINT();
	v=p+DirectionVector[CLOCKWISE_090]*3+DirectionVector[CLOCKWISE_180]*(nIn-1);
	p2=v.toPOINT();
	for(int i=0;i<nIn;i++,p1.x+=Grid::Interval*2,p2.x+=Grid::Interval*2)
		MoveToEx(hdc,p1.x,p1.y,NULL),LineTo(hdc,p2.x,p2.y);
}

void Draw_Gate(HDC hdc,vector2D p,Direction d,bool isOR,int nIn)
{
	POINT pnt,p1;
	HDC hDcGateClass = (nIn == 1 ? Drawing::hDcOne : (isOR ? Drawing::hDcOr : Drawing::hDcAnd));

	pnt=p.toPOINT();

	switch(d)
	{
	case CLOCKWISE_000:
		DrawGateRight(hdc,p,nIn);
		p1=(p+DirectionVector[CLOCKWISE_180]*2+DirectionVector[CLOCKWISE_270]*nIn).toPOINT();
		StretchBlt(hdc,p1.x+1,p1.y+1,Grid::Interval*2-2,Grid::Interval*2-2,hDcGateClass,0,0,38,38,SRCCOPY);
		MoveToEx(hdc,pnt.x,pnt.y,NULL);
		pnt.x+=Grid::Interval;
		LineTo(hdc,pnt.x,pnt.y);
		break;
	case CLOCKWISE_090:
		DrawGateDown(hdc,p,nIn);
		p1=(p+DirectionVector[CLOCKWISE_270]*2+DirectionVector[CLOCKWISE_000]*(nIn-2)).toPOINT();
		StretchBlt(hdc,p1.x+1,p1.y+1,Grid::Interval*2-2,Grid::Interval*2-2,hDcGateClass,0,0,38,38,SRCCOPY);
		MoveToEx(hdc,pnt.x,pnt.y,NULL);
		pnt.y+=Grid::Interval;
		LineTo(hdc,pnt.x,pnt.y);
		break;
	case CLOCKWISE_180:
		DrawGateLeft(hdc,p,nIn);
		p1=(p+DirectionVector[CLOCKWISE_090]*(nIn-2)).toPOINT();
		StretchBlt(hdc,p1.x+1,p1.y+1,Grid::Interval*2-2,Grid::Interval*2-2,hDcGateClass,0,0,38,38,SRCCOPY);
		MoveToEx(hdc,pnt.x,pnt.y,NULL);
		pnt.x-=Grid::Interval;
		LineTo(hdc,pnt.x,pnt.y);
		break;
	case CLOCKWISE_270:
		DrawGateUp(hdc,p,nIn);
		p1=(p+DirectionVector[CLOCKWISE_180]*nIn).toPOINT();
		StretchBlt(hdc,p1.x+1,p1.y+1,Grid::Interval*2-2,Grid::Interval*2-2,hDcGateClass,0,0,38,38,SRCCOPY);
		MoveToEx(hdc,pnt.x,pnt.y,NULL);
		pnt.y-=Grid::Interval;
		LineTo(hdc,pnt.x,pnt.y);
		break;
	}
}

void Draw_Gate_With_Not(HDC hdc,vector2D p,Direction d,bool isOR,int nIn)
{
	POINT pnt;
	pnt=p.toPOINT();

	Draw_Gate(hdc,p,d,isOR,nIn);

	switch(d)
	{
	case CLOCKWISE_000:
		Ellipse(hdc,pnt.x,pnt.y-Grid::Interval/4,pnt.x+Grid::Interval/2,pnt.y+Grid::Interval/4);
		break;
	case CLOCKWISE_090:
		Ellipse(hdc,pnt.x-Grid::Interval/4,pnt.y,pnt.x+Grid::Interval/4,pnt.y+Grid::Interval/2);
		break;
	case CLOCKWISE_180:
		Ellipse(hdc,pnt.x-Grid::Interval/2,pnt.y-Grid::Interval/4,pnt.x,pnt.y+Grid::Interval/4);
		break;
	case CLOCKWISE_270:
		Ellipse(hdc,pnt.x-Grid::Interval/4,pnt.y-Grid::Interval/2,pnt.x+Grid::Interval/4,pnt.y);
		break;
	}
}

//ゲートの描画
void Data::DrawGate(HDC hdc)
{
	HPEN hPen=CreatePen(PS_SOLID,1,RGB(255,0,0));
	for(unsigned int i=0;i<gate.size();i++)
	{
		Gate g=gate[i];
		if (g.isNOT())
			Draw_Gate_With_Not(hdc, node(g.NodeOutIndex()).position() - DirectionVector[g.direction()], g.direction(), g.isOR(), g.nIn());
		else
			Draw_Gate(hdc, node(g.NodeOutIndex()).position() - DirectionVector[g.direction()], g.direction(), g.isOR(), g.nIn());
	}
	DeleteObject(hPen);
}

//ゲートのテンプレートの描画
void DrawTemplate(HDC hdc)
{
	HPEN hPen,hOldPen;
	hPen=CreatePen(PS_SOLID,1,RGB(127,127,127));
	hOldPen=(HPEN)SelectObject(hdc,hPen);	//正しい処理か確認
	if(Place::NewGateOperationisNOT)
		Draw_Gate_With_Not(hdc, Interface::AlignedMouseCoordinates, Place::NewDirection, Place::NewGateOperationisOR, Place::NewGateIn);
	else
		Draw_Gate(hdc, Interface::AlignedMouseCoordinates, Place::NewDirection, Place::NewGateOperationisOR, Place::NewGateIn);
//	ClassifyOperationAndDraw(hdc,Place::NewGateOperation,Interface::AlignedMouseCoordinates,Place::NewDirection,Place::NewGateOperation==NOT ? 1: Place::NewGateIn);
	SelectObject(hdc,hOldPen);	//正しい処理か確認
	DeleteObject(hPen);
}

void Data::DrawBusIn(HDC hdc)
{
	for(unsigned int i=0;i<busin.size();i++)
	{
		vector2D v;
		POINT pnt;
		v=node(busin[i].NodeInIndex()).position()+DirectionVector[busin[i].direction()];
		pnt=v.toPOINT();
		switch(busin[i].direction())
		{
		case CLOCKWISE_000:
			DrawGateLeft(hdc,v,busin[i].nOut());
			MoveToEx(hdc,pnt.x,pnt.y,NULL);
			pnt.x-=Grid::Interval;
			LineTo(hdc,pnt.x,pnt.y);
			break;
		case CLOCKWISE_090:
			DrawGateUp(hdc,v,busin[i].nOut());
			MoveToEx(hdc,pnt.x,pnt.y,NULL);
			pnt.y-=Grid::Interval;
			LineTo(hdc,pnt.x,pnt.y);
			break;
		case CLOCKWISE_180:
			DrawGateRight(hdc,v,busin[i].nOut());
			MoveToEx(hdc,pnt.x,pnt.y,NULL);
			pnt.x+=Grid::Interval;
			LineTo(hdc,pnt.x,pnt.y);
			break;
		case CLOCKWISE_270:
			DrawGateDown(hdc,v,busin[i].nOut());
			MoveToEx(hdc,pnt.x,pnt.y,NULL);
			pnt.y+=Grid::Interval;
			LineTo(hdc,pnt.x,pnt.y);
			break;
		}
	}
}

void DrawBusInTemplate(HDC hdc)
{
	vector2D v;
	POINT pnt;
	HPEN hPen,hOldPen;

	hPen=CreatePen(PS_SOLID,1,RGB(127,127,127));
	hOldPen=(HPEN)SelectObject(hdc,hPen);

	v=Interface::AlignedMouseCoordinates;
	pnt=v.toPOINT();
	switch(Place::NewDirection)
	{
	case CLOCKWISE_000:
		DrawGateLeft(hdc,v,Place::NewBusWidth);
		MoveToEx(hdc,pnt.x,pnt.y,NULL);
		pnt.x-=Grid::Interval;
		LineTo(hdc,pnt.x,pnt.y);
		break;
	case CLOCKWISE_090:
		DrawGateUp(hdc,v,Place::NewBusWidth);
		MoveToEx(hdc,pnt.x,pnt.y,NULL);
		pnt.y-=Grid::Interval;
		LineTo(hdc,pnt.x,pnt.y);
		break;
	case CLOCKWISE_180:
		DrawGateRight(hdc,v,Place::NewBusWidth);
		MoveToEx(hdc,pnt.x,pnt.y,NULL);
		pnt.x+=Grid::Interval;
		LineTo(hdc,pnt.x,pnt.y);
		break;
	case CLOCKWISE_270:
		DrawGateDown(hdc,v,Place::NewBusWidth);
		MoveToEx(hdc,pnt.x,pnt.y,NULL);
		pnt.y+=Grid::Interval;
		LineTo(hdc,pnt.x,pnt.y);
		break;
	}

	SelectObject(hdc,hOldPen);	//正しい処理か確認
	DeleteObject(hPen);
}

void Data::DrawBusOut(HDC hdc)
{
	for(unsigned int i=0;i<busout.size();i++)
	{
		vector2D v;
		POINT pnt;
		v=node(busout[i].NodeOutIndex()).position()-DirectionVector[busout[i].direction()];
		pnt=v.toPOINT();

		switch(busout[i].direction())
		{
		case CLOCKWISE_000:
			DrawGateRight(hdc,v,busout[i].nIn());
			MoveToEx(hdc,pnt.x,pnt.y,NULL);
			pnt.x+=Grid::Interval;
			LineTo(hdc,pnt.x,pnt.y);
			break;
		case CLOCKWISE_090:
			DrawGateDown(hdc,v,busout[i].nIn());
			MoveToEx(hdc,pnt.x,pnt.y,NULL);
			pnt.y+=Grid::Interval;
			LineTo(hdc,pnt.x,pnt.y);
			break;
		case CLOCKWISE_180:
			DrawGateLeft(hdc,v,busout[i].nIn());
			MoveToEx(hdc,pnt.x,pnt.y,NULL);
			pnt.x-=Grid::Interval;
			LineTo(hdc,pnt.x,pnt.y);
			break;
		case CLOCKWISE_270:
			DrawGateUp(hdc,v,busout[i].nIn());
			MoveToEx(hdc,pnt.x,pnt.y,NULL);
			pnt.y-=Grid::Interval;
			LineTo(hdc,pnt.x,pnt.y);
			break;
		}
	}
}

void DrawBusOutTemplate(HDC hdc)
{
	vector2D v;
	POINT pnt;
	HPEN hPen,hOldPen;

	hPen=CreatePen(PS_SOLID,1,RGB(127,127,127));
	hOldPen=(HPEN)SelectObject(hdc,hPen);

	v=Interface::AlignedMouseCoordinates;
	pnt=v.toPOINT();
	switch(Place::NewDirection)
	{
	case CLOCKWISE_000:
		DrawGateRight(hdc,v,Place::NewBusWidth);
		MoveToEx(hdc,pnt.x,pnt.y,NULL);
		pnt.x+=Grid::Interval;
		LineTo(hdc,pnt.x,pnt.y);
		break;
	case CLOCKWISE_090:
		DrawGateDown(hdc,v,Place::NewBusWidth);
		MoveToEx(hdc,pnt.x,pnt.y,NULL);
		pnt.y+=Grid::Interval;
		LineTo(hdc,pnt.x,pnt.y);
		break;
	case CLOCKWISE_180:
		DrawGateLeft(hdc,v,Place::NewBusWidth);
		MoveToEx(hdc,pnt.x,pnt.y,NULL);
		pnt.x-=Grid::Interval;
		LineTo(hdc,pnt.x,pnt.y);
		break;
	case CLOCKWISE_270:
		DrawGateUp(hdc,v,Place::NewBusWidth);
		MoveToEx(hdc,pnt.x,pnt.y,NULL);
		pnt.y-=Grid::Interval;
		LineTo(hdc,pnt.x,pnt.y);
		break;
	}

	SelectObject(hdc,hOldPen);	//正しい処理か確認
	DeleteObject(hPen);
}

//モジュールの描画
void Data::DrawModule(HDC hdc)
{
	vector2D v;
	POINT pnt;

	POINT upper_left_point;
	POINT lower_right_point;


	for(unsigned int i=0;i<module.size();i++)
	{
		Module &mdl=module[i];

		v=mdl.Center()+DirectionVector[CLOCKWISE_270]*(mdl.GetSize(2)-1)+DirectionVector[CLOCKWISE_180]*(mdl.InternalHalfWidth()+mdl.Margin(2));
		pnt=v.toPOINT();
		SelectObject(hdc,Drawing::hFontH);
		for(int i=0;i<mdl.GetSize(2);i++,pnt.y+=Grid::Interval*2)
		{
			switch(mdl.Get(2,i).GetDeviceClass())
			{
				int len;
			case module_in:
				len=strlen(mdl.GetInput(mdl.Get(2,i).GetDeviceIndex()).name());
				TextOut(hdc,pnt.x+1,pnt.y-Grid::Interval+1,mdl.GetInput(mdl.Get(2,i).GetDeviceIndex()).name(),len);
				break;
			case module_out:
				len=strlen(mdl.GetOutput(mdl.Get(2,i).GetDeviceIndex()).name());
				TextOut(hdc,pnt.x+1,pnt.y-Grid::Interval+1,mdl.GetOutput(mdl.Get(2,i).GetDeviceIndex()).name(),len);
				break;
			}
		}

		v=mdl.Center()+DirectionVector[CLOCKWISE_000]*(mdl.GetSize(3)-1)+DirectionVector[CLOCKWISE_270]*(mdl.InternalHalfHeight()+mdl.Margin(3));
		pnt=v.toPOINT();
		SelectObject(hdc,Drawing::hFontV);
		for(int i=0;i<mdl.GetSize(3);i++,pnt.x-=Grid::Interval*2)
		{
			switch(mdl.Get(3,i).GetDeviceClass())
			{
				int len;
			case module_in:
				len=strlen(mdl.GetInput(mdl.Get(3,i).GetDeviceIndex()).name());
				TextOut(hdc,pnt.x+Grid::Interval-1,pnt.y+1,mdl.GetInput(mdl.Get(3,i).GetDeviceIndex()).name(),len);
				break;
			case module_out:
				len=strlen(mdl.GetOutput(mdl.Get(3,i).GetDeviceIndex()).name());
				TextOut(hdc,pnt.x+Grid::Interval-1,pnt.y+1,mdl.GetOutput(mdl.Get(3,i).GetDeviceIndex()).name(),len);
				break;
			}
		}

		v=mdl.Center()+DirectionVector[CLOCKWISE_090]*(mdl.GetSize(0)-1)+DirectionVector[CLOCKWISE_000]*(mdl.InternalHalfWidth()+mdl.Margin(0));
		pnt=v.toPOINT();
		SelectObject(hdc,Drawing::hFontH);
		for(int i=0;i<mdl.GetSize(0);i++,pnt.y-=Grid::Interval*2)
		{
			switch(mdl.Get(0,i).GetDeviceClass())
			{
				int len;
			case module_in:
				len=strlen(mdl.GetInput(mdl.Get(0,i).GetDeviceIndex()).name());
				TextOut(hdc,pnt.x-(Grid::Interval-1)*len-2,pnt.y-Grid::Interval+1,mdl.GetInput(mdl.Get(0,i).GetDeviceIndex()).name(),len);
				break;
			case module_out:
				len=strlen(mdl.GetOutput(mdl.Get(0,i).GetDeviceIndex()).name());
				TextOut(hdc,pnt.x-(Grid::Interval-1)*len-2,pnt.y-Grid::Interval+1,mdl.GetOutput(mdl.Get(0,i).GetDeviceIndex()).name(),len);
				break;
			}
		}

		v=mdl.Center()+DirectionVector[CLOCKWISE_180]*(mdl.GetSize(1)-1)+DirectionVector[CLOCKWISE_090]*(mdl.InternalHalfHeight()+mdl.Margin(1));
		pnt=v.toPOINT();
		SelectObject(hdc,Drawing::hFontV);
		for(int i=0;i<mdl.GetSize(1);i++,pnt.x+=Grid::Interval*2)
		{
			switch(mdl.Get(1,i).GetDeviceClass())
			{
				int len;
			case module_in:
				len=strlen(mdl.GetInput(mdl.Get(1,i).GetDeviceIndex()).name());
				TextOut(hdc,pnt.x+Grid::Interval-1,pnt.y-(Grid::Interval-1)*len-2,mdl.GetInput(mdl.Get(1,i).GetDeviceIndex()).name(),len);
				break;
			case module_out:
				len=strlen(mdl.GetOutput(mdl.Get(1,i).GetDeviceIndex()).name());
				TextOut(hdc,pnt.x+Grid::Interval-1,pnt.y-(Grid::Interval-1)*len-2,mdl.GetOutput(mdl.Get(1,i).GetDeviceIndex()).name(),len);
				break;
			}
		}

		v=mdl.Center()+DirectionVector[CLOCKWISE_270]*(mdl.GetSize(2)-1)+DirectionVector[CLOCKWISE_180]*(mdl.InternalHalfWidth()+mdl.Margin(2));
		pnt=v.toPOINT();
		for(int i=0;i<mdl.GetSize(2);i++,pnt.y+=Grid::Interval*2)	MoveToEx(hdc,pnt.x,pnt.y,NULL),LineTo(hdc,pnt.x-Grid::Interval,pnt.y);

		v=mdl.Center()+DirectionVector[CLOCKWISE_000]*(mdl.GetSize(3)-1)+DirectionVector[CLOCKWISE_270]*(mdl.InternalHalfHeight()+mdl.Margin(3));
		pnt=v.toPOINT();
		for(int i=0;i<mdl.GetSize(3);i++,pnt.x-=Grid::Interval*2)	MoveToEx(hdc,pnt.x,pnt.y,NULL),LineTo(hdc,pnt.x,pnt.y-Grid::Interval);

		v=mdl.Center()+DirectionVector[CLOCKWISE_090]*(mdl.GetSize(0)-1)+DirectionVector[CLOCKWISE_000]*(mdl.InternalHalfWidth()+mdl.Margin(0));
		pnt=v.toPOINT();
		for(int i=0;i<mdl.GetSize(0);i++,pnt.y-=Grid::Interval*2)MoveToEx(hdc,pnt.x,pnt.y,NULL),LineTo(hdc,pnt.x+Grid::Interval,pnt.y);

		v=mdl.Center()+DirectionVector[CLOCKWISE_180]*(mdl.GetSize(1)-1)+DirectionVector[CLOCKWISE_090]*(mdl.InternalHalfHeight()+mdl.Margin(1));
		pnt=v.toPOINT();
		for(int i=0;i<mdl.GetSize(1);i++,pnt.x+=Grid::Interval*2)	MoveToEx(hdc,pnt.x,pnt.y,NULL),LineTo(hdc,pnt.x,pnt.y+Grid::Interval);

		v = mdl.Center() + DirectionVector[CLOCKWISE_270] * (mdl.InternalHalfHeight() + mdl.Margin(3)) + DirectionVector[CLOCKWISE_180] * (mdl.InternalHalfWidth() + mdl.Margin(2));
		upper_left_point = v.toPOINT();
		v = mdl.Center() + DirectionVector[CLOCKWISE_090] * (mdl.InternalHalfHeight() + mdl.Margin(1)) + DirectionVector[CLOCKWISE_000] * (mdl.InternalHalfWidth() + mdl.Margin(0));
		lower_right_point = v.toPOINT();
		Rectangle(hdc, upper_left_point, lower_right_point);
	}
}

void DrawModuleTemplate(HDC hdc,Module *pMdl)
{
	HPEN hPen,hOldPen;

	vector2D v;
	POINT pnt;

	POINT upper_left_point;
	POINT lower_right_point;

	SetTextColor(hdc,RGB(127,127,127));

	hPen=CreatePen(PS_SOLID,1,RGB(127,127,127));
	hOldPen=(HPEN)SelectObject(hdc,hPen);	//正しい処理か確認

	v=Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_270]*(pMdl->GetSize(2)-1)+DirectionVector[CLOCKWISE_180]*(pMdl->InternalHalfWidth()+pMdl->Margin(2));
	pnt=v.toPOINT();
	SelectObject(hdc,Drawing::hFontH);
	for(int i=0;i<pMdl->GetSize(2);i++,pnt.y+=Grid::Interval*2)
	{
		switch(pMdl->Get(2,i).GetDeviceClass())
		{
			int len;
		case module_in:
			len=strlen(pMdl->GetInput(pMdl->Get(2,i).GetDeviceIndex()).name());
			TextOut(hdc,pnt.x+1,pnt.y-Grid::Interval+1,pMdl->GetInput(pMdl->Get(2,i).GetDeviceIndex()).name(),len);
			break;
		case module_out:
			len=strlen(pMdl->GetOutput(pMdl->Get(2,i).GetDeviceIndex()).name());
			TextOut(hdc,pnt.x+1,pnt.y-Grid::Interval+1,pMdl->GetOutput(pMdl->Get(2,i).GetDeviceIndex()).name(),len);
			break;
		}
	}

	v=Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_000]*(pMdl->GetSize(3)-1)+DirectionVector[CLOCKWISE_270]*(pMdl->InternalHalfHeight()+pMdl->Margin(3));
	pnt=v.toPOINT();
	SelectObject(hdc,Drawing::hFontV);
	for(int i=0;i<pMdl->GetSize(3);i++,pnt.x-=Grid::Interval*2)
	{
		switch(pMdl->Get(3,i).GetDeviceClass())
		{
			int len;
		case module_in:
			len=strlen(pMdl->GetInput(pMdl->Get(3,i).GetDeviceIndex()).name());
			TextOut(hdc,pnt.x+Grid::Interval-1,pnt.y+1,pMdl->GetInput(pMdl->Get(3,i).GetDeviceIndex()).name(),len);
			break;
		case module_out:
			len=strlen(pMdl->GetOutput(pMdl->Get(3,i).GetDeviceIndex()).name());
			TextOut(hdc,pnt.x+Grid::Interval-1,pnt.y+1,pMdl->GetOutput(pMdl->Get(3,i).GetDeviceIndex()).name(),len);
			break;
		}
	}

	v=Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_090]*(pMdl->GetSize(0)-1)+DirectionVector[CLOCKWISE_000]*(pMdl->InternalHalfWidth()+pMdl->Margin(0));
	pnt=v.toPOINT();
	SelectObject(hdc,Drawing::hFontH);
	for(int i=0;i<pMdl->GetSize(0);i++,pnt.y-=Grid::Interval*2)
	{
		switch(pMdl->Get(0,i).GetDeviceClass())
		{
			int len;
		case module_in:
			len=strlen(pMdl->GetInput(pMdl->Get(0,i).GetDeviceIndex()).name());
			TextOut(hdc,pnt.x-(Grid::Interval-1)*len-2,pnt.y-Grid::Interval+1,pMdl->GetInput(pMdl->Get(0,i).GetDeviceIndex()).name(),len);
			break;
		case module_out:
			len=strlen(pMdl->GetOutput(pMdl->Get(0,i).GetDeviceIndex()).name());
			TextOut(hdc,pnt.x-(Grid::Interval-1)*len-2,pnt.y-Grid::Interval+1,pMdl->GetOutput(pMdl->Get(0,i).GetDeviceIndex()).name(),len);
			break;
		}
	}

	v=Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_180]*(pMdl->GetSize(1)-1)+DirectionVector[CLOCKWISE_090]*(pMdl->InternalHalfHeight()+pMdl->Margin(1));
	pnt=v.toPOINT();
	SelectObject(hdc,Drawing::hFontV);
	for(int i=0;i<pMdl->GetSize(1);i++,pnt.x+=Grid::Interval*2)
	{
		switch(pMdl->Get(1,i).GetDeviceClass())
		{
			int len;
		case module_in:
			len=strlen(pMdl->GetInput(pMdl->Get(1,i).GetDeviceIndex()).name());
			TextOut(hdc,pnt.x+Grid::Interval-1,pnt.y-(Grid::Interval-1)*len-2,pMdl->GetInput(pMdl->Get(1,i).GetDeviceIndex()).name(),len);
			break;
		case module_out:
			len=strlen(pMdl->GetOutput(pMdl->Get(1,i).GetDeviceIndex()).name());
			TextOut(hdc,pnt.x+Grid::Interval-1,pnt.y-(Grid::Interval-1)*len-2,pMdl->GetOutput(pMdl->Get(1,i).GetDeviceIndex()).name(),len);
			break;
		}
	}

	v=Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_270]*(pMdl->GetSize(2)-1)+DirectionVector[CLOCKWISE_180]*(pMdl->InternalHalfWidth()+pMdl->Margin(2));
	pnt=v.toPOINT();
	for(int i=0;i<pMdl->GetSize(2);i++,pnt.y+=Grid::Interval*2)	MoveToEx(hdc,pnt.x,pnt.y,NULL),LineTo(hdc,pnt.x-Grid::Interval,pnt.y);

	v=Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_000]*(pMdl->GetSize(3)-1)+DirectionVector[CLOCKWISE_270]*(pMdl->InternalHalfHeight()+pMdl->Margin(3));
	pnt=v.toPOINT();
	for(int i=0;i<pMdl->GetSize(3);i++,pnt.x-=Grid::Interval*2)	MoveToEx(hdc,pnt.x,pnt.y,NULL),LineTo(hdc,pnt.x,pnt.y-Grid::Interval);

	v=Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_090]*(pMdl->GetSize(0)-1)+DirectionVector[CLOCKWISE_000]*(pMdl->InternalHalfWidth()+pMdl->Margin(0));
	pnt=v.toPOINT();
	for(int i=0;i<pMdl->GetSize(0);i++,pnt.y-=Grid::Interval*2)MoveToEx(hdc,pnt.x,pnt.y,NULL),LineTo(hdc,pnt.x+Grid::Interval,pnt.y);

	v=Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_180]*(pMdl->GetSize(1)-1)+DirectionVector[CLOCKWISE_090]*(pMdl->InternalHalfHeight()+pMdl->Margin(1));
	pnt=v.toPOINT();
	for(int i=0;i<pMdl->GetSize(1);i++,pnt.x+=Grid::Interval*2)	MoveToEx(hdc,pnt.x,pnt.y,NULL),LineTo(hdc,pnt.x,pnt.y+Grid::Interval);

	v = Interface::AlignedMouseCoordinates + DirectionVector[CLOCKWISE_270] * (pMdl->InternalHalfHeight() + pMdl->Margin(3)) + DirectionVector[CLOCKWISE_180] * (pMdl->InternalHalfWidth() + pMdl->Margin(2));
	upper_left_point = v.toPOINT();
	v = Interface::AlignedMouseCoordinates + DirectionVector[CLOCKWISE_090] * (pMdl->InternalHalfHeight() + pMdl->Margin(1)) + DirectionVector[CLOCKWISE_000] * (pMdl->InternalHalfWidth() + pMdl->Margin(0));
	lower_right_point = v.toPOINT();
	Rectangle(hdc, upper_left_point, lower_right_point);
	
	SelectObject(hdc,hOldPen);	//正しい処理か確認
	DeleteObject(hPen);
}

POINT CalculateOffset()
{
	POINT a;
	a.x=(Interface::ClientRect.right-Interface::ClientRect.left)/2+Interface::Scroll_Coordinates.x;
	a.y=(Interface::ClientRect.bottom-Interface::ClientRect.top)/2+Interface::Scroll_Coordinates.y;
	return a;
}

void Data::DrawNode(HDC hdc)
{
	HPEN hGreenPen,hRedPen,hOldPen;
	Node n;
	vector2D v;
	hGreenPen=CreatePen(PS_SOLID,0,RGB(0,255,0));
	hRedPen=CreatePen(PS_SOLID,0,RGB(255,0,0));
	hOldPen=(HPEN)SelectObject(hdc,hGreenPen);
	for(unsigned int i=0;i<node_data.size();i++)
	{
		if(!node_data[i].isAllRight())
		{
			POINT p1,p2;
			if(node_data[i].isShort())SelectObject(hdc,hRedPen);
			n=node(i);
			p1=n.position().toPOINT();
			for(int j=0;j<8;j++)
			{
				MoveToEx(hdc,p1.x,p1.y,NULL);
				if(n.isBranchExist((Direction)j))
				{
					v=DirectionVector[j];
					p2=v.MulGridInterval();
					p2.x=p1.x+p2.x/3;
					p2.y=p1.y+p2.y/3;
					LineTo(hdc,p2.x,p2.y);
				}
			}
			SelectObject(hdc,hGreenPen);
		}
	}
	SelectObject(hdc,hOldPen);
	DeleteObject(hRedPen);
	DeleteObject(hGreenPen);
	for(unsigned int i=0;i<node_data.size();i++)
	{
		POINT p1;
		n=node(i);
		p1=n.position().toPOINT();
		SetPixelV(hdc, p1.x, p1.y, RGB(0, 0, 0));
		if(node_data[i].nBranch()>=3)
		{
			MoveToEx(hdc,p1.x-1,p1.y-1,NULL),LineTo(hdc,p1.x+1,p1.y-1);
			MoveToEx(hdc,p1.x+1,p1.y-1,NULL),LineTo(hdc,p1.x+1,p1.y+1);
			MoveToEx(hdc,p1.x+1,p1.y+1,NULL),LineTo(hdc,p1.x-1,p1.y+1);
			MoveToEx(hdc,p1.x-1,p1.y+1,NULL),LineTo(hdc,p1.x-1,p1.y-1);
		}
	}
}

void Data::DrawConductor(HDC hdc)
{
	HPEN hBluePen,hOldPen;
	POINT p;
	hBluePen=CreatePen(PS_SOLID,1,RGB(0,0,255));
	for(unsigned int i=0;i<conductor.size();i++)
	{
		if (conductor[i].isMultipath())
		{
			hOldPen = (HPEN)SelectObject(hdc, hBluePen);

			p = node(conductor[i].nodeindex0()).position().toPOINT();
			MoveToEx(hdc, p.x, p.y, NULL);
			p = node(conductor[i].nodeindex1()).position().toPOINT();
			LineTo(hdc, p.x, p.y);

			SelectObject(hdc, hOldPen);	//正しい処理か確認
		}
		else
		{
			p = node(conductor[i].nodeindex0()).position().toPOINT();
			MoveToEx(hdc, p.x, p.y, NULL);
			p = node(conductor[i].nodeindex1()).position().toPOINT();
			LineTo(hdc, p.x, p.y);
		}
	}
	DeleteObject(hBluePen);
}

void DrawConductorTemplate(HDC hdc)
{
	HPEN hPen,hOldPen;
	POINT p;
	hPen=CreatePen(PS_SOLID,1,RGB(127,127,127));
	hOldPen=(HPEN)SelectObject(hdc,hPen);	//正しい処理か確認

	p=Interface::FirstAlignedMouseCoordinates.toPOINT();
	MoveToEx(hdc,p.x,p.y,NULL);
	p=AlignAngle(Interface::FirstAlignedMouseCoordinates,Interface::AlignedMouseCoordinates).toPOINT();
	LineTo(hdc,p.x,p.y);

	SelectObject(hdc,hOldPen);	//正しい処理か確認
	DeleteObject(hPen);
}

void Data::DrawInputLabel(HDC hdc)
{
	POINT p1,p2;
	vector2D v;
	Direction d;
	SetTextColor(hdc,0);
	for(unsigned int i=0;i<input.size();i++)
	{
		int len;
		len=lstrlen(input[i].name());
		d=input[i].direction();
		p1=(node(input[i].nodeindex()).position()-DirectionVector[input[i].direction()]).toPOINT();
		MoveToEx(hdc,p1.x,p1.y,NULL);
		v=DirectionVector[d];
		p2=v.MulGridInterval();
		LineTo(hdc,p1.x+p2.x,p1.y+p2.y);
		MoveToEx(hdc,p1.x,p1.y,NULL);
		v=DirectionVector[(d+3)&7];
		p2=v.MulGridInterval();
		LineTo(hdc,p1.x+p2.x/2,p1.y+p2.y/2);
		MoveToEx(hdc,p1.x,p1.y,NULL);
		v=DirectionVector[(d+5)&7];
		p2=v.MulGridInterval();
		LineTo(hdc,p1.x+p2.x/2,p1.y+p2.y/2);
		switch(d)
		{
		case CLOCKWISE_000:
			SelectObject(hdc,Drawing::hFontH);
			TextOut(hdc,p1.x-Grid::Interval*(len+input[i].nWire()+1),p1.y-Grid::Interval,input[i].name(),len);
			break;
		case CLOCKWISE_090:
			SelectObject(hdc,Drawing::hFontV);
			TextOut(hdc,p1.x+Grid::Interval,p1.y-Grid::Interval*(len+input[i].nWire()+1),input[i].name(),len);
			break;
		case CLOCKWISE_180:
			SelectObject(hdc,Drawing::hFontH);
			TextOut(hdc,p1.x+Grid::Interval*(1+input[i].nWire()),p1.y-Grid::Interval,input[i].name(),len);
			break;
		case CLOCKWISE_270:
			SelectObject(hdc,Drawing::hFontV);
			TextOut(hdc,p1.x+Grid::Interval,p1.y+Grid::Interval*(1+input[i].nWire()),input[i].name(),len);
			break;
		}
	}
}

void Data::DrawInputState(HDC hdc)
{
	POINT p;
	Direction d;
	for(unsigned int i=0;i<input.size();i++)
	{
		for(int j=0;j<input[i].nWire();j++)
		{
			bool state=input[i].GetState(j);
			char *str;
			d=input[i].direction();
			p=(node(input[i].nodeindex()).position()-DirectionVector[input[i].direction()]).toPOINT();
			if(state)
				SetTextColor(hdc,RGB(255,0,0));
			else
				SetTextColor(hdc,RGB(0,0,255));
			str=state ? "1" : "0";
			switch(d)
			{
			case CLOCKWISE_000:
				SelectObject(hdc,Drawing::hFontH);
				TextOut(hdc,p.x-(int)(Grid::Interval*(1.5+j)),p.y-Grid::Interval,str,1);
				break;
			case CLOCKWISE_090:
				SelectObject(hdc,Drawing::hFontV);
				TextOut(hdc,p.x+Grid::Interval,p.y-(int)(Grid::Interval*(1.5+j)),str,1);
				break;
			case CLOCKWISE_180:
				SelectObject(hdc,Drawing::hFontH);
				TextOut(hdc,p.x+(int)(Grid::Interval*(input[i].nWire()-0.5-j)),p.y-Grid::Interval,str,1);
				break;
			case CLOCKWISE_270:
				SelectObject(hdc,Drawing::hFontV);
				TextOut(hdc,p.x+Grid::Interval,p.y+(int)(Grid::Interval*(input[i].nWire()-0.5-j)),str,1);
				break;
			}
		}
	}
	SetTextColor(hdc,RGB(0,0,0));
}

void DrawInputTemplate(HDC hdc)
{
	HPEN hPen,hOldPen;
	POINT p1,p2;
	vector2D v;
	hPen=CreatePen(PS_SOLID,1,RGB(127,127,127));
	hOldPen=(HPEN)SelectObject(hdc,hPen);

	p1=Interface::AlignedMouseCoordinates.toPOINT();
	MoveToEx(hdc,p1.x,p1.y,NULL);
	v=DirectionVector[Place::NewDirection];
	p2=v.MulGridInterval();
	LineTo(hdc,p1.x+p2.x,p1.y+p2.y);
	MoveToEx(hdc,p1.x,p1.y,NULL);
	v=DirectionVector[(Place::NewDirection+3)&7];
	p2=v.MulGridInterval();
	LineTo(hdc,p1.x+p2.x/2,p1.y+p2.y/2);
	MoveToEx(hdc,p1.x,p1.y,NULL);
	v=DirectionVector[(Place::NewDirection+5)&7];
	p2=v.MulGridInterval();
	LineTo(hdc,p1.x+p2.x/2,p1.y+p2.y/2);

	SelectObject(hdc,hOldPen);
	DeleteObject(hPen);
}

void Data::DrawOutputLabel(HDC hdc)
{
	POINT p1,p2;
	vector2D v;
	Direction d;
	SetTextColor(hdc,0);
	for(unsigned int i=0;i<output.size();i++)
	{
		int len;
		len=lstrlen(output[i].name());
		d=output[i].direction();
		p1=(node(output[i].nodeindex()).position()-DirectionVector[output[i].direction()]).toPOINT();
		MoveToEx(hdc,p1.x,p1.y,NULL);
		v=DirectionVector[d];
		p2=v.MulGridInterval();
		LineTo(hdc,p1.x+p2.x,p1.y+p2.y);
		MoveToEx(hdc,p1.x,p1.y,NULL);
		v=DirectionVector[(d+1)&7];
		p2=v.MulGridInterval();
		LineTo(hdc,p1.x+p2.x/2,p1.y+p2.y/2);
		MoveToEx(hdc,p1.x,p1.y,NULL);
		v=DirectionVector[(d+7)&7];
		p2=v.MulGridInterval();
		LineTo(hdc,p1.x+p2.x/2,p1.y+p2.y/2);

		switch(d)
		{
		case CLOCKWISE_000:
			SelectObject(hdc,Drawing::hFontH);
			TextOut(hdc,p1.x-Grid::Interval*(len+1+node(output[i].nodeindex()).nWire()),p1.y-Grid::Interval,output[i].name(),len);
			break;
		case CLOCKWISE_090:
			SelectObject(hdc,Drawing::hFontV);
			TextOut(hdc,p1.x+Grid::Interval,p1.y-Grid::Interval*(len+1+node(output[i].nodeindex()).nWire()),output[i].name(),len);
			break;
		case CLOCKWISE_180:
			SelectObject(hdc,Drawing::hFontH);
			TextOut(hdc,p1.x+Grid::Interval*(1+node(output[i].nodeindex()).nWire()),p1.y-Grid::Interval,output[i].name(),len);
			break;
		case CLOCKWISE_270:
			SelectObject(hdc,Drawing::hFontV);
			TextOut(hdc,p1.x+Grid::Interval,p1.y+Grid::Interval*(1+node(output[i].nodeindex()).nWire()),output[i].name(),len);
			break;
		}
	}
}

void Data::DrawOutputState(HDC hdc)
{
	char *str;
	POINT p;
	for(unsigned int i=0;i<output.size();i++)
	{
		for(int j=0;j<node(output[i].nodeindex()).nWire();j++)
		{
			bool state=false;
			p=(node(output[i].nodeindex()).position()-DirectionVector[output[i].direction()]).toPOINT();
			str="";
			switch(output[i].GetSourceDevClass())
			{
			case bus_in_out:
				state=busin[output[i].GetSourceDevIndex()].GetState(output[i].GetSourceDevSubIndex());
				str=state ? "1" : "0";
				break;
			case bus_out_out:
				state=busout[output[i].GetSourceDevIndex()].GetState(j);
				str=state ? "1" : "0";
				break;
			case label_input:
				state=input[output[i].GetSourceDevIndex()].GetState(j);
				str=state ? "1" : "0";
				break;
			case gate_out:
				state=gate[output[i].GetSourceDevIndex()].GetState(j);
				str=state ? "1" : "0";
				break;
			case module_out:
				//ここでエラー
				state=module[output[i].GetSourceDevIndex()].GetState(output[i].GetSourceDevSubIndex(),j);
				str=state ? "1" : "0";
				break;
			}
			if(state)
				SetTextColor(hdc,RGB(255,0,0));
			else
				SetTextColor(hdc,RGB(0,0,255));

			switch(output[i].direction())
			{
			case CLOCKWISE_000:
				SelectObject(hdc,Drawing::hFontH);
				TextOut(hdc,p.x-(int)(Grid::Interval*(1.5+j)),p.y-Grid::Interval,str,1);
				break;
			case CLOCKWISE_090:
				SelectObject(hdc,Drawing::hFontV);
				TextOut(hdc,p.x+Grid::Interval,p.y-(int)(Grid::Interval*(1.5+j)),str,1);
				break;
			case CLOCKWISE_180:
				SelectObject(hdc,Drawing::hFontH);
				TextOut(hdc,p.x+(int)(Grid::Interval*(node(output[i].nodeindex()).nWire()-0.5-j)),p.y-Grid::Interval,str,1);
				break;
			case CLOCKWISE_270:
				SelectObject(hdc,Drawing::hFontV);
				TextOut(hdc,p.x+Grid::Interval,p.y+(int)(Grid::Interval*(node(output[i].nodeindex()).nWire()-0.5-j)),str,1);
				break;
			}
		}
	}
	SetTextColor(hdc,RGB(0,0,0));
}

void DrawOutputTemplate(HDC hdc)
{
	HPEN hPen,hOldPen;
	POINT p1,p2;
	vector2D v;
	hPen=CreatePen(PS_SOLID,1,RGB(127,127,127));
	hOldPen=(HPEN)SelectObject(hdc,hPen);

	p1=Interface::AlignedMouseCoordinates.toPOINT();
	MoveToEx(hdc,p1.x,p1.y,NULL);
	v=DirectionVector[ReverseDirection(Place::NewDirection)];
	p2=v.MulGridInterval();
	LineTo(hdc,p1.x+p2.x,p1.y+p2.y);
	MoveToEx(hdc,p1.x,p1.y,NULL);
	v=DirectionVector[(ReverseDirection(Place::NewDirection)+1)&7];
	p2=v.MulGridInterval();
	LineTo(hdc,p1.x+p2.x/2,p1.y+p2.y/2);
	MoveToEx(hdc,p1.x,p1.y,NULL);
	v=DirectionVector[(ReverseDirection(Place::NewDirection)+7)&7];
	p2=v.MulGridInterval();
	LineTo(hdc,p1.x+p2.x/2,p1.y+p2.y/2);

	SelectObject(hdc,hOldPen);
	DeleteObject(hPen);
}

void Data::DrawNote(HDC hdc)
{
	POINT p1, p2;
	vector2D v;
	Direction d;
	SetTextColor(hdc, 0);
	for (unsigned int i = 0; i<note.size(); i++)
	{
		int len;
		len = lstrlen(note[i].name());
		d = note[i].direction();
		p1 = note[i].position().toPOINT();
		MoveToEx(hdc, p1.x, p1.y, NULL);
		v = DirectionVector[(d + 3) & 7];
		p2 = v.MulGridInterval();
		LineTo(hdc, p1.x + p2.x / 2, p1.y + p2.y / 2);
		MoveToEx(hdc, p1.x, p1.y, NULL);
		v = DirectionVector[(d + 5) & 7];
		p2 = v.MulGridInterval();
		LineTo(hdc, p1.x + p2.x / 2, p1.y + p2.y / 2);

		switch (d)
		{
		case CLOCKWISE_000:
			SelectObject(hdc, Drawing::hFontH);
			TextOut(hdc, p1.x + Grid::Interval, p1.y - Grid::Interval, note[i].name(), len);
			break;
		case CLOCKWISE_090:
			SelectObject(hdc, Drawing::hFontV);
			TextOut(hdc, p1.x + Grid::Interval, p1.y + Grid::Interval, note[i].name(), len);
			break;
		case CLOCKWISE_180:
			SelectObject(hdc, Drawing::hFontH);
			TextOut(hdc, p1.x - Grid::Interval*(len + 1), p1.y - Grid::Interval, note[i].name(), len);
			break;
		case CLOCKWISE_270:
			SelectObject(hdc, Drawing::hFontV);
			TextOut(hdc, p1.x + Grid::Interval, p1.y - Grid::Interval*(len + 1), note[i].name(), len);
			break;
		}
	}
}

void DrawNoteTemplate(HDC hdc)
{
	HPEN hPen,hOldPen;
	POINT p1,p2;
	vector2D v;

	hPen=CreatePen(PS_SOLID,1,RGB(127,127,127));
	hOldPen=(HPEN)SelectObject(hdc,hPen);

	p1 = Interface::AlignedMouseCoordinates.toPOINT();
	MoveToEx(hdc, p1.x, p1.y, NULL);
	v = DirectionVector[(Place::NewDirection + 3) & 7];
	p2 = v.MulGridInterval();
	LineTo(hdc, p1.x + p2.x / 2, p1.y + p2.y / 2);
	MoveToEx(hdc, p1.x, p1.y, NULL);
	v = DirectionVector[(Place::NewDirection + 5) & 7];
	p2 = v.MulGridInterval();
	LineTo(hdc, p1.x + p2.x / 2, p1.y + p2.y / 2);

	SelectObject(hdc,hOldPen);
	DeleteObject(hPen);
}

void Data::DrawPasteTemplate(HDC hdc)
{
	vector2D AngleVector = DirectionVector[Place::NewDirection];
	vector2D MouseCoordinates=Interface::AlignedMouseCoordinates;
	int tmpNewBusWidth=Place::NewBusWidth;
	Direction tmpNewDirection = Place::NewDirection;
	for(unsigned int i=0;i<note.size();i++)
	{
		vector2D pos;
		pos=note[i].position();
		Interface::AlignedMouseCoordinates=pos*AngleVector+MouseCoordinates;
		Place::NewDirection = Direction((note[i].direction()+tmpNewDirection)&7);
		DrawNoteTemplate(hdc);
	}
	for(unsigned int i=0;i<input.size();i++)
	{
		vector2D pos;
		pos=node_data[input[i].nodeindex()].position()-DirectionVector[input[i].direction()];
		Interface::AlignedMouseCoordinates=pos*AngleVector+MouseCoordinates;
		Place::NewDirection = Direction((input[i].direction()+tmpNewDirection)&7);
		strcpy_s(Place::NewLabelName,input[i].name());
		DrawInputTemplate(hdc);
	}
	for(unsigned int i=0;i<output.size();i++)
	{
		vector2D pos;
		pos=node_data[output[i].nodeindex()].position()-DirectionVector[output[i].direction()];
		Interface::AlignedMouseCoordinates=pos*AngleVector+MouseCoordinates;
		Place::NewDirection = Direction((ReverseDirection(output[i].direction())+tmpNewDirection)&7);
		strcpy_s(Place::NewLabelName,output[i].name());
		DrawOutputTemplate(hdc);
	}
	for(unsigned int i=0;i<busin.size();i++)
	{
		Interface::AlignedMouseCoordinates=(node_data[busin[i].NodeInIndex()].position()+DirectionVector[busin[i].direction()])*AngleVector+MouseCoordinates;
		Place::NewDirection=Direction((busin[i].direction()+tmpNewDirection)&7);
		Place::NewBusWidth=busin[i].nOut();
		DrawBusInTemplate(hdc);
	}
	for(unsigned int i=0;i<busout.size();i++)
	{
		Interface::AlignedMouseCoordinates=(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[busout[i].ReverseDirection()])*AngleVector+MouseCoordinates;
		Place::NewDirection=Direction((busout[i].direction()+tmpNewDirection)&7);
		Place::NewBusWidth=busout[i].nIn();
		DrawBusOutTemplate(hdc);
	}
	unsigned int TurnOutOfNewGateIn = Place::NewGateIn;
	for(unsigned int i=0;i<gate.size();i++)
	{
		Interface::AlignedMouseCoordinates=(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[gate[i].ReverseDirection()])*AngleVector+MouseCoordinates;
		Place::NewDirection=Direction((gate[i].direction()+tmpNewDirection)&7);
		Place::NewGateOperationisNOT = gate[i].isNOT();
		Place::NewGateOperationisOR = gate[i].isOR();
		Place::NewGateIn=gate[i].nIn();
		DrawTemplate(hdc);
	}
	Place::NewGateIn = TurnOutOfNewGateIn;
	for(unsigned int i=0;i<conductor.size();i++)
	{
		Interface::FirstAlignedMouseCoordinates=(node_data[conductor[i].nodeindex0()].position())*AngleVector+MouseCoordinates;
		Interface::AlignedMouseCoordinates=(node_data[conductor[i].nodeindex1()].position())*AngleVector+MouseCoordinates;
		DrawConductorTemplate(hdc);
	}
	for(unsigned int i=0;i<module.size();i++)
	{
		Interface::AlignedMouseCoordinates=module[i].Center()*AngleVector+MouseCoordinates;
		Place::NewDirection = module[i].direction();
		module[i].SetDirection(Direction((Place::NewDirection + tmpNewDirection) & 7));
		DrawModuleTemplate(hdc,&module[i]);
		module[i].SetDirection(Place::NewDirection);
	}
	Interface::AlignedMouseCoordinates=MouseCoordinates;
	Place::NewBusWidth=tmpNewBusWidth;
	Place::NewDirection = tmpNewDirection;
}

void Data::Draw(HDC hdc)
{
	DrawModule(hdc);
	DrawGate(hdc);
	DrawConductor(hdc);
	DrawBusIn(hdc);
	DrawBusOut(hdc);
	DrawInputLabel(hdc);
	DrawInputState(hdc);
	DrawOutputLabel(hdc);
	DrawOutputState(hdc);
	DrawNode(hdc);
	DrawNote(hdc);
}


//指定された範囲を描画
void DrawRange(HDC hdc,vector2D v1,vector2D v2)
{
	HPEN hPen,hOldPen;
	POINT p1,p2;
	hPen=CreatePen(PS_SOLID,1,RGB(255,255,0));
	hOldPen=(HPEN)SelectObject(hdc,hPen);	//正しい処理か確認

	p1=v1.toPOINT();
	p2=v2.toPOINT();

	MoveToEx(hdc,p1.x,p1.y,NULL);
	LineTo(hdc,p2.x,p1.y);
	LineTo(hdc,p2.x,p2.y);
	LineTo(hdc,p1.x,p2.y);
	LineTo(hdc,p1.x,p1.y);

	SelectObject(hdc,hOldPen);	//正しい処理か確認
	DeleteObject(hPen);
}

void DrawBack(HDC hdc)
{
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;
	hPen = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
	hOldPen = (HPEN)SelectObject(hdc, hPen);	//正しい処理か確認
	hBrush = CreateSolidBrush(RGB(255, 255, 255));
	hOldBrush = (HBRUSH)SelectObject(hdc, hBrush);

	Rectangle(hdc, Interface::ClientRect.left, Interface::ClientRect.top, Interface::ClientRect.right, Interface::ClientRect.bottom);

	SelectObject(hdc, hOldPen);	//正しい処理か確認
	DeleteObject(hPen);
	SelectObject(hdc, hOldBrush);
	DeleteObject(hBrush);
}

//描画
void Proc_Paint(State state)
{
	PAINTSTRUCT ps;
	HDC hdc,hDrawDC;
	HBITMAP hDrawBmp;

	hdc=BeginPaint(hMainWindow,&ps);
	Offset=CalculateOffset();
	hDrawDC=CreateCompatibleDC(hdc);
	hDrawBmp=CreateCompatibleBitmap(hdc,Interface::ClientRect.right,Interface::ClientRect.bottom);
	SelectObject(hDrawDC,hDrawBmp);
	DrawBack(hDrawDC);
	if(Grid::Draw) DrawGrid(hDrawDC);
	main_data.Draw(hDrawDC);
	if(state==range && Interface::Mouse_Drag) DrawRange(hDrawDC,Interface::FirstAlignedMouseCoordinates,Interface::AlignedMouseCoordinates);
	if(state==range_selected) DrawRange(hDrawDC,Edit::coordinates1,Edit::coordinates2);
	if(state==placegate) DrawTemplate(hDrawDC);
	if(state==placemodule) DrawModuleTemplate(hDrawDC,&Place::NewModule);
	if(state==placewire && Interface::Mouse_Drag) DrawConductorTemplate(hDrawDC);
	if(state==placein) DrawInputTemplate(hDrawDC);
	if(state==placeout) DrawOutputTemplate(hDrawDC);
	if(state==note) DrawNoteTemplate(hDrawDC);
	if(state==paste) copy_data.DrawPasteTemplate(hDrawDC);
	if(state==placebusin) DrawBusInTemplate(hDrawDC);
	if(state==placebusout) DrawBusOutTemplate(hDrawDC);
	BitBlt(hdc, 0, 0,Interface::ClientRect.right,Interface::ClientRect.bottom,hDrawDC, 0, 0, SRCCOPY );
	DeleteObject(hDrawBmp);
	DeleteDC(hDrawDC);
	EndPaint(hMainWindow,&ps);
}