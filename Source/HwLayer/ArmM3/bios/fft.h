	extern "C" void fftR4(short *y, short *x, int N);


  /*static*/ void BIOS::FFT::Window(si16* arrSignal, const ui16*arrWindowHalf, int n)
  {
    short *p = arrSignal;
    ui32 tmp;
    for (int i=0; i<n; i++)
    {
      tmp = *p;
      if (i<n/2)
        tmp *= arrWindowHalf[i];
      else
        tmp *= arrWindowHalf[n-i-1];
      tmp >>= 16;
      *p = tmp;
      p+= 2;
    }
    
  }

	/*static*/ void BIOS::FFT::Convert( si16* arrOutput, si16* arrInput, int n )
  {
    fftR4(arrOutput, arrInput, n);
  }

	/*static*/ ui32 BIOS::FFT::Sqrt( ui32 value )
	{
	  int nv, v = value>>1, c = 0;
	  if (!v)
	    return value;
	  do
	  {
	    nv = (v + value/v)>>1;
	    if (abs(v - nv) <= 1) // I have an available fast absolute value in this forum. If you have it. use the next one.
	      return nv;
	    v = nv;
	  }
	  while (c++ < 25);
	  return nv;
	}

