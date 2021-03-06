
#ifndef LEX_H_
#define LEX_H_

#include <string>
#include <iostream>
#include <stdio.h>
#include "nfa.h"
#include "dfa.h"
#include "state.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>

using std::string;
using std::cout;
using std::endl;
using std::cerr;
using std::ios;

class Lex
{

public:
    static const int MAXID = 100000;
    string lexFile;
    std::ostream *out = NULL;
    std::istream *in = NULL;

    std::vector<ReToNFA*> reToNFAList;
    NToDFA* pNToDFA = NULL;

    Lex (string _lexFile) {


        std::ifstream* ifile = new std::ifstream();
        ifile->open(_lexFile);
        if(!ifile->is_open()) {
            printError(0, "Please input valid file");

        }
        in = ifile;

        this->lexFile = _lexFile;
        this->out = &std::cout;
    }

    Lex (string _lexFile, string _outCFile) {
        std::ifstream* ifile = new std::ifstream();
        ifile->open(_lexFile);
        if(!ifile->is_open()) {
            printError(0, "Please input valid file");

        }
        in = ifile;
        this->lexFile = _lexFile;

        std::ofstream *fout = new std::ofstream(_outCFile);
        this->out = fout;
    }


    ~Lex() {
        for (auto iter = reToNFAList.begin(); iter != reToNFAList.end(); ++iter) {
            delete *iter;
        }
        reToNFAList.clear();
        if (pNToDFA != NULL) {
            delete pNToDFA;
        }

        ((std::ifstream*)in)->close();
        delete this->in;

        if (this->out == &std::cout)
            return;
        ((std::ofstream*)out)->close();
        delete this->out;
    }


    void scaner();

    // 记录自定义的类型的判断函数, digit->isDigit
    std::map<string, string> funcMap;

    // 记录自定义的类型的符号, 以便进行转换 digit->a
    std::map<string, char> typeToch;

    // 此处记录符号与判断函数对应 a -> isDigit
    std::map<char, string> chTofunc;

    // 辅助函数: 记录不同的判断函数对应的字符串,

    void getFunc(string str, int line);


    /**
     *   将每一行中的正则表达式与对应的处理函数相分离开来. 而后将正则表达式的处理结果进行存放,
     * 这里将正则表达式中的特殊部分以及需要转义的字符进行初步处理, 而后将其转化为NFA, 并且存入
     * NFA列表中, 以便于之后进行NFA合并
     */
    void getRegular(string str, int line);

    std::pair<string, string> getReAndFunc(string str);


    void dfaMerge() {
        for (int i = 0; i < reToNFAList.size(); ++i) {
            ReToNFA* nfa = reToNFAList.at(i);
            nfaToList.merge(nfa);
        }
    }


    NFAToLIST nfaToList;

    void nfaToDFA() {
        pNToDFA = new NToDFA(&nfaToList);
        pNToDFA->nfaTodfa();
    }

    void printWarnning(int line, string str) {
        cout << "[warning]:  in line " << line << " " << str << endl;
    }

    void printError(int line, string str) {
        cout << "[error]:  in line " << line <<  " " << str << endl;
        exit(1);
    }

    void outCodeTop();

    void outCodeMid();

    void outCodeBottom();

    void output() {
        outCodeTop ();
        outCodeMid();
        outCodeBottom();
    }

};

void test();

#endif 
