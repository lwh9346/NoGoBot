#cgo LDFLAGS: -lm
#pragma GCC optimize(3)
#include <malloc.h>
#include <math.h>
#include <time.h>
#define bool char
#define true 1
#define false 0

//ValidPositionResult是获取可落子位置的返回结果，应传入一个指针给该函数以便该函数将数据写回
struct ValidPositionResult {
    int numS;      //己方可落子数
    int numR;      //对方可落子数
    char resS[81]; //己方可落子位置，1可下0不可下
    char resR[81]; //对方可落子位置，1可下0不可下
};
//找气的时候的分组
struct Group {
    bool self;       //是否为当前方
    int posCount;    //该组含有的子数
    int groupID;     //组别
    int gasNum;      //拥有的气的数量
    char gasPos[81]; //表明某个点是否为气，不是坐标
    char pos[81];    //该组所有子的坐标
};
//气
struct Gas {
    int ownerID[4];       //该气的所有组的组别
    int ownerGasCount[4]; //对应的组有多少个气
    int ownerCount;       //组数
    bool onlyGasS;        //己方唯一气
    bool onlyGasR;        //对方唯一气
    bool notOnlyGasS;     //己方非唯一气
    bool notOnlyGasR;     //对方非唯一气
};
//分组
void grouping(int pos, int groupID, signed char board[81], signed char groupMark[81]) {
    groupMark[pos] = groupID;
    int x = pos / 9, y = pos % 9;
    int t = board[pos];
    int up = pos - 9;
    int down = pos + 9;
    int left = pos - 1;
    int right = pos + 1;
    if (x != 0 && groupMark[up] == -1 && t == board[up]) {
        grouping(up, groupID, board, groupMark);
    }
    if (x != 8 && groupMark[down] == -1 && t == board[down]) {
        grouping(down, groupID, board, groupMark);
    }
    if (y != 0 && groupMark[left] == -1 && t == board[left]) {
        grouping(left, groupID, board, groupMark);
    }
    if (y != 8 && groupMark[right] == -1 && t == board[right]) {
        grouping(right, groupID, board, groupMark);
    }
}
void GetValidPosition(signed char board[81], struct ValidPositionResult *res) {
    //变量初始化
    signed char groupMark[81];
    struct Group groups[81];
    struct Gas gas[81];
    int groupNum = 0;
    res->numR = 0;
    res->numS = 0;
    for (int i = 0; i < 81; i++) {
        groupMark[i] = -1;
        gas[i].ownerCount = 0;
        gas[i].notOnlyGasR = false;
        gas[i].notOnlyGasS = false;
        gas[i].onlyGasR = false;
        gas[i].onlyGasS = false;
        groups[i].groupID = i;
        groups[i].posCount = 0;
        groups[i].gasNum = 0;
        for (int j = 0; j < 81; j++) {
            groups[i].gasPos[j] = 0;
        }
    }

    //分组
    for (int i = 0; i < 81; i++) {
        if (groupMark[i] != -1 || board[i] == 0) { //跳过已分组元素与空格
            continue;
        }
        grouping(i, groupNum, board, groupMark);
        groupNum++;
    }

    //填充分组结构体
    for (int i = 0; i < 81; i++) {
        if (groupMark[i] == -1) {
            continue;
        }
        groups[groupMark[i]].pos[groups[groupMark[i]].posCount] = i;
        groups[groupMark[i]].posCount++;
    }

    //找气
    for (int i = 0; i < groupNum; i++) {
        groups[i].self = board[groups[i].pos[0]] == 1; //正反双方的判断放在这了
        for (int j = 0; j < groups[i].posCount; j++) { //标记气
            int pos = groups[i].pos[j];
            int x = pos / 9, y = pos % 9;
            int up = pos - 9;
            int down = pos + 9;
            int left = pos - 1;
            int right = pos + 1;
            if (x != 0 && board[up] == 0) {
                groups[i].gasPos[up] = 1;
            }
            if (x != 8 && board[down] == 0) {
                groups[i].gasPos[down] = 1;
            }
            if (y != 0 && board[left] == 0) {
                groups[i].gasPos[left] = 1;
            }
            if (y != 8 && board[right] == 0) {
                groups[i].gasPos[right] = 1;
            }
        }
        int gasPos[81];
        int gasCount = 0;
        for (int j = 0; j < 81; j++) { //气数统计
            if (groups[i].gasPos[j] == 1) {
                gasPos[gasCount] = j;
                gasCount++;
            }
        }
        for (int j = 0; j < gasCount; j++) { //填充气结构体
            gas[gasPos[j]].ownerGasCount[gas[gasPos[j]].ownerCount] = gasCount;
            gas[gasPos[j]].ownerID[gas[gasPos[j]].ownerCount] = i;
            gas[gasPos[j]].ownerCount++;
        }
    }

    //处理气数据
    for (int i = 0; i < 81; i++) {
        for (int j = 0; j < gas[i].ownerCount; j++) {
            if (groups[gas[i].ownerID[j]].self) { //己方
                if (gas[i].ownerGasCount[j] == 1) {
                    gas[i].onlyGasS = true;
                    continue;
                }
                gas[i].notOnlyGasS = true;
            } else { //对方
                if (gas[i].ownerGasCount[j] == 1) {
                    gas[i].onlyGasR = true;
                    continue;
                }
                gas[i].notOnlyGasR = true;
            }
        }
    }

    //获取最终结果
    for (int pos = 0; pos < 81; pos++) {
        int x = pos / 9, y = pos % 9;
        int up = pos - 9;
        int down = pos + 9;
        int left = pos - 1;
        int right = pos + 1;
        bool atTop = x == 0;
        bool atBut = x == 8;
        bool atLef = y == 0;
        bool atRig = y == 8;

        //己方结果
        if (board[pos] != 0 || gas[pos].onlyGasR) { //非空格或对方唯一气，不能下
            res->resS[pos] = 0;
            goto r;
        }
        if (gas[pos].notOnlyGasS) { //己方非唯一气，能下
            res->resS[pos] = 1;
            res->numS++;
            goto r;
        }
        if (gas[pos].onlyGasS) { //如果是己方唯一气，还需判断
            //如果边上还有气就可以下
            if ((!atTop && board[up] == 0) || (!atBut && board[down] == 0) || (!atLef && board[left] == 0) || (!atRig && board[right] == 0)) {
                res->resS[pos] = 1;
                res->numS++;
                goto r;
            }
            //边上没气就下不了了
            res->resS[pos] = 0;
            goto r;
        }
        if ((atTop || board[up] == -1) && (atBut || board[down] == -1) && (atLef || board[left] == -1) && (atRig || board[right] == -1)) { //死点
            res->resS[pos] = 0;
            goto r;
        }
        //啥都不是
        res->resS[pos] = 1;
        res->numS++;

    r:
        //对方结果
        if (board[pos] != 0 || gas[pos].onlyGasS) { //非空格或对方唯一气，不能下
            res->resR[pos] = 0;
            continue;
        }
        if (gas[pos].notOnlyGasR) { //己方非唯一气，能下
            res->resR[pos] = 1;
            res->numR++;
            continue;
        }
        if (gas[pos].onlyGasR) { //如果是己方唯一气，还需判断
            //如果边上还有气就可以下
            if ((!atTop && board[up] == 0) || (!atBut && board[down] == 0) || (!atLef && board[left] == 0) || (!atRig && board[right] == 0)) {
                res->resR[pos] = 1;
                res->numR++;
                continue;
            }
            //边上没气就下不了了
            res->resR[pos] = 0;
            continue;
        }
        if ((atTop || board[up] == 1) && (atBut || board[down] == 1) && (atLef || board[left] == 1) && (atRig || board[right] == 1)) { //死点
            res->resR[pos] = 0;
            continue;
        }
        //啥都不是
        res->resR[pos] = 1;
        res->numR++;
    }
}
//
//MCTS部分
//
//树节点
struct TreeNode {
    struct TreeNode *parent;
    struct TreeNode *children[81];
    signed char board[81];
    int childrenAction[81];
    int childrenCount;
    int childrenCountMax;
    double q; //位于+-n之间的实数
    int n;
    int *totalN;
    struct ValidPositionResult res;
};
//新建树节点
struct TreeNode *createNewTreeNode(struct TreeNode *parent, int action) {
    struct TreeNode *t = malloc(sizeof(struct TreeNode));
    t->q = 0.0;
    t->n = 0;
    t->parent = parent;
    t->childrenCount = 0;
    t->totalN = parent->totalN;
    for (int i = 0; i < 81; i++) {
        t->board[i] = -parent->board[i];
    }
    t->board[action] = -1;
    GetValidPosition(t->board, &t->res);
    t->childrenCountMax = 0;
    for (int i = 0; i < 81; i++) {
        if (t->res.resS[i] == 1) {
            t->childrenAction[t->childrenCountMax] = i;
            t->childrenCountMax++;
        }
    }
    return t;
}
//创建根节点
//棋盘不需要反转
//totalN是计数器
struct TreeNode *createRootTreeNode(signed char board[81], int *totalN) {
    struct TreeNode *t = malloc(sizeof(struct TreeNode));
    t->q = 0.0;
    t->n = 0;
    t->parent = NULL;
    t->childrenCount = 0;
    t->totalN = totalN;
    for (int i = 0; i < 81; i++) {
        t->board[i] = board[i];
    }
    GetValidPosition(t->board, &t->res);
    t->childrenCountMax = 0;
    for (int i = 0; i < 81; i++) {
        if (t->res.resS[i] == 1) {
            t->childrenAction[t->childrenCountMax] = i;
            t->childrenCountMax++;
        }
    }
    return t;
}
struct TreeNode *treePolicy(struct TreeNode *t) {
    if (t->childrenCountMax == 0) {
        return t;
    }
    if (t->childrenCount < t->childrenCountMax) {
        struct TreeNode *c = createNewTreeNode(t, t->childrenAction[t->childrenCount]);
        t->children[t->childrenCount] = c;
        t->childrenCount++;
        return c;
    }
    double maxScore = -1.0;
    int maxI = 0;
    for (int i = 0; i < t->childrenCount; i++) {
        double s = t->children[i]->q / (double)(t->children[i]->n) + 0.2 * sqrt(log((double)(*t->totalN)) / (double)(t->children[i]->n)); //UCT公式
        if (s > maxScore) {
            maxScore = s;
            maxI = i;
        }
    }
    return treePolicy(t->children[maxI]);
}
double defaultPolicy(struct TreeNode *t) {
    if (t->res.numR + t->res.numS == 0) {
        return 0.0;
    }
    if (t->res.numR == 0) {
        return 1.0;
    }
    if (t->res.numS == 0) {
        return -1.0;
    }
    return tanh((double)(t->res.numS - t->res.numR) / (double)(t->res.numR + t->res.numS));
}
void backup(double delta, struct TreeNode *t) {
    int i = 0;
    while (t != NULL) {
        t->q += i % 2 == 1 ? delta : -delta;
        t->n++;
        t = t->parent;
        i++;
    }
}
void deleteTree(struct TreeNode *t) {
    for (int i = 0; i < t->childrenCount; i++) {
        if (t->children[i]->childrenCount == 0) {
            free(t->children[i]);
        } else {
            deleteTree(t->children[i]);
        }
    }
    free(t);
}
struct DebugData {
    double winningRate; //胜率（不准确）
    int nodeCount;      //mcts搜索的节点数
};
//获取当前棋盘下的最佳行动
//输入的棋盘不需要正反方反转，但是需要xy反转(不然调试的时候很难受)
//+1是己方，0是空格，-1是对方
//时限单位为秒，超过时限后才会停止
//debug是调试信息，在外部新建一个对象后传指针进来即可
//如果不需要调试信息的话，可以传nullptr进来
//该函数执行完毕后会自动释放内存
//返回值为x*9+y
int GetBestAction(signed char board[81], double timeOut, struct DebugData *debug) {
    int totalN = 0;
    int end = (int)(timeOut * (double)(CLOCKS_PER_SEC)) + clock();
    struct TreeNode *root = createRootTreeNode(board, &totalN);
    while (clock() < end) {
        //为了减少获取时间的性能开销，执行16步以后再获取时间
        for (int i = 0; i < 16; i++) {
            totalN++;
            struct TreeNode *t = treePolicy(root);
            double delta = defaultPolicy(t);
            backup(delta, t);
        }
    }
    int maxN = root->children[0]->n;
    int maxI = 0;
    for (int i = 1; i < root->childrenCount; i++) {
        if (root->children[i]->n > maxN) {
            maxI = i;
            maxN = root->children[i]->n;
        }
    }
    int bestAction = root->childrenAction[maxI];
    if (debug != NULL) {
        debug->nodeCount = totalN;
        debug->winningRate = (root->children[maxI]->q / (double)(root->children[maxI]->n) + 1) * 0.5;
    }
    deleteTree(root);
    return bestAction;
}