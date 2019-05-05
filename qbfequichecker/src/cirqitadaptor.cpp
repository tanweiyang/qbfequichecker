/**
 * @file cirqitadaptor.cpp
 * @date Dec 1 2012
 * @author Wei Yang Tan
 *
 */

#include "cirqitadaptor.h"

#ifdef USE_SOLVER_CIRQIT

#include "Common.h"
#include "Circuit.h"
#include "Parser.h"
#include "NodeFactory.h"
#include "Solver.h"

#include <fstream>
#include <iostream>
#include <cassert>


bool verbose;
bool silent;


namespace QBFEquiChecker_NS
{

const char *CirQitAdaptor::ISCAS_FILE = "cirqitadaptor.iscas";


struct CirQitAdaptor::PImpl_
{
};

CirQitAdaptor::CirQitAdaptor() :
        pImpl_(new CirQitAdaptor::PImpl_)
{
}

CirQitAdaptor::~CirQitAdaptor()
{
    delete pImpl_;
}

void CirQitAdaptor::solve(const char *iscas_file)
{
    /*************************************************************************
     Process arguments
     *************************************************************************/
    // (*) !!!
    // sigh... if only they have const quantifier, which should be the case
    char *filename = const_cast<char *>(iscas_file);
    char *proof_file = NULL;

    verbose = false;
    bool print_iscas = false;
    int solver_time_out = -1; // (*) negative = no time out

    Common::larify = false;
    Common::qpro = 0;

    /*************************************************************************
     Parse and preprocess
     *************************************************************************/
    Circuit circuit;

    clock_t start, end;
    start = clock();

    Common::timeout = solver_time_out;
    Common::start_time = start;

    bool timeout = false;
    bool solvable;
    solvable = false;

    try
    {
        initNodeFactory();

        if (!parse(filename, circuit))
        {
            assert(false);
            return;
        }

        Common::checkTime();

        end = clock();
        start = clock();

        if (Common::value == VALUE_UNASSIGNED)
        {
            if (!circuit.preprocess())
                return;
        }

        Common::checkTime();
        end = clock();

//        if (TO_QBF && Common::value == VALUE_UNASSIGNED)
//        {
//            circuit.printQBF();
//            return;
//        }

        if ((print_iscas || Common::do_proof)
                && Common::value == VALUE_UNASSIGNED)
        {
            std::ofstream fout;
            fout.open(iscas_file);

            if (!fout.is_open())
            {
                throw(" can't open ISCAS file");
                return;
            }

            circuit.printIscas(fout);

            fout.close();
            if (print_iscas)
            {
                // don't solve
                return;
            }
        }

        if (Common::do_proof && Common::value == VALUE_UNASSIGNED)
        {
            Common::proof_file.open(proof_file);
            if (!Common::proof_file.is_open())
            {
                throw(" can't open proof file");
                return;
            }
        }

        /*************************************************************************
         Solve
         *************************************************************************/

        Solver solver(&circuit, verbose, solver_time_out);

        start = clock();
        if (Common::value == VALUE_UNASSIGNED)
            solvable = solver.solve();
        else
        {
            if (!silent)
                std::cerr << "Solved by preprocessing\n";
            assert(Common::value == VALUE_TRUE || Common::value == VALUE_FALSE);
            solvable = (Common::value == VALUE_TRUE);
        }
        end = clock();
        if (!silent)
        {
            std::cerr << "solve time: "
                    << ((double) (end - start)) / (double) CLOCKS_PER_SEC
                    << std::endl;
            if (solvable)
            {
                std::cerr << "solution found" << std::endl;
            }
            else
            {
                std::cerr << "no solution found" << std::endl;
            }

            solver.printStats();
        }
    } catch (int ) //(*) TODO: sensible exception handling
    {
        timeout = true;
        if (!silent)
            std::cerr << "time out" << std::endl;
    }

    if (Common::do_proof)
    {
        Common::proof_file.close();
    }

    deleteAllNodes();
}


SolverResult_Type CirQitAdaptor::solve(
        const FormulaNode& formulaNode,
        const std::list< std::pair<Index_t, Quantifier_Type> >& quantVarList,
        const Size_Type& /*numOfVarNodes*/,
        std::map<Index_t, Logic_t>& varValues)
{
    std::string outStr;
    assert(0);
    //FormulaNode& node = formulaNode.convertISCAS(outStr);
}

} // namespace QBFEquiChecker_NS


#endif // USE_SOLVER_CIRQIT
