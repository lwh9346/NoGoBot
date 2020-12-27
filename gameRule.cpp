namespace gameRule {
//ValidPositionResult 是获取可落子位置的返回结果，应传入一个指针给该函数以便该函数将数据写回
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

void grouping(int pos, int groupID, signed char board[81], signed char groupMark[81]);

//GetValidPosition 调用前要新建一个返回值结构体并将指针传进来
void GetValidPosition(signed char board[81], ValidPositionResult *res) {
    //变量初始化
    signed char groupMark[81];
    Group groups[81];
    int groupNum = 0;
    res->numR = 0;
    res->numS = 0;
    for (int i = 0; i < 81; i++) {
        groupMark[i] = -1;
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
        groups[i].pos[groups[i].posCount] = i;
        groups[i].posCount++;
    }

    //找气
    for (int i = 0; i < groupNum; i++) {
        groups[i].self = board[groups[i].pos[0]] == 1;//正反双方的判断放在这了
        for (int j = 0; j < groups[i].posCount; j++) {
            int pos = groups[i].pos[j];
            int x = pos / 9, y = pos % 9;
            int t = board[pos];
            int up = pos - 9;
            int down = pos + 9;
            int left = pos - 1;
            int right = pos + 1;
            if (x != 0 && board[up] == 0) {
                groups[i].gasPos[up]=1;
            }
            if (x != 8 && board[down] == 0) {
                groups[i].gasPos[down]=1;
            }
            if (y != 0 && board[left] == 0) {
                groups[i].gasPos[left]=1;
            }
            if (y != 8 && board[right] == 0) {
                groups[i].gasPos[right]=1;
            }
        }
        //TODO:统计气数，并填充气结构体（该结构体还没完成）
    }
}

void grouping(int pos, int groupID, signed char board[81], signed char groupMark[81]) {
    groupMark[pos] = groupID;
    int x = pos / 9, y = pos % 9;
    int t = board[pos];
    int up = pos - 9;
    int down = pos + 9;
    int left = pos - 1;
    int right = pos + 1;
    if (x != 0 && groupMark[up] == -1 && t == board[up]) {
        grouping(left, groupID, board, groupMark);
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
} // namespace gameRule
