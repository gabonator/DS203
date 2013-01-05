/*
typedef unsigned char ui8;
typedef unsigned int ui32;
typedef unsigned short ui16;
typedef signed char si8;
typedef int INT;
typedef unsigned int UINT;
typedef const char * PCSTR;
typedef char * PSTR;
typedef char CHAR;
typedef int BOOL;
typedef float FLOAT;
#define TRUE 1
#define FALSE 0
#define NULL 0
#define _ASSERT(a) if (!(a)) { _asm { int 3 } }
*/
#define _SAFE(a) if (!(a)) { _ASSERT(0); return CEvalOperand(CEvalOperand::eoError); }

class CEvalClasses 
{
public:
	enum {
		RpnLength = 32,
		StackLength = 16
		//,
		//MaxTokenLength = 20
	};

public:
	class CStream 
	{
	public:
		virtual INT GetSize() = 0;
		virtual CHAR Get() = 0;
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

		CArray( TYPE *pSource, INT nLength )
		{
			m_arrElements = pSource;
			m_nCount = 0;
			m_nMaxCount = nLength;
		}
		
		void Init( TYPE *pSource, INT nLength )
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

		void Resize( INT nDif )
		{
			m_nCount += nDif;
			_ASSERT( m_nCount >= 0 && m_nCount < m_nMaxCount );
		}

		INT GetSize()
		{
			return m_nCount;
		}

		TYPE& operator []( INT i )
		{
			if ( i < 0 )
				i += m_nCount;
			_ASSERT( i >= 0 && i < GetSize() );
			return m_arrElements[i];
		}
	};

	class CEvalOperand;
	typedef CEvalOperand (*PEvalFunc)( CArray<CEvalOperand>& arrOperands );

	class CEvalVariable
	{
	public:
		virtual void Set( CEvalOperand& pOperand ) = 0;
		virtual CEvalOperand Get() = 0;
	};

	class CEvalToken {
	public:
		enum ePrecedence {
			PrecedenceToken = -1,
			PrecedenceNone = 0,
			PrecedenceMul = 9,
			PrecedenceAdd = 8,
			PrecedenceLow = 4,
			PrecedenceFunc = 20,
			PrecedenceFuncLow = 5,
			PrecedenceConst = 30,
			PrecedenceVar = 31
		};

		const CHAR*			m_pszToken;
		const si8			m_ePrecedence;
		const PEvalFunc		m_pEval;
	};

	/*
	class CEvalToken 
	{
	public:
		enum ePrecedence {
			PrecedenceToken = -1,
			PrecedenceNone = 0,
			PrecedenceMul = 9,
			PrecedenceAdd = 8,
			PrecedenceLow = 4,
			PrecedenceFunc = 20,
			PrecedenceFuncLow = 5,
			PrecedenceConst = 30,
			PrecedenceVar = 31
		};

		CHAR			m_pszToken[MaxTokenLength];
		ui8				m_nTokenLen;
		si8				m_ePrecedence;
		PEvalFunc		m_pEval;

		CEvalToken()
		{
		}

		CEvalToken(PCSTR pszToken, si8 Precedence = PrecedenceNone, PEvalFunc pEval = NULL) :
			m_ePrecedence( Precedence ),
			m_pEval( pEval )
		{
			if ( pszToken )
			{
				m_nTokenLen = (ui8) strlen(pszToken);
				_ASSERT( m_nTokenLen < MaxTokenLength-1 );
				strcpy( m_pszToken, pszToken );
			}
			else
			{
				m_pszToken[0] = 0;
				m_nTokenLen = 0;
			}
		}
	};
	*/

	class CEvalOperand 
	{
	public:
		enum EOperandType {
			eoInteger,
			eoFloat,
			eoString,
			eoCString,
			eoAttribute,
			eoOperator,
			eoError,
			eoNone,
			eoStream,
			eoVariable
		};
		
	public:
		EOperandType m_eType;

		union {
			INT				m_iData;
			FLOAT			m_fData;
			const CEvalToken	*m_pOperator;
			CStream			*m_pStream;
			char			*m_pString;
			const char		*m_pcString;
			ui8				m_pData8[8];
			ui32			m_pData32[2];
			CEvalVariable	*m_pVariable;
		} m_Data;

	public:
		explicit CEvalOperand( EOperandType eType ) : m_eType( eType )
		{
		}

		CEvalOperand()
		{
		}

		CEvalOperand( FLOAT f ) : m_eType( eoFloat )
		{
			m_Data.m_fData = f;
		}

		CEvalOperand( INT i ) : m_eType( eoInteger )
		{
			m_Data.m_iData = i;
		}

		CEvalOperand( CHAR* str, INT nLength, EOperandType eType = eoString ) :
			m_eType( eType )
		{
			m_Data.m_pString = str;
			m_Data.m_pData32[1] = nLength;
		}

		CEvalOperand( const CHAR* str, INT nLength = -1, EOperandType eType = eoCString ) :
			m_eType( eType )
		{
			m_Data.m_pcString = str;
			m_Data.m_pData32[1] = (nLength > -1) ? nLength : strlen(str);
		}

		CEvalOperand( const CEvalToken* pToken ) : m_eType( eoOperator )
		{
			m_Data.m_pOperator = pToken;
		}

		CEvalOperand( CStream* pStream ) : m_eType( eoStream )
		{
			m_Data.m_pStream = pStream;
		}

		CEvalOperand( CEvalVariable *pVariable ) : m_eType( eoVariable )
		{
			m_Data.m_pVariable = pVariable;
		}

		FLOAT GetFloat()
		{
			if ( m_eType == eoInteger )
				return (FLOAT) m_Data.m_iData;
			if ( m_eType == eoFloat )
				return (FLOAT) m_Data.m_fData;
			if ( m_eType == eoVariable )
				return this->m_Data.m_pVariable->Get().GetFloat();
			_ASSERT( 0 );
			return 0.0f;
		}

		INT GetInteger()
		{
			if ( m_eType == eoInteger )
				return m_Data.m_iData;
			if ( m_eType == eoFloat )
				return (INT) m_Data.m_fData;
			if ( m_eType == eoVariable )
				return this->m_Data.m_pVariable->Get().GetInteger();
			_ASSERT( 0 );
			return 0L;
		}

		BOOL Is( EOperandType eType )
		{
			return m_eType == eType;
		}

		BOOL Is( const CEvalToken* pToken )
		{
			if ( m_eType != eoOperator )
				return FALSE;
			return m_Data.m_pOperator == pToken;
		}
	};
};

class CEvalTools 
{
public:
	INT HexToInt(CHAR c)
	{
		if ( c < '0' || c > 'f' )
			return -1;

		if ( (c>>4) == 3 ) 
			return c-'0'; 
		else 
			return (c&0xf)+9;
	}

	FLOAT StrIsFloat ( CHAR *s )
	{
		while ( *s >= '0' && *s <= '9' )
			s++;
		if (*s == '.')
			return TRUE;
		return FALSE;
	}
	
	FLOAT StrToFloat ( CHAR *s, CHAR **endp )
	{
		*endp = NULL;
		
		CHAR* digit = s;
		FLOAT sign = 1.0f;
		FLOAT result = 0.0f;

		// check sign
		if (*digit == '-') 
		{
			sign = -1;
			digit++;
		}

		//--- get integer portion
		while (*digit >= '0' && *digit <='9') 
		{
			result = (result * 10) + *digit-'0';
			digit++;
		}

		//--- get decimal point and fraction, if any.
		if (*digit == '.') 
		{
			digit++;
			FLOAT scale = 0.1f;

			while (*digit >= '0' && *digit <='9') 
			{
				result += (*digit-'0') * scale;
				scale *= 0.1f;
				digit++;
			}
		}

		//--- error if we're not at the end of the number
		if (*digit != 0) 
			*endp = digit;

		//--- set to sign given at the front
		result = result * sign;
		
		return result;
	}

	INT StrToInt ( CHAR *s, CHAR **endp )
	{
		*endp = NULL;
		
		CHAR* digit = s;
		INT result = 0;

		while (*digit >= '0' && *digit <='9') 
		{
			result = (result * 10) + *digit-'0';
			digit++;
		}

		if (*digit != 0) 
			*endp = digit;

		return result;
	}

};

class CEvalCore : public CEvalTools, public CEvalClasses
{
public:	
	CEvalOperand m_arrRpn_[RpnLength];
	CArray<CEvalOperand> m_arrRpn;
	PSTR m_pEndPtr;

private:
	virtual const CEvalToken* isOperator( CHAR* pszExpression ) = NULL;

public:
	CEvalOperand Eval( PSTR pszExpression )
	{
		m_arrRpn.Init( m_arrRpn_, RpnLength );

		if ( !ConvertToRpn( pszExpression ) )
			return CEvalOperand( CEvalOperand::eoError );

		return EvalRpn();
	}

	BOOL ConvertToRpn( PSTR pszExpression )
	{
		CEvalOperand arrStack_[StackLength];
		CArray <CEvalOperand> arrStack( arrStack_, StackLength );

		PSTR pszEnd = pszExpression + strlen( pszExpression );
		
		const CEvalToken *pPrevToken = NULL;
		const CEvalToken *pTokLPar = isOperator( (char*)"(" );
		const CEvalToken *pTokRPar = isOperator( (char*)")" );
		const CEvalToken *pTokDelim = isOperator( (char*)"," );
		const CEvalToken *pTokEq = isOperator( (char*)"=" );
		const CEvalToken *pTokTerm = isOperator( (char*)";" );
		const CEvalToken *pToken = pTokLPar;

		m_pEndPtr = NULL;

		for ( ; pszExpression < pszEnd; pszExpression++ )
		{
			if ( *pszExpression == ' ' )
				continue;
			
			pPrevToken = pToken;
			pToken = isOperator( pszExpression );
			if ( pToken == pTokTerm )
			{
				pszExpression += strlen(pToken->m_pszToken);
				m_pEndPtr = pszExpression;
				break;
			}

			if ( pToken == pTokLPar || pToken == pTokEq )
			{
				arrStack.Add( pToken );
				continue;
			}

			if ( pToken == pTokRPar || pToken == pTokDelim )
			{
				while ( TRUE )
				{
					if ( arrStack.IsEmpty() )
						return FALSE;

					if ( arrStack.GetLast().Is( pTokLPar ) ||
						 arrStack.GetLast().Is( pTokEq ) ||
						 arrStack.GetLast().Is( pTokDelim ) )
					{
						arrStack.RemoveLast();
						break;
					}

					m_arrRpn.Add( arrStack.RemoveLast() );
				}
				if ( pToken == pTokDelim )
				{
					CEvalOperand Delim( pTokDelim );
					m_arrRpn.Add( Delim );
					arrStack.Add( Delim );
				}
				continue;
			}

			// user function
			if ( pToken && ( pToken->m_ePrecedence == CEvalToken::PrecedenceFunc ||
				pToken->m_ePrecedence == CEvalToken::PrecedenceFuncLow ) )
			{
				arrStack.Add( pToken );
				pszExpression += strlen(pToken->m_pszToken) - 1;
				continue;
			}

			// user constant
			if ( pToken != NULL && 
				 ( pToken->m_ePrecedence == CEvalToken::PrecedenceConst ||
				   pToken->m_ePrecedence == CEvalToken::PrecedenceVar )
				 )
			{
				m_arrRpn.Add( CEvalOperand(pToken) );
				pszExpression += strlen(pToken->m_pszToken) - 1;
				continue;
			}

			if ( !pToken )
			{
				if ( *pszExpression == '\'' )
				{
					// string
					PSTR pszStart = ++pszExpression;
					while ( *pszExpression && *pszExpression != '\'' )
						pszExpression++;

					m_arrRpn.Add( CEvalOperand( pszStart, (INT)(pszExpression-pszStart) ) );
					pszExpression++;
				} else
				if ( *pszExpression == '0' && pszExpression[1] == 'x' )
				{
					// number
					UINT dwRet = 0;
					pszExpression += 2;
					INT nDigit = 0;
					while ( (nDigit = HexToInt(*pszExpression)) >= 0 )
					{
						dwRet <<= 4;
						dwRet |= nDigit;
						pszExpression++;
					}

					m_arrRpn.Add( CEvalOperand( (INT)dwRet ) );
				} else
				if ( (*pszExpression >= '0' && *pszExpression <= '9') || (*pszExpression == '.') )
				{
					// number
					char* pszNew = NULL;
					if ( StrIsFloat(pszExpression) )
					{
						FLOAT fValue = StrToFloat(pszExpression, &pszNew);
						if ( pszNew == pszExpression )
						{
							_ASSERT(0);
							return FALSE;
						}
						pszExpression = pszNew;
						m_arrRpn.Add( CEvalOperand( fValue ) );
					} else
					{
						INT lValue = StrToInt(pszExpression, &pszNew);
						if ( pszNew == pszExpression )
						{
							_ASSERT(0);
							return FALSE;
						}
						pszExpression = pszNew;
						m_arrRpn.Add( CEvalOperand( lValue ) );
					}
					if (!pszExpression)	// end of string
						break;
				} else
				{
					// string
					char* pszStart = pszExpression;
					while ( ( *pszExpression >= 'a' && *pszExpression <= 'z' ) || 
							( *pszExpression >= 'A' && *pszExpression <= 'Z' ) || 
							( *pszExpression >= '0' && *pszExpression <= '9' ) || 
							( *pszExpression == '.' ) || 
							( *pszExpression == '_' ) )
					{
						pszExpression++;
					}

					if ( pszExpression - pszStart == 0 )
					{
						return FALSE;
					}

					m_arrRpn.Add( CEvalOperand(pszStart, (INT)(pszExpression-pszStart), CEvalOperand::eoAttribute ) );
				}
				pszExpression--; // trochu hlupe, ale for nam to zinkrementuje
				continue;
			}
			
			// pToken is valid
			if ( pPrevToken != NULL && 
				 pPrevToken != pTokRPar && 
				 pPrevToken->m_ePrecedence != CEvalToken::PrecedenceConst && 
				 pPrevToken->m_ePrecedence != CEvalToken::PrecedenceVar )
			{
				m_arrRpn.Add( CEvalOperand( (INT)0 ) );
			}

			// get precedence
			int precedence = pToken->m_ePrecedence;
			int topPrecedence = 0;
			pszExpression += strlen(pToken->m_pszToken) - 1;

			while ( TRUE )
			{
				CEvalOperand myOper;
				// get top's precedence
				if ( !arrStack.IsEmpty() )
				{
					myOper = arrStack.GetLast();

					if ( myOper.m_eType == CEvalOperand::eoOperator && 
						 myOper.m_Data.m_pOperator->m_ePrecedence >= 0 )
						topPrecedence = myOper.m_Data.m_pOperator->m_ePrecedence;
					else
						topPrecedence = 1;
				}

				if ( arrStack.IsEmpty() || 
					 arrStack.GetLast().Is( pTokRPar ) || 
					 arrStack.GetLast().Is( pTokDelim ) ||
					 precedence > topPrecedence )
				{
					arrStack.Add( pToken );
					break;
				}
				// operator has lower precedence then pop it
				else
				{
					arrStack.RemoveLast();
					m_arrRpn.Add( myOper );
				}
			}
		}

		while ( !arrStack.IsEmpty() )
		{
			if ( arrStack.GetLast().Is( pTokLPar ) ) 
				return FALSE; //eval_unbalanced;
			
			m_arrRpn.Add( arrStack.RemoveLast() );
		}

		return TRUE; 
	}	

	CEvalOperand EvalRpn()
	{
		CEvalOperand arrOperands_[StackLength];
		CArray<CEvalOperand> arrOperands( arrOperands_, StackLength );

		const CEvalToken *pTokDelim = isOperator( (char*)"," );

		for ( int i = 0; i < m_arrRpn.GetSize(); i++ )
		{
			CEvalOperand* pCurrent = &(m_arrRpn[i]);

			if ( pCurrent->m_eType == CEvalOperand::eoOperator && 
				 pCurrent->m_Data.m_pOperator != pTokDelim )
			{
				CEvalOperand opResult = pCurrent->m_Data.m_pOperator->m_pEval( arrOperands );
				if ( opResult.Is( CEvalOperand::eoError ) )
					return opResult;
				arrOperands.Add( opResult );
			}
			else
				arrOperands.Add( *pCurrent );
		}
		
		if( arrOperands.GetSize() == 1 )
		{
			return arrOperands.RemoveLast();
		}

		return CEvalOperand( CEvalOperand::eoError );
	}

};

class CEval : public CEvalCore
{
public:
	static CEvalOperand _Mul( CArray<CEvalOperand>& arrOperands )
	{
		_SAFE( arrOperands.GetSize() >= 2 );
		FLOAT fResult = arrOperands[-2].GetFloat() * arrOperands[-1].GetFloat();
		arrOperands.Resize(-2);
		return fResult;
	}
	static CEvalOperand _Div( CArray<CEvalOperand>& arrOperands )
	{
		_SAFE( arrOperands.GetSize() >= 2 );
		_SAFE( arrOperands[-1].GetFloat() != 0.0f );
		FLOAT fResult = arrOperands[-2].GetFloat() / arrOperands[-1].GetFloat();
		arrOperands.Resize(-2);
		return fResult;
	}
	static CEvalOperand _Add( CArray<CEvalOperand>& arrOperands )
	{
		_SAFE( arrOperands.GetSize() >= 2 );
		return arrOperands.RemoveLast().GetFloat() + arrOperands.RemoveLast().GetFloat();
	}
	static CEvalOperand _Sub( CArray<CEvalOperand>& arrOperands )
	{
		_SAFE( arrOperands.GetSize() >= 2 );
		FLOAT fResult = arrOperands[-2].GetFloat() - arrOperands[-1].GetFloat();
		arrOperands.Resize(-2);
		return fResult;
	}

	static CEvalOperand _Set( CArray<CEvalOperand>& arrOperands )
	{
		_SAFE( arrOperands.GetSize() >= 2 );
		_SAFE( arrOperands[-2].Is( CEvalOperand::eoVariable ) );
		arrOperands[-2].m_Data.m_pVariable->Set( arrOperands[-1] );
		arrOperands.Resize(-2);
		return CEvalOperand( CEvalOperand::eoNone );
	}

	static const CEvalToken* getOperators()
	{
		static const CEvalToken myTokens[] = 
		{
			{ "(", -1, NULL },
			{ ")", -1, NULL },
			{ ",", -1, NULL },
			{ ";", -1, NULL },
			{ "=", -1, _Set },

			{ "*", CEvalToken::PrecedenceMul, _Mul },
			{ "/", CEvalToken::PrecedenceMul, _Div },
			{ "+", CEvalToken::PrecedenceAdd, _Add },
			{ "-", CEvalToken::PrecedenceAdd, _Sub },

			{ NULL, -1, NULL }
		};
		return myTokens;
	}

	const CEvalToken* isOperator( char* pszExpression )
	{
		const CEvalToken *pFind = getOperators();
		const CEvalToken *pBest = NULL;

		// ak to je func/var, hladat najdlhsiu
		for (; pFind->m_pszToken; pFind++)
			if ( strncmp( pszExpression, pFind->m_pszToken, strlen(pFind->m_pszToken) ) == 0 )
			{
				if ( pFind->m_ePrecedence == CEvalToken::PrecedenceFunc ||
					 pFind->m_ePrecedence == CEvalToken::PrecedenceVar )
				{
					if ( !pBest || strlen(pFind->m_pszToken) > strlen(pBest->m_pszToken) )
						pBest = pFind;
				} else
					return pFind;
			}

		return pBest;
	}

};
