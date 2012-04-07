static int Correct(int nVertPos, int nAdc)
{
	static const si16 curveQin[] = {-20, 280};
	static const si32 curveQout[] = {9380, -154816};
	static CSettings::CalibrationCurve<si16, si32, int, int, int, 2> curveQ( curveQin, curveQout );

	static const si16 curveKin[] = {-20, 15, 75, 90, 245, 280};
	static const si32 curveKout[] = {581, 580, 581, 582, 584, 580};
	static CSettings::CalibrationCurve<si16, si32, int, int, int, 6> curveK( curveKin, curveKout );

	// for current nVertPos
	int nCoefK = curveK.Get( nVertPos );
	int nCoefQ = curveQ.Get( nVertPos );
	int nZero = nCoefQ/nCoefK;

	return -nZero + ( ((nCoefQ + nAdc * nCoefK)*5) >> (16-5) ); // *32/200mV 5=1/200mV
}

static int GetZero(int nVertPos)
{
	static const si16 curveQin[] = {-20, 280};
	static const si32 curveQout[] = {9380, -154816};
	static CSettings::CalibrationCurve<si16, si32, int, int, int, 2> curveQ( curveQin, curveQout );

	static const si16 curveKin[] = {-20, 15, 75, 90, 245, 280};
	static const si32 curveKout[] = {581, 580, 581, 582, 584, 580};
	static CSettings::CalibrationCurve<si16, si32, int, int, int, 6> curveK( curveKin, curveKout );

	// for current nVertPos
	int nCoefK = curveK.Get( nVertPos );
	int nCoefQ = curveQ.Get( nVertPos );
	int nZero = nCoefQ/nCoefK;
	return -nZero;
// volt = k*x + q
// 0 = k*x+q
// x = -q/k
}
