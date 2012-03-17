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

template <class TYPE>
class CArray
{
	TYPE	*m_arrElements;
	ui16	m_nCount;
	ui16	m_nMaxCount;
public:
	CArray()
	{
	}

	CArray( TYPE *pSource, int nLength )
	{
		m_arrElements = pSource;
		m_nCount = 0;
		m_nMaxCount = nLength;
	}
	
	void Init( TYPE *pSource, int nLength )
	{
		m_arrElements = pSource;
		m_nCount = 0;
		m_nMaxCount = nLength;
	}

	BOOL IsEmpty()
	{
		_ASSERT( m_arrElements );
		return m_nCount == 0;
	}

	void Add(TYPE t)
	{
		_ASSERT( m_nCount < m_nMaxCount );
		m_arrElements[m_nCount++] = t;
	}

	TYPE &GetLast()
	{
		_ASSERT( m_nCount > 0 );
		return m_arrElements[m_nCount-1];
	}
	
	TYPE RemoveLast()
	{
		_ASSERT( m_nCount > 0 );
		TYPE& t = m_arrElements[--m_nCount];
		return t;
	}

	void Resize( int nDif )
	{
		m_nCount += nDif;
		_ASSERT( m_nCount >= 0 && m_nCount < m_nMaxCount );
	}

	int GetSize()
	{
		return m_nCount;
	}

	TYPE& operator []( int i)
	{
		if ( i < 0 )
			i += m_nCount;
		_ASSERT( i >= 0 && i < GetSize() );
		return m_arrElements[i];
	}
	void RemoveAt( int i )
	{
		_ASSERT( i < GetSize() );
		for ( ; i < GetSize()-1; i++ )
			m_arrElements[i] = m_arrElements[i+1];
		Resize(-1);
	}
};


#endif