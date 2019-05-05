/**
 * @file formulaparserexception.h
 * @author Wei Yang Tan
 * @date 21 Nov 2012
 */


#ifndef FORMULAPARSEREXCEPTION_H
#define FORMULAPARSEREXCEPTION_H


#include <exception>
#include <string>


namespace QBFEquiChecker_NS
{
	
class FormulaParserException : public std::exception
{
	public:
		FormulaParserException(const char *msg) throw();
		FormulaParserException(const std::string& msg) throw();

		virtual ~FormulaParserException() throw();

		virtual const char *what() const throw();


	private:
		std::string msg_;
		
};

} // namespace QBFEquiChecker_NS


#endif // FORMULAPARSEREXCEPTION_H

