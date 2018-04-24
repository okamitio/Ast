#include"common.h"

Comment::Comment(Direction dir,vector2D v,char *t)
{
	d = dir;
	pos=v;
	for(int i=0;i<SIZE_OF_LABEL_NAME;i++) txt[i]=t[i];			//名前
}

Node::Node()
{
	pos=DirectionVector[NO_DIRECTION];
	for(int i=0;i<8;i++)
	{
		branch[i].SetDeviceClass(no_device);
		branch[i].SetIndexAndSubIndex(-1,-1);
	}
	buswidth=0,path=false,Multipath=false,Short=false,AllRight=false;
}

Node::Node(int bw)
{
	pos=DirectionVector[NO_DIRECTION];
	for(int i=0;i<8;i++)
	{
		branch[i].SetDeviceClass(no_device);
		branch[i].SetIndexAndSubIndex(-1,-1);
	}
	buswidth=bw,path=false,Multipath=false,Short=false,AllRight=false;
}

Label::Label(Direction dir,int n,char *nm)
{
	d=dir;	//方向
	node_index=n;		//端子座標を保持するノードのインデックス
	for(int i=0;i<SIZE_OF_LABEL_NAME;i++) nam[i]=nm[i];			//名前
}

LabelIn::LabelIn(Direction dir,int n,char *nm,int buswidth):Label(dir,n,nm)
{
	for(int i=0;i<buswidth;i++) state.push_back(false);
}


Conductor::Conductor(int n0,int n1,Direction di)
{
	buswidth=0;
	Multipath=false;
	Short=false;
	nod[0]=n0;
	nod[1]=n1;
	d=di;
}


Conductor::Conductor(int n0,int n1,Direction di,int nWire)
{
	buswidth=nWire;
	Multipath=false;
	Short=false;
	nod[0]=n0;
	nod[1]=n1;
	d=di;
}

Gate::Gate(bool not, bool isor,Direction dir,int out,std::vector<SourceData> &in)
{
	invert = not;
	isOr = isor;
	d=dir;
	node_out=out;
	array_in=in;
}

Gate::Gate(bool not, bool isor, Direction dir,int out,std::vector<SourceData> &in,int buswidth)
{
	invert = not;
	isOr = isor;
	d=dir;
	node_out=out;
	array_in=in;
	for(int i=0;i<buswidth;i++)state.push_back(false);
}

BusIn::BusIn(Direction dir,std::vector<int> &out,SourceData in)
{
	d=dir;
	node_out=out;
	node_in=in;
}

BusIn::BusIn(Direction dir,std::vector<int> &out,SourceData in,int buswidth)
{
	d=dir;
	node_out=out;
	node_in=in;
	for(int i=0;i<buswidth;i++)state.push_back(false);
}

BusOut::BusOut(Direction dir,int out,std::vector<SourceData> &in)
{
	d=dir;
	node_out=out;
	array_in=in;
}

BusOut::BusOut(Direction dir,int out,std::vector<SourceData> &in,int buswidth)
{
	d=dir;
	node_out=out;
	array_in=in;
	for(int i=0;i<buswidth;i++)state.push_back(false);
}
