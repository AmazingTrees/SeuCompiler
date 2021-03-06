#include "dfa.h"


DState* NToDFA::pickupFromList(std::list<DState*> *pDsList) {
    for(auto pList = pDsList->begin(); pList != pDsList->end(); pList++) {
        DState* tmpDs = (*pList);
        if (tmpDs->hasTravel == false) {
            return tmpDs;
        }
    }
    return NULL;
}

DState* NToDFA::alreadyHave(std::list<DState *> *pDsList, DState* ds) {

    for(auto pList = pDsList->begin(); pList != pDsList->end(); pList++) {
        DState* tmpDs = (*pList);

        if (ds->isSimilar(tmpDs)) { // 此处调用状态的比较函数
            return tmpDs;
        }
    }
    return NULL;
}

void NToDFA::free(DState *ds) {
    if (ds == NULL || haveTravel.find(ds) != haveTravel.end()) {
        return ;
    }

    haveTravel.insert(ds);
    for (auto pDs = ds->out.begin(); pDs != ds->out.end(); pDs++) {
        std::pair<DState* const, int > tmpDsPair = *pDs;
        DState* tmpDs = tmpDsPair.first;
        //delete tmpDs;
        free(tmpDs);
    }
    delete ds;
}


void NToDFA::showDFA(DState *ds) {

    if (ds == NULL) {
        return;
    }

    if (haveTravel.find(ds) != haveTravel.end()) {
        return;
    }
    if (stateToid.find(ds) == stateToid.end()) {
        stateToid.insert(std::make_pair(ds, id));
        id ++;
    }

    haveTravel.insert(ds);

    for (auto pDs = ds->out.begin(); pDs != ds->out.end(); pDs++) {
        std::pair<DState* const, int > tmpDsPair = *pDs;
        DState* tmpDs = tmpDsPair.first;
        if (stateToid.find(tmpDs) == stateToid.end()) {
            stateToid.insert(std::make_pair(tmpDs, id));
            id ++;
        }

        if (tmpDs->isEnd) {
            printf("%d -> %c -> [%d]\n", stateToid[ds], tmpDsPair.second, stateToid[tmpDs]);
            if (!tmpDs->endFunc.empty()) {
                printf("Endfunc is %s\n", tmpDs->endFunc.c_str()) ;
            }
        } else {
            printf("%d -> %c -> %d\n", stateToid[ds], tmpDsPair.second, stateToid[tmpDs]);
        }

        showDFA(tmpDs);
    }
}

DState* NToDFA::nfaTodfa(NFA *nfa) {

    State *start = nfa->getStart();

    //printf("start pointer is %p\n", start);
    std::set<int> char_list = nfa->getCharSet();

    //printf("\n");

    if (start == NULL) {
        return NULL;
    }
    std::list<DState*> dsList;

    DState *ds = new DState();
    ds->addCoreState(start);
    ds->getAllState();

    dsList.push_back(ds);
    this->dStateToid.insert(std::make_pair(ds, this->dsCnt));
    this->idTodState.insert(std::make_pair(this->dsCnt, ds));
    this->dsCnt ++;

    bool isModify = true;

    DState *pDs;

    while((pDs = pickupFromList(&dsList)) != NULL) {        // 从链表中选择一个尚未遍历的状态

        pDs->hasTravel = true;

        for(auto pChar = char_list.begin(); pChar != char_list.end(); pChar++) {

            DState *tmpDs = new DState();

            for(auto stateIterator = pDs->allState.begin();
                stateIterator != pDs->allState.end();
                stateIterator ++) {

                State* s = (*stateIterator);
                if (s->c == *pChar) {
                   // printf("The char is %c\n", s->c);
                    tmpDs->addCoreState(s->out);
                }
            }

            if (tmpDs->coreState.size() == 0) {
               // printf("can't reach by %c\n\n\n", *pChar);
                delete tmpDs;
                continue;
            }

            // 比较现有的状态与先前状态是否重复, 如果重复, 那么采用另一种方式加入

            DState *oldDs = alreadyHave(&dsList, tmpDs);

            if (oldDs != NULL) {        // 说明已经存在该状态
                delete tmpDs;
                //printf("already has this state\n");
                pDs->addDState(oldDs, *pChar);
            } else {
                tmpDs->getAllState();
                pDs->addDState(tmpDs, *pChar);
                dsList.push_back(tmpDs);
                this->dStateToid.insert(std::make_pair(tmpDs, this->dsCnt));
                this->idTodState.insert(std::make_pair(this->dsCnt, tmpDs));
                this->dsCnt ++;
            }
        }
    }

    dstart = ds;
    return dstart;
}





