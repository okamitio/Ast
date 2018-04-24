#include"common.h"
#include<iostream>
#include<fstream>


extern HWND hMainWindow;

namespace Grid
{
	extern int Interval;
}


void Data::DrawModule(std::ofstream &f)
{
	vector2D v;
	POINT pnt;

	POINT upper_left_point;
	POINT lower_right_point;


	for(unsigned int i=0;i<module.size();i++)
	{
		Module &mdl=module[i];

		v=mdl.Center()+DirectionVector[CLOCKWISE_270]*(mdl.InternalHalfHeight()+mdl.Margin(3))+DirectionVector[CLOCKWISE_180]*(mdl.InternalHalfWidth()+mdl.Margin(2));
		upper_left_point=v.toPOINT();
		v=mdl.Center()+DirectionVector[CLOCKWISE_090]*(mdl.InternalHalfHeight()+mdl.Margin(1))+DirectionVector[CLOCKWISE_000]*(mdl.InternalHalfWidth()+mdl.Margin(0));
		lower_right_point=v.toPOINT();
		f << "<rect x=\"" << upper_left_point.x << "\" y=\"" << upper_left_point.y << "\" width=\"" << lower_right_point.x - upper_left_point.x << "\" height=\"" << lower_right_point.y - upper_left_point.y << "\" stroke=\"#000000\" fill=\"none\" />" << std::endl;

		v=mdl.Center()+DirectionVector[CLOCKWISE_270]*(mdl.GetSize(2)-1)+DirectionVector[CLOCKWISE_180]*(mdl.InternalHalfWidth()+mdl.Margin(2));
		pnt=v.toPOINT();
		for(int i=0;i<mdl.GetSize(2);i++,pnt.y+=Grid::Interval*2)
		{
			switch(mdl.Get(2,i).GetDeviceClass())
			{
			case module_in:
				f << "<g font-size=\"" << Grid::Interval*2-4 << "\">";
				f << "<text x=\"" << pnt.x+1 << "\" y=\"" << pnt.y + Grid::Interval/2 <<"\" text-anchor=\"start\">" << mdl.GetInput(mdl.Get(2,i).GetDeviceIndex()).name() << "</text>";
				f << "</g>" << std::endl;
				break;
			case module_out:
				f << "<g font-size=\"" << Grid::Interval*2-4 << "\">";
				f << "<text x=\"" << pnt.x+1 << "\" y=\"" << pnt.y +Grid::Interval/2 <<"\" text-anchor=\"start\">" << mdl.GetOutput(mdl.Get(2,i).GetDeviceIndex()).name() << "</text>";
				f << "</g>" << std::endl;
				break;
			}
		}

		v=mdl.Center()+DirectionVector[CLOCKWISE_000]*(mdl.GetSize(3)-1)+DirectionVector[CLOCKWISE_270]*(mdl.InternalHalfHeight()+mdl.Margin(3));
		pnt=v.toPOINT();
		for(int i=0;i<mdl.GetSize(3);i++,pnt.x-=Grid::Interval*2)
		{
			switch(mdl.Get(3,i).GetDeviceClass())
			{
			case module_in:
				f << "<g writing-mode=\"tb\" font-size=\"" << Grid::Interval*2-4 << "\">";
				f << "<text x=\"" << pnt.x-Grid::Interval/2 << "\" y=\"" << pnt.y+1 <<"\" text-anchor=\"start\">" << mdl.GetInput(mdl.Get(3,i).GetDeviceIndex()).name() << "</text>";
				f << "</g>" << std::endl;
				break;
			case module_out:
				f << "<g writing-mode=\"tb\" font-size=\"" << Grid::Interval*2-4 << "\">";
				f << "<text x=\"" << pnt.x-Grid::Interval/2 << "\" y=\"" << pnt.y+1 <<"\" text-anchor=\"start\">" << mdl.GetOutput(mdl.Get(3,i).GetDeviceIndex()).name() << "</text>";
				f << "</g>" << std::endl;
				break;
			}
		}

		v=mdl.Center()+DirectionVector[CLOCKWISE_090]*(mdl.GetSize(0)-1)+DirectionVector[CLOCKWISE_000]*(mdl.InternalHalfWidth()+mdl.Margin(0));
		pnt=v.toPOINT();
		for(int i=0;i<mdl.GetSize(0);i++,pnt.y-=Grid::Interval*2)
		{
			switch(mdl.Get(0,i).GetDeviceClass())
			{
			case module_in:
				f << "<g font-size=\"" << Grid::Interval*2-4 << "\">";
				f << "<text x=\"" << pnt.x-1 << "\" y=\"" << pnt.y + Grid::Interval/2 <<"\" text-anchor=\"end\">" << mdl.GetInput(mdl.Get(0,i).GetDeviceIndex()).name() << "</text>";
				f << "</g>" << std::endl;
				break;
			case module_out:
				f << "<g font-size=\"" << Grid::Interval*2-4 << "\">";
				f << "<text x=\"" << pnt.x-1 << "\" y=\"" << pnt.y + Grid::Interval/2 <<"\" text-anchor=\"end\">" << mdl.GetOutput(mdl.Get(0,i).GetDeviceIndex()).name() << "</text>";
				f << "</g>" << std::endl;
				break;
			}
		}

		v=mdl.Center()+DirectionVector[CLOCKWISE_180]*(mdl.GetSize(1)-1)+DirectionVector[CLOCKWISE_090]*(mdl.InternalHalfHeight()+mdl.Margin(1));
		pnt=v.toPOINT();
		for(int i=0;i<mdl.GetSize(1);i++,pnt.x+=Grid::Interval*2)
		{
			switch(mdl.Get(1,i).GetDeviceClass())
			{
			case module_in:
				f << "<g writing-mode=\"tb\" font-size=\"" << Grid::Interval*2-4 << "\">";
				f << "<text x=\"" << pnt.x-Grid::Interval/2 << "\" y=\"" << pnt.y-1 <<"\" text-anchor=\"end\">" << mdl.GetInput(mdl.Get(1,i).GetDeviceIndex()).name() << "</text>";
				f << "</g>" << std::endl;
				break;
			case module_out:
				f << "<g writing-mode=\"tb\" font-size=\"" << Grid::Interval*2-4 << "\">";
				f << "<text x=\"" << pnt.x-Grid::Interval/2 << "\" y=\"" << pnt.y-1 <<"\" text-anchor=\"end\">" << mdl.GetOutput(mdl.Get(1,i).GetDeviceIndex()).name() << "</text>";
				f << "</g>" << std::endl;
				break;
			}
		}

		v=mdl.Center()+DirectionVector[CLOCKWISE_270]*(mdl.GetSize(2)-1)+DirectionVector[CLOCKWISE_180]*(mdl.InternalHalfWidth()+mdl.Margin(2));
		pnt=v.toPOINT();
		for(int i=0;i<mdl.GetSize(2);i++,pnt.y+=Grid::Interval*2)
			f << "<line x1=\"" << pnt.x << "\" y1=\"" << pnt.y << "\" x2=\"" << pnt.x-Grid::Interval << "\" y2=\"" << pnt.y << "\" stroke=\"#000000\" />" << std::endl;

		v=mdl.Center()+DirectionVector[CLOCKWISE_000]*(mdl.GetSize(3)-1)+DirectionVector[CLOCKWISE_270]*(mdl.InternalHalfHeight()+mdl.Margin(3));
		pnt=v.toPOINT();
		for(int i=0;i<mdl.GetSize(3);i++,pnt.x-=Grid::Interval*2)
			f << "<line x1=\"" << pnt.x << "\" y1=\"" << pnt.y << "\" x2=\"" << pnt.x << "\" y2=\"" << pnt.y-Grid::Interval << "\" stroke=\"#000000\" />" << std::endl;

		v=mdl.Center()+DirectionVector[CLOCKWISE_090]*(mdl.GetSize(0)-1)+DirectionVector[CLOCKWISE_000]*(mdl.InternalHalfWidth()+mdl.Margin(0));
		pnt=v.toPOINT();
		for(int i=0;i<mdl.GetSize(0);i++,pnt.y-=Grid::Interval*2)
			f << "<line x1=\"" << pnt.x << "\" y1=\"" << pnt.y << "\" x2=\"" << pnt.x+Grid::Interval << "\" y2=\"" << pnt.y << "\" stroke=\"#000000\" />" << std::endl;

		v=mdl.Center()+DirectionVector[CLOCKWISE_180]*(mdl.GetSize(1)-1)+DirectionVector[CLOCKWISE_090]*(mdl.InternalHalfHeight()+mdl.Margin(1));
		pnt=v.toPOINT();
		for(int i=0;i<mdl.GetSize(1);i++,pnt.x+=Grid::Interval*2)
			f << "<line x1=\"" << pnt.x << "\" y1=\"" << pnt.y << "\" x2=\"" << pnt.x << "\" y2=\"" << pnt.y+Grid::Interval << "\" stroke=\"#000000\" />" << std::endl;
	}
}

void DrawGateRight(std::ofstream &f,vector2D p,int nIn,char *gateclass)
{
	POINT p1,p2;
	vector2D v;
	v=p+DirectionVector[CLOCKWISE_180]*2+DirectionVector[CLOCKWISE_270]*nIn;
	p1=v.toPOINT();
	v=p+DirectionVector[CLOCKWISE_090]*nIn;
	p2=v.toPOINT();
	f << "<rect x=\"" << p1.x << "\" y=\"" << p1.y << "\" width=\"" << Grid::Interval*2 << "\" height=\"" << p2.y-p1.y << "\" stroke=\"#000000\" fill=\"none\" />" << std::endl;
	v=p+DirectionVector[CLOCKWISE_180]*2+DirectionVector[CLOCKWISE_270]*(nIn-1);
	p1=v.toPOINT();
	v=p+DirectionVector[CLOCKWISE_180]*3+DirectionVector[CLOCKWISE_270]*(nIn-1);
	p2=v.toPOINT();
	for(int i=0;i<nIn;i++,p1.y+=Grid::Interval*2,p2.y+=Grid::Interval*2)
		f << "<line x1=\"" << p1.x << "\" y1=\"" << p1.y << "\" x2=\"" << p2.x << "\" y2=\"" << p2.y << "\" stroke=\"#000000\" />" << std::endl;
	v=p+DirectionVector[CLOCKWISE_180]*2+DirectionVector[CLOCKWISE_270]*nIn;
	p1=v.toPOINT();
	f << "<g font-size=\"" << (double)Grid::Interval*4/strlen(gateclass) << "\">";
	f << "<text x=\"" << p1.x << "\" y=\"" << p1.y+Grid::Interval*3/2 <<"\" text-anchor=\"start\">" << gateclass << "</text>";
	f << "</g>" << std::endl;
}

void DrawGateDown(std::ofstream &f,vector2D p,int nIn,char *gateclass)
{
	POINT p1,p2;
	vector2D v;
	v=p+DirectionVector[CLOCKWISE_270]*2+DirectionVector[CLOCKWISE_180]*nIn;
	p1=v.toPOINT();
	v=p+DirectionVector[CLOCKWISE_000]*nIn;
	p2=v.toPOINT();
	f << "<rect x=\"" << p1.x << "\" y=\"" << p1.y << "\" width=\"" << p2.x-p1.x << "\" height=\"" << Grid::Interval*2 << "\" stroke=\"#000000\" fill=\"none\" />" << std::endl;
	v=p+DirectionVector[CLOCKWISE_270]*2+DirectionVector[CLOCKWISE_000]*(nIn-1);
	p1=v.toPOINT();
	v=p+DirectionVector[CLOCKWISE_270]*3+DirectionVector[CLOCKWISE_000]*(nIn-1);
	p2=v.toPOINT();
	for(int i=0;i<nIn;i++,p1.x-=Grid::Interval*2,p2.x-=Grid::Interval*2)
		f << "<line x1=\"" << p1.x << "\" y1=\"" << p1.y << "\" x2=\"" << p2.x << "\" y2=\"" << p2.y << "\" stroke=\"#000000\" />" << std::endl;
	v=p+DirectionVector[CLOCKWISE_270]*2+DirectionVector[CLOCKWISE_000]*(nIn-2);
	p1=v.toPOINT();
	f << "<g font-size=\"" << (double)Grid::Interval*4/strlen(gateclass) << "\">";
	f << "<text x=\"" << p1.x << "\" y=\"" << p1.y+Grid::Interval*3/2 <<"\" text-anchor=\"start\">" << gateclass << "</text>";
	f << "</g>" << std::endl;
}

void DrawGateLeft(std::ofstream &f,vector2D p,int nIn,char *gateclass)
{
	POINT p1,p2;
	vector2D v;
	v=p+DirectionVector[CLOCKWISE_270]*nIn;
	p1=v.toPOINT();
	v=p+DirectionVector[CLOCKWISE_000]*2+DirectionVector[CLOCKWISE_090]*nIn;
	p2=v.toPOINT();
	f << "<rect x=\"" << p1.x << "\" y=\"" << p1.y << "\" width=\"" << Grid::Interval*2 << "\" height=\"" << p2.y-p1.y << "\" stroke=\"#000000\" fill=\"none\" />" << std::endl;
	v=p+DirectionVector[CLOCKWISE_000]*2+DirectionVector[CLOCKWISE_090]*(nIn-1);
	p1=v.toPOINT();
	v=p+DirectionVector[CLOCKWISE_000]*3+DirectionVector[CLOCKWISE_090]*(nIn-1);
	p2=v.toPOINT();
	for(int i=0;i<nIn;i++,p1.y-=Grid::Interval*2,p2.y-=Grid::Interval*2)
		f << "<line x1=\"" << p1.x << "\" y1=\"" << p1.y << "\" x2=\"" << p2.x << "\" y2=\"" << p2.y << "\" stroke=\"#000000\" />" << std::endl;
	v=p+DirectionVector[CLOCKWISE_090]*(nIn-1);
	p1=v.toPOINT();
	f << "<g font-size=\"" << (double)Grid::Interval*4/strlen(gateclass) << "\">";
	f << "<text x=\"" << p1.x << "\" y=\"" << p1.y+Grid::Interval*3/4 <<"\" text-anchor=\"start\">" << gateclass << "</text>";
	f << "</g>" << std::endl;
}

void DrawGateUp(std::ofstream &f,vector2D p,int nIn,char *gateclass)
{
	POINT p1,p2;
	vector2D v;
	v=p+DirectionVector[CLOCKWISE_180]*nIn;
	p1=v.toPOINT();
	v=p+DirectionVector[CLOCKWISE_000]*nIn+DirectionVector[CLOCKWISE_090]*2;
	p2=v.toPOINT();
	f << "<rect x=\"" << p1.x << "\" y=\"" << p1.y << "\" width=\"" << p2.x-p1.x << "\" height=\"" << p2.y-p1.y << "\" stroke=\"#000000\" fill=\"none\" />" << std::endl;
	v=p+DirectionVector[CLOCKWISE_090]*2+DirectionVector[CLOCKWISE_180]*(nIn-1);
	p1=v.toPOINT();
	v=p+DirectionVector[CLOCKWISE_090]*3+DirectionVector[CLOCKWISE_180]*(nIn-1);
	p2=v.toPOINT();
	for(int i=0;i<nIn;i++,p1.x+=Grid::Interval*2,p2.x+=Grid::Interval*2)
		f << "<line x1=\"" << p1.x << "\" y1=\"" << p1.y << "\" x2=\"" << p2.x << "\" y2=\"" << p2.y << "\" stroke=\"#000000\" />" << std::endl;
	v=p+DirectionVector[CLOCKWISE_180]*nIn;
	p1=v.toPOINT();
	f << "<g font-size=\"" << (double)Grid::Interval*4/strlen(gateclass) << "\">";
	f << "<text x=\"" << p1.x << "\" y=\"" << p1.y+Grid::Interval*3/2 <<"\" text-anchor=\"start\">" << gateclass << "</text>";
	f << "</g>" << std::endl;
}

void Draw_Gate(std::ofstream &f,vector2D p,Direction d,bool isOR,int nIn)
{
	POINT pnt;
	char *gateclass = (nIn == 1 ? " 1 " : (isOR ? "†1" : "•"));
	pnt=p.toPOINT();

	switch(d)
	{
	case CLOCKWISE_000:
		DrawGateRight(f,p,nIn,gateclass);
		f << "<line x1=\"" << pnt.x << "\" y1=\"" << pnt.y << "\" x2=\"" << pnt.x+Grid::Interval << "\" y2=\"" << pnt.y << "\" stroke=\"#000000\" />" << std::endl;
		break;
	case CLOCKWISE_090:
		DrawGateDown(f,p,nIn,gateclass);
		f << "<line x1=\"" << pnt.x << "\" y1=\"" << pnt.y << "\" x2=\"" << pnt.x << "\" y2=\"" << pnt.y+Grid::Interval << "\" stroke=\"#000000\" />" << std::endl;
		break;
	case CLOCKWISE_180:
		DrawGateLeft(f,p,nIn,gateclass);
		f << "<line x1=\"" << pnt.x << "\" y1=\"" << pnt.y << "\" x2=\"" << pnt.x-Grid::Interval << "\" y2=\"" << pnt.y << "\" stroke=\"#000000\" />" << std::endl;
		break;
	case CLOCKWISE_270:
		DrawGateUp(f,p,nIn,gateclass);
		f << "<line x1=\"" << pnt.x << "\" y1=\"" << pnt.y << "\" x2=\"" << pnt.x << "\" y2=\"" << pnt.y-Grid::Interval << "\" stroke=\"#000000\" />" << std::endl;
		break;
	}
}

void Draw_Gate_With_Not(std::ofstream &f,vector2D p,Direction d,bool isOR,int nIn)
{
	POINT pnt;
	pnt=p.toPOINT();

	Draw_Gate(f,p,d,isOR,nIn);

	switch(d)
	{
	case CLOCKWISE_000:
		f << "<circle cx=\"" << pnt.x+Grid::Interval/4 << "\" cy=\"" << pnt.y << "\" r=\"" << Grid::Interval/4 << "\" stroke=\"#000000\" fill=\"#ffffff\" />" << std::endl;
		break;
	case CLOCKWISE_090:
		f << "<circle cx=\"" << pnt.x << "\" cy=\"" << pnt.y+Grid::Interval/4 << "\" r=\"" << Grid::Interval/4 << "\" stroke=\"#000000\" fill=\"#ffffff\" />" << std::endl;
		break;
	case CLOCKWISE_180:
		f << "<circle cx=\"" << pnt.x-Grid::Interval/4 << "\" cy=\"" << pnt.y << "\" r=\"" << Grid::Interval/4 << "\" stroke=\"#000000\" fill=\"#ffffff\" />" << std::endl;
		break;
	case CLOCKWISE_270:
		f << "<circle cx=\"" << pnt.x << "\" cy=\"" << pnt.y-Grid::Interval/4 << "\" r=\"" << Grid::Interval/4 << "\" stroke=\"#000000\" fill=\"#ffffff\" />" << std::endl;
		break;
	}
}


void Data::DrawGate(std::ofstream &f)
{
	for(unsigned int i=0;i<gate.size();i++)
	{
		Gate g=gate[i];
		if (g.isNOT())
			Draw_Gate_With_Not(f, node(g.NodeOutIndex()).position() - DirectionVector[g.direction()], g.direction(),g.isOR(), g.nIn());
		else
			Draw_Gate(f, node(g.NodeOutIndex()).position() - DirectionVector[g.direction()], g.direction(),g.isOR(), g.nIn());
//		ClassifyOperationAndDraw(f,g.operation(),node(g.NodeOutIndex()).position()-DirectionVector[g.direction()],g.direction(),g.nIn());
	}
}

void Data::DrawConductor(std::ofstream &f)
{
	POINT p1,p2;
	for(unsigned int i=0;i<conductor.size();i++)
	{
		p1=node(conductor[i].nodeindex0()).position().toPOINT();
		p2=node(conductor[i].nodeindex1()).position().toPOINT();

		f << "<line x1=\"" << p1.x << "\" y1=\"" << p1.y << "\" x2=\"" << p2.x << "\" y2=\"" << p2.y << "\" stroke=\"" << (conductor[i].isMultipath() ? "#0000ff" : "#000000") << "\" />" << std::endl;
	}
}

void DrawBusIO(std::ofstream &f, vector2D p, Direction d, int nIn)
{
	POINT pnt;
	char *gateclass = "";
	pnt = p.toPOINT();

	switch (d)
	{
	case CLOCKWISE_000:
		DrawGateRight(f, p, nIn, gateclass);
		f << "<line x1=\"" << pnt.x << "\" y1=\"" << pnt.y << "\" x2=\"" << pnt.x + Grid::Interval << "\" y2=\"" << pnt.y << "\" stroke=\"#000000\" />" << std::endl;
		break;
	case CLOCKWISE_090:
		DrawGateDown(f, p, nIn, gateclass);
		f << "<line x1=\"" << pnt.x << "\" y1=\"" << pnt.y << "\" x2=\"" << pnt.x << "\" y2=\"" << pnt.y + Grid::Interval << "\" stroke=\"#000000\" />" << std::endl;
		break;
	case CLOCKWISE_180:
		DrawGateLeft(f, p, nIn, gateclass);
		f << "<line x1=\"" << pnt.x << "\" y1=\"" << pnt.y << "\" x2=\"" << pnt.x - Grid::Interval << "\" y2=\"" << pnt.y << "\" stroke=\"#000000\" />" << std::endl;
		break;
	case CLOCKWISE_270:
		DrawGateUp(f, p, nIn, gateclass);
		f << "<line x1=\"" << pnt.x << "\" y1=\"" << pnt.y << "\" x2=\"" << pnt.x << "\" y2=\"" << pnt.y - Grid::Interval << "\" stroke=\"#000000\" />" << std::endl;
		break;
	}
}

void Data::DrawBusIn(std::ofstream &f)
{
	for(unsigned int i=0;i<busin.size();i++)
	{
		DrawBusIO(f,node(busin[i].NodeInIndex()).position()-DirectionVector[busin[i].ReverseDirection()],busin[i].ReverseDirection(),busin[i].nOut());
	}
}

void Data::DrawBusOut(std::ofstream &f)
{
	for(unsigned int i=0;i<busout.size();i++)
	{
		DrawBusIO(f,node(busout[i].NodeOutIndex()).position()-DirectionVector[busout[i].direction()],busout[i].direction(),busout[i].nIn());
	}
}

void Data::DrawInputLabel(std::ofstream &f)
{
	POINT p1,p2;
	vector2D v;
	Direction d;
	for(unsigned int i=0;i<input.size();i++)
	{
		int len;
		len=lstrlen(input[i].name());
		d=input[i].direction();
		p1=(node(input[i].nodeindex()).position()-DirectionVector[input[i].direction()]).toPOINT();

		v=DirectionVector[d];
		p2=v.MulGridInterval();
			f << "<line x1=\"" << p1.x << "\" y1=\"" << p1.y << "\" x2=\"" << p1.x+p2.x << "\" y2=\"" << p1.y+p2.y << "\" stroke=\"" << "#000000\" />" << std::endl;
		v=DirectionVector[(d+3)&7];
		p2=v.MulGridInterval();
			f << "<line x1=\"" << p1.x << "\" y1=\"" << p1.y << "\" x2=\"" << p1.x+p2.x/2 << "\" y2=\"" << p1.y+p2.y/2 << "\" stroke=\"" << "#000000\" />" << std::endl;
		v=DirectionVector[(d+5)&7];
		p2=v.MulGridInterval();
			f << "<line x1=\"" << p1.x << "\" y1=\"" << p1.y << "\" x2=\"" << p1.x+1+p2.x/2 << "\" y2=\"" << p1.y+p2.y/2 << "\" stroke=\"" << "#000000\" />" << std::endl;

		switch(d)
		{
		case CLOCKWISE_000:
			f << "<g font-size=\"" << Grid::Interval*2-4 << "\">";
			f << "<text x=\"" << p1.x-Grid::Interval << "\" y=\"" << p1.y+Grid::Interval/2 <<"\" text-anchor=\"end\">" << input[i].name() << "</text>";
			f << "</g>" << std::endl;
			break;
		case CLOCKWISE_090:
			f << "<g writing-mode=\"tb\" font-size=\"" << Grid::Interval*2-4 << "\">";
			f << "<text x=\"" << p1.x-Grid::Interval/2 << "\" y=\"" << p1.y-Grid::Interval <<"\" text-anchor=\"end\" glyph-orientation-vertical=\"90\">" << input[i].name() << "</text>";
			f << "</g>" << std::endl;
			break;
		case CLOCKWISE_180:
			f << "<g font-size=\"" << Grid::Interval*2-4 << "\">";
			f << "<text x=\"" << p1.x+Grid::Interval << "\" y=\"" << p1.y+Grid::Interval/2 <<"\" text-anchor=\"start\">" << input[i].name() << "</text>";
			f << "</g>" << std::endl;
			break;
		case CLOCKWISE_270:
			f << "<g writing-mode=\"tb\" font-size=\"" << Grid::Interval*2-4 << "\">";
			f << "<text x=\"" << p1.x-Grid::Interval/2 << "\" y=\"" << p1.y+Grid::Interval <<"\" text-anchor=\"start\" glyph-orientation-vertical=\"90\">" << input[i].name() << "</text>";
			f << "</g>" << std::endl;
			break;
		}
	}
}

void Data::DrawOutputLabel(std::ofstream &f)
{
	POINT p1,p2;
	vector2D v;
	Direction d;
	for(unsigned int i=0;i<output.size();i++)
	{
		int len;
		len=lstrlen(output[i].name());
		d=output[i].direction();
		p1=(node(output[i].nodeindex()).position()-DirectionVector[output[i].direction()]).toPOINT();

		v=DirectionVector[d];
		p2=v.MulGridInterval();
			f << "<line x1=\"" << p1.x << "\" y1=\"" << p1.y << "\" x2=\"" << p1.x+p2.x << "\" y2=\"" << p1.y+p2.y << "\" stroke=\"" << "#000000\" />" << std::endl;
		v=DirectionVector[(d+1)&7];
		p2=v.MulGridInterval();
			f << "<line x1=\"" << p1.x << "\" y1=\"" << p1.y << "\" x2=\"" << p1.x+p2.x/2 << "\" y2=\"" << p1.y+p2.y/2 << "\" stroke=\"" << "#000000\" />" << std::endl;
		v=DirectionVector[(d+7)&7];
		p2=v.MulGridInterval();
			f << "<line x1=\"" << p1.x << "\" y1=\"" << p1.y << "\" x2=\"" << p1.x+p2.x/2 << "\" y2=\"" << p1.y+p2.y/2 << "\" stroke=\"" << "#000000\" />" << std::endl;

		switch(d)
		{
		case CLOCKWISE_000:
			f << "<g font-size=\"" << Grid::Interval*2-4 << "\">";
			f << "<text x=\"" << p1.x-Grid::Interval << "\" y=\"" << p1.y+Grid::Interval/2 <<"\" text-anchor=\"end\">" << output[i].name() << "</text>";
			f << "</g>" << std::endl;
			break;
		case CLOCKWISE_090:
			f << "<g writing-mode=\"tb\" font-size=\"" << Grid::Interval*2-4 << "\">";
			f << "<text x=\"" << p1.x-Grid::Interval/2 << "\" y=\"" << p1.y-Grid::Interval <<"\" text-anchor=\"end\" glyph-orientation-vertical=\"90\">" << output[i].name() << "</text>";
			f << "</g>" << std::endl;
			break;
		case CLOCKWISE_180:
			f << "<g font-size=\"" << Grid::Interval*2-4 << "\">";
			f << "<text x=\"" << p1.x+Grid::Interval << "\" y=\"" << p1.y+Grid::Interval/2 <<"\" text-anchor=\"start\">" << output[i].name() << "</text>";
			f << "</g>" << std::endl;
			break;
		case CLOCKWISE_270:
			f << "<g writing-mode=\"tb\" font-size=\"" << Grid::Interval*2-4 << "\">";
			f << "<text x=\"" << p1.x-Grid::Interval/2 << "\" y=\"" << p1.y+Grid::Interval <<"\" text-anchor=\"start\" glyph-orientation-vertical=\"90\">" << output[i].name() << "</text>";
			f << "</g>" << std::endl;
			break;
		}
	}
}

void Data::DrawNode(std::ofstream &f)
{
	Node n;
	vector2D v;
	for(unsigned int i=0;i<node_data.size();i++)
	{
		if(!node_data[i].isAllRight())
		{
			POINT p1,p2;
			n=node(i);
			p1=n.position().toPOINT();
			for(int j=0;j<8;j++)
			{
				if(n.isBranchExist((Direction)j))
				{

					v=DirectionVector[j];
					p2=v.MulGridInterval();
					p2.x=p1.x+p2.x/3;
					p2.y=p1.y+p2.y/3;
					f << "<line x1=\"" << p1.x << "\" y1=\"" << p1.y << "\" x2=\"" << p2.x << "\" y2=\"" << p2.y << "\" stroke=\"" << (node_data[i].isShort() ? "#ff0000" : "#00ff00") << "\" />" << std::endl;
				}
			}
		}
	}

	for(unsigned int i=0;i<node_data.size();i++)
	{
		POINT p1;
		n=node(i);
		p1=n.position().toPOINT();
		if(node_data[i].nBranch()>=3)
			f << "<circle cx=\"" << p1.x << "\" cy=\"" << p1.y << "\" r=\"" << 2 << "\" stroke=\"#000000\" fill=\"#000000\" />" << std::endl;
	}
}

void Data::DrawNote(std::ofstream &f)
{
	POINT p1, p2;
	vector2D v;
	Direction d;
	for (unsigned int i = 0; i<note.size(); i++)
	{
		int len;
		len = lstrlen(note[i].name());
		d = note[i].direction();
		p1 = note[i].position().toPOINT();

		v = DirectionVector[(d + 3) & 7];
		p2 = v.MulGridInterval();
		f << "<line x1=\"" << p1.x << "\" y1=\"" << p1.y << "\" x2=\"" << p1.x + p2.x / 2 << "\" y2=\"" << p1.y + p2.y / 2 << "\" stroke=\"" << "#000000\" />" << std::endl;
		v = DirectionVector[(d + 5) & 7];
		p2 = v.MulGridInterval();
		f << "<line x1=\"" << p1.x << "\" y1=\"" << p1.y << "\" x2=\"" << p1.x + p2.x / 2 << "\" y2=\"" << p1.y + p2.y / 2 << "\" stroke=\"" << "#000000\" />" << std::endl;

		switch (d)
		{
		case CLOCKWISE_000:
			f << "<g font-size=\"" << Grid::Interval * 2 - 4 << "\">";
			f << "<text x=\"" << p1.x + Grid::Interval << "\" y=\"" << p1.y + Grid::Interval / 2 << "\" text-anchor=\"start\">" << note[i].name() << "</text>";
			f << "</g>" << std::endl;
			break;
		case CLOCKWISE_090:
			f << "<g writing-mode=\"tb\" font-size=\"" << Grid::Interval * 2 - 4 << "\">";
			f << "<text x=\"" << p1.x - Grid::Interval / 2 << "\" y=\"" << p1.y + Grid::Interval << "\" text-anchor=\"start\" glyph-orientation-vertical=\"90\">" << note[i].name() << "</text>";
			f << "</g>" << std::endl;
			break;
		case CLOCKWISE_180:
			f << "<g font-size=\"" << Grid::Interval * 2 - 4 << "\">";
			f << "<text x=\"" << p1.x - Grid::Interval << "\" y=\"" << p1.y + Grid::Interval / 2 << "\" text-anchor=\"end\">" << note[i].name() << "</text>";
			f << "</g>" << std::endl;
			break;
		case CLOCKWISE_270:
			f << "<g writing-mode=\"tb\" font-size=\"" << Grid::Interval * 2 - 4 << "\">";
			f << "<text x=\"" << p1.x - Grid::Interval / 2 << "\" y=\"" << p1.y - Grid::Interval << "\" text-anchor=\"end\" glyph-orientation-vertical=\"90\">" << note[i].name() << "</text>";
			f << "</g>" << std::endl;
			break;
		}
	}
}

//‘S‚Ä‚Ì•`‰æ—v‘f‚ªŽû‚Ü‚é”ÍˆÍ
RECT Data::SpanRect()
{
	RECT rc;
	POINT pt;

	rc.bottom=0x80000000;
	rc.top=0x7fffffff;
	rc.right=0x80000000;
	rc.left=0x7fffffff;

	for(unsigned int i=0;i<module.size();i++)
	{
		Module &mdl=module[i];

		pt=(mdl.Center()+DirectionVector[CLOCKWISE_270]*(mdl.InternalHalfHeight()+mdl.Margin(3)+1)+DirectionVector[CLOCKWISE_180]*(mdl.InternalHalfWidth()+mdl.Margin(2)+1)).toPOINT();
		if(pt.x<rc.left)rc.left=pt.x;
		if(pt.y<rc.top)rc.top=pt.y;
		pt=(mdl.Center()+DirectionVector[CLOCKWISE_090]*(mdl.InternalHalfHeight()+mdl.Margin(1)+1)+DirectionVector[CLOCKWISE_000]*(mdl.InternalHalfWidth()+mdl.Margin(0)+1)).toPOINT();
		if(pt.x>rc.right)rc.right=pt.x;
		if(pt.y>rc.bottom)rc.bottom=pt.y;
	}
	for(unsigned int i=0;i<gate.size();i++)
	{
		switch(gate[i].direction())
		{
		case CLOCKWISE_000:
				pt=(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_180]*3+DirectionVector[CLOCKWISE_270]*gate[i].nIn()).toPOINT();
				if(pt.x<rc.left)rc.left=pt.x;
				if(pt.y<rc.top)rc.top=pt.y;
				pt=(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_090]*gate[i].nIn()).toPOINT();
				if(pt.x>rc.right)rc.right=pt.x;
				if(pt.y>rc.bottom)rc.bottom=pt.y;
			break;
		case CLOCKWISE_090:
				pt=(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_270]*3+DirectionVector[CLOCKWISE_180]*gate[i].nIn()).toPOINT();
				if(pt.x<rc.left)rc.left=pt.x;
				if(pt.y<rc.top)rc.top=pt.y;
				pt=(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_000]*gate[i].nIn()).toPOINT();
				if(pt.x>rc.right)rc.right=pt.x;
				if(pt.y>rc.bottom)rc.bottom=pt.y;
			break;
		case CLOCKWISE_180:
				pt=(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_270]*gate[i].nIn()).toPOINT();
				if(pt.x<rc.left)rc.left=pt.x;
				if(pt.y<rc.top)rc.top=pt.y;
				pt=(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_000]*3+DirectionVector[CLOCKWISE_090]*gate[i].nIn()).toPOINT();
				if(pt.x>rc.right)rc.right=pt.x;
				if(pt.y>rc.bottom)rc.bottom=pt.y;
			break;
		case CLOCKWISE_270:
				pt=(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_180]*gate[i].nIn()).toPOINT();
				if(pt.x<rc.left)rc.left=pt.x;
				if(pt.y<rc.top)rc.top=pt.y;
				pt=(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_090]*3+DirectionVector[CLOCKWISE_000]*gate[i].nIn()).toPOINT();
				if(pt.x>rc.right)rc.right=pt.x;
				if(pt.y>rc.bottom)rc.bottom=pt.y;
			break;
		}
	}
	for(unsigned int i=0;i<conductor.size();i++)
	{
		pt=node_data[conductor[i].nodeindex0()].position().toPOINT();
		if(pt.x<rc.left)rc.left=pt.x;
		if(pt.y<rc.top)rc.top=pt.y;
		if(pt.x>rc.right)rc.right=pt.x;
		if(pt.y>rc.bottom)rc.bottom=pt.y;
		pt=node_data[conductor[i].nodeindex1()].position().toPOINT();
		if(pt.x<rc.left)rc.left=pt.x;
		if(pt.y<rc.top)rc.top=pt.y;
		if(pt.x>rc.right)rc.right=pt.x;
		if(pt.y>rc.bottom)rc.bottom=pt.y;
	}
	for(unsigned int i=0;i<busin.size();i++)
	{
		switch(busin[i].direction())
		{
		case CLOCKWISE_000:
				pt=(node_data[busin[i].NodeInIndex()].position()-DirectionVector[CLOCKWISE_090]*busin[i].nOut()).toPOINT();
				if(pt.x<rc.left)rc.left=pt.x;
				if(pt.y<rc.top)rc.top=pt.y;
				pt=(node_data[busin[i].NodeInIndex()].position()-DirectionVector[CLOCKWISE_180]*3-DirectionVector[CLOCKWISE_270]*busin[i].nOut()).toPOINT();
				if(pt.x>rc.right)rc.right=pt.x;
				if(pt.y>rc.bottom)rc.bottom=pt.y;
			break;
		case CLOCKWISE_090:
				pt=(node_data[busin[i].NodeInIndex()].position()-DirectionVector[CLOCKWISE_000]*busin[i].nOut()).toPOINT();
				if(pt.x<rc.left)rc.left=pt.x;
				if(pt.y<rc.top)rc.top=pt.y;
				pt=(node_data[busin[i].NodeInIndex()].position()-DirectionVector[CLOCKWISE_270]*3-DirectionVector[CLOCKWISE_180]*busin[i].nOut()).toPOINT();
				if(pt.x>rc.right)rc.right=pt.x;
				if(pt.y>rc.bottom)rc.bottom=pt.y;
			break;
		case CLOCKWISE_180:
				pt=(node_data[busin[i].NodeInIndex()].position()-DirectionVector[CLOCKWISE_000]*3-DirectionVector[CLOCKWISE_090]*busin[i].nOut()).toPOINT();
				if(pt.x<rc.left)rc.left=pt.x;
				if(pt.y<rc.top)rc.top=pt.y;
				pt=(node_data[busin[i].NodeInIndex()].position()-DirectionVector[CLOCKWISE_270]*busin[i].nOut()).toPOINT();
				if(pt.x>rc.right)rc.right=pt.x;
				if(pt.y>rc.bottom)rc.bottom=pt.y;
			break;
		case CLOCKWISE_270:
				pt=(node_data[busin[i].NodeInIndex()].position()-DirectionVector[CLOCKWISE_090]*3-DirectionVector[CLOCKWISE_000]*busin[i].nOut()).toPOINT();
				if(pt.x<rc.left)rc.left=pt.x;
				if(pt.y<rc.top)rc.top=pt.y;
				pt=(node_data[busin[i].NodeInIndex()].position()-DirectionVector[CLOCKWISE_180]*busin[i].nOut()).toPOINT();
				if(pt.x>rc.right)rc.right=pt.x;
				if(pt.y>rc.bottom)rc.bottom=pt.y;
			break;
		}
	}
	for(unsigned int i=0;i<busout.size();i++)
	{
		switch(busout[i].direction())
		{
		case CLOCKWISE_000:
				pt=(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_180]*3+DirectionVector[CLOCKWISE_270]*busout[i].nIn()).toPOINT();
				if(pt.x<rc.left)rc.left=pt.x;
				if(pt.y<rc.top)rc.top=pt.y;
				pt=(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_090]*busout[i].nIn()).toPOINT();
				if(pt.x>rc.right)rc.right=pt.x;
				if(pt.y>rc.bottom)rc.bottom=pt.y;
			break;
		case CLOCKWISE_090:
				pt=(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_270]*3+DirectionVector[CLOCKWISE_180]*busout[i].nIn()).toPOINT();
				if(pt.x<rc.left)rc.left=pt.x;
				if(pt.y<rc.top)rc.top=pt.y;
				pt=(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_000]*busout[i].nIn()).toPOINT();
				if(pt.x>rc.right)rc.right=pt.x;
				if(pt.y>rc.bottom)rc.bottom=pt.y;
			break;
		case CLOCKWISE_180:
				pt=(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_270]*busout[i].nIn()).toPOINT();
				if(pt.x<rc.left)rc.left=pt.x;
				if(pt.y<rc.top)rc.top=pt.y;
				pt=(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_000]*3+DirectionVector[CLOCKWISE_090]*busout[i].nIn()).toPOINT();
				if(pt.x>rc.right)rc.right=pt.x;
				if(pt.y>rc.bottom)rc.bottom=pt.y;
			break;
		case CLOCKWISE_270:
				pt=(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_180]*busout[i].nIn()).toPOINT();
				if(pt.x<rc.left)rc.left=pt.x;
				if(pt.y<rc.top)rc.top=pt.y;
				pt=(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_090]*3+DirectionVector[CLOCKWISE_000]*busout[i].nIn()).toPOINT();
				if(pt.x>rc.right)rc.right=pt.x;
				if(pt.y>rc.bottom)rc.bottom=pt.y;
			break;
		}
	}
	for(unsigned int i=0;i<input.size();i++)
	{
		switch(input[i].direction())
		{
		case CLOCKWISE_000:
			pt=(node_data[input[i].nodeindex()].position()+DirectionVector[CLOCKWISE_270]+DirectionVector[CLOCKWISE_180]*(strlen(input[i].name())+3)).toPOINT();
			if(pt.x<rc.left)rc.left=pt.x;
			if(pt.y<rc.top)rc.top=pt.y;
			pt=(node_data[input[i].nodeindex()].position()+DirectionVector[CLOCKWISE_090]+DirectionVector[CLOCKWISE_000]).toPOINT();
			if(pt.x>rc.right)rc.right=pt.x;
			if(pt.y>rc.bottom)rc.bottom=pt.y;
			break;
		case CLOCKWISE_090:
			pt=(node_data[input[i].nodeindex()].position()+DirectionVector[CLOCKWISE_270]*(strlen(input[i].name())+3)+DirectionVector[CLOCKWISE_180]).toPOINT();
			if(pt.x<rc.left)rc.left=pt.x;
			if(pt.y<rc.top)rc.top=pt.y;
			pt=(node_data[input[i].nodeindex()].position()+DirectionVector[CLOCKWISE_090]+DirectionVector[CLOCKWISE_000]).toPOINT();
			if(pt.x>rc.right)rc.right=pt.x;
			if(pt.y>rc.bottom)rc.bottom=pt.y;
			break;
		case CLOCKWISE_180:
			pt=(node_data[input[i].nodeindex()].position()+DirectionVector[CLOCKWISE_270]+DirectionVector[CLOCKWISE_180]).toPOINT();
			if(pt.x<rc.left)rc.left=pt.x;
			if(pt.y<rc.top)rc.top=pt.y;
			pt=(node_data[input[i].nodeindex()].position()+DirectionVector[CLOCKWISE_090]+DirectionVector[CLOCKWISE_000]*(strlen(input[i].name())+3)).toPOINT();
			if(pt.x>rc.right)rc.right=pt.x;
			if(pt.y>rc.bottom)rc.bottom=pt.y;
			break;
		case CLOCKWISE_270:
			pt=(node_data[input[i].nodeindex()].position()+DirectionVector[CLOCKWISE_270]+DirectionVector[CLOCKWISE_180]).toPOINT();
			if(pt.x<rc.left)rc.left=pt.x;
			if(pt.y<rc.top)rc.top=pt.y;
			pt=(node_data[input[i].nodeindex()].position()+DirectionVector[CLOCKWISE_090]*(strlen(input[i].name())+3)+DirectionVector[CLOCKWISE_000]).toPOINT();
			if(pt.x>rc.right)rc.right=pt.x;
			if(pt.y>rc.bottom)rc.bottom=pt.y;
			break;
		}
	}
	for(unsigned int i=0;i<output.size();i++)
	{
		switch(output[i].direction())
		{
		case CLOCKWISE_000:
			pt=(node_data[output[i].nodeindex()].position()+DirectionVector[CLOCKWISE_270]+DirectionVector[CLOCKWISE_180]*(strlen(output[i].name())+3)).toPOINT();
			if(pt.x<rc.left)rc.left=pt.x;
			if(pt.y<rc.top)rc.top=pt.y;
			pt=(node_data[output[i].nodeindex()].position()+DirectionVector[CLOCKWISE_090]+DirectionVector[CLOCKWISE_000]).toPOINT();
			if(pt.x>rc.right)rc.right=pt.x;
			if(pt.y>rc.bottom)rc.bottom=pt.y;
			break;
		case CLOCKWISE_090:
			pt=(node_data[output[i].nodeindex()].position()+DirectionVector[CLOCKWISE_270]*(strlen(output[i].name())+3)+DirectionVector[CLOCKWISE_180]).toPOINT();
			if(pt.x<rc.left)rc.left=pt.x;
			if(pt.y<rc.top)rc.top=pt.y;
			pt=(node_data[output[i].nodeindex()].position()+DirectionVector[CLOCKWISE_090]+DirectionVector[CLOCKWISE_000]).toPOINT();
			if(pt.x>rc.right)rc.right=pt.x;
			if(pt.y>rc.bottom)rc.bottom=pt.y;
			break;
		case CLOCKWISE_180:
			pt=(node_data[output[i].nodeindex()].position()+DirectionVector[CLOCKWISE_270]+DirectionVector[CLOCKWISE_180]).toPOINT();
			if(pt.x<rc.left)rc.left=pt.x;
			if(pt.y<rc.top)rc.top=pt.y;
			pt=(node_data[output[i].nodeindex()].position()+DirectionVector[CLOCKWISE_090]+DirectionVector[CLOCKWISE_000]*(strlen(output[i].name())+3)).toPOINT();
			if(pt.x>rc.right)rc.right=pt.x;
			if(pt.y>rc.bottom)rc.bottom=pt.y;
			break;
		case CLOCKWISE_270:
			pt=(node_data[output[i].nodeindex()].position()+DirectionVector[CLOCKWISE_270]+DirectionVector[CLOCKWISE_180]).toPOINT();
			if(pt.x<rc.left)rc.left=pt.x;
			if(pt.y<rc.top)rc.top=pt.y;
			pt=(node_data[output[i].nodeindex()].position()+DirectionVector[CLOCKWISE_090]*(strlen(output[i].name())+3)+DirectionVector[CLOCKWISE_000]).toPOINT();
			if(pt.x>rc.right)rc.right=pt.x;
			if(pt.y>rc.bottom)rc.bottom=pt.y;
			break;
		}
	}
	for(unsigned int i=0;i<note.size();i++)
	{
		switch (note[i].direction())
		{
		case CLOCKWISE_000:
			pt = (note[i].position() + DirectionVector[CLOCKWISE_270] + DirectionVector[CLOCKWISE_180]).toPOINT();
			if (pt.x<rc.left)rc.left = pt.x;
			if (pt.y<rc.top)rc.top = pt.y;
			pt = (note[i].position() + DirectionVector[CLOCKWISE_090] + DirectionVector[CLOCKWISE_000] * (strlen(note[i].name()) + 1)).toPOINT();
			if (pt.x>rc.right)rc.right = pt.x;
			if (pt.y>rc.bottom)rc.bottom = pt.y;
			break;
		case CLOCKWISE_090:
			pt = (note[i].position() + DirectionVector[CLOCKWISE_270] + DirectionVector[CLOCKWISE_180]).toPOINT();
			if (pt.x<rc.left)rc.left = pt.x;
			if (pt.y<rc.top)rc.top = pt.y;
			pt = (note[i].position() + DirectionVector[CLOCKWISE_090] * (strlen(note[i].name()) + 1) + DirectionVector[CLOCKWISE_000]).toPOINT();
			if (pt.x>rc.right)rc.right = pt.x;
			if (pt.y>rc.bottom)rc.bottom = pt.y;
			break;
		case CLOCKWISE_180:
			pt = (note[i].position() + DirectionVector[CLOCKWISE_270] + DirectionVector[CLOCKWISE_180] * (strlen(note[i].name()) + 1)).toPOINT();
			if (pt.x<rc.left)rc.left = pt.x;
			if (pt.y<rc.top)rc.top = pt.y;
			pt = (note[i].position() + DirectionVector[CLOCKWISE_090] + DirectionVector[CLOCKWISE_000]).toPOINT();
			if (pt.x>rc.right)rc.right = pt.x;
			if (pt.y>rc.bottom)rc.bottom = pt.y;
			break;
		case CLOCKWISE_270:
			pt = (note[i].position() + DirectionVector[CLOCKWISE_270] * (strlen(note[i].name()) + 1) + DirectionVector[CLOCKWISE_180]).toPOINT();
			if (pt.x<rc.left)rc.left = pt.x;
			if (pt.y<rc.top)rc.top = pt.y;
			pt = (note[i].position() + DirectionVector[CLOCKWISE_090] + DirectionVector[CLOCKWISE_000]).toPOINT();
			if (pt.x>rc.right)rc.right = pt.x;
			if (pt.y>rc.bottom)rc.bottom = pt.y;
			break;
		}
	}

	rc.bottom+=Grid::Interval;
	rc.left-=Grid::Interval;
	rc.right+=Grid::Interval;
	rc.top-=Grid::Interval;
	return rc;
}

void Data::Draw(std::ofstream &f)
{
	char s[256];
	RECT rc;
	f << "<?xml version=\"1.0\" encoding=\"Shift-JIS\"?>" << std::endl;
	rc=SpanRect();
	sprintf_s(s,sizeof(s),"%d %d %d %d",rc.left,rc.top,rc.right-rc.left,rc.bottom-rc.top);
	f << "<svg xmlns=\"http://www.w3.org/2000/svg\" xmlns:xlink=\"http://www.w3.org/1999/xlink\" viewBox=\"" << s << "\">" << std::endl;
	DrawModule(f);
	DrawGate(f);
	DrawConductor(f);
	DrawBusIn(f);
	DrawBusOut(f);
	DrawInputLabel(f);
	DrawOutputLabel(f);
	DrawNode(f);
	DrawNote(f);
	f << "</svg>" << std::endl;
}

bool SaveSVG(Data d)
{
	OPENFILENAME ofn;
	char szFile[MAX_PATH]="";
	char szFileTitle[MAX_PATH]="";

	memset(&ofn,0,sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hMainWindow;
	ofn.lpstrFilter = TEXT("SVG(*.svg)\0*.svg\0\0");
	ofn.lpstrFile = szFile;
	ofn.lpstrFileTitle = szFileTitle;
	ofn.nFilterIndex = 1;
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = TEXT("svg");
	ofn.lpstrTitle = TEXT("SVGŒ`Ž®‚Å‰ñ˜H}‚ðo—Í");

	if(GetSaveFileName(&ofn) == 0) return false;

	std::ofstream f(szFile);
	d.Draw(f);
	f.close();
	return true;
}
