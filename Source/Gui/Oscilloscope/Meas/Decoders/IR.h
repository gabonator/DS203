// based on http://github.com/shirriff/Arduino-IRremote

class CIrDecoder : public CEdgeDecoder 
{
	class decode_results {
	public:
		int decode_type; // NEC, SONY, RC5, UNKNOWN
		unsigned int panasonicAddress; // This is only used for decoding Panasonic data
		unsigned long value; // Decoded value
		int bits; // Number of bits in decoded value
		ui16* rawbuf;
		int rawlen;
	} results;

	enum {
		MARK = 0,
		SPACE = 1,
		RAWBUF = 100,
		MARK_EXCESS=100,
		NEC=1,
		SONY=2,
		RC5=3,
		RC6=4,
		DISH=5,
		SHARP=6,
		PANASONIC=7,
		JVC=8,
		SANYO=9,
		MITSUBISHI=10,

		REPEAT=0xffffffff,
		_GAP=5000, // Minimum map between transmissions
		USECPERTICK=50,  // microseconds per clock interrupt tick
		GAP_TICKS=(_GAP/USECPERTICK),

		NEC_HDR_MARK	=9000,
		NEC_HDR_SPACE	=4500,
		NEC_BIT_MARK	=560,
		NEC_ONE_SPACE	=1600,
		NEC_ZERO_SPACE	=560,
		NEC_RPT_SPACE	=2250,

		SONY_HDR_MARK	=2400,
		SONY_HDR_SPACE	=600,
		SONY_ONE_MARK	=1200,
		SONY_ZERO_MARK	=600,
		SONY_RPT_LENGTH =45000,
		SONY_DOUBLE_SPACE_USECS  =500,  // usually ssee 713 - not using ticks as get number wrapround

		SANYO_HDR_MARK	=3500,  // seen range 3500
		SANYO_HDR_SPACE	=950, //  seen 950
		SANYO_ONE_MARK	=2400, // seen 2400  
		SANYO_ZERO_MARK =700, //  seen 700
		SANYO_DOUBLE_SPACE_USECS  =800,  // usually ssee 713 - not using ticks as get number wrapround
		SANYO_RPT_LENGTH =45000,

		NEC_BITS =32,
		SONY_BITS =12,
		SANYO_BITS =12,
		MITSUBISHI_BITS =16,
		MIN_RC5_SAMPLES =11,
		MIN_RC6_SAMPLES =1,
		PANASONIC_BITS =48,
		JVC_BITS =16,

		MITSUBISHI_HDR_SPACE=350, //  7*50+100
		MITSUBISHI_ONE_MARK	=1950, // 41*50-100
		MITSUBISHI_ZERO_MARK  =750, // 17*50-100

		RC5_T1		=889,
		RC5_RPT_LENGTH	=46000,
		RC6_HDR_MARK	=2666,
		RC6_HDR_SPACE	=889,
		RC6_T1		=444,
		RC6_RPT_LENGTH=46000,

		PANASONIC_HDR_MARK =3502,
		PANASONIC_HDR_SPACE =1750,
		PANASONIC_BIT_MARK =502,
		PANASONIC_ONE_SPACE =1244,
		PANASONIC_ZERO_SPACE =400,
		JVC_HDR_MARK =8000,
		JVC_HDR_SPACE =4000,
		JVC_BIT_MARK =600,
		JVC_ONE_SPACE =1600,
		JVC_ZERO_SPACE=550,
		JVC_RPT_LENGTH=60000,

		Q=0

	};

	enum Ercvstate
	{
		STATE_IDLE=2,
		STATE_MARK=3,
		STATE_SPACE=4,
		STATE_STOP=5
	};

	struct {
		enum {
		};
		Ercvstate rcvstate;          // state machine
		unsigned long timer;     // state timer, counts 50uS ticks.
		ui16 rawbuf[RAWBUF]; // raw data
		uint8_t rawlen;         // counter of entries in rawbuf
	} irparams;

public:
	CIrDecoder()
	{
		irparams.rcvstate = STATE_IDLE;
		irparams.rawlen = 0;
		irparams.timer = 0;
		results.rawbuf = irparams.rawbuf;
		results.rawlen = irparams.rawlen;
		results.panasonicAddress = 0;
		results.value = 0;
		memset(irparams.rawbuf, 0, sizeof(irparams.rawbuf));
		
		// simulate gap, decoder will skip first edge, because it's size is unknown
		//Push( GAP_TICKS+10, 0 );
	}

	virtual bool OnEdge(int nLength, int nRising)
	{
		float fSampling = Settings.Runtime.m_fTimeRes / CWndGraph::BlkX;
		long nIntervals = (long)(fSampling * nLength / 50e-6);
		Push( nIntervals, 1-nRising );
		return true;
	}

	void Push(int ticks, int irdata)
	{
		irparams.timer = ticks; 
		//BIOS::DBG::Print( "%d", irdata );
		//irparams.timer++; // One more 50us tick
		if (irparams.rawlen >= RAWBUF) {
			// Buffer overflow
			irparams.rcvstate = STATE_STOP;
		}
		switch(irparams.rcvstate) {
		case STATE_IDLE: // In the middle of a gap
			if (irdata == MARK) {
				if (irparams.timer < GAP_TICKS) {
					// Not big enough to be a gap.
					irparams.timer = 0;
				} 
				else {
					// gap just ended, record duration and start recording transmission
					irparams.rawlen = 0;
					_ASSERT( irparams.timer < 65000 );
					irparams.rawbuf[irparams.rawlen++] = (ui16)irparams.timer;
					irparams.timer = 0;
					irparams.rcvstate = STATE_MARK;
				}
			}
			break;
		case STATE_MARK: // timing MARK
			if (irdata == SPACE) {   // MARK ended, record time
				irparams.rawbuf[irparams.rawlen++] = (ui16)irparams.timer;
				irparams.timer = 0;
				irparams.rcvstate = STATE_SPACE;
			}
			break;
		case STATE_SPACE: // timing SPACE
			if (irdata == MARK) { // SPACE just ended, record it
				irparams.rawbuf[irparams.rawlen++] = (ui16)irparams.timer;
				irparams.timer = 0;
				irparams.rcvstate = STATE_MARK;
			} 
			else { // SPACE
				if (irparams.timer > GAP_TICKS) {
					irparams.rcvstate = STATE_STOP;
				} 
			}
			break;
		case STATE_STOP: // waiting, measuring gap
			if (irdata == MARK) { // reset gap timer
				irparams.timer = 0;
			}
			break;
		}
	}

	bool Decode() {
		//BIOS::DBG::Print( "\n" );
		if (decodeNEC(&results)) return true;
		if (decodeSony(&results)) return true;
		if (decodeSanyo(&results)) return true;
		if (decodeMitsubishi(&results)) return true;
		if (decodeRC5(&results)) return true;
		if (decodeRC6(&results)) return true;
		if (decodePanasonic(&results)) return true;
		if (decodeJVC(&results)) return true;
		return false;
	}

	bool Visualize()
	{
		if (!Decode())
			return false;

		int x = 20;
		int y = 30;
		x += 1*BIOS::LCD::Print(x, y, RGB565(ff0000), 0x0101, "Ir: ");
		const char *strType = "Unknown";
		switch ( results.decode_type )
		{
		case NEC: strType = "NEC"; break;
		case SONY: strType = "SONY"; break;
		case RC5: strType = "RC5"; break;
		case RC6: strType = "RC6"; break;
		case SANYO: strType = "SANYO"; break;
		case MITSUBISHI: strType = "MITSUBISHI"; break;
		case PANASONIC: strType = "PANASONIC"; break;
		case JVC: strType = "JVC"; break;
		} 
		if ( results.value == REPEAT )
			x += 8*BIOS::LCD::Printf(x, y, RGB565(ffffff), 0x0101, "%s (repeat)", 
				strType );
		else
			x += 8*BIOS::LCD::Printf(x, y, RGB565(ffffff), 0x0101, "%s (%x, %d bits)", 
				strType, results.value, results.bits );

		return true;
	}

	// decoders
	long decodeNEC(decode_results *results) {
		long data = 0;
		int offset = 1; // Skip first space
		// Initial mark
		if (!MATCH_MARK(results->rawbuf[offset], NEC_HDR_MARK)) {
			return false;
		}
		offset++;
		// Check for repeat
		if (irparams.rawlen == 4 &&
			MATCH_SPACE(results->rawbuf[offset], NEC_RPT_SPACE) &&
			MATCH_MARK(results->rawbuf[offset+1], NEC_BIT_MARK)) {
				results->bits = 0;
				results->value = REPEAT;
				results->decode_type = NEC;
				return true;
		}
		if (irparams.rawlen < 2 * NEC_BITS + 4) {
			return false;
		}
		// Initial space  
		if (!MATCH_SPACE(results->rawbuf[offset], NEC_HDR_SPACE)) {
			return false;
		}
		offset++;
		for (int i = 0; i < (int)NEC_BITS; i++) {
			if (!MATCH_MARK(results->rawbuf[offset], NEC_BIT_MARK)) {
				return false;
			}
			offset++;
			if (MATCH_SPACE(results->rawbuf[offset], NEC_ONE_SPACE)) {
				data = (data << 1) | 1;
			} 
			else if (MATCH_SPACE(results->rawbuf[offset], NEC_ZERO_SPACE)) {
				data <<= 1;
			} 
			else {
				return false;
			}
			offset++;
		}
		// Success
		results->bits = NEC_BITS;
		results->value = data;
		results->decode_type = NEC;
		return true;
	}

	long decodeSony(decode_results *results) {
		long data = 0;
		if (irparams.rawlen < 2 * SONY_BITS + 2) {
			return false;
		}
		int offset = 0; // Dont skip first space, check its size
	
		if (results->rawbuf[offset] < SONY_DOUBLE_SPACE_USECS/50) {
			results->bits = 0;
			results->value = REPEAT;
			results->decode_type = SANYO;
			return true;
		}
		offset++;

		// Initial mark
		if (!MATCH_MARK(results->rawbuf[offset], SONY_HDR_MARK)) {
			return false;
		}
		offset++;

		while (offset + 1 < irparams.rawlen) {
			if (!MATCH_SPACE(results->rawbuf[offset], SONY_HDR_SPACE)) {
				break;
			}
			offset++;
			if (MATCH_MARK(results->rawbuf[offset], SONY_ONE_MARK)) {
				data = (data << 1) | 1;
			} 
			else if (MATCH_MARK(results->rawbuf[offset], SONY_ZERO_MARK)) {
				data <<= 1;
			} 
			else {
				return false;
			}
			offset++;
		}
		// Success
		results->bits = (offset - 1) / 2;
		if (results->bits < 12) {
			results->bits = 0;
			return false;
		}
		results->value = data;
		results->decode_type = SONY;
		return true;
	}

	long decodeSanyo(decode_results *results) {
		long data = 0;
		if (irparams.rawlen < 2 * SANYO_BITS + 2) {
			return false;
		}
		int offset = 0; // Skip first space
		// Initial space  
		if (results->rawbuf[offset] < SANYO_DOUBLE_SPACE_USECS/50) {
			// Serial.print("IR Gap found: ");
			results->bits = 0;
			results->value = REPEAT;
			results->decode_type = SANYO;
			return true;
		}
		offset++;

		// Initial mark
		if (!MATCH_MARK(results->rawbuf[offset], SANYO_HDR_MARK)) {
			return false;
		}
		offset++;

		// Skip Second Mark
		if (!MATCH_MARK(results->rawbuf[offset], SANYO_HDR_MARK)) {
			return false;
		}
		offset++;

		while (offset + 1 < irparams.rawlen) {
			if (!MATCH_SPACE(results->rawbuf[offset], SANYO_HDR_SPACE)) {
				break;
			}
			offset++;
			if (MATCH_MARK(results->rawbuf[offset], SANYO_ONE_MARK)) {
				data = (data << 1) | 1;
			} 
			else if (MATCH_MARK(results->rawbuf[offset], SANYO_ZERO_MARK)) {
				data <<= 1;
			} 
			else {
				return false;
			}
			offset++;
		}
		// Success
		results->bits = (offset - 1) / 2;
		if (results->bits < 12) {
			results->bits = 0;
			return false;
		}
		results->value = data;
		results->decode_type = SANYO;
		return true;
	}

	long decodeMitsubishi(decode_results *results) {
		long data = 0;
		if (irparams.rawlen < 2 * MITSUBISHI_BITS + 2) {
			return false;
		}
		int offset = 0; // Skip first space
		offset++;
		// Initial Space
		if (!MATCH_MARK(results->rawbuf[offset], MITSUBISHI_HDR_SPACE)) {
			return false;
		}
		offset++;
		while (offset + 1 < irparams.rawlen) {
			if (MATCH_MARK(results->rawbuf[offset], MITSUBISHI_ONE_MARK)) {
				data = (data << 1) | 1;
			} 
			else if (MATCH_MARK(results->rawbuf[offset], MITSUBISHI_ZERO_MARK)) {
				data <<= 1;
			} 
			else {
				// Serial.println("A"); Serial.println(offset); Serial.println(results->rawbuf[offset]);
				return false;
			}
			offset++;
			if (!MATCH_SPACE(results->rawbuf[offset], MITSUBISHI_HDR_SPACE)) {
				// Serial.println("B"); Serial.println(offset); Serial.println(results->rawbuf[offset]);
				break;
			}
			offset++;
		}
		// Success
		results->bits = (offset - 1) / 2;
		if (results->bits < (int)MITSUBISHI_BITS) {
			results->bits = 0;
			return false;
		}
		results->value = data;
		results->decode_type = MITSUBISHI;
		return true;
	}

	int getRClevel(decode_results *results, int *offset, int *used, int t1) {
		if (*offset >= results->rawlen) {
			// After end of recorded buffer, assume SPACE.
			return SPACE;
		}
		int width = results->rawbuf[*offset];
		int val = ((*offset) % 2) ? MARK : SPACE;
		int correction = (val == MARK) ? MARK_EXCESS : - MARK_EXCESS;

		int avail;
		if (MATCH(width, t1 + correction)) {
			avail = 1;
		} 
		else if (MATCH(width, 2*t1 + correction)) {
			avail = 2;
		} 
		else if (MATCH(width, 3*t1 + correction)) {
			avail = 3;
		} 
		else {
			return -1;
		}

		(*used)++;
		if (*used >= avail) {
			*used = 0;
			(*offset)++;
		}
		return val;   
	}

	long decodeRC5(decode_results *results) {
		if (irparams.rawlen < MIN_RC5_SAMPLES + 2) {
			return false;
		}
		int offset = 1; // Skip gap space
		long data = 0;
		int used = 0;
		// Get start bits
		if (getRClevel(results, &offset, &used, RC5_T1) != MARK) return false;
		if (getRClevel(results, &offset, &used, RC5_T1) != SPACE) return false;
		if (getRClevel(results, &offset, &used, RC5_T1) != MARK) return false;
		int nbits;
		for (nbits = 0; offset < irparams.rawlen; nbits++) {
			int levelA = getRClevel(results, &offset, &used, RC5_T1); 
			int levelB = getRClevel(results, &offset, &used, RC5_T1);
			if (levelA == SPACE && levelB == MARK) {
				// 1 bit
				data = (data << 1) | 1;
			} 
			else if (levelA == MARK && levelB == SPACE) {
				// zero bit
				data <<= 1;
			} 
			else {
				return false;
			} 
		}

		// Success
		results->bits = nbits;
		results->value = data;
		results->decode_type = RC5;
		return true;
	}

	long decodeRC6(decode_results *results) {
		if (results->rawlen < (int)MIN_RC6_SAMPLES) {
			return false;
		}
		int offset = 1; // Skip first space
		// Initial mark
		if (!MATCH_MARK(results->rawbuf[offset], RC6_HDR_MARK)) {
			return false;
		}
		offset++;
		if (!MATCH_SPACE(results->rawbuf[offset], RC6_HDR_SPACE)) {
			return false;
		}
		offset++;
		long data = 0;
		int used = 0;
		// Get start bit (1)
		if (getRClevel(results, &offset, &used, RC6_T1) != MARK) return false;
		if (getRClevel(results, &offset, &used, RC6_T1) != SPACE) return false;
		int nbits;
		for (nbits = 0; offset < results->rawlen; nbits++) {
			int levelA, levelB; // Next two levels
			levelA = getRClevel(results, &offset, &used, RC6_T1); 
			if (nbits == 3) {
				// T bit is double wide; make sure second half matches
				if (levelA != getRClevel(results, &offset, &used, RC6_T1)) return false;
			} 
			levelB = getRClevel(results, &offset, &used, RC6_T1);
			if (nbits == 3) {
				// T bit is double wide; make sure second half matches
				if (levelB != getRClevel(results, &offset, &used, RC6_T1)) return false;
			} 
			if (levelA == MARK && levelB == SPACE) { // reversed compared to RC5
				// 1 bit
				data = (data << 1) | 1;
			} 
			else if (levelA == SPACE && levelB == MARK) {
				// zero bit
				data <<= 1;
			} 
			else {
				return false; // Error
			} 
		}
		// Success
		results->bits = nbits;
		results->value = data;
		results->decode_type = RC6;
		return true;
	}

	long decodePanasonic(decode_results *results) {
		unsigned long long data = 0;
		int offset = 1;

		if (!MATCH_MARK(results->rawbuf[offset], PANASONIC_HDR_MARK)) {
			return false;
		}
		offset++;
		if (!MATCH_MARK(results->rawbuf[offset], PANASONIC_HDR_SPACE)) {
			return false;
		}
		offset++;

		// decode address
		for (int i = 0; i < (int)PANASONIC_BITS; i++) {
			if (!MATCH_MARK(results->rawbuf[offset++], PANASONIC_BIT_MARK)) {
				return false;
			}
			if (MATCH_SPACE(results->rawbuf[offset],PANASONIC_ONE_SPACE)) {
				data = (data << 1) | 1;
			} else if (MATCH_SPACE(results->rawbuf[offset],PANASONIC_ZERO_SPACE)) {
				data <<= 1;
			} else {
				return false;
			}
			offset++;
		}
		results->value = (unsigned long)data;
		results->panasonicAddress = (unsigned int)(data >> 32);
		results->decode_type = PANASONIC;
		results->bits = PANASONIC_BITS;
		return true;
	}

	long decodeJVC(decode_results *results) {
		long data = 0;
		int offset = 1; // Skip first space
		// Check for repeat
		if (irparams.rawlen - 1 == 33 &&
			MATCH_MARK(results->rawbuf[offset], JVC_BIT_MARK) &&
			MATCH_MARK(results->rawbuf[irparams.rawlen-1], JVC_BIT_MARK)) {
				results->bits = 0;
				results->value = REPEAT;
				results->decode_type = JVC;
				return true;
		} 
		// Initial mark
		if (!MATCH_MARK(results->rawbuf[offset], JVC_HDR_MARK)) {
			return false;
		}
		offset++; 
		if (irparams.rawlen < 2 * JVC_BITS + 1 ) {
			return false;
		}
		// Initial space 
		if (!MATCH_SPACE(results->rawbuf[offset], JVC_HDR_SPACE)) {
			return false;
		}
		offset++;
		for (int i = 0; i < (int)JVC_BITS; i++) {
			if (!MATCH_MARK(results->rawbuf[offset], JVC_BIT_MARK)) {
				return false;
			}
			offset++;
			if (MATCH_SPACE(results->rawbuf[offset], JVC_ONE_SPACE)) {
				data = (data << 1) | 1;
			} 
			else if (MATCH_SPACE(results->rawbuf[offset], JVC_ZERO_SPACE)) {
				data <<= 1;
			} 
			else {
				return false;
			}
			offset++;
		}
		//Stop bit
		if (!MATCH_MARK(results->rawbuf[offset], JVC_BIT_MARK)){
			return false;
		}
		// Success
		results->bits = JVC_BITS;
		results->value = data;
		results->decode_type = JVC;
		return true;
	}

	#define TICKS_LOW(us) (int) (((us)*LTOL/USECPERTICK))
	#define TICKS_HIGH(us) (int) (((us)*UTOL/USECPERTICK + 1))
	#define TOLERANCE 25  // percent tolerance in measurements
	#define LTOL (1.0 - TOLERANCE/100.) 
	#define UTOL (1.0 + TOLERANCE/100.) 
	int MATCH(int measured, int desired) {return measured >= TICKS_LOW(desired) && measured <= TICKS_HIGH(desired);}
	int MATCH_MARK(int measured_ticks, int desired_us) {return MATCH(measured_ticks, (desired_us + MARK_EXCESS));}
	int MATCH_SPACE(int measured_ticks, int desired_us) {return MATCH(measured_ticks, (desired_us - MARK_EXCESS));}
	#undef TICKS_LOW
	#undef TICKS_HIGH
	#undef UTOL 
	#undef LTOL
	#undef TOLERANCE
};
