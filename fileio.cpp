#include"common.h"

#include<iostream>
#include<fstream>

int filever;	//バージョン 0:0.8 1:0.84 2:0.85
const char *verstr[] = { "Ast0.8","Ast0.84","Ast0.85"};


extern HWND hMainWindow;

extern Data main_data;


namespace Place
{
	extern Module NewModule;
}

namespace FileIO
{
	extern char szFile[MAX_PATH];
	extern char szFileTitle[MAX_PATH];
}


void GetFileVer(std::ifstream &f)
{
	char s[SIZE_OF_LABEL_NAME];
	f.getline(s, SIZE_OF_LABEL_NAME);
	filever = -1;
	for (int i = 0; i < 3; i++)if (strcmp(s, verstr[i])==0)filever = i;
}

std::istream &operator>>(std::istream &stream,vector2D &v)
{
	stream >> v.x >> v.y;
	return stream;
}

std::ostream &operator<<(std::ostream &stream,vector2D v)
{
	stream << v.x << " " << v.y << " ";
	return stream;
}

std::istream &operator>>(std::istream &stream,Comment &l)
{
	if (filever >= 2)
		stream >> (int&)l.d;
	else
		l.d = CLOCKWISE_000;
	stream >> l.pos;
	stream.get();
	stream.getline(l.txt,SIZE_OF_LABEL_NAME);
	return stream;
}

std::ostream &operator<<(std::ostream &stream,Comment l)
{
	stream << l.d << " ";
	stream << l.pos;
	stream << l.txt;
	stream << "\n";
	return stream;
}

std::istream &operator>>(std::istream &stream,Label &l)
{
	stream >> (int&)l.d;
	stream >> l.node_index;
	stream.get();
	stream.getline(l.nam,SIZE_OF_LABEL_NAME);
	return stream;
}

std::ostream &operator<<(std::ostream &stream,Label l)
{
	stream << l.d << " ";
	stream << l.node_index << " ";
	stream << l.nam;
	stream << "\n";
	return stream;
}

template<class X> std::istream &operator>>(std::istream &stream,std::vector<X> &x)
{
	unsigned int size;
	stream >> size;
	for(unsigned int i=0;i<size;i++)
	{
		X tmp;
		stream >> tmp;
		x.push_back(tmp);
	}
	return stream;
}

template<class X> std::ostream &operator<<(std::ostream &stream,std::vector<X> &x)
{
	stream << x.size() << " ";
	for(unsigned int i=0;i<x.size();i++) stream << x[i] << " ";
	return stream;
}

std::istream &operator>>(std::istream &stream,LabelIn &l)
{
	stream >> l.state;
	stream >> (Label&)l;
	return stream;
}

std::ostream &operator<<(std::ostream &stream,LabelIn l)
{
	stream << l.state << " ";
	stream << (Label)l;
	return stream;
}

std::istream &operator>>(std::istream &stream,DeviceIndex &sd)
{
	stream >> (int&)sd.ConnectedDevClass;
	stream >> sd.Index;
	stream >> sd.SubIndex;
	return stream;
}

std::ostream &operator<<(std::ostream &stream,DeviceIndex sd)
{
	stream << sd.ConnectedDevClass << " ";
	stream << sd.Index << " ";
	stream << sd.SubIndex << " ";
	return stream;
}

std::istream &operator>>(std::istream &stream,LabelOut &l)
{
	stream >> l.sd;
	stream >> (Label&)l;
	return stream;
}

std::ostream &operator<<(std::ostream &stream,LabelOut l)
{
	stream << l.sd;
	stream << (Label)l;
	return stream;
}

std::istream &operator>>(std::istream &stream,BusIn &b)
{
	stream >> b.state;
	stream >> (int&)b.d;
	stream >> b.node_in;
	stream >> b.node_out;
	return stream;
}

std::ostream &operator<<(std::ostream &stream,BusIn b)
{
	stream << b.state << " ";
	stream << b.d << " ";
	stream << b.node_in << " ";
	stream << b.node_out << " ";
	return stream;
}

std::istream &operator>>(std::istream &stream,BusOut &b)
{
	stream >> b.state;
	stream >> (int&)b.d;
	stream >> b.array_in;
	stream >> b.node_out;
	return stream;
}

std::ostream &operator<<(std::ostream &stream,BusOut b)
{
	stream << b.state << " ";
	stream << b.d << " ";
	stream << b.array_in << " ";
	stream << b.node_out << " ";
	return stream;
}

std::istream &operator>>(std::istream &stream,Gate &g)
{

	stream >> g.state;
	if (filever >= 2)
	{
		stream >> (int&)g.invert;
		stream >> (int&)g.isOr;
	}
	else
	{
		//論理演算(旧分類方法)
		enum Operation { NOT, OR, AND, NOR, NAND };
		int op;
		stream >> op;
		switch (op)
		{
		case NOT:
			g.invert = true;
			g.isOr = false;
			break;
		case OR:
			g.invert = false;
			g.isOr = true;
			break;
		case AND:
			g.invert = false;
			g.isOr = false;
			break;
		case NOR:
			g.invert = true;
			g.isOr = true;
			break;
		case NAND:
			g.invert = true;
			g.isOr = false;
			break;
		}
	}
	stream >> (int&)g.d;
	stream >> g.node_out;
	unsigned int size;
	stream >> size;
	if(!stream.good()) return stream;
	for(unsigned int i=0;i<size;i++)
	{
		SourceData gin;
		stream >> gin;
		g.array_in.push_back(gin);
	}
	return stream;
}

std::ostream &operator<<(std::ostream &stream,Gate g)
{
	stream << g.state << " ";
	stream << g.invert << " ";
	stream << g.isOr << " ";
	stream << g.d << "  ";
	stream << g.node_out << "  ";
	stream << g.array_in.size() << "  ";
	for(unsigned int i=0;i<g.array_in.size();i++) stream << g.array_in[i] << " ";
	stream << "\n";
	return stream;
}

std::istream &operator>>(std::istream &stream,Conductor &c)
{
	stream >> c.buswidth;
	stream >> c.Multipath;
	stream >> c.Short;
	stream >> (int&)c.d;
	stream >> c.nod[0];
	stream >> c.nod[1];
	return stream;
}

std::ostream &operator<<(std::ostream &stream,Conductor c)
{
	stream << c.buswidth << " ";
	stream << c.Multipath << " ";
	stream << c.Short << " ";
	stream << c.d << " ";
	stream << c.nod[0] << " ";
	stream << c.nod[1] << " ";
	stream << "\n";
	return stream;
}

std::istream &operator>>(std::istream &stream,Node &n)
{
	stream >> n.buswidth;
	stream >> n.pos;
	stream >> n.Multipath;
	stream >> n.Short;
	stream >> n.AllRight;
	n.path=false;
	for(int i=0;i<8;i++) stream >> n.branch[i];
	return stream;
}

std::ostream &operator<<(std::ostream &stream,Node n)
{
	stream << n.buswidth << " ";
	stream << n.pos << " ";
	stream << n.Multipath << " ";
	stream << n.Short << " ";
	stream << n.AllRight << " ";
	for(int i=0;i<8;i++) stream << n.branch[i];
	stream << "\n";
	return stream;
}

std::istream &operator>>(std::istream &stream,SourceData &in)
{
	stream >> in.node_in;
	stream >> in.sd;
	return stream;
}

std::ostream &operator<<(std::ostream &stream,SourceData in)
{
	stream << in.node_in << " ";
	stream << in.sd;
	return stream;
}


std::istream &operator>>(std::istream &stream,ModuleTerminal &mt)
{
	stream >> mt.di;
	stream >> mt.nWire;
	return stream;
}

std::ostream &operator<<(std::ostream &stream,ModuleTerminal mt)
{
	stream << mt.di << " ";
	stream << mt.nWire << " ";
	return stream;
}

std::istream &operator>>(std::istream &stream,Module &m)
{
	unsigned int size;

	if (filever >= 1)
		stream >> (int&)m.d;
	else
		m.d = CLOCKWISE_000;

	stream >> m.center;
	stream >> m.internal_half_width;
	stream >> m.internal_half_height;
	stream >> m.margin[3];
	stream >> m.margin[0];
	stream >> m.margin[1];
	stream >> m.margin[2];

	stream >> size;
	if(!stream.good()) return stream;
	for(unsigned int i=0;i<size;i++)
	{
		ModuleTerminal mt;
		stream >> mt;
		m.terminal[3].push_back(mt);
	}

	stream >> size;
	if(!stream.good()) return stream;
	for(unsigned int i=0;i<size;i++)
	{
		ModuleTerminal mt;
		stream >> mt;
		m.terminal[0].push_back(mt);
	}

	stream >> size;
	if(!stream.good()) return stream;
	for(unsigned int i=0;i<size;i++)
	{
		ModuleTerminal mt;
		stream >> mt;
		m.terminal[1].push_back(mt);
	}

	stream >> size;
	if(!stream.good()) return stream;
	for(unsigned int i=0;i<size;i++)
	{
		ModuleTerminal mt;
		stream >> mt;
		m.terminal[2].push_back(mt);
	}

	stream >> size;
	if(!stream.good()) return stream;
	for(unsigned int i=0;i<size;i++)
	{
		int j;
		stream >> j;
		m.node_out.push_back(j);
	}

	stream >> size;
	if(!stream.good()) return stream;
	for(unsigned int i=0;i<size;i++)
	{
		SourceData in;
		stream >> in;
		m.array_in.push_back(in);
	}

	stream >> (Data&)m;

	return stream;
}

std::ostream &operator<<(std::ostream &stream,Module m)
{
	stream << m.d << " ";
	stream << m.center << "  ";
	stream << m.internal_half_width << " ";
	stream << m.internal_half_height << " ";
	stream << m.margin[3] << " ";
	stream << m.margin[0] << " ";
	stream << m.margin[1] << " ";
	stream << m.margin[2] << " ";
	stream << "\n";

	stream << m.terminal[3].size() << "\n";
	for(unsigned int i=0;i<m.terminal[3].size();i++) stream << m.terminal[3][i];
	stream << "\n";
	stream << m.terminal[0].size() << "\n";
	for(unsigned int i=0;i<m.terminal[0].size();i++) stream << m.terminal[0][i];
	stream << "\n";
	stream << m.terminal[1].size() << "\n";
	for(unsigned int i=0;i<m.terminal[1].size();i++) stream << m.terminal[1][i];
	stream << "\n";
	stream << m.terminal[2].size() << "\n";
	for(unsigned int i=0;i<m.terminal[2].size();i++) stream << m.terminal[2][i];
	stream << "\n";

	stream << m.node_out.size() << "\n";
	for(unsigned int i=0;i<m.node_out.size();i++) stream << m.node_out[i] << " ";
	stream << "\n\n";
	stream << m.array_in.size() << "\n";
	for(unsigned int i=0;i<m.array_in.size();i++) stream << m.array_in[i] << " ";
	stream << "\n\n";

	stream << (Data)m;

	return stream;

}

std::istream &operator>>(std::istream &stream,Data &d)
{
	int size;

	stream >> size;
	if(!stream.good()) return stream;
	for(int i=0;i<size;i++)
	{
		Comment t(NO_DIRECTION,DirectionVector[NO_DIRECTION],"");
		stream >> t;
		d.note.push_back(t);
	}

	stream >> size;
	if(!stream.good()) return stream;
	for(int i=0;i<size;i++)
	{
		LabelIn l(NO_DIRECTION,-1,"");
		stream >> l;
		d.input.push_back(l);
	}

	stream >> size;
	if(!stream.good()) return stream;
	for(int i=0;i<size;i++)
	{
		LabelOut l(NO_DIRECTION,-1,"");
		stream >> l;
		d.output.push_back(l);
	}

	stream >> size;
	if(!stream.good()) return stream;
	for(int i=0;i<size;i++)
	{
		std::vector<int> v;
		SourceData sd;
		BusIn b(NO_DIRECTION,v,sd);
		stream >> b;
		d.busin.push_back(b);
	}

	stream >> size;
	if(!stream.good()) return stream;
	for(int i=0;i<size;i++)
	{
		std::vector<SourceData> in;
		BusOut b(NO_DIRECTION,-1,in);
		stream >> b;
		d.busout.push_back(b);
	}

	stream >> size;
	if(!stream.good()) return stream;
	for(int i=0;i<size;i++)
	{
		std::vector<SourceData> v;
		Gate g(false,false,NO_DIRECTION,-1,v);
		stream >> g;
		d.gate.push_back(g);
	}

	stream >> size;
	if(!stream.good()) return stream;
	for(int i=0;i<size;i++)
	{
		Conductor c(-1,-1,NO_DIRECTION);
		stream >> c;
		d.conductor.push_back(c);
	}

	stream >> size;
	if(!stream.good()) return stream;
	for(int i=0;i<size;i++)
	{
		Node n;
		stream >> n;
		d.node_data.push_back(n);
	}

	stream >> size;
	if(!stream.good()) return stream;
	for(int i=0;i<size;i++)
	{
		Module m;
		d.module.push_back(m);
		stream >> d.module[i];
	}

	return stream;
}

std::ostream &operator<<(std::ostream &stream,Data d)
{
	stream << d.note.size() << "\n\n";
	for(unsigned int i=0;i<d.note.size();i++) stream << d.note[i];
	stream << "\n\n";

	stream << d.input.size() <<"\n\n";
	for(unsigned int i=0;i<d.input.size();i++) stream << d.input[i];
	stream << "\n\n";

	stream << d.output.size() << "\n\n";
	for(unsigned int i=0;i<d.output.size();i++) stream << d.output[i];
	stream << "\n\n";

	stream << d.busin.size() << "\n\n";
	for(unsigned int i=0;i<d.busin.size();i++) stream << d.busin[i];
	stream << "\n\n";

	stream << d.busout.size() << "\n\n";
	for(unsigned int i=0;i<d.busout.size();i++) stream << d.busout[i];
	stream << "\n\n";

	stream << d.gate.size() << "\n\n";
	for(unsigned int i=0;i<d.gate.size();i++) stream << d.gate[i];
	stream << "\n\n";

	stream << d.conductor.size() << "\n\n";
	for(unsigned int i=0;i<d.conductor.size();i++) stream << d.conductor[i];
	stream << "\n\n";

	stream << d.node_data.size() << "\n\n";
	for(unsigned int i=0;i<d.node_data.size();i++) stream << d.node_data[i];
	stream << "\n\n";

	stream << d.module.size() << "\n\n";
	for(unsigned int i=0;i<d.module.size();i++) stream << d.module[i];
	stream << "\n\n";

	return stream;
}

bool SaveAs(Data d,char *szF,char *szFT)
{
	OPENFILENAME ofn;

	memset(&ofn,0,sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hMainWindow;
	ofn.lpstrFilter = TEXT("log(*.log)\0*.log\0\0");
	ofn.lpstrFile = szF;
	ofn.lpstrFileTitle = szFT;
	ofn.nFilterIndex = 1;
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = TEXT("log");
	ofn.lpstrTitle = TEXT("名前を付けて保存する");

	if(GetSaveFileName(&ofn) == 0) return false;

	std::ofstream f(szF);
	f << "Ast0.85" << std::endl << std::endl;
	f << d;
	f.close();
	return true;
}

bool Save()
{
	if(strlen(FileIO::szFile)==0) return SaveAs(main_data,FileIO::szFile,FileIO::szFileTitle);
	else
	{
		std::ofstream f(FileIO::szFile);
		if(f)
		{
			f << "Ast0.85" << std::endl << std::endl;
			f << main_data;
			f.close();
		}
		return true;
	}
}

bool LoadData()
{
	std::ifstream f(FileIO::szFile);
	if(f)
	{
		GetFileVer(f);
		if(filever==-1)
		{
			main_data.Clear();
			FileIO::szFile[0] = 0;
			FileIO::szFileTitle[0] = 0;
			MessageBox(hMainWindow, "扱えないファイルフォーマットのファイルです", "エラー", MB_OK);
			f.close();
			return false;
		}
		else
		{
			main_data.Clear();
			f >> main_data;
			if (!f.good())
			{
				main_data.Clear();
				FileIO::szFile[0] = 0;
				FileIO::szFileTitle[0] = 0;
				MessageBox(hMainWindow, "ファイルが壊れています", "エラー", MB_OK);
				f.close();
				return false;
			}
		}
		f.close();
	}
	return true;
}

bool Open()
{
	OPENFILENAME ofn;

	memset(&ofn,0,sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hMainWindow;
	ofn.lpstrFilter = TEXT("log(*.log)\0*.log\0\0");
	ofn.lpstrFile = FileIO::szFile;
	ofn.lpstrFileTitle = FileIO::szFileTitle;
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = TEXT("log");
	ofn.lpstrTitle = TEXT("ファイルを開く");

	if(GetOpenFileName(&ofn) == 0) return false;

	return LoadData();
}

void Module::Add(int j, ModuleTerminal di)
{
	terminal[j].push_back(di);
	for (int i = terminal[j].size() - 1; i > 0; i--)
	{
		int c;
		vector2D v1, v2;
		v1 = node(terminal[j][i].di.GetDeviceClass() == module_in ? input[terminal[j][i].di.GetDeviceIndex()].nodeindex() : output[terminal[j][i].di.GetDeviceIndex()].nodeindex()).position();
		v2 = node(terminal[j][i - 1].di.GetDeviceClass() == module_in ? input[terminal[j][i - 1].di.GetDeviceIndex()].nodeindex() : output[terminal[j][i - 1].di.GetDeviceIndex()].nodeindex()).position();
		switch (j)
		{
			case CLOCKWISE_000>>1:
				c = CompareY(v1, v2);
				if (c<0) return;
				else if (c == 0)
				{
					c = CompareX(v1, v2);
					if (c<0)return;
				}

				break;
			case CLOCKWISE_090>>1:
				c = CompareX(v1, v2);
				if (c>0) return;
				else if (c == 0)
				{
					c = CompareY(v1, v2);
					if (c<0)return;
				}
				break;
			case CLOCKWISE_180>>1:
				c = CompareY(v1, v2);
				if (c>0) return;
				else if (c == 0)
				{
					c = CompareX(v1, v2);
					if (c>0)return;
				}
				break;
			case CLOCKWISE_270 >> 1:
				c = CompareX(v1, v2);
				if (c<0) return;
				else if (c == 0)
				{
					c = CompareY(v1, v2);
					if (c>0)return;
				}
				break;
		}
		ModuleTerminal tmp;
		tmp = terminal[j][i];
		terminal[j][i] = terminal[j][i - 1];
		terminal[j][i - 1] = tmp;
	}

}


bool LoadModule()
{
	char szFile_Modul[MAX_PATH]="";
	char szFileTitle_Modul[MAX_PATH]="";
	OPENFILENAME ofn;

	memset(&ofn,0,sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hMainWindow;
	ofn.lpstrFilter = TEXT("log(*.log)\0*.log\0\0");
	ofn.lpstrFile = szFile_Modul;
	ofn.lpstrFileTitle = szFileTitle_Modul;
	ofn.nMaxFile = MAX_PATH;
	ofn.nMaxFileTitle = MAX_PATH;
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = TEXT("log");
	ofn.lpstrTitle = TEXT("モジュールをロード");

	if(GetOpenFileName(&ofn) == 0) return false;

	std::ifstream f(szFile_Modul);
	if(!f)return false;

	GetFileVer(f);
	if (filever == -1)
	{
		MessageBox(hMainWindow, "扱えないファイルフォーマットのファイルです", "エラー", MB_OK);
		f.close();
		return false;
	}
	else
	{
		Place::NewModule.Clear();
		f >> (Data&)Place::NewModule;
		if (!f.good())
		{
			Place::NewModule.Clear();
			MessageBox(hMainWindow, "ファイルが壊れています", "エラー", MB_OK);
			f.close();
			return false;
		}
	}
	f.close();

	if(Place::NewModule.nInput()==0 || Place::NewModule.nOutput()==0)
	{
		Place::NewModule.Clear();
		MessageBox(hMainWindow,"入力端子と出力端子がそれぞれ少なくとも1つ以上必要です","エラー",MB_OK);
		return false;
	}

	ModuleTerminal mt;

	mt.di.SetDeviceClass(module_in);
	for(unsigned int i=0;i<Place::NewModule.nInput();i++)
	{
		mt.nWire=Place::NewModule.GetInput(i).nWire();
		mt.di.SetIndexAndSubIndex(i,-1);
		Place::NewModule.Add(((Place::NewModule.GetInputDirection(i) >> 1) + 2) &3,mt);
	}

	mt.di.SetDeviceClass(module_out);
	for(unsigned int i=0;i<Place::NewModule.nOutput();i++)
	{
		mt.nWire=Place::NewModule.node(Place::NewModule.GetOutput(i).nodeindex()).nWire();
		mt.di.SetIndexAndSubIndex(i,-1);
		Place::NewModule.Add(((Place::NewModule.GetOutputDirection(i) >> 1) + 2) &3, mt);
	}

	Place::NewModule.Set_internal_half_length(Place::NewModule.GetLocalSize(3)>Place::NewModule.GetLocalSize(1) ? Place::NewModule.GetLocalSize(3) : Place::NewModule.GetLocalSize(1),Place::NewModule.GetLocalSize(0)>Place::NewModule.GetLocalSize(2) ? Place::NewModule.GetLocalSize(0) : Place::NewModule.GetLocalSize(2));

	unsigned int margin[4] = { 0,0,0,0 };
	for (int j = 0; j < 4; j++)
	{
		for (int i = 0; i<Place::NewModule.GetLocalSize(j); i++)
		{
			switch (Place::NewModule.GetLocal(j, i).GetDeviceClass())
			{
			case module_in:
				if (margin[j]<strlen(Place::NewModule.GetInput(Place::NewModule.GetLocal(j, i).GetDeviceIndex()).name())) margin[j] = strlen(Place::NewModule.GetInput(Place::NewModule.GetLocal(j, i).GetDeviceIndex()).name());
				break;
			case module_out:
				if (margin[j]<strlen(Place::NewModule.GetOutput(Place::NewModule.GetLocal(j, i).GetDeviceIndex()).name())) margin[j] = strlen(Place::NewModule.GetOutput(Place::NewModule.GetLocal(j, i).GetDeviceIndex()).name());
				break;
			}
		}
	}

	Place::NewModule.Set_margin(margin[3], margin[0], margin[1], margin[2]);

	return true;
}