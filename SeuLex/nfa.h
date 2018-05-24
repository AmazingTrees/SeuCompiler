#ifndef NFA_H_
#define NFA_H_

#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <stack>
#include <set>
#include <map>
#include <algorithm>
#include "state.h"

using namespace std;
class NFA {
public:
    virtual State *getStart() = 0;
    virtual std::set<int> getCharSet() = 0;
};

/*
 * 每条正则语句对应于一条NFA
 *
 */
class ReToNFA: public NFA
{
public:
    ReToNFA(char *str)
    {
        strcpy(this->re, str);
    }

    ReToNFA(char *str, string func)
    {
        strcpy(this->re, str);
        this->func = func;
    }

    ~ReToNFA()
    {
        if (nfa_s == NULL) {
            return;
        }
        haveTravel.clear();
        free(nfa_s);
        nfa_s = NULL;
        nfa_e = NULL;
    }

    /**
     * 析构对象时, 一个对象不可以重复删除
     */
    void free(State *start) {
        if (start == NULL ||
            haveTravel.find(start) != haveTravel.end() ) {
            return;
        }

        haveTravel.insert(start);

        free(start->out);
        start->out = NULL;
        free(start->out1);
        start->out1 = NULL;

        delete start;
    }

    void printNFA() {
        haveTravel.clear();
        this->showNFA(this->nfa_s);
    }

    void strToNFA() {
        ReTopost(this->re);
        postTonfa(this->buf);

    }

    State *getStart() {
        return this->nfa_s;
    }

    std::set<int> getCharSet() {
        return this->char_set;
    }


private:

    /*
     * 由后缀表达式形式转化为NFA
     */
    State *postTonfa(char * postfix);
    void showNFA(State * start);

    /**
     * 此段代码来源:
     * https://swtch.com/~rsc/regexp/
     *
     *
     * Convert infix regexp re to postfix notation
     * Insert . as explicit concatenation operator.
     *
     * 中缀转化为后缀, 同时插入'.'作为连接符
     * 例如ab, 转化为后缀为'ab.'
     *
     * 字符具有高于运算符的优先级, "m|food"匹配"m"或是"food", 如果想要匹配
     * "mood"或是"food", 需要使用"(m|f)ood"进行匹配.
     * 上面的两个正则表达式需要以这样的方式进行传递: '\\+|\\-', '\\(|\\)' ,  使用两个转义'\'是由于c语言自身的原因, 本
     * 身会进行一次转义, 因此, 我们需要自己来添加
     */
    char *ReTopost(char * re);

    char buf[1024] = {0};
    char re[1024] = {0};
    string func;            // 该NFA所对应的终止状态的处理函数, 该NFA所对应的结局
    State * nfa_s = NULL;   // 保存解析后的数据
    State * nfa_e = NULL;   // 最终符合条件的状态
    std::set<int> char_set; // 输入符号集合


    int id = 0;
    std::set<State *> haveTravel;
    std::map<State *, int> stateToid;
};

/*
 *   该类主要用来合并NFA, 合并完成后的NFA仍然为NFA, 只是不同的NFA链所对应的最终处理函数不同,
 */
class NFAToLIST: public NFA {
public:

    State *nfa_s = NULL;
    std::set<int> char_set;

    std::set<State*> st_create; // 统计新建的资源, 为了最终释放

    NFAToLIST() {}

    ~NFAToLIST() {
        for (auto state: st_create) {
            delete state;
        }
        st_create.clear();
    }

    /**
     * 合并操作, 主要用来合并NFA, 合并时, 新建一个新的起始节点, 再将两个NFA的字符集进行合并,
     */
    NFAToLIST* merge(ReToNFA *ReToNFA) {
        State *create = new State(Split, ReToNFA->getStart(), nfa_s);
        std::set<int> char_set1 = ReToNFA->getCharSet();

        char_set.insert(char_set1.begin(), char_set1.end());
        st_create.insert(create);
        nfa_s = create;
        return this;
    }

    State* getStart() {
        return this->nfa_s;
    }

    std::set<int> getCharSet() {
        return this->char_set;
    }
};

#endif
