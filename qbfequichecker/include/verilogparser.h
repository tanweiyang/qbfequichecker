/**
 * @file verilogparser.h
 * @author Wei Yang Tan
 * @date 24 Nov 2012
 */

#ifndef VERILOGPARSER_H
#define VERILOGPARSER_H


#include "qbfequichecker_common.h"
#include <string>
#include <map>
#include <list>


namespace QBFEquiChecker_NS
{

/**
 * @class VerilogParser
 *
 * The class for implementing some simple parsing of verilog files
 */
class VerilogParser
{
    public:
        VerilogParser();

        /**
         * Note: Does not support array of wires yet
         * @param filename
         * @return
         */
        bool parseFile(const char *filename);

        inline const std::list<std::string>& getInputWires() const
        {
            return input_wires_;
        }


        inline const std::list<std::string>& getOutputWires() const
        {
            return output_wires_;
        }


        inline const std::map<std::string, std::string> getWiresFormulas() const
        {
            return wires_formulas_;
        }


        void reset();

    private:
        std::map<std::string, std::string> wires_formulas_;
        std::list<std::string> input_wires_;
        std::list<std::string> output_wires_;
};


} // namespace QBFEquiChecker_NS


#endif // VERILOGPARSER_H

