#include "./gameRule.cpp"
using namespace std;

struct TreeNode {
    TreeNode *parent;
    TreeNode *children[81];
    signed char board[81];
    int childrenAction[81];
    int childrenCount;
    int childrenCountMax;
    double q;
    int n;
    int *totalN;
    gameRule::ValidPositionResult res;
};

//action是上一把的家伙下的棋
TreeNode *createNewTreeNode(TreeNode *parent, int action) {
    TreeNode *t = new TreeNode;
    t->q = 0.0;
    t->n = 0;
    t->parent = parent;
    t->childrenCount = 0;
    t->totalN = parent->totalN;
    for (int i = 0; i < 81; i++) {
        t->board[i] = -parent->board[i];
    }
    t->board[action] = -1;
    gameRule::GetValidPosition(t->board, &t->res);
    t->childrenCountMax = 0;
    for (int i = 0; i < 81; i++) {
        if (t->res.resS[i] == 1) {
            t->childrenAction[t->childrenCountMax] = i;
            t->childrenCountMax++;
        }
    }
    return t;
}

//棋盘不需要反转
TreeNode *createRootTreeNode(signed char board[81], int *totalN) {
    TreeNode *t = new TreeNode;
    t->q = 0.0;
    t->n = 0;
    t->parent = nullptr;
    t->childrenCount = 0;
    t->totalN = totalN;
    for (int i = 0; i < 81; i++) {
        t->board[i] = board[i];
    }
    gameRule::GetValidPosition(t->board, &t->res);
    t->childrenCountMax = 0;
    for (int i = 0; i < 81; i++) {
        if (t->res.resS[i] == 1) {
            t->childrenAction[t->childrenCountMax] = i;
            t->childrenCountMax++;
        }
    }
    return t;
}
