#include "state.h"

State::State(int c,State *out,State *out1){
     this->c = c;
     this->out = out;
     this->out1 = out1;
}

void DState::findSimple(State *start)
{
     if (start == NULL) {
         return ;
     }

     if (this->allState.find(start) == this->allState.end()) {
         this->allState.insert(start);
     }

     if (start->c == Match) {            //没有出度边, 意味着该状态为匹配状态
         this->isEnd = true;
         this->endFunc = start->endFunc; //向该状态传递结束函数
     } else if (start->c == Split) {     //此状态表示有一条或两条sigma边时
         findSimple(start->out);         
         findSimple(start->out1);
     } else if (start->c < 256) {        //此状态有一条实边, 实边为out,非实边为out1
         findSimple(start->out1);
     } else {
         printf("error in dfa\n");
         exit(1);
     }
}

bool DState::isSimilar(DState *newDs)    //对比核心文件，判断两个DState是否为相同的状态
{
    if (this->coreState.size() != newDs->coreState.size()) {
        return false;
    }

    for (auto pDs = coreState.begin(); pDs != coreState.end(); pDs++) {
        if(newDs->coreState.find(*pDs) == newDs->coreState.end())
            return false;
    }

    for (auto pDs = newDs->coreState.begin(); pDs != newDs->coreState.end(); ++pDs) {
        if(this->coreState.find(*pDs) == this->coreState.end())
            return false;
    }

    return true;
}
