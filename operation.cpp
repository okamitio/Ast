#include"common.h"

extern HWND hMainWindow;

extern Data main_data;

namespace Automatic
{
	extern bool Fast;
	extern bool Execute;
	extern bool ExecutingFastOperation;
	extern HANDLE hFastOperationThread;
	extern int ClockInLabel;
}

//マルチパスであるか調べる
bool Data::SearchMultipathNode(int n,Direction d)
{
	int i;
	node_data[n].SetPathFlag();
	for(i=0;i<d;i++)
		if(node_data[n].dClass((Direction)i)==lead_wire)
			if(SearchMultipathWire(node_data[n].index((Direction)i),(Direction)i))
			{
				node_data[n].ResetPathFlag();
				return true;
			}
	for(i++;i<8;i++)
		if(node_data[n].dClass((Direction)i)==lead_wire)
			if(SearchMultipathWire(node_data[n].index((Direction)i),(Direction)i))
			{
				node_data[n].ResetPathFlag();
				return true;
			}
	node_data[n].ResetPathFlag();
	return false;
}

bool Data::SearchMultipathWire(int c,Direction d)
{
	if(d==conductor[c].direction())
	{
		if(node_data[conductor[c].nodeindex1()].GetPathFlag())
			return true;
		else if(SearchMultipathNode(conductor[c].nodeindex1(),conductor[c].ReverseDirection()))
			return true;
	}
	else
	{
		if(node_data[conductor[c].nodeindex0()].GetPathFlag())
			return true;
		else if(SearchMultipathNode(conductor[c].nodeindex0(),conductor[c].direction()))
			return true;
	}
	return false;
}

//マルチパスを示すフラグを上書き
void Data::SetMultipathFlag(int i,int n,Direction d,bool flag)
{
	DeviceIndex di;
	di.SetDeviceClass(no_device),di.SetIndexAndSubIndex(-1,-1);
	switch(node_data[n].dClass((Direction)i))
	{
	case label_output:
		output[node_data[n].index((Direction)i)].SetSourceDevClass(no_device);
		output[node_data[n].index((Direction)i)].SetSourceDevIndex(-1,-1);
		break;
	case gate_in:
		gate[node_data[n].index((Direction)i)].SetSourceDevClass(node_data[n].subindex((Direction)i),no_device);
		gate[node_data[n].index((Direction)i)].SetSourceDevIndex(node_data[n].subindex((Direction)i),-1,-1);
		break;
	case module_in:
		module[node_data[n].index((Direction)i)].SetSourceDev(node_data[n].subindex((Direction)i),di);
		break;
	case lead_wire:
		SetMultipathFlagWire(node_data[n].index((Direction)i),(Direction)i,flag);
		break;
	}
}

void Data::SetMultipathFlagNode(int n,Direction d,bool flag)
{
	int i;
	node_data[n].SetPathFlag();
	node_data[n].SetMultipath(flag);
	if(flag==true)
	{
		node_data[n].SetShort(false);
		node_data[n].SetAllRight(false);
	}
	for(i=0;i<d;i++)
		SetMultipathFlag(i,n,d,flag);
	for(i++;i<8;i++)
		SetMultipathFlag(i,n,d,flag);
	node_data[n].ResetPathFlag();
}

void Data::SetMultipathFlagWire(int c,Direction d,bool flag)
{
	conductor[c].SetMultipath(flag);
	if(flag==true)conductor[c].SetShort(false);
	if(d==conductor[c].direction())
	{
		if(!node_data[conductor[c].nodeindex1()].GetPathFlag())
			SetMultipathFlagNode(conductor[c].nodeindex1(),conductor[c].ReverseDirection(),flag);
	}
	else
	{
		if(!node_data[conductor[c].nodeindex0()].GetPathFlag())
			SetMultipathFlagNode(conductor[c].nodeindex0(),conductor[c].direction(),flag);
	}
}

//競合している出力を検索
int Data::SearchNumberOfOutputNode(int n,Direction d)
{
	int i;
	int count=0;
	for(i=0;i<d;i++)
	{
		switch(node_data[n].dClass((Direction)i))
		{
		case bus_in_out:
			count++;
			break;
		case bus_out_out:
			count++;
			break;
		case label_input:
			count++;
			break;
		case gate_out:
			count++;
			break;
		case module_out:
			count++;
			break;
		case lead_wire:
			count+=SearchNumberOfOutputWire(node_data[n].index((Direction)i),(Direction)i);
			break;
		}
	}
	for(i++;i<8;i++)
	{
		switch(node_data[n].dClass((Direction)i))
		{
		case bus_in_out:
			count++;
			break;
		case bus_out_out:
			count++;
			break;
		case label_input:
			count++;
			break;
		case gate_out:
			count++;
			break;
		case module_out:
			count++;
			break;
		case lead_wire:
			count+=SearchNumberOfOutputWire(node_data[n].index((Direction)i),(Direction)i);
			break;
		}
	}
	return count;
}

int Data::SearchNumberOfOutputWire(int c,Direction d)
{
	if(d==conductor[c].direction())
		return SearchNumberOfOutputNode(conductor[c].nodeindex1(),conductor[c].ReverseDirection());
	else
		return SearchNumberOfOutputNode(conductor[c].nodeindex0(),conductor[c].direction());
}

//競合(短絡)しているかどうかを表すフラグを上書き
void Data::SetShortFlagNode(int n,Direction d,bool flag)
{
	int i;
	node_data[n].SetShort(flag);
	if(flag)node_data[n].SetAllRight(false);
	for(i=0;i<d;i++)
	{
		if(node_data[n].dClass((Direction)i)==lead_wire)
			SetShortFlagWire(node_data[n].index((Direction)i),(Direction)i,flag);
	}
	for(i++;i<8;i++)
	{
		if(node_data[n].dClass((Direction)i)==lead_wire)
			SetShortFlagWire(node_data[n].index((Direction)i),(Direction)i,flag);
	}
}

void Data::SetShortFlagWire(int c,Direction d,bool flag)
{
	conductor[c].SetShort(flag);

	if(d==conductor[c].direction())
		SetShortFlagNode(conductor[c].nodeindex1(),conductor[c].ReverseDirection(),flag);
	else
		SetShortFlagNode(conductor[c].nodeindex0(),conductor[c].direction(),flag);
}

//導線の電位を決定しているデバイスを与える
DeviceIndex Data::GetSource(int i,int n,Direction d)
{
	DeviceIndex sd;
	switch(node_data[n].dClass((Direction)i))
	{
	case bus_in_out:
		sd.SetDeviceClass(bus_in_out);
		sd.SetIndexAndSubIndex(node_data[n].index((Direction)i),node_data[n].subindex((Direction)i));
		return sd;
	case bus_out_out:
		sd.SetDeviceClass(bus_out_out);
		sd.SetIndexAndSubIndex(node_data[n].index((Direction)i),node_data[n].subindex((Direction)i));
		return sd;
	case label_input:
		sd.SetDeviceClass(label_input);
		sd.SetIndexAndSubIndex(node_data[n].index((Direction)i),-1);
		return sd;
	case gate_out:
		sd.SetDeviceClass(gate_out);
		sd.SetIndexAndSubIndex(node_data[n].index((Direction)i),-1);
		return sd;
	case module_out:
		sd.SetDeviceClass(module_out);
		sd.SetIndexAndSubIndex(node_data[n].index((Direction)i),node_data[n].subindex((Direction)i));
		return sd;
	case lead_wire:
		sd=GetSourceWire(node_data[n].index((Direction)i),(Direction)i);
		if(sd.GetDeviceClass()!=no_device)return sd;
		break;
	}
	return sd;
}

DeviceIndex Data::GetSourceNode(int n,Direction d)
{
	DeviceIndex sd;
	int i;
	for(i=0;i<d;i++)
	{
		sd=GetSource(i,n,d);
		if(sd.GetDeviceClass()!=no_device) return sd;
	}
	for(i++;i<8;i++)
	{
		sd=GetSource(i,n,d);
		if(sd.GetDeviceClass()!=no_device) return sd;
	}
	return sd;
}

DeviceIndex Data::GetSourceWire(int c,Direction d)
{
	if(d==conductor[c].direction())
		return GetSourceNode(conductor[c].nodeindex1(),conductor[c].ReverseDirection());
	else
		return GetSourceNode(conductor[c].nodeindex0(),conductor[c].direction());
}

void Data::SetSourceModule(int n,bool newstate,int wirenumber)
{
	if(input[n].GetState(wirenumber)!=newstate)
	{
		input[n].SetState(newstate,wirenumber);
		if(node_data[input[n].nodeindex()].isAllRight())
		{
			DeviceIndex sd;
			sd.SetDeviceClass(label_input);
			sd.SetIndexAndSubIndex(n,-1);
			SetSourceNode(input[n].nodeindex(),NO_DIRECTION,sd,wirenumber);
		}
	}
}

struct gate_phase
{
	Data* module;
	int index;
	bool newstate;
	int wirenumber;
};

bool selector=false;
std::vector<gate_phase> frontier[2];

//導線につながっているデバイスに電位を決定しているデバイスの情報を与える
void Data::SetSource(int n,DeviceIndex sd,int i,int wirenumber)
{
	int SelectedDeviceIndex=node_data[n].index((Direction)i);
	bool newstate=false;
	switch(node_data[n].dClass((Direction)i))
	{
	case bus_in_in:
		busin[SelectedDeviceIndex].SetSourceDev(sd);
		switch(sd.GetDeviceClass())
		{
		case label_input:
			newstate=input[sd.GetDeviceIndex()].GetState(wirenumber);
			break;
		case gate_out:
			newstate=gate[sd.GetDeviceIndex()].GetState(wirenumber);
			break;
		case module_out:
			newstate=module[sd.GetDeviceIndex()].GetState(sd.GetDeviceSubIndex(),wirenumber);
			break;
		case bus_in_out:
			newstate=busin[sd.GetDeviceIndex()].GetState(sd.GetDeviceSubIndex());
			break;
		case bus_out_out:
			newstate=busout[sd.GetDeviceIndex()].GetState(wirenumber);
			break;
		}
		if(newstate!=busin[SelectedDeviceIndex].GetState(wirenumber))
		{
			if(node(busin[SelectedDeviceIndex].NodeOutIndex(wirenumber)).isAllRight())
			{
				DeviceIndex nextsd;
				busin[SelectedDeviceIndex].SetState(newstate,wirenumber);
				nextsd.SetDeviceClass(bus_in_out);
				nextsd.SetIndexAndSubIndex(SelectedDeviceIndex,wirenumber);
				SetSourceNode(busin[SelectedDeviceIndex].NodeOutIndex(wirenumber),NO_DIRECTION,nextsd,0);
			}
		}
		break;
	case bus_out_in:
		busout[SelectedDeviceIndex].SetSourceDev(node_data[n].subindex((Direction)i),sd);
		switch(sd.GetDeviceClass())
		{
		case label_input:
			newstate=input[sd.GetDeviceIndex()].GetState(0);
			break;
		case gate_out:
			newstate=gate[sd.GetDeviceIndex()].GetState(0);
			break;
		case module_out:
			newstate=module[sd.GetDeviceIndex()].GetState(sd.GetDeviceSubIndex(),0);
			break;
		case bus_in_out:
			newstate=busin[sd.GetDeviceIndex()].GetState(sd.GetDeviceSubIndex());
			break;
		case bus_out_out:
			newstate=busout[sd.GetDeviceIndex()].GetState(0);
			break;
		}
		if(newstate!=busout[SelectedDeviceIndex].GetState(node_data[n].subindex((Direction)i)))
		{
			if(node(busout[SelectedDeviceIndex].NodeOutIndex()).isAllRight())
			{
				DeviceIndex nextsd;
				busout[SelectedDeviceIndex].SetState(newstate,node_data[n].subindex((Direction)i));
				nextsd.SetDeviceClass(bus_out_out);
				nextsd.SetIndexAndSubIndex(SelectedDeviceIndex,node_data[n].subindex((Direction)i));
				SetSourceNode(busout[SelectedDeviceIndex].NodeOutIndex(),NO_DIRECTION,nextsd,node_data[n].subindex((Direction)i));
			}
		}
		break;
	case label_output:
		output[SelectedDeviceIndex].SetSourceDev(sd);
		if(this->GetParent()!=NULL)//モジュール内ならば出力
		{
			DeviceIndex nextsd;
			nextsd.SetDeviceClass(module_out);
			Module *mdl=(Module*)((Module*)this)->GetParent();
			if(mdl->node(((Module*)this)->NodeOut(SelectedDeviceIndex)).isAllRight())
			{
				nextsd.SetIndexAndSubIndex(mdl->node(((Module*)this)->NodeOut(SelectedDeviceIndex)).index(ReverseDirection((Direction)((i+((Module*)this)->direction())&6))),mdl->node(((Module*)this)->NodeOut(SelectedDeviceIndex)).subindex(ReverseDirection((Direction)((i + ((Module*)this)->direction()) & 6))));
				mdl->SetSourceNode(((Module*)this)->NodeOut(SelectedDeviceIndex),NO_DIRECTION,nextsd,wirenumber);
			}
		}
		break;
	case gate_in:
		gate[SelectedDeviceIndex].SetSourceDev(node_data[n].subindex((Direction)i),sd);
		if (gate[SelectedDeviceIndex].isOR())
		{
			int j;
			for (j = 0; j < gate[SelectedDeviceIndex].nIn(); j++)
			{
				if (gate[SelectedDeviceIndex].GetSourceDevClass(j) == label_input)
					if (input[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(wirenumber))break;
				if (gate[SelectedDeviceIndex].GetSourceDevClass(j) == bus_in_out)
					if (busin[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(gate[SelectedDeviceIndex].GetSourceDevSubIndex(j)))break;
				if (gate[SelectedDeviceIndex].GetSourceDevClass(j) == bus_out_out)
					if (busout[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(wirenumber))break;
				if (gate[SelectedDeviceIndex].GetSourceDevClass(j) == gate_out)
					if (gate[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(wirenumber))break;
				if (gate[SelectedDeviceIndex].GetSourceDevClass(j) == module_out)
					if (module[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(gate[SelectedDeviceIndex].GetSourceDevSubIndex(j), wirenumber))break;
			}
			newstate = ((j < gate[SelectedDeviceIndex].nIn())!=gate[SelectedDeviceIndex].isNOT());
		}
		else
		{
			int j;
			for (j = 0; j<gate[SelectedDeviceIndex].nIn(); j++)
			{
				if (gate[SelectedDeviceIndex].GetSourceDevClass(j) == label_input)
					if (!input[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(wirenumber))break;
				if (gate[SelectedDeviceIndex].GetSourceDevClass(j) == bus_in_out)
					if (!busin[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(gate[SelectedDeviceIndex].GetSourceDevSubIndex(j)))break;
				if (gate[SelectedDeviceIndex].GetSourceDevClass(j) == bus_out_out)
					if (!busout[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(wirenumber))break;
				if (gate[SelectedDeviceIndex].GetSourceDevClass(j) == gate_out)
					if (!gate[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(wirenumber))break;
				if (gate[SelectedDeviceIndex].GetSourceDevClass(j) == module_out)
					if (!module[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(gate[SelectedDeviceIndex].GetSourceDevSubIndex(j), wirenumber))break;
			}
			newstate = ((j == gate[SelectedDeviceIndex].nIn())!=gate[SelectedDeviceIndex].isNOT());
		}
/*		switch(gate[SelectedDeviceIndex].operation())
		{
			int j;
		case NOT:
			if(gate[SelectedDeviceIndex].GetSourceDevClass(0)==label_input)
				newstate=(!input[gate[SelectedDeviceIndex].GetSourceDevIndex(0)].GetState(wirenumber));
			else if(gate[SelectedDeviceIndex].GetSourceDevClass(0)==bus_in_out)
				newstate=(!busin[gate[SelectedDeviceIndex].GetSourceDevIndex(0)].GetState(gate[SelectedDeviceIndex].GetSourceDevSubIndex(0)));
			else if(gate[SelectedDeviceIndex].GetSourceDevClass(0)==bus_out_out)
				newstate=(!busout[gate[SelectedDeviceIndex].GetSourceDevIndex(0)].GetState(wirenumber));
			else if(gate[SelectedDeviceIndex].GetSourceDevClass(0)==gate_out)
				newstate=(!gate[gate[SelectedDeviceIndex].GetSourceDevIndex(0)].GetState(wirenumber));
			else if(gate[SelectedDeviceIndex].GetSourceDevClass(0)==module_out)
				newstate=(!module[gate[SelectedDeviceIndex].GetSourceDevIndex(0)].GetState(gate[SelectedDeviceIndex].GetSourceDevSubIndex(0),wirenumber));
			else
				newstate=gate[SelectedDeviceIndex].GetState(wirenumber);
			break;
		case OR:
			for(j=0;j<gate[SelectedDeviceIndex].nIn();j++)
			{
				if(gate[SelectedDeviceIndex].GetSourceDevClass(j)==label_input)
					if(input[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(wirenumber))break;
				if(gate[SelectedDeviceIndex].GetSourceDevClass(j)==bus_in_out)
					if(busin[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(gate[SelectedDeviceIndex].GetSourceDevSubIndex(j)))break;
				if(gate[SelectedDeviceIndex].GetSourceDevClass(j)==bus_out_out)
					if(busout[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(wirenumber))break;
				if(gate[SelectedDeviceIndex].GetSourceDevClass(j)==gate_out)
					if(gate[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(wirenumber))break;
				if(gate[SelectedDeviceIndex].GetSourceDevClass(j)==module_out)
					if(module[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(gate[SelectedDeviceIndex].GetSourceDevSubIndex(j),wirenumber))break;
			}
			newstate=(j<gate[SelectedDeviceIndex].nIn());
			break;
		case AND:
			for(j=0;j<gate[SelectedDeviceIndex].nIn();j++)
			{
				if(gate[SelectedDeviceIndex].GetSourceDevClass(j)==label_input)
					if(!input[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(wirenumber))break;
				if(gate[SelectedDeviceIndex].GetSourceDevClass(j)==bus_in_out)
					if(!busin[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(gate[SelectedDeviceIndex].GetSourceDevSubIndex(j)))break;
				if(gate[SelectedDeviceIndex].GetSourceDevClass(j)==bus_out_out)
					if(!busout[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(wirenumber))break;
				if(gate[SelectedDeviceIndex].GetSourceDevClass(j)==gate_out)
					if(!gate[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(wirenumber))break;
				if(gate[SelectedDeviceIndex].GetSourceDevClass(j)==module_out)
					if(!module[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(gate[SelectedDeviceIndex].GetSourceDevSubIndex(j),wirenumber))break;
			}
			newstate=(j==gate[SelectedDeviceIndex].nIn());
			break;
		case NOR:
			for(j=0;j<gate[SelectedDeviceIndex].nIn();j++)
			{
				if(gate[SelectedDeviceIndex].GetSourceDevClass(j)==label_input)
					if(input[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(wirenumber))break;
				if(gate[SelectedDeviceIndex].GetSourceDevClass(j)==bus_in_out)
					if(busin[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(gate[SelectedDeviceIndex].GetSourceDevSubIndex(j)))break;
				if(gate[SelectedDeviceIndex].GetSourceDevClass(j)==bus_out_out)
					if(busout[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(wirenumber))break;
				if(gate[SelectedDeviceIndex].GetSourceDevClass(j)==gate_out)
					if(gate[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(wirenumber))break;
				if(gate[SelectedDeviceIndex].GetSourceDevClass(j)==module_out)
					if(module[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(gate[SelectedDeviceIndex].GetSourceDevSubIndex(j),wirenumber))break;
			}
			newstate=(j==gate[SelectedDeviceIndex].nIn());
			break;
		case NAND:
			for(j=0;j<gate[SelectedDeviceIndex].nIn();j++)
			{
				if(gate[SelectedDeviceIndex].GetSourceDevClass(j)==label_input)
					if(!input[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(wirenumber))break;
				if(gate[SelectedDeviceIndex].GetSourceDevClass(j)==bus_in_out)
					if(!busin[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(gate[SelectedDeviceIndex].GetSourceDevSubIndex(j)))break;
				if(gate[SelectedDeviceIndex].GetSourceDevClass(j)==bus_out_out)
					if(!busout[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(wirenumber))break;
				if(gate[SelectedDeviceIndex].GetSourceDevClass(j)==gate_out)
					if(!gate[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(wirenumber))break;
				if(gate[SelectedDeviceIndex].GetSourceDevClass(j)==module_out)
					if(!module[gate[SelectedDeviceIndex].GetSourceDevIndex(j)].GetState(gate[SelectedDeviceIndex].GetSourceDevSubIndex(j),wirenumber))break;
			}
			newstate=(j<gate[SelectedDeviceIndex].nIn());
			break;
		}
		*/
		if(newstate!=gate[SelectedDeviceIndex].GetState(wirenumber))
		{
			unsigned int j;
			for(j=0;j<frontier[selector].size();j++)
				if(frontier[selector][j].module==this && frontier[selector][j].index==SelectedDeviceIndex && frontier[selector][j].wirenumber==wirenumber)break;
			if(j==frontier[selector].size())
			{
				gate_phase p;
				p.index=SelectedDeviceIndex;
				p.module=this;
				p.newstate=newstate;
				p.wirenumber=wirenumber;
				frontier[selector].push_back(p);
			}
		}
		else
		{
			unsigned int j;
			for(j=0;j<frontier[selector].size();j++)
				if(frontier[selector][j].module==this && frontier[selector][j].index==SelectedDeviceIndex && frontier[selector][j].wirenumber==wirenumber)break;
			if(j<frontier[selector].size())
				Delete(frontier[selector],j);
		}
		break;
	case module_in:
		module[SelectedDeviceIndex].SetSourceDev(node_data[n].subindex((Direction)i),sd);
		switch(sd.GetDeviceClass())
		{
		case label_input:
			newstate=input[sd.GetDeviceIndex()].GetState(wirenumber);
			break;
		case bus_in_out:
			newstate=busin[sd.GetDeviceIndex()].GetState(sd.GetDeviceSubIndex());
			break;
		case bus_out_out:
			newstate=busout[sd.GetDeviceIndex()].GetState(wirenumber);
			break;
		case gate_out:
			newstate=gate[sd.GetDeviceIndex()].GetState(wirenumber);
			break;
		case module_out:
			newstate=module[sd.GetDeviceIndex()].GetState(sd.GetDeviceSubIndex(),wirenumber);
			break;
		}
		if(sd.GetDeviceClass()!=no_device)
			module[SelectedDeviceIndex].SetSourceModule(node_data[n].subindex((Direction)i),newstate,wirenumber);
		break;
	case lead_wire:
		SetSourceWire(SelectedDeviceIndex,(Direction)i,sd,wirenumber);
		break;
	}
}

void Data::SetSourceNode(int n,Direction d,DeviceIndex sd,int wirenumber)
{
	int i;
	node_data[n].SetAllRight(sd.GetDeviceClass()!=no_device);
	for(i=0;i<d;i++)
		SetSource(n,sd,i,wirenumber);
	for(i++;i<8;i++)
		SetSource(n,sd,i,wirenumber);
}

void Data::SetSourceWire(int c,Direction d,DeviceIndex sd,int wirenumber)
{
	if(d==conductor[c].direction())
		return SetSourceNode(conductor[c].nodeindex1(),conductor[c].ReverseDirection(),sd,wirenumber);
	else
		return SetSourceNode(conductor[c].nodeindex0(),conductor[c].direction(),sd,wirenumber);
}

void Data::InitModuleParentPointer()
{
	for(unsigned int i=0;i<module.size();i++)
	{
		module[i].SetParent(this);
		module[i].InitModuleParentPointer();
	}
}

void Data::Operate(int n,DeviceIndex sd,int wirenumber)
{
	const int MAX_GATE_PATH=1024;
	int j;
	frontier[selector].clear();
	SetSourceNode(n,NO_DIRECTION,sd,wirenumber);
	if(frontier[selector].size()==0) return;

	for(unsigned int i=0;i<frontier[selector].size();i++)
		frontier[selector][i].module->gate[frontier[selector][i].index].SetState(frontier[selector][i].newstate,frontier[selector][i].wirenumber);

	for(j=1;j<MAX_GATE_PATH;j++)
	{
		selector=!selector;
		frontier[selector].clear();
		for(unsigned int i=0;i<frontier[!selector].size();i++)
		{
			if(frontier[!selector][i].module->node_data[frontier[!selector][i].module->gate[frontier[!selector][i].index].NodeOutIndex()].isAllRight())
			{
				DeviceIndex nextsd;
				nextsd.SetDeviceClass(gate_out);
				nextsd.SetIndexAndSubIndex(frontier[!selector][i].index,-1);
				frontier[!selector][i].module->SetSourceNode(frontier[!selector][i].module->gate[frontier[!selector][i].index].NodeOutIndex(),NO_DIRECTION,nextsd,frontier[!selector][i].wirenumber);
			}
		}
		for(unsigned int i=0;i<frontier[selector].size();i++)
			frontier[selector][i].module->gate[frontier[selector][i].index].SetState(frontier[selector][i].newstate,frontier[selector][i].wirenumber);
		if(frontier[selector].size()==0)break;
	}

	if(j==MAX_GATE_PATH)
	{
		Automatic::Fast=false;
		Automatic::Execute=false;
		Automatic::ClockInLabel=-1;
		MessageBox(hMainWindow,"発振","報告",MB_OK);
	}
}

void Data::DetectErrorAndConnect(unsigned int n,int wirenumber)
{
	if(SearchMultipathNode(n,NO_DIRECTION)) //等電位の導線が複数の経路を取っていないか
	{
		SetMultipathFlagNode(n,NO_DIRECTION,true);	//マルチパスのフラグを立てる
	}
	else
	{
		int noon;
		SetMultipathFlagNode(n,NO_DIRECTION,false);	//マルチパスのフラグを下ろす

		noon=SearchNumberOfOutputNode(n,NO_DIRECTION);	//いくつの出力が競合しているか調べる
		if(noon>1)
			SetShortFlagNode(n,NO_DIRECTION,true); //短絡を表すフラグを立てる
		else
		{
			SetShortFlagNode(n,NO_DIRECTION,false);//正常
			if(noon==1)
			{
				DeviceIndex sd;
				sd=GetSourceNode(n,NO_DIRECTION);
				InitModuleParentPointer();
				Operate(n,sd,wirenumber);
			}
			else
			{
				DeviceIndex sd;
				sd.SetDeviceClass(no_device);
				sd.SetIndexAndSubIndex(-1,-1);
				InitModuleParentPointer();
				Operate(n,sd,wirenumber);
			}
		}
	}
}

bool Module::GetState(int i,int wirenumber)
{
	switch(GetOutput(i).GetSourceDevClass())
	{
		case label_input:
			return input[output[i].GetSourceDevIndex()].GetState(wirenumber);
			break;
		case bus_in_out:
			return busin[output[i].GetSourceDevIndex()].GetState(output[i].GetSourceDevSubIndex());
			break;
		case bus_out_out:
			return busout[output[i].GetSourceDevIndex()].GetState(wirenumber);
			break;
		case gate_out:
			return gate[output[i].GetSourceDevIndex()].GetState(wirenumber);
			break;
		case module_out:
			return module[output[i].GetSourceDevIndex()].GetState(output[i].GetSourceDevSubIndex(),wirenumber);
			break;
	}
	return 0;
}