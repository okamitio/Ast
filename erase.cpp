#include"common.h"

namespace Interface
{
	extern POINT MouseCoordinates;	//マウス座標
	extern vector2D AlignedMouseCoordinates;	//Grid::Interval間隔で
	extern vector2D FirstAlignedMouseCoordinates;	//始点
}

bool Data::EraseBranch(int index,Direction d)
{
	node_data[index].write_branch(d,no_device,-1,-1);
	if(node_data[index].nBranch()==0)
	{
		Delete(node_data,index);
		for(unsigned int i=0;i<input.size();i++)
			if(input[i].nodeindex()>index)input[i].dec_node_index();
		for(unsigned int i=0;i<output.size();i++)
			if(output[i].nodeindex()>index)output[i].dec_node_index();
		for(unsigned int i=0;i<gate.size();i++)
		{
			if(gate[i].NodeOutIndex()>index)gate[i].dec_out_index();
			for(int j=0;j<gate[i].nIn();j++)
				if(gate[i].NodeInIndex(j)>index)gate[i].dec_in_index(j);
		}
		for(unsigned int i=0;i<conductor.size();i++)
		{
			if(conductor[i].nodeindex0()>index)conductor[i].dec_node0();
			if(conductor[i].nodeindex1()>index)conductor[i].dec_node1();
		}
		for(unsigned int i=0;i<busin.size();i++)
		{
			if(busin[i].NodeInIndex()>index)busin[i].dec_in_index();
			for(int j=0;j<busin[i].nOut();j++)
				if(busin[i].NodeOutIndex(j)>index)busin[i].dec_out_index(j);
		}
		for(unsigned int i=0;i<busout.size();i++)
		{
			if(busout[i].NodeOutIndex()>index)busout[i].dec_out_index();
			for(int j=0;j<busout[i].nIn();j++)
				if(busout[i].NodeInIndex(j)>index)busout[i].dec_in_index(j);
		}
		for(unsigned int i=0;i<module.size();i++)
		{
			for(unsigned int j=0;j<module[i].nOutput();j++)
				if(module[i].NodeOut(j)>index)module[i].dec_NodeOut(j);
			for(unsigned int j=0;j<module[i].nInput();j++)
				if(module[i].NodeIn(j)>index)module[i].dec_NodeIn(j);
		}
		return true;
	}
	return false;
}

void Data::MoveOver(DeviceClassification dev,int n)
{
	for(unsigned int i=0;i<node_data.size();i++)
		for(unsigned int j=0;j<8;j++)
			if(node_data[i].dClass((Direction)j)==dev && node_data[i].index((Direction)j)>n)
				node_data[i].dec_index(j);


	for(unsigned int i=0;i<gate.size();i++)
		for(int j=0;j<gate[i].nIn();j++)
		{
			DeviceIndex di=gate[i].GetSourceDev(j);
			if(di.GetDeviceClass()==dev && di.GetDeviceIndex()>n)
				gate[i].SetSourceDevIndex(j,di.GetDeviceIndex()-1,di.GetDeviceSubIndex());
		}

	for(unsigned int i=0;i<busin.size();i++)
		if(busin[i].GetSourceDevClass()==dev && busin[i].GetSourceDevIndex()>n)
		{
			DeviceIndex di=busin[i].GetSourceDev();
			busin[i].SetSourceDevIndex(di.GetDeviceIndex()-1,di.GetDeviceSubIndex());
		}

	for(unsigned int i=0;i<busout.size();i++)
		for(int j=0;j<busout[i].nIn();j++)
		{
			DeviceIndex di=busout[i].GetSourceDev(j);
			if(di.GetDeviceClass()==dev && di.GetDeviceIndex()>n)
				busout[i].SetSourceDevIndex(j,di.GetDeviceIndex()-1,di.GetDeviceSubIndex());
		}

	for(unsigned int i=0;i<output.size();i++)
		if(output[i].GetSourceDevClass()==dev && output[i].GetSourceDevIndex()>n)
		{
			DeviceIndex di=output[i].GetSourceDev();
			output[i].SetSourceDevIndex(di.GetDeviceIndex()-1,di.GetDeviceSubIndex());
		}

	for(unsigned int i=0;i<module.size();i++)
		for(unsigned int j=0;j<module[i].nInput();j++)
		{
			DeviceIndex di=module[i].InArray(j);
			if(di.GetDeviceClass()==dev && di.GetDeviceIndex()>n)
			{
				di.DecIndex();
				module[i].SetSourceDev(j,di);
			}
		}
}


void Data::EraseInputLabel(unsigned int n)
{
	int nodeindex=input[n].nodeindex();
	if(EraseBranch(input[n].nodeindex(),input[n].ReverseDirection()))nodeindex=-1;
	Delete(input,n);
	MoveOver(label_input,n);
	if(nodeindex>=0)
	{
		for(int i=0;i<node(nodeindex).nWire();i++)
			DetectErrorAndConnect(nodeindex,i);
	}
}

void Data::EraseOutputLabel(unsigned int n)
{
	int nodeindex=output[n].nodeindex();
	if(EraseBranch(output[n].nodeindex(),output[n].ReverseDirection()))nodeindex=-1;
	Delete(output,n);
	MoveOver(label_output,n);
	if(nodeindex>=0)
	{
		for(int i=0;i<node(nodeindex).nWire();i++)
			DetectErrorAndConnect(nodeindex,i);
	}
}

void Data::EraseGate(unsigned int n)
{
	int nodeindex=gate[n].NodeOutIndex();
	if(EraseBranch(gate[n].NodeOutIndex(),gate[n].ReverseDirection()))nodeindex=-1;
	if(nodeindex==-1)
	{
		for(int i=0;i<gate[n].nIn();i++)
			EraseBranch(gate[n].NodeInIndex(i),gate[n].direction());
	}
	else
	{
		for(int i=0;i<gate[n].nIn();i++)
		{
			int index=gate[n].NodeInIndex(i);
			if(EraseBranch(index,gate[n].direction()))if(nodeindex>index)nodeindex--;
		}
	}

	Delete(gate,n);
	MoveOver(gate_out,n);
	MoveOver(gate_in,n);
	if(nodeindex>=0)
	{
		for(int i=0;i<node(nodeindex).nWire();i++)
			DetectErrorAndConnect(nodeindex,i);
	}
}

void Data::EraseConductor(unsigned int n)
{
	int nodeindex0=conductor[n].nodeindex0();
	if(EraseBranch(nodeindex0,conductor[n].direction())) nodeindex0=-1;

	int nodeindex1=conductor[n].nodeindex1();
	if(EraseBranch(nodeindex1,conductor[n].ReverseDirection()))
	{
		if(nodeindex0>nodeindex1) nodeindex0--; //ここの不等式に等号をつけるかどうかの判断は棚上げ
		nodeindex1=-1;
	}

	Delete(conductor,n);
	MoveOver(lead_wire,n);

	if(nodeindex0>=0)
	{
		for(int i=0;i<node(nodeindex0).nWire();i++)
			DetectErrorAndConnect(nodeindex0,i);
	}
	if(nodeindex1>=0)
	{
		for(int i=0;i<node(nodeindex1).nWire();i++)
			DetectErrorAndConnect(nodeindex1,i);
	}
}

void Data::EraseModule(unsigned int n)
{
	std::vector<int> nodeindex;
	for(unsigned int i=0;i<module[n].nOutput();i++)nodeindex.push_back(module[n].NodeOut(i));

	for (int h = 0; h < 4; h++)
	{
		for (int i = 0; i<module[n].GetSize(h); i++)
		{
			const Direction dir[4] = { CLOCKWISE_180,CLOCKWISE_270,CLOCKWISE_000,CLOCKWISE_090 };
			switch (module[n].Get(h, i).GetDeviceClass())
			{
				int index;
			case module_out:
				index = module[n].NodeOut(module[n].Get(h, i).GetDeviceIndex());
				if (EraseBranch(index, dir[h]))
				{
					nodeindex[module[n].Get(h, i).GetDeviceIndex()] = -1;
					for (unsigned int j = 0; j<nodeindex.size(); j++)
						if (nodeindex[j]>index)nodeindex[j]--;
				}
				break;
			case module_in:
				index = module[n].NodeIn(module[n].Get(h, i).GetDeviceIndex());
				if (EraseBranch(index, dir[h]))
				{
					for (unsigned int j = 0; j<nodeindex.size(); j++)
						if (nodeindex[j]>index)nodeindex[j]--;
				}
				break;
			}
		}
	}

	Delete(module,n);
	MoveOver(module_out,n);
	MoveOver(module_in,n);
	for(unsigned int i=0;i<nodeindex.size();i++)
	{
		if(nodeindex[i]>=0)
			for(int j=0;j<node(nodeindex[i]).nWire();j++)
				DetectErrorAndConnect(nodeindex[i],j);
	}
}

void Data::EraseBusIn(unsigned int n)
{
	int index;
	std::vector<int> nodeindex;
	for(int i=0;i<busin[n].nOut();i++)nodeindex.push_back(busin[n].NodeOutIndex(i));

	for(int i=0;i<busin[n].nOut();i++)
	{
		index=busin[n].NodeOutIndex(i);
		if(EraseBranch(index,busin[n].ReverseDirection()))
		{
			nodeindex[i]=-1;
			for(unsigned int j=0;j<nodeindex.size();j++)
				if(nodeindex[j]>index)nodeindex[j]--;
		}
	}

	index=busin[n].NodeInIndex();
	if(EraseBranch(index,busin[n].direction()))
	{
		for(unsigned int j=0;j<nodeindex.size();j++)
			if(nodeindex[j]>index)nodeindex[j]--;
	}

	Delete(busin,n);
	MoveOver(bus_in_out,n);
	MoveOver(bus_in_in,n);
	for(unsigned int i=0;i<nodeindex.size();i++)
	{
		if(nodeindex[i]>=0)
			for(int j=0;j<node(nodeindex[i]).nWire();j++)
				DetectErrorAndConnect(nodeindex[i],j);
	}
}

void Data::EraseBusOut(unsigned int n)
{
	int nodeindex=busout[n].NodeOutIndex();
	if(EraseBranch(busout[n].NodeOutIndex(),busout[n].ReverseDirection()))nodeindex=-1;
	if(nodeindex==-1)
	{
		for(int i=0;i<busout[n].nIn();i++)
			EraseBranch(busout[n].NodeInIndex(i),busout[n].direction());
	}
	else
	{
		for(int i=0;i<busout[n].nIn();i++)
		{
			int index=busout[n].NodeInIndex(i);
			if(EraseBranch(index,busout[n].direction()))if(nodeindex>index)nodeindex--;
		}
	}

	Delete(busout,n);
	MoveOver(bus_out_out,n);
	MoveOver(bus_out_in,n);
	if(nodeindex>=0)
	{
		for(int i=0;i<node(nodeindex).nWire();i++)
			DetectErrorAndConnect(nodeindex,i);
	}
}

bool Data::EraseDevice()
{
	bool b=false;
	for(unsigned int i=0;i<note.size();i++)
		if(Interface::AlignedMouseCoordinates==note[i].position())
			Delete(note,i--),b=true;
	for(unsigned int i=0;i<input.size();i++)
		if(Interface::AlignedMouseCoordinates==node(input[i].nodeindex()).position()-DirectionVector[input[i].direction()])
			EraseInputLabel(i--),b=true;
	for(unsigned int i=0;i<output.size();i++)
		if(Interface::AlignedMouseCoordinates==node(output[i].nodeindex()).position()-DirectionVector[output[i].direction()])
			EraseOutputLabel(i--),b=true;
	for(unsigned int i=0;i<gate.size();i++)
	{
		switch(gate[i].direction())
		{
		case CLOCKWISE_000:
			if(	(Interface::MouseCoordinates.x-(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_180]*3).toPOINT().x)>0 &&
				(Interface::MouseCoordinates.y-(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_270]*gate[i].nIn()).toPOINT().y)>0 &&
				(Interface::MouseCoordinates.x-(node_data[gate[i].NodeOutIndex()].position()-DirectionVector[CLOCKWISE_000]).toPOINT().x)<0 &&
				(Interface::MouseCoordinates.y-(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_090]*gate[i].nIn()).toPOINT().y)<0)
				EraseGate(i--),b=true;
			break;
		case CLOCKWISE_090:
			if(	(Interface::MouseCoordinates.x-(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_180]*gate[i].nIn()).toPOINT().x)>0 &&
				(Interface::MouseCoordinates.y-(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_270]*3).toPOINT().y)>0 &&
				(Interface::MouseCoordinates.x-(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_000]*gate[i].nIn()).toPOINT().x)<0 &&
				(Interface::MouseCoordinates.y-(node_data[gate[i].NodeOutIndex()].position()-DirectionVector[CLOCKWISE_090]).toPOINT().y)<0)
				EraseGate(i--),b=true;
			break;
		case CLOCKWISE_180:
			if(	(Interface::MouseCoordinates.x-(node_data[gate[i].NodeOutIndex()].position()-DirectionVector[CLOCKWISE_180]).toPOINT().x)>0 &&
				(Interface::MouseCoordinates.y-(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_270]*gate[i].nIn()).toPOINT().y)>0 &&
				(Interface::MouseCoordinates.x-(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_000]*3).toPOINT().x)<0 &&
				(Interface::MouseCoordinates.y-(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_090]*gate[i].nIn()).toPOINT().y)<0)
				EraseGate(i--),b=true;
			break;
		case CLOCKWISE_270:
			if(	(Interface::MouseCoordinates.x-(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_180]*gate[i].nIn()).toPOINT().x)>0 &&
				(Interface::MouseCoordinates.y-(node_data[gate[i].NodeOutIndex()].position()-DirectionVector[CLOCKWISE_270]).toPOINT().y)>0 &&
				(Interface::MouseCoordinates.x-(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_000]*gate[i].nIn()).toPOINT().x)<0 &&
				(Interface::MouseCoordinates.y-(node_data[gate[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_090]*3).toPOINT().y)<0)
				EraseGate(i--),b=true;
			break;
		}
	}
	for(unsigned int i=0;i<busin.size();i++)
	{
		switch(busin[i].ReverseDirection())
		{
		case CLOCKWISE_000:
			if(	(Interface::MouseCoordinates.x-(node_data[busin[i].NodeInIndex()].position()+DirectionVector[CLOCKWISE_180]*3).toPOINT().x)>0 &&
				(Interface::MouseCoordinates.y-(node_data[busin[i].NodeInIndex()].position()+DirectionVector[CLOCKWISE_270]*busin[i].nOut()).toPOINT().y)>0 &&
				(Interface::MouseCoordinates.x-(node_data[busin[i].NodeInIndex()].position()-DirectionVector[CLOCKWISE_000]).toPOINT().x)<0 &&
				(Interface::MouseCoordinates.y-(node_data[busin[i].NodeInIndex()].position()+DirectionVector[CLOCKWISE_090]*busin[i].nOut()).toPOINT().y)<0)
				EraseBusIn(i--),b=true;
			break;
		case CLOCKWISE_090:
			if(	(Interface::MouseCoordinates.x-(node_data[busin[i].NodeInIndex()].position()+DirectionVector[CLOCKWISE_180]*busin[i].nOut()).toPOINT().x)>0 &&
				(Interface::MouseCoordinates.y-(node_data[busin[i].NodeInIndex()].position()+DirectionVector[CLOCKWISE_270]*3).toPOINT().y)>0 &&
				(Interface::MouseCoordinates.x-(node_data[busin[i].NodeInIndex()].position()+DirectionVector[CLOCKWISE_000]*busin[i].nOut()).toPOINT().x)<0 &&
				(Interface::MouseCoordinates.y-(node_data[busin[i].NodeInIndex()].position()-DirectionVector[CLOCKWISE_090]).toPOINT().y)<0)
				EraseBusIn(i--),b=true;
			break;
		case CLOCKWISE_180:
			if(	(Interface::MouseCoordinates.x-(node_data[busin[i].NodeInIndex()].position()-DirectionVector[CLOCKWISE_180]).toPOINT().x)>0 &&
				(Interface::MouseCoordinates.y-(node_data[busin[i].NodeInIndex()].position()+DirectionVector[CLOCKWISE_270]*busin[i].nOut()).toPOINT().y)>0 &&
				(Interface::MouseCoordinates.x-(node_data[busin[i].NodeInIndex()].position()+DirectionVector[CLOCKWISE_000]*3).toPOINT().x)<0 &&
				(Interface::MouseCoordinates.y-(node_data[busin[i].NodeInIndex()].position()+DirectionVector[CLOCKWISE_090]*busin[i].nOut()).toPOINT().y)<0)
				EraseBusIn(i--),b=true;
			break;
		case CLOCKWISE_270:
			if(	(Interface::MouseCoordinates.x-(node_data[busin[i].NodeInIndex()].position()+DirectionVector[CLOCKWISE_180]*busin[i].nOut()).toPOINT().x)>0 &&
				(Interface::MouseCoordinates.y-(node_data[busin[i].NodeInIndex()].position()-DirectionVector[CLOCKWISE_270]).toPOINT().y)>0 &&
				(Interface::MouseCoordinates.x-(node_data[busin[i].NodeInIndex()].position()+DirectionVector[CLOCKWISE_000]*busin[i].nOut()).toPOINT().x)<0 &&
				(Interface::MouseCoordinates.y-(node_data[busin[i].NodeInIndex()].position()+DirectionVector[CLOCKWISE_090]*3).toPOINT().y)<0)
				EraseBusIn(i--),b=true;
			break;
		}
	}
	for(unsigned int i=0;i<busout.size();i++)
	{
		switch(busout[i].direction())
		{
		case CLOCKWISE_000:
			if(	(Interface::MouseCoordinates.x-(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_180]*3).toPOINT().x)>0 &&
				(Interface::MouseCoordinates.y-(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_270]*busout[i].nIn()).toPOINT().y)>0 &&
				(Interface::MouseCoordinates.x-(node_data[busout[i].NodeOutIndex()].position()-DirectionVector[CLOCKWISE_000]).toPOINT().x)<0 &&
				(Interface::MouseCoordinates.y-(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_090]*busout[i].nIn()).toPOINT().y)<0)
				EraseBusOut(i--),b=true;
			break;
		case CLOCKWISE_090:
			if(	(Interface::MouseCoordinates.x-(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_180]*busout[i].nIn()).toPOINT().x)>0 &&
				(Interface::MouseCoordinates.y-(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_270]*3).toPOINT().y)>0 &&
				(Interface::MouseCoordinates.x-(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_000]*busout[i].nIn()).toPOINT().x)<0 &&
				(Interface::MouseCoordinates.y-(node_data[busout[i].NodeOutIndex()].position()-DirectionVector[CLOCKWISE_090]).toPOINT().y)<0)
				EraseBusOut(i--),b=true;
			break;
		case CLOCKWISE_180:
			if(	(Interface::MouseCoordinates.x-(node_data[busout[i].NodeOutIndex()].position()-DirectionVector[CLOCKWISE_180]).toPOINT().x)>0 &&
				(Interface::MouseCoordinates.y-(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_270]*busout[i].nIn()).toPOINT().y)>0 &&
				(Interface::MouseCoordinates.x-(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_000]*3).toPOINT().x)<0 &&
				(Interface::MouseCoordinates.y-(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_090]*busout[i].nIn()).toPOINT().y)<0)
				EraseBusOut(i--),b=true;
			break;
		case CLOCKWISE_270:
			if(	(Interface::MouseCoordinates.x-(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_180]*busout[i].nIn()).toPOINT().x)>0 &&
				(Interface::MouseCoordinates.y-(node_data[busout[i].NodeOutIndex()].position()-DirectionVector[CLOCKWISE_270]).toPOINT().y)>0 &&
				(Interface::MouseCoordinates.x-(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_000]*busout[i].nIn()).toPOINT().x)<0 &&
				(Interface::MouseCoordinates.y-(node_data[busout[i].NodeOutIndex()].position()+DirectionVector[CLOCKWISE_090]*3).toPOINT().y)<0)
				EraseBusOut(i--),b=true;
			break;
		}
	}
	for(unsigned int i=0;i<conductor.size();i++)
		if (Interface::AlignedMouseCoordinates==node(conductor[i].nodeindex0()).position() && Interface::FirstAlignedMouseCoordinates==node(conductor[i].nodeindex1()).position() ||
			Interface::AlignedMouseCoordinates==node(conductor[i].nodeindex1()).position() && Interface::FirstAlignedMouseCoordinates==node(conductor[i].nodeindex0()).position() )
			EraseConductor(i--),b=true;
	for(unsigned int i=0;i<module.size();i++)
		if(	(Interface::MouseCoordinates.x-(module[i].Center()+DirectionVector[CLOCKWISE_180]*(module[i].Margin(2)+module[i].InternalHalfWidth())).toPOINT().x)>0 &&
			(Interface::MouseCoordinates.y-(module[i].Center()+DirectionVector[CLOCKWISE_270]*(module[i].Margin(3)+module[i].InternalHalfHeight())).toPOINT().y)>0 &&
			(Interface::MouseCoordinates.x-(module[i].Center()+DirectionVector[CLOCKWISE_000]*(module[i].Margin(0)+module[i].InternalHalfWidth())).toPOINT().x)<0 &&
			(Interface::MouseCoordinates.y-(module[i].Center()+DirectionVector[CLOCKWISE_090]*(module[i].Margin(1)+module[i].InternalHalfHeight())).toPOINT().y)<0)
			EraseModule(i--),b=true;
	return b;
}