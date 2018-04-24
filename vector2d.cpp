#include"common.h"
#include<cmath>

namespace Grid
{
	extern int Interval;
}

extern POINT Offset;

//“™†
bool vector2D::operator==(vector2D a)
{
	return x==a.x&&y==a.y;
}

//•s“™†
bool vector2D::operator!=(vector2D a)
{
	return x!=a.x||y!=a.y;
}

//‰ÁZ‹y‚Ñ‘ã“ü
void vector2D::operator+=(vector2D a)
{
	x+=a.x;
	y+=a.y;
}

//‰ÁZ
vector2D vector2D::operator+(vector2D a)
{
	vector2D tmp;
	tmp.x=x+a.x;
	tmp.y=y+a.y;
	return tmp;
}

//Œ¸Z
vector2D vector2D::operator-(vector2D a)
{
	vector2D tmp;
	tmp.x=x-a.x;
	tmp.y=y-a.y;
	return tmp;
}

//•¡‘f”‚ğ‚İ‚È‚µ‚ÄæZ
vector2D vector2D::operator*(vector2D a)
{
	vector2D tmp;
	tmp.x = x * a.x - y * a.y;
	tmp.y = x * a.y + y * a.x;
	return tmp;
}

//ƒXƒJƒ‰[’l‚ğ‚©‚¯‚é
vector2D operator*(const vector2D v,int n)
{
	vector2D tmp;
	tmp.x=v.x*n;
	tmp.y=v.y*n;
	return tmp;
}

const double pi=3.141592654;	//‰~ü—¦

Direction AlignedDirection(vector2D center,vector2D end)
{
	double angle;
	vector2D n=end-center;
	if(n.x==0 && n.y==0)return NO_DIRECTION;
	angle = atan2((double)n.y,(double)n.x)/pi;
	if     (angle<-(double)7/8) return CLOCKWISE_180;
	else if(angle<-(double)5/8) return CLOCKWISE_225;
	else if(angle<-(double)3/8) return CLOCKWISE_270;
	else if(angle<-(double)1/8) return CLOCKWISE_315;
	else if(angle<+(double)1/8) return CLOCKWISE_000;
	else if(angle<+(double)3/8) return CLOCKWISE_045;
	else if(angle<+(double)5/8) return CLOCKWISE_090;
	else if(angle<+(double)7/8) return CLOCKWISE_135;
	else                return CLOCKWISE_180;
}

vector2D AlignAngle(vector2D center,vector2D end)
{
	double angle;
	vector2D v;
	vector2D n=end-center;
	if(n.x==0 && n.y==0) return center;
	angle = atan2((double)n.y,(double)n.x)/pi;
	if     (angle<-(double)7/8) v.x=n.x,	v.y=0;
	else if(angle<-(double)6/8) v.x=n.x,	v.y=n.x;
	else if(angle<-(double)5/8) v.x=n.y,	v.y=n.y;
	else if(angle<-(double)3/8) v.x=0,	v.y=n.y;
	else if(angle<-(double)2/8) v.x=-n.y,v.y=n.y;
	else if(angle<-(double)1/8) v.x=n.x,	v.y=-n.x;
	else if(angle< (double)1/8) v.x=n.x,	v.y=0;
	else if(angle< (double)2/8) v.x=n.x,	v.y=n.x;
	else if(angle< (double)3/8) v.x=n.y,	v.y=n.y;
	else if(angle< (double)5/8) v.x=0,	v.y=n.y;
	else if(angle< (double)6/8) v.x=-n.y,v.y=n.y;
	else if(angle< (double)7/8) v.x=n.x,	v.y=-n.x;
	else                v.x=n.x,	v.y=0;
	v+=center;
	return v;
}

POINT vector2D::MulGridInterval()
{
	POINT p;
	p.x=x*Grid::Interval;
	p.y=y*Grid::Interval;
	return p;
}

POINT vector2D::toPOINT()
{
	POINT p;
	p=MulGridInterval();
	p.x+=Offset.x;
	p.y+=Offset.y;
	return p;
}