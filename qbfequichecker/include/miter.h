/**
 * @file miter.h
 * @author Wei Yang Tan
 * @date 28 Nov 2012
 */

#ifndef MITER_H
#define MITER_H


#include "qbfequichecker_common.h"
#include "uncopyable.h"
#include <string>
#include <map>
#include <vector>


namespace QBFEquiChecker_NS
{

/**
 * @class Miter
 *
 * Creates a miter
 */
class Miter : private Uncopyable
{
    public:
        Miter();
        ~Miter();


        /**
         * Add a new output wire and its formula. Note that if you are adding
         * an outputWire that has already existed, it will just "AND" the
         * previous formula.
         *
         * @param outputWire
         * @param formula
         * @return false if the input strings are not well-formed (e.g. empty
         *         strings)
         */
        bool addFormula(const std::string& outputWire,
                        const std::string& formula);


        /**
         * Apply a nxor onto the formula of outputWire1 and outputWire2. These
         * new formulas will be used conjunctively as the output for
         * satisfiability checking
         *
         * @param outputWire1
         * @param outputWire2
         * @return false if either parameter is empty string or if either
         *         parameters contain invalid tokens
         */
        bool connectOutputWires(const std::string& outputWire1,
                                const std::string& outputWire2);


        /**
         * Replace inputWire1 string with inputWire2 for all the added formula
         *
         * @param inputWire1
         * @param inputWire2
         * @return false if either parameter is empty string or if either
         *         parameters contain invalid tokens
         */
        bool connectInputWires(const std::string& inputWire1,
                               const std::string& inputWire2);


        /**
         * @return "AND" all formula; string replacement
         */
        std::string generateMiterFormula() const;


        std::vector<std::string> getConnectedInputWires() const;


        /**
         * Force the input wire to be true or false by adding a new unit clause
         */
        void setInputWireValue(const std::string& wireLabel, bool value);


        /**
         * @return "AND" all formula; use double implication
         */
        //std::string generateMiterFormula2() const;


        /**
         * Clear all the data
         */
        void clear();


        /// debug
        std::map<std::string, std::string>& getMap();


    private:
        struct PImpl_;
        PImpl_ *pImpl_;
};


}


#endif // MITER_H
