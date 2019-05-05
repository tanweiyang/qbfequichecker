/**
 * @file miter.cpp
 *
 * @date Nov 28, 2012
 * @author tanweiyang
 */

#include "miter.h"
#include <map>
#include <utility>
#include <algorithm>
#include <vector>
#include <cassert>


namespace QBFEquiChecker_NS
{

bool isValidWireName(const std::string& wireName);


class FindAndReplaceString
{
    public:
        FindAndReplaceString(const std::string& strToBeReplaced,
                             const std::string& replacingStr) :
            strToBeReplaced_(strToBeReplaced),
            replacingStr_(replacingStr),
            has_replaced_(false)
        {
        }

        bool hasReplaced() const
        {
            return has_replaced_;
        }

        size_t findNameTokenIndex(std::string& inputString)
        {
            size_t index = inputString.find(strToBeReplaced_);

            if(index == std::string::npos)
            {
                return index;
            }

            if(index > 0 && isVarName(inputString[index - 1]))
            {
                return std::string::npos;
            }

            if(index < inputString.length() && isVarName(inputString[index + 1]))
            {
                return std::string::npos;
            }

            return index;
        }


        void operator()(std::string& formula)
        {
            size_t len = strToBeReplaced_.length();

            // find is not enough!!! must make sure it is not a substring
            // of another wire name!
            size_t index = formula.find(strToBeReplaced_);

            while(index != std::string::npos)
            {
                if((index == 0 || !isVarName(formula[index - 1])) &&
                   (index + len == formula.length() || !isVarName(formula[index + len])))
                {
                    formula = formula.replace(index, len, replacingStr_);
                    has_replaced_ = true;
                }

                index = formula.find(strToBeReplaced_, index + len);
            }
        }

    private:
        std::string strToBeReplaced_;
        std::string replacingStr_;
        bool has_replaced_;

};


struct Miter::PImpl_ : private Uncopyable
{
    // replace the newly connected miter output internal wires
    // with all the corresponding formula from wire_formula_
    void replace_with_output_wires_formula(std::string& new_formula);


    // key is the output wire name; value is the formula
    std::map<std::string, std::string> wire_formula_;

    // output-nxored formulas
    std::vector<std::string> output_wires_;

    // unit clauses
    std::vector<std::string> unitClauses_;
};


Miter::Miter() : pImpl_(new PImpl_)
{
}


Miter::~Miter()
{
    delete pImpl_;
}


bool Miter::addFormula(const std::string& outputWire,
                       const std::string& formula)
{
    if(outputWire.length() == 0 || formula.length() == 0)
        return false;

    this->pImpl_->wire_formula_.insert(std::make_pair(outputWire, formula));
    return true;
}


bool Miter::connectOutputWires(const std::string& outputWire1,
                               const std::string& outputWire2)
{
    if(isValidWireName(outputWire1) == false ||
       isValidWireName(outputWire2) == false)
    {
        return false;
    }

    std::string tmp = this->pImpl_->wire_formula_[outputWire2];
    std::string tmp2 = this->pImpl_->wire_formula_[outputWire1];

    if(tmp.length() == 0 || tmp2.length() == 0)
        return false;

    tmp = "~((" + tmp2 + ") ^ (" + tmp + "))";

    //this->pImpl_->wire_formula_[outputWire2] = tmp;
    this->pImpl_->wire_formula_.erase(outputWire1);
    this->pImpl_->wire_formula_.erase(outputWire2);
    //this->pImpl_->replace_with_output_wires_formula(tmp);

    this->pImpl_->output_wires_.push_back(tmp);

    return true;
}


bool Miter::connectInputWires(const std::string& inputWire1,
                              const std::string& inputWire2)
{
    if(isValidWireName(inputWire1) == false ||
       isValidWireName(inputWire2) == false)
    {
        return false;
    }

    bool hasReplaced = false;

    for (std::map<std::string, std::string>::iterator iter = this->pImpl_->wire_formula_.begin();
         iter != this->pImpl_->wire_formula_.end();
         ++iter )
    {
        FindAndReplaceString functor(inputWire1, inputWire2);
        functor(iter->second);

        if(functor.hasReplaced() == true)
            hasReplaced = true;
    }


    return hasReplaced;
}


/*
std::string Miter::generateMiterFormula() const
{
    std::string str = "";

    for (std::vector<std::string>::iterator iter = this->pImpl_->output_wires_.begin();
         iter != this->pImpl_->output_wires_.end();
         ++iter)
    {
        // First we want to replace all the strings with other output wires;
        // due to ordering, we need to keep iterating and replacing strings until
        // it hits a fixed point
        std::string old_formula = "";
        size_t i = 0;
        for(i = 0; old_formula != *iter && i <= this->pImpl_->wire_formula_.size(); ++i)
        {
            old_formula = *iter;
            this->pImpl_->replace_with_output_wires_formula(*iter);
        }

//        if(this->pImpl_->wire_formula_.size() != 0 && i > this->pImpl_->wire_formula_.size())
//        {
//            // !!! combinational circuit has feedback loops !!!
//            assert(0);
//        }

        // Then we concatenate all the output boolean formulas
        if(str.length() == 0)
            str = "(" + *iter + ")";
        else
            str = str + " & (" + *iter + ")";
    }

    return str;
}
*/


std::string Miter::generateMiterFormula() const
{
    std::string str = "";

    for (std::map<std::string, std::string>::iterator iter = this->pImpl_->wire_formula_.begin();
         iter != this->pImpl_->wire_formula_.end();
         ++iter)
    {
        // double implication
        std::string lhs = iter->first;
        std::string rhs = iter->second;

        std::string tmp = "((~(" + lhs + ")) | (" + rhs + ")) & ((" + lhs + ") | (~(" + rhs + ")))";

        if(str.length() == 0)
            str = "(" + tmp + ")";
        else
            str = str + " & (" + tmp + ")";
    }

    for (std::vector<std::string>::iterator iter = this->pImpl_->output_wires_.begin();
         iter != this->pImpl_->output_wires_.end();
         ++iter)
    {
        // Then we concatenate all the output boolean formulas
        str = str + " & (" + *iter + ")";
    }


    for (std::vector<std::string>::iterator iter = this->pImpl_->unitClauses_.begin();
         iter != this->pImpl_->unitClauses_.end();
         ++iter)
    {
        str = str + " & (" + *iter + ")";
    }

    return str;
}


void Miter::clear()
{
    this->pImpl_->wire_formula_.clear();
}


std::map<std::string, std::string>& Miter::getMap()
{
    return this->pImpl_->wire_formula_;
}


void Miter::setInputWireValue(const std::string& wireLabel, bool value)
{
    if(value == true)
    {
        this->pImpl_->unitClauses_.push_back(wireLabel);
    }
    else
    {
        std::string tmp = "~" + wireLabel;
        this->pImpl_->unitClauses_.push_back(tmp);
    }
}


void Miter::PImpl_::replace_with_output_wires_formula(std::string& new_formula)
{
    std::map<std::string, std::string>::const_iterator end_iter =
            this->wire_formula_.end();

    // iterate through all the output wires' formula.
    for(std::map<std::string, std::string>::iterator iter = this->wire_formula_.begin();
        iter != end_iter;
        ++iter)
    {
        // search and replace all the output wire name in the new_formula
        FindAndReplaceString functor(iter->first, "(" + iter->second + ")");
        functor(new_formula);
    }
}


bool isValidWireName(const std::string& wireName)
{
    std::string::const_iterator iter =
        std::find_if(wireName.begin(), wireName.end(), isVarName);

    return (wireName.end() != iter);
}


} // namespace QBFEquiChecker_NS

