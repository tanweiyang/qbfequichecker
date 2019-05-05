/**
 * @file formulaparserexception.cpp
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */

#include "formulaparserexception.h"


namespace QBFEquiChecker_NS
{

FormulaParserException::FormulaParserException(const char *msg) throw() :
	std::exception(), msg_("FormulaParserException: ")
{	
	msg_ += msg;
}

FormulaParserException::FormulaParserException(const std::string& msg) throw() :
	std::exception(), msg_("FormulaParserException: ")
{	
	msg_ += msg;
}

FormulaParserException::~FormulaParserException() throw()
{
}


const char* FormulaParserException::what() const throw()
{
	return msg_.c_str();
}


} // namespace QBFEquiChecker_NS

