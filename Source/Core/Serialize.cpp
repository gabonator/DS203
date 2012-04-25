#include "Serialize.h"
	
	CSerialize& CSerialize::operator <<( CStream& stream )
	{
		stream << CStream(this, sizeof(*this));
		return *this;
	}

	CSerialize& CSerialize::operator >>( CStream& stream )
	{
		stream >> CStream(this, sizeof(*this));
		return *this;
	}
	