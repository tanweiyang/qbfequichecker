/**
 * @file verilog_parser.cpp
 * @author Wei Yang Tan
 * @date 4 Dec 2012
 */

#include "verilogparser.h"
#include <fstream>
#include <sstream>

namespace QBFEquiChecker_NS
{

std::string trimWhiteSpace(const std::string& input)
 {
     unsigned int i = 0, j = 0;
     for(i = 0; i < input.length() && isspace(input[i]); ++i){}
     for(j = input.length(); j > 0 && isspace(input[j - 1]); --j){}

     assert(j >= i);
     return input.substr(i, j - i);
 }


VerilogParser::VerilogParser()
{
}


std::list<std::string> parseKeyWord(std::string inputString, const std::string& keyword)
{
    std::list<std::string> result;

    size_t first_index = inputString.find(keyword);
    if(first_index != std::string::npos)
    {
        first_index += keyword.length();
        size_t second_index = inputString.find(';', first_index);

        if(second_index != std::string::npos)
        {
            std::string extractedStr = inputString.substr(first_index, second_index - first_index);

            first_index = 0;
            second_index = extractedStr.find(',', first_index);
            while(second_index != std::string::npos)
            {
                std::string tmp = trimWhiteSpace(extractedStr.substr(first_index, second_index - first_index));
                result.push_back(tmp);

                first_index = second_index + 1;
                second_index = extractedStr.find(',', first_index);
            }
            second_index = extractedStr.find(';', first_index);
            std::string tmp = trimWhiteSpace(extractedStr.substr(first_index, second_index - first_index));
            result.push_back(tmp);

        }
        else
        {
            assert(0);
        }
    }

    return result;
}


std::map<std::string, std::string> parseAssignWires(std::string inputString)
{
    const char *KEY_WORD = "assign";
    const int KEY_LEN = 6;

    std::map<std::string, std::string> results;

    size_t first_index = inputString.find(KEY_WORD);
    while(first_index != std::string::npos)
    {
        first_index += KEY_LEN;

        size_t second_index = inputString.find('=', first_index);
        size_t third_index = inputString.find(';', second_index);

        if(second_index == std::string::npos ||
           third_index == std::string::npos)
        {
            assert(0);
        }

        std::string outVar = trimWhiteSpace(
            inputString.substr(first_index, second_index - first_index));

        std::string formula = trimWhiteSpace(
            inputString.substr(second_index + 1, (third_index - second_index) - 1));

        results.insert(std::make_pair(outVar, formula));

        inputString = inputString.erase(0, third_index);

        first_index = inputString.find(KEY_WORD);
    }

    return results;
}


bool VerilogParser::parseFile(const char *filename)
{
    static const char *INPUT = "input";
    static const char *OUTPUT = "output";

    std::ifstream fileStream(filename);
    std::string tmpStr;
    std::stringbuf sb(tmpStr);

    if(fileStream.fail())
        return false;

    fileStream >> &sb;
    tmpStr = sb.str();

    this->input_wires_ = parseKeyWord(tmpStr, INPUT);
    this->output_wires_ = parseKeyWord(tmpStr, OUTPUT);
    this->wires_formulas_ = parseAssignWires(tmpStr);

    fileStream.close();

    return true;
}


void VerilogParser::reset()
{
    wires_formulas_.clear();
    input_wires_.clear();
    output_wires_.clear();
}

} // namespace QBFEquiChecker_NS
