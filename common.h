#include<windows.h>
#include<vector>

//状態
enum State{scroll,range,range_selected,paste,placegate,placemodule,expandmodule,placewire,placein,placeout,placebusin,placebusout,erase,note};

//8方位と方向無し
enum Direction{CLOCKWISE_000,CLOCKWISE_045,CLOCKWISE_090,CLOCKWISE_135,CLOCKWISE_180,CLOCKWISE_225,CLOCKWISE_270,CLOCKWISE_315,NO_DIRECTION};

//ある方向の反対の方向を与える
inline Direction ReverseDirection(Direction d)
{
	return Direction((d+4)&7);
}


//GDIのPOINT構造体はピクセル単位
//vector2DはGrid::Intervalを掛けてPOINT構造体になる
class vector2D
{
	int x;
	int y;
public:
	vector2D(){x=0,y=0;}
	vector2D(int a,int b){x=a,y=b;}

	int Getx(){return x;}
	int Gety(){return y;}

	//Grid::Intervalを掛けてPOINTに変換
	POINT MulGridInterval();
	POINT toPOINT();
	bool operator==(vector2D a);
	bool operator!=(vector2D a);
	void operator+=(vector2D a);
	vector2D operator+(vector2D a);
	vector2D operator-(vector2D a);
	vector2D operator*(vector2D a);	//複素数とみなして乗算
	friend vector2D operator*(const vector2D v,int n);
	friend Direction AlignedDirection(vector2D center,vector2D end);
	friend vector2D AlignAngle(vector2D center,vector2D end);

	friend int CompareX(vector2D v1,vector2D v2){return v1.x-v2.x;}
	friend int CompareY(vector2D v1,vector2D v2){return v1.y-v2.y;}

	friend std::istream &operator>>(std::istream &stream,vector2D &v);
	friend std::ostream &operator<<(std::ostream &stream,vector2D v);
};

//8方位へ向かう方向ベクトルと零ベクトル
const vector2D DirectionVector[9]={vector2D(1,0),vector2D(1,1),vector2D(0,1),vector2D(-1,1),vector2D(-1,0),vector2D(-1,-1),vector2D(0,-1),vector2D(1,-1)
,vector2D(0,0)};


//ラベル名を格納する配列のサイズ
const int SIZE_OF_LABEL_NAME=32;

//注釈文字列
class Comment
{
	Direction d;
	vector2D pos;
	char txt[SIZE_OF_LABEL_NAME];
public:
	Comment(Direction dir,vector2D v,char *t);
	Direction direction() { return d; }
	vector2D position(){return pos;}
	char *name(){return txt;}
	friend std::istream &operator>>(std::istream &stream,Comment &l);
	friend std::ostream &operator<<(std::ostream &stream,Comment l);
};

class Label
{
	Direction d;	//方向
	int node_index;		//端子座標を保持するノードのインデックス
	char nam[SIZE_OF_LABEL_NAME];			//名前;
protected:
	Label(Direction dir,int n,char *nm);
public:
	Direction ReverseDirection(){return Direction((d+4)&7);}
	Direction direction(){return d;}
	char *name(){return nam;}
	int nodeindex(){return node_index;}
	void dec_node_index(){node_index--;}

	friend std::istream &operator>>(std::istream &stream,Label &l);
	friend std::ostream &operator<<(std::ostream &stream,Label l);
};

class LabelIn :public Label
{
	std::vector<bool> state;		//状態の真偽値
public:
	LabelIn(Direction dir,int n,char *nm):Label(dir,n,nm){;}
	LabelIn(Direction dir,int n,char *nm,int buswidth);
	bool GetState(int wirenumber){return state[wirenumber];}
	void SetState(bool b,int wirenumber){state[wirenumber]=b;}

	int nWire(){return state.size();}

	friend std::istream &operator>>(std::istream &stream,LabelIn &l);
	friend std::ostream &operator<<(std::ostream &stream,LabelIn l);
};

//デバイスの分類
enum DeviceClassification{no_device,label_input,label_output,lead_wire,gate_out,gate_in,module_out,module_in,bus_in_in,bus_in_out,bus_out_in,bus_out_out};

class DeviceIndex
{
	DeviceClassification ConnectedDevClass;	//デバイスの分類
	int Index;		//デバイスの配列の添え字
	int SubIndex;
public:
	DeviceIndex(){ConnectedDevClass=no_device,Index=-1,SubIndex=-1;}

	DeviceClassification GetDeviceClass(){return ConnectedDevClass;}
	int GetDeviceIndex(){return Index;}
	int GetDeviceSubIndex(){return SubIndex;}

	void SetDeviceClass(DeviceClassification devcls){ConnectedDevClass=devcls;}
	void SetIndexAndSubIndex(int idx,int sidx){Index=idx,SubIndex=sidx;}

	void DecIndex(){Index--;}

	friend std::istream &operator>>(std::istream &stream,DeviceIndex &sd);
	friend std::ostream &operator<<(std::ostream &stream,DeviceIndex sd);
};

class LabelOut :public Label
{
	DeviceIndex sd;
public:
	LabelOut(Direction dir,int n,char *nm):Label(dir,n,nm){;}
	DeviceClassification GetSourceDevClass(){return sd.GetDeviceClass();}
	DeviceIndex GetSourceDev(){return sd;}
	int GetSourceDevIndex(){return sd.GetDeviceIndex();}
	int GetSourceDevSubIndex(){return sd.GetDeviceSubIndex();}
	void SetSourceDev(DeviceIndex di){sd=di;}
	void SetSourceDevClass(DeviceClassification d){sd.SetDeviceClass(d);}
	void SetSourceDevIndex(int idx,int sidx){sd.SetIndexAndSubIndex(idx,sidx);}

	friend std::istream &operator>>(std::istream &stream,LabelOut &l);
	friend std::ostream &operator<<(std::ostream &stream,LabelOut l);
};

class Node	//節
{
	int buswidth;	//バス幅
	vector2D pos;	//位置
	bool path;	//この節は通過済み
	bool Multipath;		//マルチパス
	bool Short;		//短絡
	bool AllRight;	//正常
	DeviceIndex branch[8];	//8方位のデバイス
public:
	Node();
	Node(int bw);
	vector2D position(){return pos;}
	void setposition(vector2D v){pos=v;}
	void write_branch(Direction d,DeviceClassification dclass,int idx,int sidx);//方向dに枝を追加
	void initbranch(Direction d){write_branch(d,no_device,-1,-1);}
	bool isBranchExist(Direction d){return branch[d].GetDeviceClass()!=no_device;}	//方向dに枝があればtrue
	DeviceClassification dClass(Direction d){return branch[d].GetDeviceClass();}
	int index(Direction d){return branch[d].GetDeviceIndex();}
	int subindex(Direction d){return branch[d].GetDeviceSubIndex();}
	void SetIndexAndSubIndex(Direction d,int idx,int sidx){branch[d].SetIndexAndSubIndex(idx,sidx);}
	int nBranch();
	void dec_index(int i){branch[i].DecIndex();}

	bool GetPathFlag(){return path;}
	void SetPathFlag(){path=true;}
	void ResetPathFlag(){path=false;}

	void SetMultipath(bool b){Multipath=b;}
	bool isMultipath(){return Multipath;}

	void SetShort(bool b){Short=b;}
	bool isShort(){return Short;}

	void SetAllRight(bool b){AllRight=b;}
	bool isAllRight(){return AllRight;}

	int nWire(){return buswidth;}

	friend std::istream &operator>>(std::istream &stream,Node &n);
	friend std::ostream &operator<<(std::ostream &stream,Node n);
};

class SourceData
{
	int node_in;	//入力するノードのインデックス
	DeviceIndex sd;
public:
	SourceData(){node_in=-1;}

	DeviceIndex& SourceDevice(){return sd;}

	int NodeInIndex(){return node_in;}
	void SetNodeInIndex(int i){node_in=i;}
	void DecNodeInIndex(){node_in--;}

	friend std::istream &operator>>(std::istream &stream,SourceData &in);
	friend std::ostream &operator<<(std::ostream &stream,SourceData in);
};

class Gate
{
	std::vector<bool> state;		//状態の真偽値
	bool invert;	//否定
	bool isOr;	//true:論理和 false:論理積
	Direction d;//出力する方向
	int node_out;			//出力端子座標を保持するノードのインデックス
	std::vector<SourceData> array_in;	//入力端子座標を保持するノードのインデックスの配列
public:
	Gate(bool not,bool isor,Direction dir,int out,std::vector<SourceData> &in);
	Gate(bool not, bool isor,Direction dir,int out,std::vector<SourceData> &in,int buswidth);
	bool isOR() { return isOr; }
	bool isNOT() { return invert; }
	Direction ReverseDirection(){return Direction((d+4)&7);}
	Direction direction(){return d;}
	int nIn(){return array_in.size();}
	int NodeOutIndex(){return node_out;}
	void dec_out_index(){node_out--;}
	int NodeInIndex(int i){return array_in[i].NodeInIndex();}
	void dec_in_index(int i){array_in[i].DecNodeInIndex();}

	bool GetState(int wirenumber){return state[wirenumber];}
	void SetState(bool b,int wirenumber){state[wirenumber]=b;}

	void SetSourceDev(int subindex,DeviceIndex di){array_in[subindex].SourceDevice()=di;}
	void SetSourceDevClass(int subindex,DeviceClassification d){array_in[subindex].SourceDevice().SetDeviceClass(d);}
	void SetSourceDevIndex(int subindex,int idx,int sidx){array_in[subindex].SourceDevice().SetIndexAndSubIndex(idx,sidx);}

	DeviceClassification GetSourceDevClass(int subindex){return array_in[subindex].SourceDevice().GetDeviceClass();}
	DeviceIndex GetSourceDev(int subindex){return array_in[subindex].SourceDevice();}
	int GetSourceDevIndex(int subindex){return array_in[subindex].SourceDevice().GetDeviceIndex();}
	int GetSourceDevSubIndex(int subindex){return array_in[subindex].SourceDevice().GetDeviceSubIndex();}

	friend std::istream &operator>>(std::istream &stream,Gate &g);
	friend std::ostream &operator<<(std::ostream &stream,Gate g);
};

class Conductor
{
	int buswidth;	//バス幅
	bool Multipath;
	bool Short;
	Direction d;
	int nod[2];	//始点と終点の座標を保持するノードのインデックス
public:
	Conductor(int n0,int n1,Direction di);
	Conductor(int n0,int n1,Direction di,int nWire);
	int nodeindex0(){return nod[0];}
	int nodeindex1(){return nod[1];}
	Direction ReverseDirection(){return Direction((d+4)&7);}
	Direction direction(){return d;}	//0から1への方向
	void dec_node0(){nod[0]--;}
	void dec_node1(){nod[1]--;}

	void SetMultipath(bool b){Multipath=b;}
	bool isMultipath(){return Multipath;}

	void SetShort(bool b){Short=b;}
	bool isShort(){return Short;}

	int nWire(){return buswidth;}

	friend std::istream &operator>>(std::istream &stream,Conductor &c);
	friend std::ostream &operator<<(std::ostream &stream,Conductor c);
};

class BusIn
{
	std::vector<bool> state;		//状態の真偽値
	Direction d;
	std::vector<int> node_out;
	SourceData node_in;
public:
	BusIn(Direction dir,std::vector<int> &out,SourceData in);
	BusIn(Direction dir,std::vector<int> &out,SourceData in,int buswidth);
	Direction ReverseDirection(){return Direction((d+4)&7);}
	Direction direction(){return d;}
	int nOut(){return node_out.size();}
	int NodeInIndex(){return node_in.NodeInIndex();}
	void dec_in_index(){node_in.DecNodeInIndex();}
	int NodeOutIndex(int i){return node_out[i];}
	void dec_out_index(int i){node_out[i]--;}

	bool GetState(int wirenumber){return state[wirenumber];}
	void SetState(bool b,int wirenumber){state[wirenumber]=b;}

	void SetSourceDev(DeviceIndex di){node_in.SourceDevice()=di;}
	void SetSourceDevClass(DeviceClassification d){node_in.SourceDevice().SetDeviceClass(d);}
	void SetSourceDevIndex(int idx,int sidx){node_in.SourceDevice().SetIndexAndSubIndex(idx,sidx);}

	DeviceClassification GetSourceDevClass(){return node_in.SourceDevice().GetDeviceClass();}
	DeviceIndex GetSourceDev(){return node_in.SourceDevice();}
	int GetSourceDevIndex(){return node_in.SourceDevice().GetDeviceIndex();}
	int GetSourceDevSubIndex(){return node_in.SourceDevice().GetDeviceSubIndex();}

	friend std::istream &operator>>(std::istream &stream,BusIn &b);
	friend std::ostream &operator<<(std::ostream &stream,BusIn b);
};

class BusOut
{
	std::vector<bool> state;		//状態の真偽値
	Direction d;//出力する方向
	int node_out;			//出力端子座標を保持するノードのインデックス
	std::vector<SourceData> array_in;	//入力端子座標を保持するノードのインデックスの配列
public:
	BusOut(Direction dir,int out,std::vector<SourceData> &in);
	BusOut(Direction dir,int out,std::vector<SourceData> &in,int buswidth);

	Direction ReverseDirection(){return Direction((d+4)&7);}
	Direction direction(){return d;}
	int nIn(){return array_in.size();}
	int NodeOutIndex(){return node_out;}
	void dec_out_index(){node_out--;}
	int NodeInIndex(int i){return array_in[i].NodeInIndex();}
	void dec_in_index(int i){array_in[i].DecNodeInIndex();}

	bool GetState(int wirenumber){return state[wirenumber];}
	void SetState(bool b,int wirenumber){state[wirenumber]=b;}

	void SetSourceDev(int subindex,DeviceIndex di){array_in[subindex].SourceDevice()=di;}
	void SetSourceDevClass(int subindex,DeviceClassification d){array_in[subindex].SourceDevice().SetDeviceClass(d);}
	void SetSourceDevIndex(int subindex,int idx,int sidx){array_in[subindex].SourceDevice().SetIndexAndSubIndex(idx,sidx);}

	DeviceClassification GetSourceDevClass(int subindex){return array_in[subindex].SourceDevice().GetDeviceClass();}
	DeviceIndex GetSourceDev(int subindex){return array_in[subindex].SourceDevice();}
	int GetSourceDevIndex(int subindex){return array_in[subindex].SourceDevice().GetDeviceIndex();}
	int GetSourceDevSubIndex(int subindex){return array_in[subindex].SourceDevice().GetDeviceSubIndex();}

	friend std::istream &operator>>(std::istream &stream,BusOut &b);
	friend std::ostream &operator<<(std::ostream &stream,BusOut b);
};


class Module;

class Data
{
	std::vector<Node> node_data;			//節
protected:
	std::vector<BusIn> busin;	//バス入力
	std::vector<BusOut> busout;	//バス出力
	std::vector<LabelIn> input;		//入力
	std::vector<LabelOut> output;		//出力
	std::vector<Gate> gate;				//ゲート
	std::vector<Module> module; //モジュール
	std::vector<Comment> note;			//注釈
private:
	std::vector<Conductor> conductor;	//導線

	template <class X> void Delete(std::vector<X> &ar,unsigned int n)
	{
		for(unsigned int i=n;i<ar.size()-1;i++) ar[i]=ar[i+1];
		ar.pop_back();
	}


//protected:
	//vector2D GetNodeCoordinate(int nodeindex){return node(nodeindex).position();}
//private:

	RECT SpanRect();		//全ての描画要素が収まる範囲

	void MoveOver(DeviceClassification dev,int n);

	bool EraseBranch(int index,Direction d);	//枝を一つ消去しまだ枝が残っていればfalse

	void EraseInputLabel(unsigned int n);
	void EraseOutputLabel(unsigned int n);
	void EraseGate(unsigned int n);
	void EraseBusIn(unsigned int n);
	void EraseBusOut(unsigned int n);
	void EraseConductor(unsigned int n);
	void EraseModule(unsigned int n);

	void DrawGate(HDC hdc);
	void DrawModule(HDC hdc);
	void DrawNode(HDC hdc);
	void DrawConductor(HDC hdc);
	void DrawBusIn(HDC hdc);
	void DrawBusOut(HDC hdc);
	void DrawInputLabel(HDC hdc);
	void DrawOutputLabel(HDC hdc);
	void DrawNote(HDC hdc);

	void DrawGate(std::ofstream &f);
	void DrawModule(std::ofstream &f);
	void DrawNode(std::ofstream &f);
	void DrawConductor(std::ofstream &f);
	void DrawBusIn(std::ofstream &f);
	void DrawBusOut(std::ofstream &f);
	void DrawInputLabel(std::ofstream &f);
	void DrawOutputLabel(std::ofstream &f);
	void DrawNote(std::ofstream &f);

	void SetMultipathFlag(int i,int n,Direction d,bool flag);
	void SetMultipathFlagNode(int n,Direction d,bool flag);
	void SetMultipathFlagWire(int c,Direction d,bool flag);	//cに繋がっている全ての導線のMultipathフラグに書き込み
	bool SearchMultipathNode(int n,Direction d);//導線がループしているかどうか調べる
	bool SearchMultipathWire(int c,Direction d);

	void SetShortFlagNode(int n,Direction d,bool flag);
	void SetShortFlagWire(int c,Direction d,bool flag);
	int SearchNumberOfOutputNode(int n,Direction d);
	int SearchNumberOfOutputWire(int c,Direction d);

	DeviceIndex GetSource(int i,int n,Direction d);
	DeviceIndex GetSourceNode(int n,Direction d);
	DeviceIndex GetSourceWire(int c,Direction d);

	void SetSource(int n,DeviceIndex sd,int i,int wirenumber);
	void SetSourceNode(int n,Direction d,DeviceIndex sd,int wirenumber);
	void SetSourceWire(int c,Direction d,DeviceIndex sd,int wirenumber);
	void Operate(int n,DeviceIndex sd,int wirenumber);

	void DetectErrorAndConnect(unsigned int n,int wirenumber);	//エラーチェックとコネクション
public:
	void InitModuleParentPointer();

	void DrawInputState(HDC hdc);
	void DrawOutputState(HDC hdc);

	void SetSourceModule(int n,bool newstate,int wirenumber);

	bool AbleToPlaceBusIn();
	void PlaceBusIn();
	bool AbleToPlaceBusOut();
	void PlaceBusOut();
	bool AbleToPlaceLabelIn();
	bool AbleToPlaceLabelOut();
	void PlaceInLabel();	//入力ラベル設置
	void PlaceOutLabel();	//出力ラベル設置
	bool AbleToPlaceGate();	//ゲート設置可能か判定
	void PlaceGate();		//ゲート設置
	bool AbleToPlaceConductor();	//導線設置可能か判定
	void PlaceConductor();	//導線設置
	bool AbleToPlaceModule();	//モジュールが設置可能か判定
	void PlaceModule();//モジュール設置
	bool ExpandModule();	//モジュールの展開
	void WriteNote();

	bool EraseDevice();	//ラベル、ゲート、導線、モジュール削除

	void CopyRange(bool Erase,bool Copy);		//削除、カット、コピー
	bool AbleToPaste();//貼り付けられるか
	void Paste();		//貼り付け

	void Draw(HDC hdc); //描画
	void Draw(std::ofstream &f);

	Node node(int i){return node_data[i];} //i番目の節を返す

	unsigned int nInput(){return input.size();}
	unsigned int nOutput(){return output.size();}
	LabelIn GetInput(int i){return input[i];}
	LabelOut GetOutput(int i){return output[i];}

	void NegateInIndex(int i,int wirenumber);		//入力の状態を反転
	void NegateInCoordinate();

	bool GetOutState(int i,int wirenumber);		//出力の状態を取得

	void JumpToSource();//導線の電圧を決定しているデバイスへ飛ぶ

	void Clear();

//状態を表す文字列
	void status(char *s);

	virtual Data* GetParent(){return NULL;}

	void DrawPasteTemplate(HDC hdc);

	friend std::istream &operator>>(std::istream &stream,Data &d);
	friend std::ostream &operator<<(std::ostream &stream,Data d);

};

struct ModuleTerminal
{
	DeviceIndex di;
	int nWire;
};


class Module:public Data
{
	Direction d;	//基準となる向き
	vector2D center;	//基準となる位置
	Data* parent;		//一階層上のData

//各種長さ
	int internal_half_width;
	int internal_half_height;

	int margin[4]; //基準となる向きから時計回りに  0:0度 1:90度 2:180度 3:270度

//各方向の端子の配列

	std::vector<ModuleTerminal> terminal[4]; //基準となる向きから時計回りに  0:0度 1:90度 2:180度 3:270度

	/*
	std::vector<ModuleTerminal> right;
	std::vector<ModuleTerminal> down;
	std::vector<ModuleTerminal> left;
	std::vector<ModuleTerminal> up;
	*/

	std::vector<SourceData> array_in;	//入力ノードのインデックスの配列
	std::vector<int> node_out;	//出力ノードのインデックスの配列


public:
	void SetParent(Data* p){parent=p;}
	Data* GetParent(){return parent;}

	bool GetState(int i,int wirenumber);
	
	void SetSourceDev(int subindex,DeviceIndex di){array_in[subindex].SourceDevice()=di;}
	DeviceIndex InArray(int i){return array_in[i].SourceDevice();}
	
	int NodeIn(int i){return array_in[i].NodeInIndex();}
	void dec_NodeIn(int i){array_in[i].DecNodeInIndex();}
	int NodeOut(int i){return node_out[i];}
	void dec_NodeOut(int i){node_out[i]--;}

	void SetDirection(Direction di) { d = di; }
	Direction direction() { return d; }
	void SetCenter(vector2D v){center=v;}
	vector2D Center(){return center;}

	void LetNodeIn(std::vector<SourceData> &in){array_in=in;}
	void LetNodeOut(std::vector<int> &out){node_out=out;}

	Direction GetInputDirection(int i){return input[i].direction();}
	Direction GetOutputDirection(int i){return output[i].direction();}

	void Add(int j, ModuleTerminal di);

	int GetSize(int j) { return terminal[(j + ((d >> 1) ^ 3) + 1) & 3].size(); }
	int GetLocalSize(int j) { return terminal[j].size(); }

	DeviceIndex Get(int j, int i) { return terminal[(j + ((d >> 1) ^ 3) + 1) & 3][i].di; }
	DeviceIndex GetLocal(int j, int i) { return terminal[j][i].di; }

	int GetBusWidth(int j, int i) { return terminal[(j + ((d >> 1) ^ 3) + 1) & 3][i].nWire; }

	void Set_internal_half_length(int width,int height){internal_half_width=width,internal_half_height=height;}
	int InternalHalfWidth() { return (d == CLOCKWISE_000) || (d == CLOCKWISE_180) ? internal_half_width : internal_half_height; }
	int InternalHalfHeight() { return (d == CLOCKWISE_000) || (d == CLOCKWISE_180) ? internal_half_height : internal_half_width; }
	void Set_margin(int up,int right,int down,int left){margin[3]=up,margin[0]=right,margin[1]=down,margin[2]=left;}
	int Margin(int j) { return margin[(j + ((d >> 1) ^ 3) + 1) & 3]; }

	void Clear();

	friend std::istream &operator>>(std::istream &stream,Module &d);
	friend std::ostream &operator<<(std::ostream &stream,Module d);
};