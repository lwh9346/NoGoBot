class gameRule {
public:
    //ValidPositionResult是获取可落子位置的返回结果，应传入一个指针给该函数以便该函数将数据写回
    struct ValidPositionResult {
        int numS;      //己方可落子数
        int numR;      //对方可落子数
        char resS[81]; //己方可落子位置，1可下0不可下
        char resR[81]; //对方可落子位置，1可下0不可下
    };

private:
    //找气的时候的分组
    struct Group {
        bool self;       //是否为当前方
        int posCount;    //该组含有的子数
        int groupID;     //组别
        int gasNum;      //拥有的气的数量
        char gasPos[81]; //表明某个点是否为气，不是坐标
        char pos[81];    //该组所有子的坐标
    };

private:
    struct Gas {
        int ownerID[4];       //该气的所有组的组别
        int ownerGasCount[4]; //对应的组有多少个气
        int ownerCount;       //组数
        bool onlyGasS;        //己方唯一气
        bool onlyGasR;        //对方唯一气
        bool notOnlyGasS;     //己方非唯一气
        bool notOnlyGasR;     //对方非唯一气
    };

public:
    //GetValidPosition调用前要新建一个返回值结构体并将指针传进来
    static void GetValidPosition(signed char board[81], ValidPositionResult *res) {
        //变量初始化
        signed char groupMark[81];
        Group groups[81];
        Gas gas[81];
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

private:
    static void grouping(int pos, int groupID, signed char board[81], signed char groupMark[81]) {
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
};
