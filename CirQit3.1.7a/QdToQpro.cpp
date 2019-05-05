#include "Common.h"
#include "QdToQpro.h"
#include <algorithm>

/* Return the supposedly unique existential lit in cl that is not false.
 * 0 means none was found.
 */
int findEimplied(int j, SubFmla &cl, vector<int> &assignedE, Qdepths &scopeof);

void updateOpps(SubFmlas &subfs, Where &occsOpp, vector<int> &litsCnt,
		vector<int> &posCnt, vector<int> &negCnt, int vars, int lit);

void setCnts(int j, SubFmla &cl, Qdepths &scopeof, vector<int> &assignedE,
		int vars, int &minU, int &maxE, int &cnt);

void updateSames(SubFmlas &subfs, Where &occsSame, vector<int> &litsCnt,
		vector<int> &posCnt, vector<int> &negCnt, int vars, int lit);

int findDel(SubFmla &f, int item);

void reduceOccs(SubFmla &cl, vector <int> & posCnt, vector <int> & negCnt,
		int litShorten);

/* Precondition: top, posOccsV, negOccsV are sorted.
 * For clIdx >= vars+2, top[clIdx - vars] = clIdx.
 * Therefore, clsEnd-1 = max_clIdx - vars, or max_clIdx + 1 = clsEnd + vars.
 */
int shrinkAndMove(SubFmlas &subfs, Where &posOccsV, Where &negOccsV,
		SubFmla &top, SubFmla & posAnd, SubFmla & negAnd,
		vector <int> & posCnt, vector <int> & negCnt,
		int var, int clsStart, int clsEnd);

int gatherClauses(SubFmlas &subfs, int subfId, int vars, int clsEnd,
		Where &posOccsV, Where &negOccsV, Qdepths &scopeof,
		vector <int> & posCnt, vector <int> & negCnt,
		int var);



char mypeek(ifstream & infile)
{ char c;
  c = infile.peek();
  return c;}


void setCnts(int j, SubFmla &cl, Qdepths &scopeof, vector<int> &assignedE,
		int vars, int &minU, int &maxE, int &cnt)
{
	int cnt0 = cl[1];
	maxE = 0;
	for (int i = 2; i < cnt0 + 2; i ++)
	{
	    int lit = cl[i];
	    int v = abs(lit);
	    if (assignedE[v] * lit < 0)
		continue;

	    if (scopeof[v] > 0 && maxE < scopeof[v])
		maxE = scopeof[v];
	}
	cnt = cnt0;
	minU = vars + 1;
	for (int i = 2; i < cnt0 + 2; i ++)
	{
	    int lit = cl[i];
	    int v = abs(lit);
	    if (scopeof[v] < 0 && minU > (- scopeof[v]))
		minU = (- scopeof[v]);
	    if (scopeof[v] < 0 && maxE < (- scopeof[v]))
		cnt --;
	}
	return;
}

int findEimplied(int j, SubFmla &cl, vector<int> &assignedE, Qdepths &scopeof)
{
	int implied;
	int cnt0 = cl[1];
	for (int i = 2; i < cnt0 + 2; i ++)
	{
	    int lit = cl[i];
	    int v = abs(lit);
	    int a = (v + lit == 0)? (-1) : 1;
	    if (scopeof[v] > 0 && (assignedE[v] == 0 || assignedE[v] == a))
	    {
		return lit;
	    }
	}
	return 0;
}




int updateOpps(SubFmlas &subfs, Where &occsOpp, Qdepths &scopeof,
		vector<int> &unitQueue, int oldBack, vector<int> &assignedE,
		vector<int> &minUqdepth, vector<int> &maxEqdepth,
		vector<int> &litsCnt, vector<int> &posCnt, vector<int> &negCnt,
		int vars, int lit)
{
	int var = abs(lit);
	int litComp = (- lit);
	int nOpp = occsOpp[1];
	int back = oldBack;

	for (int i = 2; i < nOpp + 2; i++)
	{
	    int clOpp = occsOpp[i];
	    int nLits = litsCnt[clOpp];
	    if (nLits > vars)
		/* already sat */
		continue;

	    SubFmla &cl = subfs[clOpp];
	    litsCnt[clOpp] --;
	    int qdepth = scopeof[var];
	    assert (qdepth >= 0);
	    if (qdepth >= maxEqdepth[clOpp] && qdepth > minUqdepth[clOpp])
		{
		int  minU, maxE, cnt;   // outputs of setCnts()
		cnt = -99;
		setCnts(clOpp, cl, scopeof, assignedE, vars, minU, maxE, cnt);
		litsCnt[clOpp] = cnt;
		maxEqdepth[clOpp] = maxE;
		minUqdepth[clOpp] = minU;
		}
	    if (litsCnt[clOpp] == 0)
	    {
		continue;
	    }

	    if (litsCnt[clOpp] == 1)
	    {
		int implied = findEimplied(clOpp, cl, assignedE, scopeof);
		if (implied == 0)
		{
		    /* eager assignment might be incorrect */
		    continue;
		}
		int v = abs(implied);
		assert (v > 0);
		int a = (v + implied == 0)? (-1) : 1;
		if (assignedE[v] + a == 0)
		{
		    /* contradictory unit clauses */
		    continue;
		}
		if (assignedE[v] == 0)
		{
		    assignedE[v] = a;
		    unitQueue[back] = implied; back ++;
		}
	    }
	}
	return back;
}

void updateSames(SubFmlas &subfs, Where &occsSame, vector<int> &litsCnt,
		vector<int> &posCnt, vector<int> &negCnt, int vars, int lit)
{
	int var = abs(lit);

	if (lit == var)
	{
		if (posCnt[var] == 0)
			return;
		posCnt[var] = 0;
	}
	else
	{
		if (negCnt[var] == 0)
			return;
		negCnt[var] = 0;
	}

	int nSame = occsSame[1];
	for (int i = 2; i < nSame + 2; i++)
	{
		int clSame = occsSame[i];
		assert (clSame > 0);    // must be disjunction here.
		/* clSame is SAT; it disappears if it hasn't already.  */
		if (litsCnt[clSame] > vars)
		    /* I gave at the office */
		    continue;

		litsCnt[clSame] += (2*vars);
		SubFmla &cl = subfs[clSame];
		reduceOccs(cl, posCnt, negCnt, lit);
	}
	return;
}




/* Return 0 if fmla as read contains [] or [x] & [-x], after univ. red.
 * The fmla should be solved instantly without restructuring in these cases.
 * Cases where this occurs after unit prop. are not considered trivial.
 */
int effUnitProp(SubFmlas &subfs, int subfId, int vars,
	Wheres &posOccs, Wheres &negOccs, Qblocks &scopes, Qdepths &scopeof,
	vector<int> &posCnt, vector<int> &negCnt, vector<int> &litsCnt)
{
	int clsEnd = subfs.size();
	int clsStart = abs(subfId)+1;
	int nontrivial;
	vector<int> unitQueue(vars + 2);
	vector<int> assignedE(vars + 2);
	vector<int> minUqdepth(clsEnd + 2);
	vector<int> maxEqdepth(clsEnd + 2);
	nontrivial = 1;
	int front, back;
	front = 0; back = 0;
	for (int j = clsStart; j < clsEnd; j++)
	{
	    int  minU, maxE, cnt;   // outputs of setCnts()
	    cnt = -99;
	    SubFmla &cl = subfs[j];
	    setCnts(j, cl, scopeof, assignedE, vars, minU, maxE, cnt);
	    litsCnt[j] = cnt;
	    maxEqdepth[j] = maxE;
	    minUqdepth[j] = minU;

	    if (litsCnt[j] == 0)
	    {
		nontrivial = 0;
		return nontrivial;
	    }

	    if (litsCnt[j] == 1)
	    {
		int implied = findEimplied(j, cl, assignedE, scopeof);
		int v = abs(implied);
		assert (v > 0);
		int a = (v + implied == 0)? (-1) : 1;
		if (assignedE[v] + a == 0)
		{
		    /* contradictory unit clauses */
		    nontrivial = 0;
		    return nontrivial;
		}
		if (assignedE[v] == 0)
		{
		assignedE[v] = a;
		unitQueue[back] = implied; back ++;
		}
	    }
	}
	/* Finally got the unitQueue seeded.  Now propagate. */
	while (back > front)
	{
	    int lit = unitQueue[front];
	    front ++;
	    int v = abs(lit);
	    Where &occsSame = (lit < 0)? negOccs[v] : posOccs[v];
	    updateSames(subfs, occsSame, litsCnt,
				posCnt, negCnt, vars, lit);
	    Where &occsOpp = (lit > 0)? negOccs[v] : posOccs[v];
	    back = updateOpps(subfs, occsOpp, scopeof, unitQueue, back,
				assignedE, minUqdepth, maxEqdepth,
				litsCnt, posCnt, negCnt, vars, lit);

	}

	return nontrivial;
}


int effPureProp(SubFmlas &subfs, int subfId, int vars,
	Wheres &posOccs, Wheres &negOccs, Qblocks &scopes, Qdepths &scopeof,
	vector<int> &posCnt, vector<int> &negCnt, vector<int> &litsCnt)
{
	int nontrivial;
	nontrivial = 1;
	vector<int> pureQueue(vars + 2);
	int front, back;
	front = 0; back = 0;

	return nontrivial;
}

/* Precondition: top, posOccsV, negOccsV are sorted.
 * For clIdx >= clsStart = vars+2, top[clIdx - vars] = clIdx.
 * Therefore, clsEnd-1 = max_clIdx - vars, or max_clIdx + 1 = clsEnd + vars.
 */
int shrinkAndMove(SubFmlas &subfs, Where &posOccsV, Where &negOccsV,
		SubFmla &top, SubFmla & posAnd, SubFmla & negAnd,
		vector <int> & posCnt, vector <int> & negCnt,
		int var, int clsStart, int clsEnd)
{
	int posOccsEnd = posOccsV[1]+2;
	int negOccsEnd = negOccsV[1]+2;
	for (int j = 2; j < posOccsEnd; j++)
	{
		int posClId = posOccsV[j];
		int topCur = posClId - clsStart + 2;
		assert (topCur >= 2);
		assert (topCur < clsEnd - clsStart + 2);
		int topClId = top[topCur];
		if (topClId == 0)
			/* different literal collected this clause */
			continue;

		assert (topClId == posClId);
		/* consume posOccsCur */
		posAnd.push_back(topClId); posAnd[1] ++;
		SubFmla &cl = subfs[topClId];
		top[topCur] = 0;
		reduceOccs(cl, posCnt, negCnt, (var));
		findDel(cl, (var));
	}

	for (int j = 2; j < negOccsEnd; j++)
	{
		int negClId = negOccsV[j];
		int topCur = negClId - clsStart + 2;
		assert (topCur >= 2);
		assert (topCur < clsEnd - clsStart + 2);
		int topClId = top[topCur];
		if (topClId == 0)
			/* different literal collected this clause */
			continue;

		assert (topClId == negClId);
		/* consume negOccsCur */
		negAnd.push_back(topClId); negAnd[1] ++;
		SubFmla &cl = subfs[topClId];
		top[topCur] = 0;
		reduceOccs(cl, posCnt, negCnt, (- var));
		findDel(cl, (- var));
	}
	return 1;
}

/* Precondition: top, posOccsV, negOccsV are sorted. */
int shrinkAndMoveAllRec(SubFmlas &subfs, Where &posOccsV, Where &negOccsV,
		SubFmla &top, SubFmla & posAnd, SubFmla & negAnd,
		vector <int> & posCnt, vector <int> & negCnt,
		int var, int topEnd, int posOccsEnd, int negOccsEnd,
		int topCur, int posOccsCur, int negOccsCur)
{
	if (negOccsCur >= negOccsEnd && posOccsCur >= posOccsEnd)
		return 1;

	if (topCur >= topEnd)
		return 0;

	int topClId = top[topCur];
	if (topClId == 0)
	{
		return shrinkAndMoveAllRec(subfs, posOccsV, negOccsV,
			top, posAnd, negAnd, posCnt, negCnt, var,
			topEnd, posOccsEnd, negOccsEnd,
			topCur + 1, posOccsCur, negOccsCur);
	}

	if (posOccsCur >= posOccsEnd || topClId < posOccsV[posOccsCur])
	{
		if (negOccsCur >= negOccsEnd || topClId < negOccsV[negOccsCur])
		{
			return shrinkAndMoveAllRec(subfs, posOccsV, negOccsV,
			top, posAnd, negAnd, posCnt, negCnt, var,
			topEnd, posOccsEnd, negOccsEnd,
			topCur + 1, posOccsCur, negOccsCur);
		}
		else if (topClId == negOccsV[negOccsCur])
		{
			negAnd.push_back(topClId); negAnd[1] ++;
			SubFmla &cl = subfs[topClId];
			top[topCur] = 0;
			reduceOccs(cl, posCnt, negCnt, (- var));
			findDel(cl, (- var));
			return shrinkAndMoveAllRec(subfs, posOccsV, negOccsV,
			top, posAnd, negAnd, posCnt, negCnt, var,
			topEnd, posOccsEnd, negOccsEnd,
			topCur + 1, posOccsCur, negOccsCur + 1);
		}
		else
		{
			return shrinkAndMoveAllRec(subfs, posOccsV, negOccsV,
			top, posAnd, negAnd, posCnt, negCnt, var,
			topEnd, posOccsEnd, negOccsEnd,
			topCur, posOccsCur, negOccsCur + 1);
		}
	}
	else if (topClId == posOccsV[posOccsCur])
	{
			posAnd.push_back(topClId); posAnd[1] ++;
			SubFmla &cl = subfs[topClId];
			top[topCur] = 0;
			reduceOccs(cl, posCnt, negCnt, (var));
			findDel(cl, (var));
			return shrinkAndMoveAllRec(subfs, posOccsV, negOccsV,
			top, posAnd, negAnd, posCnt, negCnt, var,
			topEnd, posOccsEnd, negOccsEnd,
			topCur + 1, posOccsCur + 1, negOccsCur);
	}
	else
		{
			return shrinkAndMoveAllRec(subfs, posOccsV, negOccsV,
			top, posAnd, negAnd, posCnt, negCnt, var,
			topEnd, posOccsEnd, negOccsEnd,
			topCur, posOccsCur + 1, negOccsCur);
		}
}

void reduceOccs(SubFmla &cl, vector <int> & posCnt, vector <int> & negCnt,
		int litShorten)
{
	int nLits = cl[1];
	for (int j = 2; j < nLits + 2; j ++)
	{
		int lit = cl[j];
		int v = abs(lit);
		if (lit > 0)
		{
			if (posCnt[v] > 0) posCnt[v] --;
		}
		else
		{
			if (negCnt[v] > 0) negCnt[v] --;
		}
	}
	return;
}


int findDel(SubFmla &f, int item)
{
	int cnt;
	int to, cur;
	int nLits = f[1];
	cnt = 0;
	to = 2;
	for (cur = 2; cur < nLits + 2; cur++)
	{
		if (f[cur] == item)
			cnt ++;
		else
		{
			f[to] = f[cur];
			to ++;
		}
	}
	f[1] = nLits - cnt;
	assert (f[1] == to - 2);
	return cnt;
}

int gatherClauses(SubFmlas &subfs, int subfId, int vars, int clsEnd,
		Where &posOccsV, Where &negOccsV, Qdepths &scopeof,
		vector <int> & posCnt, vector <int> & negCnt,
		int var)
{
	int posCntV = posOccsV[1];
	int negCntV = negOccsV[1];
	/* gather one side is not implemented.  Will have trivial
	 * second side if necessary.
	 */

	if (posCntV + negCntV < 4)
		/* leave this alone.  Not an error. */
		return 0;

	SubFmla & top = subfs[abs(subfId)];
	int clsStart = abs(subfId)+1;
	int newOrId = subfs.size();
	subfs.resize(newOrId + 1);
	SubFmla & newOr = subfs[newOrId];

	int posAndIdx = subfs.size();
	subfs.resize(posAndIdx + 1);
	SubFmla & posAnd = subfs[posAndIdx];
	posAnd.push_back(- posAndIdx);
	posAnd.push_back(1);
	posAnd.push_back(- var);

	int negAndIdx = subfs.size();
	subfs.resize(negAndIdx + 1);
	SubFmla & negAnd = subfs[negAndIdx];
	negAnd.push_back(- negAndIdx);
	negAnd.push_back(1);
	negAnd.push_back(var);

	/* subfs might have moved its data */
	SubFmla & top1 = subfs[abs(subfId)];
	SubFmla & newOr1 = subfs[newOrId];
	SubFmla & posAnd1 = subfs[posAndIdx];
	SubFmla & negAnd1 = subfs[negAndIdx];
	newOr1.push_back(newOrId);
	newOr1.push_back(2);
	newOr1.push_back(- posAndIdx);
	newOr1.push_back(- negAndIdx);

	int ok = shrinkAndMove(subfs, posOccsV, negOccsV,
		top1,  posAnd1,  negAnd1,
		posCnt, negCnt, var, clsStart, clsEnd);
	assert (ok);
	top1.push_back(newOrId); top1[1] ++;
	return ok;
}

int QdToQpro::reworkAndOr(SubFmlas &subfs, int subfId, int vars,
	Wheres &posOccs, Wheres &negOccs, Qblocks &scopes, Qdepths &scopeof)
{
	int topId;
	topId = subfId;
	/* Set up effective counts for restructure decisions.
	 * posOccs and negOccs will remain consistent with subfs, but
	 * all can change eventually.
	 */
	vector<int> posCnt(vars + 2);
	vector<int> negCnt(vars + 2);
	int clsEnd = subfs.size();
	int clsStart = abs(subfId)+1;
	vector<int> litsCnt(clsEnd + 2);
	for (int i = 1; i <= vars; i++)
	{
		posCnt[i] = posOccs[i][1];
		negCnt[i] = negOccs[i][1];
	}
	for (int j = clsStart; j < clsEnd; j++)
	{
		litsCnt[j] = subfs[j][1];
	}

	int nontrivial = effUnitProp(subfs, subfId, vars,
		posOccs, negOccs, scopes, scopeof,
		posCnt, negCnt, litsCnt);
	if (nontrivial == 0)
		return topId;

	int subfsEnd = clsEnd;

	/* It is too expensive to always find the maxScore, so use
	 * cyclic linear search to find a "reasonably large" score.
	 * This should be O(n log(n)) instead of O(n^2).
	 */
	int prettyBig = (1024*1024*1024);
	int oldMaxScore, tgtScore, oldV, lookedAt;
	int lowCutoff = 0;
	oldMaxScore = tgtScore = prettyBig + (prettyBig / 2);
	oldV = vars;
	while (1)
	{
	    int maxScore, maxSum, v, bestV;
	    maxScore = 0;
	    maxSum = 0;
	    bestV = 0;
	    v = oldV;
	    for (lookedAt = 0; lookedAt < vars; lookedAt++)
	    {
		v ++; if (v > vars) v -= vars;
		/* If we want to consider univs, need to count them
		 * in effUnitProp() etc.
		 */
		if (scopeof[v] < 0)
			continue;

		int pos = posCnt[v];
		int neg = negCnt[v];
		double prod = (pos + 0.0) * (neg+ 0.0);
		double sumdbl = (pos + 0.0) + (neg+ 0.0);
		int score = (prod > prettyBig) ? prettyBig : prod;
		int sum = (sumdbl > prettyBig) ? prettyBig : sumdbl;
		if (maxScore < score || (maxScore == score && maxSum < sum))
		{
			maxScore = score;
			maxSum =  sum;
			bestV = v;
		}
		if (score >= tgtScore && lookedAt < vars-1)
		{
			/* good enough */
			bestV = v;
			oldV = v;
			lookedAt ++;  // prob. unnecessay.
			break;
		}
	    }
	    if (lookedAt >= vars)
	    {
		/* maxScore is true; reduce tgtScore; <= 16 such reductions.
		 * keep tgtScore > lowCutoff as long as maxScore > lowCutoff.
		 */
		oldMaxScore = maxScore;
		tgtScore = lowCutoff + ((oldMaxScore - lowCutoff) + 3) / 4;
		oldV = vars;
	    }

	    if (maxScore > lowCutoff)
	    {
		Where & posOccsV = posOccs[bestV];
		Where & negOccsV = negOccs[bestV];
		int done = gatherClauses(subfs, topId, vars, clsEnd,
				posOccsV, negOccsV,
				scopeof, posCnt, negCnt,
				bestV);
		if (done == 0)
		{
			/* take out of contention */
			posCnt[bestV] = 0;
			negCnt[bestV] = 0;
			continue;
		}
		assert (posCnt[bestV] == 0);
		assert (negCnt[bestV] == 0);
	    }
	    else
	    {
		break;
	    }
	}
	/* subfs might have moved its data */
	SubFmla & top1 = subfs[abs(subfId)];
	int cnt = findDel(top1, 0);
	top1.resize(top1[1]+2);

	return topId;
}


bool QdToQpro::printAndOr(SubFmlas &subfs, int subfId, int vars)
{
	SubFmla & subf = subfs[abs(subfId)];
	int k = subf[1];
	if (subfId > 0)
		sqpro << "d" << endl;
	else
		sqpro << "c" << endl;

	if (subfId < 0 && (Common::qpro == 4 || Common::qpro == 5))
	{ // temp. treat lits as unary d's
	  sqpro << endl;
	  sqpro << endl;
	  for (int i = 2; i < k+2; i ++)
		{
			int e = subf[i];
			if (abs(e) > vars)
				printAndOr(subfs, e, vars);
			else if (e > 0)
			{
				sqpro << "d" << endl;
				sqpro << e << endl;
				sqpro << endl;
				sqpro << "/d" << endl;
			}
			else
			{
				sqpro << "d" << endl;
				sqpro << endl;
				sqpro << -e << endl;
				sqpro << "/d" << endl;
			}
		}
	}
	else
	{
	vector<int> posList;
	vector<int> negList;
	vector<int> subfList;
	for (int i = 2; i < k+2; i ++)
		{
			int e = subf[i];
			if (abs(e) > vars)
				subfList.push_back(e);
			else if (e > 0)
				posList.push_back(e);
			else
				negList.push_back(-e);
		}
	sort(posList.begin(), posList.end());
	sort(negList.begin(), negList.end());
	int npos = posList.size();
	int nneg = negList.size();
	int nsubf = subfList.size();
	for (int i = 0; i < npos; i ++)
		sqpro << " " << posList[i];
	sqpro << endl;
	for (int i = 0; i < nneg; i ++)
		sqpro << " " << negList[i];
	sqpro << endl;
	for (int i = 0; i < nsubf; i ++)
		printAndOr(subfs,  subfList[i], vars);
	}

	if (subfId > 0)
		sqpro << "/d" << endl;
	else
		sqpro << "/c" << endl;

	return true;
}
/***
		sqpro << "d" << endl;
			if (lit > 0)
				sqpro << " " << lit;
		sqpro << endl;
			if (lit < 0)
				sqpro << " " << -lit;
		sqpro << endl;
		sqpro << "/d" << endl;
	sqpro << "c" << endl;
	sqpro << "" << endl;
	sqpro << "" << endl;
	sqpro << "/c" << endl;
***/





bool QdToQpro::parse_qdimacs(ifstream &infile)
{
	string s, sp, scnf;
char peekc;

	// Skip comment lines
	while(!infile.eof())
	{
		infile >> ws;
		char c = infile.peek();
		if(c != 'c')
			break;

		infile.ignore(std::numeric_limits<std::streamsize>::max(),
				'\n');
		linenum ++;
	}

	if(infile.eof())
	{
		error("Qdimacs file ended with only comments!");
		return false;
	}

	// Read the p-line
	infile >> sp;
	infile >> scnf;
	if(sp != "p" || scnf != "cnf")
	{
		error(("Expected \"p cnf\", got " + sp + scnf).c_str());
		return false;
	}

	int vars, cls;
	infile >> vars;
	infile >> cls;
	infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	linenum ++;

// cout << infile.peek() << " A" << endl;;

	if(infile.fail() || vars < 1 || cls < 1)
	{
		error("Invalid number of variables or clauses");
		return false;
	}

	// Write the prologue ...
	sqpro << "QBF" << endl;
	sqpro << vars << endl;
	sqpro << "q" << endl;

	SubFmlas subfs;

	Qblocks scopes;
	scopes.resize(1);

	Wheres posOccs;
	Wheres negOccs;

	Qdepths scopeof;

	posOccs.resize(vars+1);
	negOccs.resize(vars+1);
	scopeof.resize(vars+1);
	subfs.resize(vars+1);    // dummies, to simplify indexing
	for (int i = 0; i <= vars; i++)
	{
		Where & wpos = posOccs[i];
		wpos.push_back(i); wpos.push_back(0);
		Where & wneg = negOccs[i];
		wneg.push_back(-i); wneg.push_back(0);
	}
	subfs.resize(subfs.size() + 1);  // top-level conjunction, at [vars+1].
	SubFmla & topf = subfs[vars+1];
	topf.push_back(-(vars+1)); topf.push_back(0);

	vector<int> list;
	char c, p, qtype;

// cout << infile.peek() << " E" << endl;;
	infile >>ws;
// cout << infile.peek() << " F" << endl;;
	qtype = infile.peek();
	while(qtype == 'a' || qtype == 'e')
	{
		int sz;
		if(infile)
			infile >> p;

		list.clear();
		readIntList(list, infile);
		sz = list.size();
		while (list[sz-1] != 0)
		{
			int oldsz = sz;
			readIntList(list, infile);
			sz = list.size();
			if (oldsz == sz)
			{
				error(((string)"Integer list not terminated by 0, " +
					"line " + toSt(linenum)).c_str());
				return false;
			}
		}
		int qdepth = scopes.size();
		int scopeId = (qtype == 'a')? (- qdepth) : qdepth;
		int sz1 = list.size();
		scopes.resize(scopes.size() + 1);
		Qblock & b = scopes[qdepth];
		b.push_back(scopeId); b.push_back(0);
		for (int i = 0; i < sz1; i ++)
		{
			int lit = list[i];
			if (lit == 0)
				break;
			int v = abs(lit);
			scopeof[v] = scopeId;
			b.push_back(v); b[1] ++;
		}
		sqpro << qtype;
		for (int i = 0; i < sz1; i ++)
		{
			int lit = list[i];
			if (lit == 0)
				break;
			sqpro << " " << lit;
		}
		sqpro << endl;
		qtype = infile.peek();
	}

	/* If no prefix, make them all E. Strictly only those that occur
	 * should appear, and some not in any block might occur, but we
	 * do not handle these nuances.
	 */
	if (scopes.size() == 1)
	{
		qtype = 'e';
		int qdepth = scopes.size();
		int scopeId = qdepth;
		scopes.resize(scopes.size() + 1);
		Qblock & b = scopes[qdepth];
		b.push_back(scopeId); b.push_back(0);
		for (int v = 1; v <= vars; v ++)
		{
			scopeof[v] = scopeId;
			b.push_back(v); b[1] ++;
		}
		sqpro << qtype;
		for (int v = 1; v <= vars; v ++)
		{
			sqpro << " " << v;
		}
		sqpro << endl;
	}

	// Make a conjunction of all clauses.  Unit clauses are d.../d.

	int clsFound;
	clsFound = 0;
	infile >>ws;
	while (! infile.eof())
	{
		int sz;
		list.clear();
		readIntList(list, infile);
		sz = list.size();
		while (list[sz-1] != 0)
		{
			int oldsz = sz;
			readIntList(list, infile);
			sz = list.size();
			if (oldsz == sz)
			{
				error(((string)"Integer list not terminated by 0, " +
					"line " + toSt(linenum)).c_str());
				return false;
			}
		}
		int sz1 = list.size();
		int clsId = subfs.size();    // positive for disjunction
		subfs.resize(subfs.size() + 1);
		SubFmla & subf = subfs[clsId];
		subf.push_back(clsId); subf.push_back(0);
		for (int i = 0; i < sz1; i ++)
		{
			int lit = list[i];
			if (lit == 0)
			{
				assert(i == sz1-1);
				break;
			}
			int v = abs(lit);
			subf.push_back(lit); subf[1] ++;

			Where & w = (lit > 0)? posOccs[v] : negOccs[v];
			w.push_back(clsId); w[1] ++;
		}
		/* Trying to use topf caused seg fault.  resize moved it */
		SubFmla & topf1 = subfs[vars+1];
		topf1.push_back(clsId); topf1[1] ++;
		clsFound ++;
		infile >> ws;
	}

	if (clsFound != cls)
	{
		cerr << "WARNING: Clauses found " << clsFound <<
			" differs from " << cls << " in p-line" << endl;
	}

	if (Common::qpro == 4 || Common::qpro == 5)
	{
		/* Use the vanilla translation. */
		printAndOr(subfs, -(vars+1), vars);
	}
	else
	{
		int topId = reworkAndOr(subfs, -(vars+1), vars,
				posOccs, negOccs, scopes, scopeof);
		printAndOr(subfs, topId, vars);
	}

	// Write the epilogue ...
	sqpro << "/q" << endl;
	sqpro << "QBF" << endl;

// cout << sqpro.str();

	return true;
}

bool QdToQpro::simple_parse_qdimacs(ifstream &infile)
{
	string s, sp, scnf;
char peekc;

	// Skip comment lines
	while(!infile.eof())
	{
		infile >> ws;
		char c = infile.peek();
		if(c != 'c')
			break;

		infile.ignore(std::numeric_limits<std::streamsize>::max(),
				'\n');
		linenum ++;
	}

	if(infile.eof())
	{
		error("Qdimacs file ended with only comments!");
		return false;
	}

	// Read the p-line
	infile >> sp;
	infile >> scnf;
	if(sp != "p" || scnf != "cnf")
	{
		error(("Expected \"p cnf\", got " + sp + scnf).c_str());
		return false;
	}

	int vars, cls;
	infile >> vars;
	infile >> cls;
	infile.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	linenum ++;

// cout << infile.peek() << " A" << endl;;

	if(infile.fail() || vars < 1 || cls < 1)
	{
		error("Invalid number of variables or clauses");
		return false;
	}
// cout << infile.peek() << " D" << endl;;

	// Write the prologue ...
	sqpro << "QBF" << endl;
	sqpro << vars << endl;
	sqpro << "q" << endl;

	vector<int> list;
	char c, p, qtype;

// cout << infile.peek() << " E" << endl;;
	infile >>ws;
// cout << infile.peek() << " F" << endl;;
	qtype = infile.peek();
	while(qtype == 'a' || qtype == 'e')
	{
		int sz;
		if(infile)
			infile >> p;

		list.clear();
		readIntList(list, infile);
		sz = list.size();
		while (list[sz-1] != 0)
		{
			int oldsz = sz;
			readIntList(list, infile);
			sz = list.size();
			if (oldsz == sz)
			{
				error(((string)"Integer list not terminated by 0, " +
					"line " + toSt(linenum)).c_str());
				return false;
			}
		}
		sqpro << qtype;
		int sz1 = list.size();
		// If necessary, list can be sorted before extracting.
		for (int i = 0; i < sz1; i ++)
		{
			int lit = list[i];
			if (lit == 0)
				break;
			sqpro << " " << lit;
		}
		sqpro << endl;
		qtype = infile.peek();
	}

	// Make a conjunction of all clauses.  Unit clauses are d.../d.
	sqpro << "c" << endl;
	sqpro << "" << endl;
	sqpro << "" << endl;

	int clsFound;
	clsFound = 0;
	infile >>ws;
	while (! infile.eof())
	{
		int sz;
		list.clear();
		readIntList(list, infile);
		sz = list.size();
		while (list[sz-1] != 0)
		{
			int oldsz = sz;
			readIntList(list, infile);
			sz = list.size();
			if (oldsz == sz)
			{
				error(((string)"Integer list not terminated by 0, " +
					"line " + toSt(linenum)).c_str());
				return false;
			}
		}
		sqpro << "d" << endl;
		int sz1 = list.size();
		// If necessary, list can be sorted before extracting.
		// Should use an iterator? TODO
		for (int i = 0; i < sz1; i ++)
		{
			int lit = list[i];
			if (lit == 0)
			{
				assert(i == sz1-1);
				break;
			}
			if (lit > 0)
				sqpro << " " << lit;
		}
		sqpro << endl;
		for (int i = sz1-2; i >= 0; i --)
		{
			int lit = list[i];
			assert(lit != 0);
			if (lit < 0)
				sqpro << " " << -lit;
		}
		sqpro << endl;
		sqpro << "/d" << endl;
		clsFound ++;
		infile >> ws;
	}

	sqpro << "/c" << endl;

	if (clsFound != cls)
	{
		cerr << "WARNING: Clauses found " << clsFound <<
			" differs from " << cls << " in p-line" << endl;
	}

	// Write the epilogue ...
	sqpro << "/q" << endl;
	sqpro << "QBF" << endl;

// cout << sqpro.str();

	return true;
}

/* Read in the list of signed integers, appending to vector.
 * sort is not really necessary, but is consistent with qpro specs.
 * ASSUMING ws includes '\n'.  Consume the '\n'.
 */
void QdToQpro::readIntList(vector<int> &vec, ifstream &infile)
{
	char p;
	string s;
	getline(infile, s);
	linenum ++;
	stringstream ss(s);
	ss >> ws;
	while(!ss.eof())
	{
		p = ss.peek();
		if(!isdigit(p) && !(p == '-'))
			break;
		int i;
		ss >> i;
		if (i == 0)
			sort(vec.begin(), vec.end());
		vec.push_back(i);
		ss >> ws;
	}
// cout << infile.peek() << " G " << linenum << endl;;
	return;
}


	// stringstream sqpro;
	QdToQpro::QdToQpro(){};

	// How do I destroy sqpro to be sure the space is freed? TODO
	QdToQpro::~QdToQpro()
	{
		sqpro.str("");
		// delete (basic_stringstream<char>)sqpro;
		// sqpro.rdbuf().setbuf(NULL, 4);
	}

	// Returns the number of the current line
	int QdToQpro::getLineNum()
	{
		return linenum;
	}

	bool QdToQpro::init(char *filename)
	{
		ifstream infile;

		infile.open( filename );
		if(!infile.is_open())
		{
			error(" can't open Qdimacs file");
			return false;
		}
		if(infile.eof())
		{
			error(" Qdimacs file is empty");
			return false;
		}
		linenum = 0;
		bool ok = parse_qdimacs(infile);
		infile.close();
		if (ok == false)
		{
			error(" parse_qdimacs failed");
		}
		return ok;
	}

