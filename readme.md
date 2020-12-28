# 小章鱼

基于MCTS的NoGo的Bot

## 如何使用

### 直接编译

使用`g++`编译`io.cpp`即可，建议开启`O2`或`O3`优化，效率最多有3.5倍提升。

```bash
g++ ./io.cpp -O3
```

### 提交到botzone

由于botzone要求提交单文件，故需要用脚本将文件连接在一起。运行`combineForBotZone.py`来获得"all in one"文件`aio.cpp`。

```bash
python ./combineForBotZone.py
```

## 原理简述

本bot采用的mcts架构在网上都可以搜到，但是做了一些调整：

将q值从以往的由模拟得出0或1改为由当前局面二者可落子数差距作为估算依据，其中x是当前方可落子数，y是对方可落子数
$$
q=\tanh(\frac{x-y}{x+y})
$$
这一公式避免了过分贪心，越靠后的节点取得优势给出的q值越高，使得程序拥有战略眼光。