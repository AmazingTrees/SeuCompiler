#ifndef STATE_H_
#define STATE_H_

#include <stdio.h>
#include <list>
#include <map>
#include <set>


enum {
    Match = 256,
    Split = 257
};

class State
{
public:
    int c;
    State *out;
    State *out1;
    std::string endFunc;        // 结束状态所对应的函数,该点为Match时,值可以视为有效

    State() {}
    State(int c, State *out, State *out1);
};

 
// NFA片段
class Frag
{
public:
    State *start;   // 起始节点
    State *end;     // 尾节点

    Frag() {}
    Frag(State *start, State *end){
        this->start = start;
        this->end = end;
    }
};

// DFA节点状态
class DState {
public:

    std::map<DState*, int> out;     // 记录DFA节点能够到达的状态以及路径
    std::set<State*> coreState;     // 当前状态所代表的NFA状态中的核心状态
    std::set<State*> allState;      // 记录所有可能状态
    bool hasTravel = false;         // 构建DFA时, 表示该状态是否已经被遍历
    bool isEnd = false;             // 是否为结束节点
    std::string endFunc;            // 结束状态中所对应的执行函数

    // 添加DFA的路径
    DState& addDState(DState* pDs, int path) {
        this->out.insert(std::make_pair(pDs, path));
        return *this;
    }

    // 添加到核心状态
    DState& addCoreState(State * state) {
        this->coreState.insert(state);
        return *this;
    }

    // 由所有的核心状态进行扩展, 最终表示出所有状态
    void getAllState() {
        for(auto pCore = coreState.begin(); pCore != coreState.end(); pCore++) {
            findSimple(*pCore);
        }
    }

    // 从核心点衍生到所有点, 用于核心点到其周围点的映射
    void findSimple(State *start);

    // 通过核心状态的比对, 判断两个DState是否为相同的状态
    bool isSimilar(DState *newDs);

};

#endif

