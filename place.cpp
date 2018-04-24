#include"common.h"

extern HWND hMainWindow;


namespace Interface
{
	extern vector2D AlignedMouseCoordinates;	//Grid::Interval間隔で
	extern vector2D FirstAlignedMouseCoordinates;	//始点
}


namespace Place
{
	extern unsigned int NewBusWidth;		//バス幅
	extern Direction NewDirection;	//新しい記号の方向
	extern bool NewGateOperationisNOT;	//新しい論理ゲートの演算がインバートされるか
	extern bool NewGateOperationisOR;	//新しい論理ゲートの演算がORなら真,ANDなら偽
	extern unsigned int NewGateIn; //新しい論理ゲートの入力数
	extern Module *pModule;	//モジュールへのポインタ
	extern char NewLabelName[SIZE_OF_LABEL_NAME];	//新しいラベルの名前
}

bool Data::AbleToPlaceBusIn()
{
	unsigned int i;

	//入力
	for(i=0;i<node_data.size();i++)	//全ノード探索
	{
		if(node_data[i].position()==Interface::AlignedMouseCoordinates-DirectionVector[Place::NewDirection]) //出力と一致する位置のノードが見つかれば
		{
			if(node_data[i].nWire()!=Place::NewBusWidth)return false;
			if(node_data[i].isBranchExist(Place::NewDirection))return false;
		}
	}

	//出力
	vector2D pos;
	vector2D v;
	switch(Place::NewDirection)
	{
	case CLOCKWISE_000:
		pos=Interface::AlignedMouseCoordinates-DirectionVector[CLOCKWISE_180]*3+DirectionVector[CLOCKWISE_270]*(Place::NewBusWidth-1);
		v=DirectionVector[CLOCKWISE_090]*2;
		break;
	case CLOCKWISE_090:
		pos=Interface::AlignedMouseCoordinates-DirectionVector[CLOCKWISE_270]*3+DirectionVector[CLOCKWISE_000]*(Place::NewBusWidth-1);
		v=DirectionVector[CLOCKWISE_180]*2;
		break;
	case CLOCKWISE_180:
		pos=Interface::AlignedMouseCoordinates-DirectionVector[CLOCKWISE_000]*3+DirectionVector[CLOCKWISE_090]*(Place::NewBusWidth-1);
		v=DirectionVector[CLOCKWISE_270]*2;
		break;
	case CLOCKWISE_270:
		pos=Interface::AlignedMouseCoordinates-DirectionVector[CLOCKWISE_090]*3+DirectionVector[CLOCKWISE_180]*(Place::NewBusWidth-1);
		v=DirectionVector[CLOCKWISE_000]*2;
		break;
	}
	for(i=0;i<Place::NewBusWidth;i++,pos+=v)
	{
		unsigned int j;
		for(j=0;j<node_data.size();j++)
		{
			if(node_data[j].position()==pos)
			{
				if(node_data[j].nWire()!=1)return false;
				if(node_data[j].isBranchExist(ReverseDirection(Place::NewDirection)))return false;
			}
		}
	}
	return true;
}

void Data::PlaceBusIn()
{
	unsigned int i;
	std::vector<int> out;		//出力の節のインデックス
	SourceData in;	//入力
//入力
	for(i=0;i<node_data.size();i++)	//全ノード探索
	{
		if(node_data[i].position()==Interface::AlignedMouseCoordinates-DirectionVector[Place::NewDirection]) //出力と一致する位置のノードが見つかれば
		{
			in.SetNodeInIndex(i);
			node_data[i].write_branch(Place::NewDirection,bus_in_in,busin.size(),-1);
			break;
		}
	}
	if(i==node_data.size()) //入力と一致する位置のノードが見つからなければ
	{
		Node n(Place::NewBusWidth);
		in.SetNodeInIndex(node_data.size());
		n.write_branch(Place::NewDirection,bus_in_in,busin.size(),-1);
		n.setposition(Interface::AlignedMouseCoordinates-DirectionVector[Place::NewDirection]);
		node_data.push_back(n);
	}
//出力
	vector2D pos;
	vector2D v;
	switch(Place::NewDirection)
	{
	case CLOCKWISE_000:
		pos=Interface::AlignedMouseCoordinates-DirectionVector[CLOCKWISE_180]*3+DirectionVector[CLOCKWISE_270]*(Place::NewBusWidth-1);
		v=DirectionVector[CLOCKWISE_090]*2;
		break;
	case CLOCKWISE_090:
		pos=Interface::AlignedMouseCoordinates-DirectionVector[CLOCKWISE_270]*3+DirectionVector[CLOCKWISE_000]*(Place::NewBusWidth-1);
		v=DirectionVector[CLOCKWISE_180]*2;
		break;
	case CLOCKWISE_180:
		pos=Interface::AlignedMouseCoordinates-DirectionVector[CLOCKWISE_000]*3+DirectionVector[CLOCKWISE_090]*(Place::NewBusWidth-1);
		v=DirectionVector[CLOCKWISE_270]*2;
		break;
	case CLOCKWISE_270:
		pos=Interface::AlignedMouseCoordinates-DirectionVector[CLOCKWISE_090]*3+DirectionVector[CLOCKWISE_180]*(Place::NewBusWidth-1);
		v=DirectionVector[CLOCKWISE_000]*2;
		break;
	}
	for(i=0;i<Place::NewBusWidth;i++,pos+=v)
	{
		unsigned int j;
		for(j=0;j<node_data.size();j++)
		{
			if(node_data[j].position()==pos)
			{
				out.push_back(j);
				node_data[j].write_branch(ReverseDirection(Place::NewDirection),bus_in_out,busin.size(),i);
				break;
			}
		}
		if(j==node_data.size())
		{
			Node n(1);
			out.push_back(node_data.size());
			n.setposition(pos);
			n.write_branch(ReverseDirection(Place::NewDirection),bus_in_out,busin.size(),i);
			node_data.push_back(n);
		}
	}
	BusIn b(Place::NewDirection,out,in,Place::NewBusWidth);
	busin.push_back(b);
	for(unsigned int j=0;j<Place::NewBusWidth;j++)
		DetectErrorAndConnect(in.NodeInIndex(),j);
	for(unsigned int j=0;j<out.size();j++)
		DetectErrorAndConnect(out[j],0);
}

bool Data::AbleToPlaceBusOut()
{
	for(unsigned int i=0;i<node_data.size();i++)
	{
		if(node_data[i].position()==Interface::AlignedMouseCoordinates+DirectionVector[Place::NewDirection])
		{
			if(node_data[i].nWire()!=Place::NewBusWidth)return false;
			if(node_data[i].isBranchExist(ReverseDirection(Place::NewDirection)))return false;
		}

		vector2D v;
		Node n;
		switch(Place::NewDirection)
		{
		case CLOCKWISE_000:
			n.setposition(Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_180]*3+DirectionVector[CLOCKWISE_270]*(Place::NewBusWidth-1));
			v=DirectionVector[CLOCKWISE_090]*2;
			break;
		case CLOCKWISE_090:
			n.setposition(Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_270]*3+DirectionVector[CLOCKWISE_000]*(Place::NewBusWidth-1));
			v=DirectionVector[CLOCKWISE_180]*2;
			break;
		case CLOCKWISE_180:
			n.setposition(Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_000]*3+DirectionVector[CLOCKWISE_090]*(Place::NewBusWidth-1));
			v=DirectionVector[CLOCKWISE_270]*2;
			break;
		case CLOCKWISE_270:
			n.setposition(Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_090]*3+DirectionVector[CLOCKWISE_180]*(Place::NewBusWidth-1));
			v=DirectionVector[CLOCKWISE_000]*2;
			break;
		}
		for(unsigned int j=0;j<Place::NewBusWidth;j++,n.setposition(n.position()+v))
			if(n.position()==node_data[i].position())
			{
				if(node_data[i].nWire()!=1)return false;
				if(node_data[i].isBranchExist(Place::NewDirection))return false;
			}
	}
	return true;
}

void Data::PlaceBusOut()
{
	unsigned int i;
	int out;		//出力の節のインデックス
	std::vector<SourceData> in;	//入力の配列
//出力
	for (i = 0; i < node_data.size(); i++)	//全ノード探索
		if (node_data[i].position() == Interface::AlignedMouseCoordinates + DirectionVector[Place::NewDirection]) //出力と一致する位置のノードが見つかれば
			break;
	if(i==node_data.size()) //出力と一致する位置のノードが見つからなければ
	{
		Node n(Place::NewBusWidth);
		out=node_data.size();
		n.write_branch(ReverseDirection(Place::NewDirection),bus_out_out,busout.size(),-1);
		n.setposition(Interface::AlignedMouseCoordinates+DirectionVector[Place::NewDirection]);
		node_data.push_back(n);
	}
	else
	{
		out = i;
		node_data[i].write_branch(ReverseDirection(Place::NewDirection), bus_out_out, busout.size(), -1);
	}
//入力
	vector2D pos;
	vector2D v;
	switch(Place::NewDirection)
	{
	case CLOCKWISE_000:
		pos=Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_180]*3+DirectionVector[CLOCKWISE_270]*(Place::NewBusWidth-1);
		v=DirectionVector[CLOCKWISE_090]*2;
		break;
	case CLOCKWISE_090:
		pos=Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_270]*3+DirectionVector[CLOCKWISE_000]*(Place::NewBusWidth-1);
		v=DirectionVector[CLOCKWISE_180]*2;
		break;
	case CLOCKWISE_180:
		pos=Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_000]*3+DirectionVector[CLOCKWISE_090]*(Place::NewBusWidth-1);
		v=DirectionVector[CLOCKWISE_270]*2;
		break;
	case CLOCKWISE_270:
		pos=Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_090]*3+DirectionVector[CLOCKWISE_180]*(Place::NewBusWidth-1);
		v=DirectionVector[CLOCKWISE_000]*2;
		break;
	}
	for(i=0;i<Place::NewBusWidth;i++,pos+=v)
	{
		SourceData gin;
		unsigned int j;
		for(j=0;j<node_data.size();j++)
		{
			if(node_data[j].position()==pos)
			{
				gin.SetNodeInIndex(j);
				in.push_back(gin);
				node_data[j].write_branch(Place::NewDirection,bus_out_in,busout.size(),i);
				break;
			}
		}
		if(j==node_data.size())
		{
			Node n(1);
			gin.SetNodeInIndex(node_data.size());
			in.push_back(gin);
			n.setposition(pos);
			n.write_branch(Place::NewDirection,bus_out_in,busout.size(),i);
			node_data.push_back(n);
		}
	}
	BusOut b(Place::NewDirection,out,in,Place::NewBusWidth);
	busout.push_back(b);
	for(i=0;i<in.size();i++)
		DetectErrorAndConnect(in[i].NodeInIndex(),0);
	for(int j=0;j<node(out).nWire();j++)
		for(int j=0;j<node(out).nWire();j++)
			DetectErrorAndConnect(out,j);
}

bool Data::AbleToPlaceLabelIn()
{
	for(unsigned int i=0;i<node_data.size();i++)
		if(node_data[i].position()==Interface::AlignedMouseCoordinates+DirectionVector[Place::NewDirection])
		{
			if(node_data[i].nWire()!=Place::NewBusWidth)return false;
			if(node_data[i].isBranchExist(ReverseDirection(Place::NewDirection)))return false;
		}
	return true;
}

bool Data::AbleToPlaceLabelOut()
{
	for(unsigned int i=0;i<node_data.size();i++)
		if(node_data[i].position()==Interface::AlignedMouseCoordinates-DirectionVector[Place::NewDirection])
		{
			if(node_data[i].nWire()!=Place::NewBusWidth)return false;
			if(node_data[i].isBranchExist(Place::NewDirection))return false;
		}
	return true;
}

void Data::PlaceInLabel()	//入力ラベル設置
{
	for(unsigned int i=0;i<node_data.size();i++)
	{
		LabelIn l(Place::NewDirection,i,Place::NewLabelName,Place::NewBusWidth);
		if(node_data[i].position()==Interface::AlignedMouseCoordinates+DirectionVector[l.direction()])
		{
			node_data[i].write_branch(l.ReverseDirection(),label_input,input.size(),-1);
			input.push_back(l);
			for(int j=0;j<node_data[i].nWire();j++)
				DetectErrorAndConnect(i,j);
			return;
		}
	}

	LabelIn l(Place::NewDirection,node_data.size(),Place::NewLabelName,Place::NewBusWidth);
	Node n(Place::NewBusWidth);
	n.write_branch(l.ReverseDirection(),label_input,input.size(),-1);
	n.setposition(Interface::AlignedMouseCoordinates+DirectionVector[l.direction()]);
	node_data.push_back(n);
	input.push_back(l);
	for(int j=0;j<node_data[node_data.size()-1].nWire();j++)
		DetectErrorAndConnect(node_data.size()-1,j);
}

void Data::PlaceOutLabel()
{
	for(unsigned int i=0;i<node_data.size();i++)
	{
		LabelOut l(ReverseDirection(Place::NewDirection),i,Place::NewLabelName);
		if(node_data[i].position()==Interface::AlignedMouseCoordinates+DirectionVector[l.direction()])
		{
			node_data[i].write_branch(l.ReverseDirection(),label_output,output.size(),-1);
			output.push_back(l);
			for(int j=0;j<node_data[i].nWire();j++)
				DetectErrorAndConnect(i,j);
			return;
		}
	}

	LabelOut l(ReverseDirection(Place::NewDirection),node_data.size(),Place::NewLabelName);
	Node n(Place::NewBusWidth);
	n.write_branch(l.ReverseDirection(),label_output,output.size(),-1);
	n.setposition(Interface::AlignedMouseCoordinates+DirectionVector[l.direction()]);
	node_data.push_back(n);
	output.push_back(l);
	for(int j=0;j<node_data[node_data.size()-1].nWire();j++)
		DetectErrorAndConnect(node_data.size()-1,j);
}

bool Data::AbleToPlaceGate()
{
	for(unsigned int i=0;i<node_data.size();i++)
	{
		if(node_data[i].position()==Interface::AlignedMouseCoordinates+DirectionVector[Place::NewDirection])
		{
			if(node_data[i].nWire()!=Place::NewBusWidth)return false;
			if(node_data[i].isBranchExist(ReverseDirection(Place::NewDirection)))return false;
		}

		vector2D v;
		Node n;
		switch(Place::NewDirection)
		{
		case CLOCKWISE_000:
			n.setposition(Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_180]*3+DirectionVector[CLOCKWISE_270]*(Place::NewGateIn-1));
			v=DirectionVector[CLOCKWISE_090]*2;
			break;
		case CLOCKWISE_090:
			n.setposition(Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_270]*3+DirectionVector[CLOCKWISE_000]*(Place::NewGateIn-1));
			v=DirectionVector[CLOCKWISE_180]*2;
			break;
		case CLOCKWISE_180:
			n.setposition(Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_000]*3+DirectionVector[CLOCKWISE_090]*(Place::NewGateIn -1));
			v=DirectionVector[CLOCKWISE_270]*2;
			break;
		case CLOCKWISE_270:
			n.setposition(Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_090]*3+DirectionVector[CLOCKWISE_180]*(Place::NewGateIn -1));
			v=DirectionVector[CLOCKWISE_000]*2;
			break;
		}
		for(unsigned int j=0;j<Place::NewGateIn;j++,n.setposition(n.position()+v))
			if(n.position()==node_data[i].position())
			{
				if(node_data[i].nWire()!=Place::NewBusWidth)return false;
				if(node_data[i].isBranchExist(Place::NewDirection))return false;
			}
	}
	return true;
}

void Data::PlaceGate()	//ゲート設置
{
	unsigned int i;
	int out;		//出力の節のインデックス
	std::vector<SourceData> in;	//入力の配列
//出力
	for(i=0;i<node_data.size();i++)	//全ノード探索
		if(node_data[i].position()==Interface::AlignedMouseCoordinates+DirectionVector[Place::NewDirection]) //出力と一致する位置のノードが見つかれば
			break;

	if(i==node_data.size()) //出力と一致する位置のノードが見つからなければ
	{
		Node n(Place::NewBusWidth);
		out=node_data.size();
		n.write_branch(ReverseDirection(Place::NewDirection),gate_out,gate.size(),-1);
		n.setposition(Interface::AlignedMouseCoordinates+DirectionVector[Place::NewDirection]);
		node_data.push_back(n);
	}
	else
	{
		out = i;
		node_data[i].write_branch(ReverseDirection(Place::NewDirection), gate_out, gate.size(), -1);
	}
//入力
	vector2D pos;
	vector2D v;
	switch(Place::NewDirection)
	{
	case CLOCKWISE_000:
		pos=Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_180]*3+DirectionVector[CLOCKWISE_270]*(Place::NewGateIn -1);
		v=DirectionVector[CLOCKWISE_090]*2;
		break;
	case CLOCKWISE_090:
		pos=Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_270]*3+DirectionVector[CLOCKWISE_000]*(Place::NewGateIn -1);
		v=DirectionVector[CLOCKWISE_180]*2;
		break;
	case CLOCKWISE_180:
		pos=Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_000]*3+DirectionVector[CLOCKWISE_090]*(Place::NewGateIn -1);
		v=DirectionVector[CLOCKWISE_270]*2;
		break;
	case CLOCKWISE_270:
		pos=Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_090]*3+DirectionVector[CLOCKWISE_180]*(Place::NewGateIn -1);
		v=DirectionVector[CLOCKWISE_000]*2;
		break;
	}
	for(i=0;i<Place::NewGateIn;i++,pos+=v)
	{
		SourceData gin;
		unsigned int j;
		for(j=0;j<node_data.size();j++)
		{
			if(node_data[j].position()==pos)
			{
				gin.SetNodeInIndex(j);
				in.push_back(gin);
				node_data[j].write_branch(Place::NewDirection,gate_in,gate.size(),i);
				break;
			}
		}
		if(j==node_data.size())
		{
			Node n(Place::NewBusWidth);
			gin.SetNodeInIndex(node_data.size());
			in.push_back(gin);
			n.setposition(pos);
			n.write_branch(Place::NewDirection,gate_in,gate.size(),i);
			node_data.push_back(n);
		}
	}
	Gate g(Place::NewGateOperationisNOT,Place::NewGateOperationisOR,Place::NewDirection,out,in,Place::NewBusWidth);
	gate.push_back(g);
	for(i=0;i<in.size();i++)
		for(int j=0;j<node(in[i].NodeInIndex()).nWire();j++)
			DetectErrorAndConnect(in[i].NodeInIndex(),j);
	for(int j=0;j<node(out).nWire();j++)
		DetectErrorAndConnect(out,j);
}

bool Data::AbleToPlaceModule()
{
	for(unsigned int i=0;i<node_data.size();i++)
	{
		vector2D v;
		v=Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_000]*(Place::pModule->GetSize(3)-1)+DirectionVector[CLOCKWISE_270]*(Place::pModule->InternalHalfHeight()+Place::pModule->Margin(3)+1);
		for(int j=0;j<Place::pModule->GetSize(3);j++,v+=DirectionVector[CLOCKWISE_180]*2)
				if(node_data[i].position()==v)
				{
					if(node_data[i].nWire()!=Place::pModule->GetBusWidth(3,j))return false;
					if(node_data[i].isBranchExist(CLOCKWISE_090))return false;
				}

		v=Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_090]*(Place::pModule->GetSize(0)-1)+DirectionVector[CLOCKWISE_000]*(Place::pModule->InternalHalfWidth()+Place::pModule->Margin(0)+1);
		for(int j=0;j<Place::pModule->GetSize(0);j++,v+=DirectionVector[CLOCKWISE_270]*2)
				if(node_data[i].position()==v)
				{
					if(node_data[i].nWire()!=Place::pModule->GetBusWidth(0,j))return false;
					if(node_data[i].isBranchExist(CLOCKWISE_180))return false;
				}
	
		v=Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_180]*(Place::pModule->GetSize(1)-1)+DirectionVector[CLOCKWISE_090]*(Place::pModule->InternalHalfHeight()+Place::pModule->Margin(1)+1);
		for(int j=0;j<Place::pModule->GetSize(1);j++,v+=DirectionVector[CLOCKWISE_000]*2)
				if(node_data[i].position()==v)
				{
					if(node_data[i].nWire()!=Place::pModule->GetBusWidth(1,j))return false;
					if(node_data[i].isBranchExist(CLOCKWISE_270))return false;
				}
	
		v=Interface::AlignedMouseCoordinates+DirectionVector[CLOCKWISE_270]*(Place::pModule->GetSize(2)-1)+DirectionVector[CLOCKWISE_180]*(Place::pModule->InternalHalfWidth()+Place::pModule->Margin(2)+1);
		for(int j=0;j<Place::pModule->GetSize(2);j++,v+=DirectionVector[CLOCKWISE_090]*2)
				if(node_data[i].position()==v)
				{
					if(node_data[i].nWire()!=Place::pModule->GetBusWidth(2,j))return false;
					if(node_data[i].isBranchExist(CLOCKWISE_000))return false;
				}
	}
	return true;
}

void Data::PlaceModule()
{
	int h;
	std::vector<int> out_nod(Place::pModule->nOutput(),-1);//出力のノードのインデックスの配列
	SourceData tmp;
	tmp.SetNodeInIndex(-1),tmp.SourceDevice().SetDeviceClass(no_device),tmp.SourceDevice().SetIndexAndSubIndex(-1,-1);
	std::vector<SourceData> in_nod(Place::pModule->nInput(),tmp);	//入力のノードのインデックスの配列
	vector2D v;

	vector2D tmpv=Place::pModule->Center();

	Place::pModule->SetCenter(Interface::AlignedMouseCoordinates);


	//上
	v=Place::pModule->Center()+DirectionVector[CLOCKWISE_000]*(Place::pModule->GetSize(3)-1)+DirectionVector[CLOCKWISE_270]*(Place::pModule->InternalHalfHeight()+Place::pModule->Margin(3)+1);
	for(h=0;h<Place::pModule->GetSize(3);h++,v+=DirectionVector[CLOCKWISE_180]*2)
	{
		switch(Place::pModule->Get(3,h).GetDeviceClass())
		{
			unsigned int i;
		case module_out:
			for(i=0;i<node_data.size();i++)	//全ノード探索
			{
				if(node_data[i].position()==v) //出力と一致する位置のノードが見つかれば
				{
					out_nod[Place::pModule->Get(3,h).GetDeviceIndex()]=i;
					node_data[i].write_branch(ReverseDirection(CLOCKWISE_270),module_out,module.size(),Place::pModule->Get(3,h).GetDeviceIndex());
					break;
				}
			}
			if(i==node_data.size()) //出力と一致する位置のノードが見つからなければ
			{
				Node n(Place::pModule->GetBusWidth(3,h));
				out_nod[Place::pModule->Get(3,h).GetDeviceIndex()]=node_data.size();
				n.write_branch(ReverseDirection(CLOCKWISE_270),module_out,module.size(),Place::pModule->Get(3,h).GetDeviceIndex());
				n.setposition(v);
				node_data.push_back(n);
			}
			break;
		case module_in:
			for(i=0;i<node_data.size();i++)
			{
				if(node_data[i].position()==v)
				{
					in_nod[Place::pModule->Get(3,h).GetDeviceIndex()]=tmp;
					in_nod[Place::pModule->Get(3,h).GetDeviceIndex()].SetNodeInIndex(i);
					node_data[i].write_branch(ReverseDirection(CLOCKWISE_270),module_in,module.size(),Place::pModule->Get(3,h).GetDeviceIndex());
					break;
				}
			}
			if(i==node_data.size())
			{
				Node n(Place::pModule->GetBusWidth(3,h));
				in_nod[Place::pModule->Get(3,h).GetDeviceIndex()]=tmp;
				in_nod[Place::pModule->Get(3,h).GetDeviceIndex()].SetNodeInIndex(node_data.size());
				n.setposition(v);
				n.write_branch(ReverseDirection(CLOCKWISE_270),module_in,module.size(),Place::pModule->Get(3,h).GetDeviceIndex());
				node_data.push_back(n);
			}
			break;
		}
	}

	//右
	v=Place::pModule->Center()+DirectionVector[CLOCKWISE_090]*(Place::pModule->GetSize(0)-1)+DirectionVector[CLOCKWISE_000]*(Place::pModule->InternalHalfWidth()+Place::pModule->Margin(0)+1);
	for(h=0;h<Place::pModule->GetSize(0);h++,v+=DirectionVector[CLOCKWISE_270]*2)
	{
		switch(Place::pModule->Get(0,h).GetDeviceClass())
		{
			unsigned int i;
		case module_out:
			for(i=0;i<node_data.size();i++)	//全ノード探索
			{
				if(node_data[i].position()==v) //出力と一致する位置のノードが見つかれば
				{
					out_nod[Place::pModule->Get(0,h).GetDeviceIndex()]=i;
					node_data[i].write_branch(ReverseDirection(CLOCKWISE_000),module_out,module.size(),Place::pModule->Get(0,h).GetDeviceIndex());
					break;
				}
			}
			if(i==node_data.size()) //出力と一致する位置のノードが見つからなければ
			{
				Node n(Place::pModule->GetBusWidth(0,h));
				out_nod[Place::pModule->Get(0,h).GetDeviceIndex()]=node_data.size();
				n.write_branch(ReverseDirection(CLOCKWISE_000),module_out,module.size(),Place::pModule->Get(0,h).GetDeviceIndex());
				n.setposition(v);
				node_data.push_back(n);
			}
			break;
		case module_in:
			for(i=0;i<node_data.size();i++)
			{
				if(node_data[i].position()==v)
				{
					in_nod[Place::pModule->Get(0,h).GetDeviceIndex()]=tmp;
					in_nod[Place::pModule->Get(0,h).GetDeviceIndex()].SetNodeInIndex(i);
					node_data[i].write_branch(ReverseDirection(CLOCKWISE_000),module_in,module.size(),Place::pModule->Get(0,h).GetDeviceIndex());
					break;
				}
			}
			if(i==node_data.size())
			{
				Node n(Place::pModule->GetBusWidth(0,h));
				in_nod[Place::pModule->Get(0,h).GetDeviceIndex()]=tmp;
				in_nod[Place::pModule->Get(0,h).GetDeviceIndex()].SetNodeInIndex(node_data.size());
				n.setposition(v);
				n.write_branch(ReverseDirection(CLOCKWISE_000),module_in,module.size(),Place::pModule->Get(0,h).GetDeviceIndex());
				node_data.push_back(n);
			}
			break;
		}
	}

	//下
	v=Place::pModule->Center()+DirectionVector[CLOCKWISE_180]*(Place::pModule->GetSize(1)-1)+DirectionVector[CLOCKWISE_090]*(Place::pModule->InternalHalfHeight()+Place::pModule->Margin(1)+1);
	for(h=0;h<Place::pModule->GetSize(1);h++,v+=DirectionVector[CLOCKWISE_000]*2)
	{
		switch(Place::pModule->Get(1,h).GetDeviceClass())
		{
			unsigned int i;
		case module_out:
			for(i=0;i<node_data.size();i++)	//全ノード探索
			{
				if(node_data[i].position()==v) //出力と一致する位置のノードが見つかれば
				{
					out_nod[Place::pModule->Get(1,h).GetDeviceIndex()]=i;
					node_data[i].write_branch(ReverseDirection(CLOCKWISE_090),module_out,module.size(),Place::pModule->Get(1,h).GetDeviceIndex());
					break;
				}
			}
			if(i==node_data.size()) //出力と一致する位置のノードが見つからなければ
			{
				Node n(Place::pModule->GetBusWidth(1,h));
				out_nod[Place::pModule->Get(1,h).GetDeviceIndex()]=node_data.size();
				n.write_branch(ReverseDirection(CLOCKWISE_090),module_out,module.size(),Place::pModule->Get(1,h).GetDeviceIndex());
				n.setposition(v);
				node_data.push_back(n);
			}
			break;
		case module_in:
			for(i=0;i<node_data.size();i++)
			{
				if(node_data[i].position()==v)
				{
					in_nod[Place::pModule->Get(1,h).GetDeviceIndex()]=tmp;
					in_nod[Place::pModule->Get(1,h).GetDeviceIndex()].SetNodeInIndex(i);
					node_data[i].write_branch(ReverseDirection(CLOCKWISE_090),module_in,module.size(),Place::pModule->Get(1,h).GetDeviceIndex());
					break;
				}
			}
			if(i==node_data.size())
			{
				Node n(Place::pModule->GetBusWidth(1,h));
				in_nod[Place::pModule->Get(1,h).GetDeviceIndex()]=tmp;
				in_nod[Place::pModule->Get(1,h).GetDeviceIndex()].SetNodeInIndex(node_data.size());
				n.setposition(v);
				n.write_branch(ReverseDirection(CLOCKWISE_090),module_in,module.size(),Place::pModule->Get(1,h).GetDeviceIndex());
				node_data.push_back(n);
			}
			break;
		}
	}

	//左
	v=Place::pModule->Center()+DirectionVector[CLOCKWISE_270]*(Place::pModule->GetSize(2)-1)+DirectionVector[CLOCKWISE_180]*(Place::pModule->InternalHalfWidth()+Place::pModule->Margin(2)+1);
	for(h=0;h<Place::pModule->GetSize(2);h++,v+=DirectionVector[CLOCKWISE_090]*2)
	{
		switch(Place::pModule->Get(2,h).GetDeviceClass())
		{
			unsigned int i;
		case module_out:
			for(i=0;i<node_data.size();i++)	//全ノード探索
			{
				if(node_data[i].position()==v) //出力と一致する位置のノードが見つかれば
				{
					out_nod[Place::pModule->Get(2,h).GetDeviceIndex()]=i;
					node_data[i].write_branch(ReverseDirection(CLOCKWISE_180),module_out,module.size(),Place::pModule->Get(2,h).GetDeviceIndex());
					break;
				}
			}
			if(i==node_data.size()) //出力と一致する位置のノードが見つからなければ
			{
				Node n(Place::pModule->GetBusWidth(2,h));
				out_nod[Place::pModule->Get(2,h).GetDeviceIndex()]=node_data.size();
				n.write_branch(ReverseDirection(CLOCKWISE_180),module_out,module.size(),Place::pModule->Get(2,h).GetDeviceIndex());
				n.setposition(v);
				node_data.push_back(n);
			}
			break;
		case module_in:
			for(i=0;i<node_data.size();i++)
			{
				if(node_data[i].position()==v)
				{
					in_nod[Place::pModule->Get(2,h).GetDeviceIndex()]=tmp;
					in_nod[Place::pModule->Get(2,h).GetDeviceIndex()].SetNodeInIndex(i);
					node_data[i].write_branch(ReverseDirection(CLOCKWISE_180),module_in,module.size(),Place::pModule->Get(2,h).GetDeviceIndex());
					break;
				}
			}
			if(i==node_data.size())
			{
				Node n(Place::pModule->GetBusWidth(2,h));
				in_nod[Place::pModule->Get(2,h).GetDeviceIndex()]=tmp;
				in_nod[Place::pModule->Get(2,h).GetDeviceIndex()].SetNodeInIndex(node_data.size());
				n.setposition(v);
				n.write_branch(ReverseDirection(CLOCKWISE_180),module_in,module.size(),Place::pModule->Get(2,h).GetDeviceIndex());
				node_data.push_back(n);
			}
			break;
		}
	}
	module.push_back(*Place::pModule);
	module[module.size()-1].LetNodeIn(in_nod);
	module[module.size()-1].LetNodeOut(out_nod);

	for(unsigned int g=0;g<in_nod.size();g++)
		for(int i=0;i<node(in_nod[g].NodeInIndex()).nWire();i++)
			DetectErrorAndConnect(in_nod[g].NodeInIndex(),i);
	for(unsigned int g=0;g<out_nod.size();g++)
		for(int i=0;i<node(out_nod[g]).nWire();i++)
			DetectErrorAndConnect(out_nod[g],i);

	Place::pModule->SetCenter(tmpv);
}


bool Data::AbleToPlaceConductor()
{
	for(unsigned int i=0;i<node_data.size();i++)
	{
		if(node_data[i].position()==Interface::FirstAlignedMouseCoordinates)
		{
			if(node_data[i].nWire()!=Place::NewBusWidth)return false;
			if(node_data[i].isBranchExist(AlignedDirection(Interface::FirstAlignedMouseCoordinates,Interface::AlignedMouseCoordinates)))return false;
		}

		if(node_data[i].position()==AlignAngle(Interface::FirstAlignedMouseCoordinates,Interface::AlignedMouseCoordinates))
		{
			if(node_data[i].nWire()!=Place::NewBusWidth)return false;
			if(node_data[i].isBranchExist(AlignedDirection(AlignAngle(Interface::FirstAlignedMouseCoordinates,Interface::AlignedMouseCoordinates),Interface::FirstAlignedMouseCoordinates)))return false;
		}
	}
	return true;
}

void Data::PlaceConductor()
{
	Direction d;
	unsigned int i;
	int n0,n1;

	for(i=0;i<node_data.size();i++)
	{
		if(node_data[i].position()==Interface::FirstAlignedMouseCoordinates)
		{
			node_data[i].write_branch(AlignedDirection(Interface::FirstAlignedMouseCoordinates,Interface::AlignedMouseCoordinates),lead_wire,conductor.size(),0);
			break;
		}
	}
	if(i==node_data.size())
	{
		Node n(Place::NewBusWidth);
		n.setposition(Interface::FirstAlignedMouseCoordinates);
		d=AlignedDirection(n.position(),Interface::AlignedMouseCoordinates);
		n.write_branch(d,lead_wire,conductor.size(),0);
		node_data.push_back(n);
	}
	n0=i;

	for(i=0;i<node_data.size();i++)
	{
		if(node_data[i].position()==AlignAngle(Interface::FirstAlignedMouseCoordinates,Interface::AlignedMouseCoordinates))
		{
			node_data[i].write_branch(AlignedDirection(AlignAngle(Interface::FirstAlignedMouseCoordinates,Interface::AlignedMouseCoordinates),Interface::FirstAlignedMouseCoordinates),lead_wire,conductor.size(),1);
			break;
		}
	}
	if(i==node_data.size())
	{
		Node n(Place::NewBusWidth);
		n.setposition(AlignAngle(Interface::FirstAlignedMouseCoordinates,Interface::AlignedMouseCoordinates));
		d=AlignedDirection(n.position(),Interface::FirstAlignedMouseCoordinates);
		n.write_branch(d,lead_wire,conductor.size(),1);
		node_data.push_back(n);
	}
	n1=i;

	Conductor c(n0,n1,AlignedDirection(node(n0).position(),node(n1).position()),Place::NewBusWidth);
	conductor.push_back(c);

	for(int j=0;j<c.nWire();j++)
		DetectErrorAndConnect(c.nodeindex0(),j);
}

void Data::WriteNote()
{
	Comment t(Place::NewDirection,Interface::AlignedMouseCoordinates,Place::NewLabelName);
	note.push_back(t);
}