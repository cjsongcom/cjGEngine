
#ifndef _CJPARSE_H_
#define _CJPARSE_H_



//
// Parse Utility
//

class CJCORE_API cjStrCmdLine : public cjMem
{
public:
    cjStrCmdLine(cjINT32 argc, char** argv);
    cjStrCmdLine(const char* cmdLine);
	cjStrCmdLine(const cjUni* cmdLine);

    ~cjStrCmdLine();

    cjINT32 GetAvailableArgCount();

	// ret: matchIndex, 0 for invalid
    cjINT32 GetBool(const cjUni* name);
    cjINT32 GetBool(const cjUni* name, cjBool& v);
    cjINT32 GetInt(const cjUni* name, cjINT32& v);
    cjINT32 GetFloat(const cjUni* name, cjFLOAT& v);
    cjINT32 GetStr(const cjUni* name, cjUni* v, cjUINT32 bufLen);
    cjINT32 GetFilename(cjUni* filename, cjUINT32 bufLen);

protected:
    void Init(const cjUni* cmdLine);

    cjINT32							m_argc;
    cjUni**							m_argv;		// token array
    cjUni*							m_cmdline;

    cjBool*							m_argUsed;
};



//
// String Parse
//

CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjBYTE& value);
CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjSBYTE& value);
CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjWORD& value);
CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjSWORD& value);
CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjUINT32& value);
CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjINT32& value);
CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjUINT64& value);
CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjINT64& value);

CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjFLOAT& value);
CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjName& name);
CJCORE_API cjBool cjParsecjBool(const cjNameChar* txt, const cjNameChar* match, cjBool& rstBool);

// allowChars : " ", "\r", "\n", "\t", ","
// allowChars = NULL -> automatically remove " ", "\r", "\n", "\t", ","
CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjNameChar* value, cjINT32 maxLen, cjNameChar* allowedChars=NULL);
CJCORE_API cjBool cjParse(const cjNameChar* txt, const cjNameChar* match, cjString& value, cjNameChar* allowedChars=NULL);

// '|' for ParseLine,  ex: "commandA | command B"
// cjTrue  : parsed line and continue , cjFalse : met end of line in parsing
CJCORE_API cjBool cjParseLine(const cjNameChar** txt, cjNameChar* rst, cjINT32 maxLen, cjBool exactMatch=0);
CJCORE_API cjBool cjParseLine(const cjNameChar** txt, cjString& rst, cjBool exactMatch=0);
// 
CJCORE_API cjBool cjParseCommand(const cjNameChar** txt, const cjNameChar* match);

// skip '\\' in txt when useEscape is true
CJCORE_API cjBool cjParseToken(const cjNameChar** txt, cjNameChar* rst, cjINT32 maxLen, cjBool useEscape);
CJCORE_API cjBool cjParseToken(const cjNameChar** txt, cjString& rst, cjBool useEscape);
CJCORE_API cjString cjParseToken(const cjNameChar** txt, cjBool useEscape);

CJCORE_API void cjParseSkipSpcTabCRLF(const cjNameChar** txt);



#endif

