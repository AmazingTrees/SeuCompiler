#include "fstream"
#include "lex.h"
#include "nfa.h"
#include "dfa.h"


void Lex::scaner()
{
    int state = 0;
    int line = 0;
    string str;
    string outStr;
    std::istream& input = *in;
    std::ostream& output = *out;

    while(!input.eof())
    {
        line ++;
        switch (state) {
        case 0: {
            getline(input, str);
            if (str.compare(0, 2, "%{") == 0) {
                state = 1;
                output << "//%{ start" << endl;
            } else if (str.compare(0, 2, "%!") == 0) {
                state = 2;
                output << "//%! start" << endl;
            } else if (str.compare(0, 2, "%%") == 0) {
                state = 3;
                outStr.clear();
                output << "//%% start" << endl;
            }else if (str.compare(0, 2, "//") == 0) {
                state = 4;
            } else {
                state = 0;
                if (!str.empty()) {
                    printError(line, str + "error");
                }
            }
            break;
        }
        case 1: {
            getline(input, str);
            if (str.compare(0, 2, "%}") == 0) {
                state = 0;
                output << "//%} end" << endl;
            } else {
                output << str << endl;
            }
            break;
        }
        case 2: {
            getline(input, str);
            if (str.compare(0, 2, "%!") == 0) {
                output << "//%! end" << endl;
                state = 0;
            } else {
                getFunc(str, line);
            }
            break;
        }
        case 3: {
            getline(input, str);
            if (str.compare(0, 2, "%%") == 0) {
                state = 0;
                getRegular(outStr, line);
                output << "//%% end" << endl;
            } else if (str.compare(0, 2, "%$") == 0) {
                getRegular(outStr, line);
                outStr.clear();
            } else
                outStr += str;
            break;
        }
        case 4: {
            // 此处获得的是注释
            state = 0;
            break;
        }
        default:
            printError(line, "结构不完整");
            break;
        }
    }
    if (state != 0) {
        printError(line, "结构不完整");
    }
}

void Lex::getFunc(std::string str, int line) {
    string::iterator end_pos = std::remove(str.begin(), str.end(), ' ');
    str.erase(end_pos, str.end());

    static char ch = 20;

    string::iterator iter = str.begin();
    string left, right;
    std::istringstream f(str);

    getline(f, left, '=');
    getline(f, right, '=');


    funcMap.insert(std::make_pair(left, right));
    typeToch.insert(std::make_pair(left, ch));
    chTofunc.insert(std::make_pair(ch, right));

    ch ++;

}

void Lex::getRegular(std::string str, int line) {
    std::pair<string, string> record = getReAndFunc(str);
    string re = record.first;
    string func = record.second;

    char re_s[1024] = {0};
    int it = 0;

    for (int i = 0; i < re.size(); ++i) {
        char p = re.at(i);
        switch (p) {
        case '%': {
            i++;
            re_s[it++] = '\\';
            re_s[it++] = re.at(i);
            break;
        }
        case '{': {
            i++;
            int start = i;
            while (re.at(i) != '}') {i++;}

            string atom = re.substr(start, i - start);
            if (typeToch.find(atom) != typeToch.end()) {
                re_s[it++] = typeToch.at(atom);
            }  else {
                printError(line, "error find " + atom);
            }
            break;
        }
        default:
            re_s[it++] = re.at(i);
            break;
        }
    }
    ReToNFA *preToNfa = new ReToNFA(re_s, func);
    preToNfa->strToNFA();
    reToNFAList.push_back(preToNfa);
}

std::pair<string, string> Lex::getReAndFunc(string str) {
    string re;
    string func;

    string::iterator iter = str.end();
    int stack = 1;
    while(*iter != '}') {
        iter --;
    }
    func.push_back('}');
    iter --;
    while (stack >= 1 && iter != str.begin()) {
        if (*iter == '}') {
            stack ++;
        } else if (*iter == '{') {
            stack --;
        }
        func.push_back(*iter);
        iter --;
    }

    while(*iter == ' ') {
        iter --;
    }

    re.push_back(*iter);
    while(iter != str.begin()) {
        iter --;
        re.push_back(*iter);
    }

    std::reverse(func.begin(), func.end());
    std::reverse(re.begin(), re.end());


    return std::make_pair(re, func);
}


void Lex::outCodeTop()
{
    std::ostream &o = (*out);
    o<<"#include <stdio.h>"<<endl;
    o<<"#include <stdlib.h>"<<endl;
    o<<"#include <string.h>"<<endl;
    o<<endl;
    o<<"#define SYLEX_MAXSIZE_TEXT 120"<<endl;
    o<<"#define SYLEX_MAXSIZE_BUFF 1024"<<endl;
    o<<endl;
    o<<"char SYLEX_FILE_NAME[100];"<<endl;
    o<<"char SYLEX_OUT_FILE_NAME[100];"<<endl;
    o<<"int SYLEX_LINE = 0;"<<endl;
    o<<"int SYLEX_STATE = 0;"<<endl;
    o<<"int SYLEX_TEXT_LEN = 0;"<<endl;
    o<<"char SYLEX_TEXT[SYLEX_MAXSIZE_TEXT];"<<endl;
    o<<"char SYLEX_BUFF[SYLEX_MAXSIZE_BUFF];"<<endl;
    o<<endl;
}

void Lex::outCodeMid()
{
    std::ostream &o = (*out);

    o<<"//扫描函数"<<endl;
    o<<"void SYLEX_scanner(char *str)"<<endl;
    o<<"{"<<endl;
    o<<"    char ch = ' ';"<<endl;
    o<<"    while(ch != '\\0')"<<endl;
    o<<"    {"<<endl;
    o<<"        //printf(\"%c %d\\n\", ch, SYLEX_STATE);"<<endl;
    o<<"        switch(SYLEX_STATE) {"<<endl;

    int state = 0;
    DState *start = pNToDFA->dstart;
    std::set<DState*> haveTravel;
    std::map<DState*, int> stateToid= pNToDFA->dStateToid;
    std::map<int, DState*> idTostate = pNToDFA->idTodState;


    int dfaNum = pNToDFA->dsCnt;


    for (int i = 0; i < dfaNum; ++i) {

        DState* ds = idTostate.at(i);
        o << "        case " << i << ":"  << endl;
        o << "        {" << endl;
        o << "            ch = *str++;" << endl;
        o << "            SYLEX_TEXT[SYLEX_TEXT_LEN++] = ch;" << endl;

        bool hasPath = false;
        bool isFirstIF = true;

        for (auto path = ds->out.begin(); path != ds->out.end(); path++) {

            hasPath = true;
            DState *s = path->first;
            int Key = path->second;
            int toId = stateToid.at(s);

            bool isFun = false;
            string chk_s;

            if (chTofunc.find(Key) != chTofunc.end()) {
                isFun = true;
                chk_s = chTofunc.at(Key);
                chk_s += "(ch)){";
            } else {
                char p = Key;
                chk_s = "ch == \'";
                chk_s += p;
                chk_s += "\'){";
            }

            if (isFirstIF) {
                o << "            if(";
                isFirstIF = false;
            } else {
                o << "            else if(";
            }
            o << chk_s << endl;
            o << "                SYLEX_STATE = " << toId << ";" << endl;
            o << "            }" << endl;
        }
        if (hasPath) {
            o << "               else";
        }
        o << " {" << endl;
        if (ds->isEnd) {
            o << "SYLEX_TEXT[SYLEX_TEXT_LEN-1] = '\\0';" << endl;
            o << "SYLEX_TEXT_LEN = 0;" << endl;
            o << "SYLEX_STATE = 0;" << endl;
            o << "str --;" << endl;
            o << "//---------------------" << endl;
            o << ds->endFunc << endl;
            o << "//---------------------" << endl;
        } else {
            o << "                printf(\"Error in line %d, in state " << i << " char is %c  \\n\", SYLEX_LINE, ch);" << endl;
            o << "                exit(1);" << endl;
        }
        o << "            }" << endl;
        o << "            break;" << endl;
        o << "        }" << endl;
    }
    o << "       }" << endl;
    o << "    }" << endl;
    o << "}" << endl;

}


void Lex::outCodeBottom()
{
    std::ostream &o = (*out);
    o<<"int main(int argc, char **args)"<<endl;
    o<<"{"<<endl;
    o<<"    if(argc == 1)"<<endl;
    o<<"    {"<<endl;
    o<<"        printf(\"没有输入源文件名\");"<<endl;
    o<<"        return 0;"<<endl;
    o<<"    }"<<endl;
    o<<"    else if(argc == 2)"<<endl;
    o<<"    {"<<endl;
    o<<"        strcpy(SYLEX_FILE_NAME, args[1]);"<<endl;
    o<<"        sprintf(SYLEX_OUT_FILE_NAME, \"%s.out\", SYLEX_FILE_NAME);"<<endl;
    o<<"    }"<<endl;
    o<<"    else"<<endl;
    o<<"    {"<<endl;
    o<<"        strcpy(SYLEX_FILE_NAME, args[1]);"<<endl;
    o<<"        strcpy(SYLEX_OUT_FILE_NAME, args[2]);"<<endl;
    o<<"    }"<<endl;
    o<<"    FILE* file = fopen(SYLEX_FILE_NAME, \"r\");"<<endl;
    o<<"    while(NULL != fgets(SYLEX_BUFF, SYLEX_MAXSIZE_BUFF, file))"<<endl;
    o<<"    {"<<endl;
    o<<"        ++SYLEX_LINE;"<<endl;
    o<<"        SYLEX_scanner(SYLEX_BUFF);"<<endl;
    o<<"    }"<<endl;
    o<<"    return 0;"<<endl;
    o<<"}"<<endl;
}


void test()
{

    Lex lex("../input/require.l", "../input/out.c");
    lex.scaner();
    lex.dfaMerge();
    lex.nfaToDFA();
    lex.output();
}


