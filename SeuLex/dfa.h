/*NFA->DFA*/

#ifndef DFA_H_
#define DFA_H_

#include "state.h"
#include "nfa.h"

class N2DFA
{
public:
    DState* pickupFromList(std::list<DState*> *pDsList);// 遍历所有状态并查找尚未计算的DState
    DState* alreadyHave(std::list<DState *> *pDsList, DState* ds);//检查该状态是否已经存在于列表中, 如果存在则返回状态的地址, 否则, 返回NULL
    DState *dstart;
    NFA *nfa;
    int dsCnt = 0;
    std::map<DState*, int> dState2id;
    std::map<int, DState*> id2dState;

    N2DFA(NFA *nfa) {
        this->nfa = nfa;
              }

    ~N2DFA() {
        this->haveTravel.clear();
        free(this->dstart);
              }

    std::set<DState*> haveTravel;
    std::map<DState*, int> state2id;
    int id = 0;

    void nfa2dfa() {
        this->nfa2dfa(nfa);
              }

    void printDFA() {
        this->haveTravel.clear();
        showDFA(this->dstart);
             }

private:
    DState* nfa2dfa(NFA *nfa);
    void showDFA(DState *ds);
    void free(DState *ds);//递归释放资源, 由析构函数调用

};
#endif
