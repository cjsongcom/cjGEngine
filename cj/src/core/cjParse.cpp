
#include "cjCorePCH.h"
#include "cjParse.h"


//
// cjStrCmdLine
//

cjStrCmdLine::cjStrCmdLine(cjINT32 argc, char** argv)
{
	cjString uniCmd;

	for(cjINT32 i=0; i<argc; i++)
	{
		if(!argv[i] || !*argv[i])
			continue;

		cjChar2Uni c2u(argv[i]);
		uniCmd.AddSuffix(*c2u);
	}

    Init(uniCmd.GetStr());
}

cjStrCmdLine::cjStrCmdLine(const cjUni* cmdLine)
{
    Init(cmdLine);
}

cjStrCmdLine::~cjStrCmdLine()
{
	cjMemDeAlloc(m_argUsed);

    if(m_cmdline)
    {
		cjMemDeAlloc(m_argv);
        cjMemDeAlloc(m_cmdline);
    }
}

void cjStrCmdLine::Init(const cjUni* cmdLine)
{
    struct ArgStr
    {
        cjUni*			txt;
        ArgStr*			next;
    };

    m_argc = 0;
    m_argv = 0;
    m_argUsed = 0;
    m_cmdline = 0;

    if(!cmdLine || !cjStrlen(cmdLine))
        return;

    size_t len = cjStrlen(cmdLine) + 1;
    m_cmdline = (cjUni*)cjMemAlloc(sizeof(cjUni) * len);
    cjStrcpy(m_cmdline, (cjUINT32)len, cmdLine);

	// remove '\r','\n'
    for(cjINT32 i=0; i<cjStrlen(cmdLine)+1; i++)
    {
        if(m_cmdline[i] == '\r' || m_cmdline[i] == '\n')
            m_cmdline[i] = ' ';
    }

    cjUni* token = m_cmdline;
    ArgStr* argList = 0;

    while(token && *token)
    {
        m_argc++;

        ArgStr* arg = (ArgStr*)cjMemAlloc(sizeof(ArgStr));

        arg->next = argList;
        argList = arg;

        if(token[0] == '\"')
        {
            argList->txt = ++token;
            while(*token && *token != '\"')
                token++;
        }
        else
        {
            argList->txt = token;
            while(*token && *token != ' ' && *token != '\t')
                token++;
        }

        if(*token)
            *token++ = 0;

        while(*token && (*token == ' ' || *token == '\t'))
            token++;
    }

    m_argc++;
    m_argv = (cjUni**)cjMemAlloc(sizeof(cjUni*) * m_argc);
    m_argv[0] = m_cmdline;

    cjINT32 i = m_argc-1;

    while(argList)
    {
        m_argv[i--] = argList->txt;

        ArgStr* next = argList->next;
        cjMemDeAlloc(argList);

        argList = next;
    }

    m_argUsed = (cjBool*)cjMemAlloc(sizeof(cjBool) * m_argc);
    cjMemSet(m_argUsed, cjFalse, sizeof(cjBool) * m_argc);
}

cjINT32 cjStrCmdLine::GetAvailableArgCount()
{
    for(cjINT32 k=1; k<m_argc; k++)
    {
        if(!m_argUsed[k])
            return k;
    }

    return 0;
}


cjINT32 cjStrCmdLine::GetBool(const cjUni* name)
{
    cjBool v = cjFalse;
    return GetBool(name, v);
}

cjINT32 cjStrCmdLine::GetBool(const cjUni* name, cjBool& v)
{
    cjINT32 matchIndex = 0;
    v = cjFalse;

    for(cjINT32 i=1; i<m_argc; i++)
    {
        cjUni* p = m_argv[i];

        if(!m_argUsed[i] && p[0] == '-' && !cjStrcmp(name, ++p))
        {
            m_argUsed[i] = cjTrue;
            matchIndex = i;
            v = cjTrue;

            break;
        }
    }

    return matchIndex;
}

cjINT32 cjStrCmdLine::GetInt(const cjUni* name, cjINT32& v)
{
    cjINT32 matchIndex = 0;

    for(cjINT32 k=1; k<m_argc; k++)
    {
        cjUni* p = m_argv[k];

        if(!m_argUsed[k] && p[0] == '-' && !cjStrcmp(name, ++p))
        {
            p = m_argv[k+1];

            if(m_argUsed[k+1] || (p[0] == '-' && !(p[1] >= '0' && p[1] <= '9')))
                return 0;
			
			v = cjAtoi(p);

			m_argUsed[k] = cjTrue;
            m_argUsed[k+1] = cjTrue;
            matchIndex = k;

            break;
        }
    }

    return matchIndex;
}

cjINT32 cjStrCmdLine::GetFloat(const cjUni* name, cjFLOAT& v)
{
    cjINT32 matchIndex = 0;

    for(cjINT32 k=1; k<m_argc; k++)
    {
        cjUni* p = m_argv[k];

        if(!m_argUsed[k] && p[0] == '-' && !cjStrcmp(name, ++p))
        {
            p = m_argv[k+1];

            if( m_argUsed[k+1] || (p[0] == '-' && !(p[1] >= '0' && p[1] <= '9')) )
                return 0;

			v = cjAtof(p);

            m_argUsed[k] = cjTrue;
            m_argUsed[k+1] = cjTrue;

            matchIndex = k;
            break;
        }
    }

    return matchIndex;
}

cjINT32 cjStrCmdLine::GetStr(const cjUni* name, cjUni* v, cjUINT32 bufLen)
{
    cjINT32 matchIndex = 0;
    for(cjINT32 k=1; k<m_argc; k++)
    {
        cjUni* p = m_argv[k];

        if(!m_argUsed[k] && p[0] == '-' && !cjStrcmp(name, ++p))
        {
            p = m_argv[k+1];

            if(m_argUsed[k+1] || p[0] == '-')
                return 0;

            cjStrcpy(v, bufLen, p);

            m_argUsed[k] = cjTrue;
            m_argUsed[k+1] = cjTrue;
            matchIndex = k;

            break;
        }
    }

    return matchIndex;
}

cjINT32 cjStrCmdLine::GetFilename(cjUni* filename, cjUINT32 bufLen)
{
    cjINT32 matchIndex = 0;
    for(cjINT32 k=1; k<m_argc; k++)
    {
        cjUni* p = m_argv[k];

        if(!m_argUsed[k] && p[0] != '-')
        {
            cjStrcpy(filename, bufLen, p);
            m_argUsed[k] = cjTrue;
            matchIndex = k;

            break;
        }
    }

    return matchIndex;
}


//
// String Parse
//

CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjBYTE& value)
{
	const cjNameChar* tmp = cjStrifind(txt, match);

	if(!tmp)
		return cjFalse;

	tmp += cjStrlen(match);
	value = (cjBYTE)cjAtoi(tmp);

	return value !=0 || cjIsDigit(tmp[0]) ? cjTrue : cjFalse;
}

CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjSBYTE& value)
{
	const cjNameChar* tmp = cjStrifind(txt, match);

	if(!tmp)
		return cjFalse;

	tmp += cjStrlen(match);
	value = (cjSBYTE)cjAtoi(tmp);

	return value !=0 || cjIsDigit(tmp[0]) ? cjTrue : cjFalse;
}

CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjWORD& value)
{
	const cjNameChar* tmp = cjStrifind(txt, match);

	if(!tmp)
		return cjFalse;

	tmp += cjStrlen(match);
	value = (cjWORD)cjAtoi(tmp);

	return value!=0 || cjIsDigit(tmp[0]) ? cjTrue : cjFalse;
}

CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjSWORD& value)
{
	const cjNameChar* tmp = cjStrifind(txt, match);

	if(!tmp)
		return cjFalse;

	tmp += cjStrlen(match);
	value = (cjSWORD)cjAtoi(tmp);

	return value!=0 || cjIsDigit(tmp[0]) ? cjTrue : cjFalse;
}

CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjUINT32& value)
{
	const cjNameChar* tmp = cjStrifind(txt, match);

	if(!tmp) 
		return cjFalse;

	value = cjStrtoui(tmp + cjStrlen(match), cjNULL, 10);

	return cjTrue;
}

CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjUINT64& value)
{
	return cjParse(txt, match, *(cjINT64*)&value);
}

CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjINT64& value)
{
	cjNameChar tmp[4096]; tmp[0] = cjNULL;
	cjNameChar* cur = tmp;

	if(::cjParse(txt, match, tmp, cjARRAYCNT(tmp)))
	{
		value = 0;

		// '-' for negative sign
		cjBool negSign = (*cur == '-');
		cur += negSign ? 1 : 0;

		while(*cur>='0' && *cur<='9')
			value = value*10 + *cur++ - '0'; // '1' - '0' = 1

		if(negSign)
			value = -value;

		return cjTrue;
	}
	else 
		return cjFalse;
}

CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjINT32& value)
{
	const cjNameChar* tmp = cjStrifind(txt, match);
	if(!tmp) return cjFalse;

	value = cjAtoi(tmp + cjStrlen(match));

	return cjTrue;
}

CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjFLOAT& value)
{
	const cjNameChar* tmp = cjStrifind(txt, match);

	if(!tmp) return cjFalse;

	value = cjAtof(tmp+cjStrlen(match));

	return cjTrue;
}

CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjName& name)
{
	cjNameChar tmpStr[cjNameSiz];

	if(!::cjParse(txt, match, tmpStr, cjNameSiz))
		return cjFalse;

	name = cjName(tmpStr);

	return cjTrue;
}

CJCORE_API cjBool cjParsecjBool(const cjNameChar* txt, const cjNameChar* match, cjBool& rstBool)
{
	cjNameChar tmpStr[16];

	if(cjParse(txt, match, tmpStr, 16))
	{
		rstBool = !cjStricmp(tmpStr, _CT("True")) || !cjStricmp(tmpStr, _CT("1"));

		return cjTrue;
	}
	else 
		return cjFalse;
}

CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjNameChar* value,
	cjINT32 maxLen, cjNameChar* allowedChars)
{
	const cjNameChar* found = cjStrifind(txt, match);
	const cjNameChar* start;

	if(found)
	{		
		start = found + cjStrlen(match);

		if(*start == CJ_ASCII_QUOTATION_MARK) 
		{
			// starting with '"'
			const cjNameChar* pos = start + 1;
			cjINT32 i;

			// find next '"'
			for(i=0;i<maxLen-1 && *pos; i++,pos++)
			{
				if(*pos == CJ_ASCII_QUOTATION_MARK)
					break;

				if(*pos == '\\' 
					&& (*(pos+1)=='\\' || *(pos+1)==CJ_ASCII_QUOTATION_MARK))
				{
                    pos++;

					if(!*pos)
						break;
				}

				value[i] = *pos;
			}

			value[i] = 0;
		}
		else
		{
			cjStrncpy(value, start, maxLen);
			value[maxLen-1] = 0;

			cjNameChar* temp;

			if(allowedChars)
			{
				temp = cjStrifind(allowedChars, _CT(" ")  ) == cjNULL
					? cjStrstr(value, _CT(" "))  : cjNULL; if(temp) *temp=0;

				temp = cjStrifind(allowedChars, _CT("\r") ) == cjNULL
					? cjStrstr(value, _CT("\r")) : cjNULL; if(temp) *temp=0;

				temp = cjStrifind(allowedChars, _CT("\n") ) == cjNULL
					? cjStrstr(value, _CT("\n")) : cjNULL; if(temp) *temp=0;

				temp = cjStrifind(allowedChars, _CT("\t") ) == cjNULL
					? cjStrstr(value, _CT("\t")) : cjNULL; if(temp) *temp=0;

				temp = cjStrifind(allowedChars, _CT(",")  ) == cjNULL
					? cjStrstr(value, _CT(","))  : cjNULL; if(temp) *temp=0;
			}
			else
			{				
				temp = cjStrstr(value, _CT(" "));  if(temp) *temp=0;
				temp = cjStrstr(value, _CT("\r")); if(temp) *temp=0;
				temp = cjStrstr(value, _CT("\n")); if(temp) *temp=0;
				temp = cjStrstr(value, _CT("\t")); if(temp) *temp=0;
				temp = cjStrstr(value, _CT(","));  if(temp) *temp=0;
			}
		}

		return cjTrue;
	}
	else 
		return cjFalse;

}

CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjString& value, 
	cjNameChar* allowedChars)
{
	cjNameChar tmp[4096]; tmp[0] = NULL;

	if(cjParse(txt, match, tmp, cjARRAYCNT(tmp), allowedChars))
	{
		value = tmp;

		return cjTrue;
	}
	else 
		return cjFalse;
}

// '|' for ParseLine,  ex: "commandA | command B"
CJCORE_API cjBool cjParseLine(const cjNameChar** txt, cjNameChar* rst, cjINT32 maxLen, 
	cjBool exactMatch)
{
	if(!txt || !(*txt) || !(**txt))
		return cjFalse;

	cjBool isParsed = cjFalse;
	cjBool isQuoted = cjFalse;
	cjBool ignoreLine = cjFalse;

	*rst=0;

	// skip enter, line-feed(10), return(13)
	while(**txt!=0 && **txt!=10 && **txt!=13 && (--maxLen>0))
	{
		// skip current txt line that is started with '//'
		if(!isQuoted && !exactMatch && (*txt)[0]=='/' && (*txt)[1]=='/' )
			ignoreLine = cjTrue;
		
		// handle line separator 
		// "commandA | command B"
		if(!isQuoted && !exactMatch && **txt=='|')
			break;

		isQuoted = isQuoted ^ (**txt==34); // 34="
		isParsed = cjTrue;

		// storing 
		if(!ignoreLine)
			*(rst++) = *((*txt)++);
		else
			(*txt)++; // skip comment line
	}

	if(exactMatch)
	{
		// skip return/linefeed
		if( **txt == 13) (*txt)++;
		if( **txt == 10) (*txt)++;
	}
	else
	{
		// skip line-feed/return/|(line separator)
		while(**txt==10 || **txt==13 || **txt=='|')
			(*txt)++;
	}

	*rst=0;

	return **txt !=0 || isParsed ? cjTrue : cjFalse;
}

CJCORE_API cjBool cjParseLine(const cjNameChar** txt, cjString& rst, cjBool exactMatch)
{
	cjNameChar buf[2048]; buf[0] = NULL;

	cjBool parsed = cjParseLine(txt, buf, cjARRAYCNT(buf), exactMatch);
	rst = buf;

	return parsed;
}

CJCORE_API cjBool cjParseCommand(const cjNameChar** txt, const cjNameChar* match)
{
	// skip spc and tab
	while((**txt==' ') || (**txt==9))
		(*txt)++;

	if(!cjStrnicmp(*txt, match, cjStrlen(match)))
	{
		*txt += cjStrlen(match);
		
		if(!cjIsAlphaNumeric(**txt))
		{
			// skip spc and tab
			while ((**txt==' ') || (**txt==9)) 
				(*txt)++;

			return cjTrue;
		}
		else
		{
			// still founded alpha-numeric char
			*txt -= cjStrlen(match);

			return cjFalse; // partial match.
		}
	}
	else 
		return cjFalse; // no match.
}

CJCORE_API cjBool cjParseToken(const cjNameChar** txt, cjNameChar* rst, cjINT32 maxLen, 
	cjBool useEscape)
{
	cjINT32 len=0;

	// skip spc, tab
	while(**txt==' ' || **txt==9) (*txt)++;

	if(**txt == 34) // 34 = "
	{
		// start with quoted
		(*txt)++;

		// until match next quot or reach to maxlen
		while(**txt && **txt != 34 && (len+1)<maxLen)
		{
			cjNameChar c = *((*txt)++);

			// met escape
			if(c == '\\' && useEscape)
			{
				c = *((*txt)++);

				if(!c)
					break;
			}

			if((len+1) < maxLen)
				rst[len++] = c;
		}

		if(**txt == 34)
			*txt++;
	}
	else
	{
		// unquoted string.
		// skip spc and tab
		for( ; **txt && **txt!=' ' && **txt!=9; (*txt)++)
		{
			if((len+1) < maxLen) 
				rst[len++] = **txt;
		}
	}

	rst[len]=0;

	return len != 0 ? cjTrue : cjFalse;
}

CJCORE_API cjBool cjParseToken(const cjNameChar** txt, cjString& rst, cjBool useEscape)
{
	cjNameChar buf[1024]; buf[0] = cjNULL;

	if(cjParseToken(txt, buf, cjARRAYCNT(buf), useEscape))
	{
		rst = buf;
		return cjTrue;
	}

	return cjFalse;
}

CJCORE_API cjString cjParseToken(const cjNameChar** txt, cjBool useEscape)
{
	cjNameChar buf[1024]; buf[0] = cjNULL;

	if(cjParseToken(txt, buf, cjARRAYCNT(buf), useEscape))
		return buf;
	else
		return _CT("");
}

CJCORE_API void cjParseSkipSpcTabCRLF(const cjNameChar** txt)
{
	// skip spc,tab,return,line feed
	while(**txt==' ' || **txt==9 || **txt==13 || **txt==10)
		++*txt;
}

