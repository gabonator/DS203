#ifndef __CLASSES_H__
#define __CLASSES_H__

class CPoint {
public:
	int x, y;
	CPoint()
	{
	}
	CPoint(int _x, int _y) : 
		x(_x), y(_y)
	{
	}
};

class CRect {
public:
	CRect()
	{
	}

	CRect( int _left, int _top, int _right, int _bottom ) :
	  left(_left), top(_top), right(_right), bottom(_bottom)
	{
	}
	int CenterX()
	{
		return (left+right)>>1;
	}
	void Deflate(int l, int t, int r, int b)
	{
		left += l;
		top += t;
		right -= r;
		bottom -= b;
	}

	void Inflate(int l, int t, int r, int b)
	{
		left -= l;
		top -= t;
		right += r;
		bottom += b;
	}
	int Width() const
	{
		return right - left;
	}
	int Height() const
	{
		return bottom - top;
	}
	const CPoint& TopLeft() const
	{
		return *((CPoint*)this);
	}
	void Offset(int x, int y)
	{
		left += x;
		right += x;
		top += y;
		bottom += y;
	}

	int left, top, right, bottom;
};

#endif