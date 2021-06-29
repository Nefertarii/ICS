lab要求必须在X86-64的机器上运行  
lab有两个部分  
第一部分需要用一个C程序模拟高速缓存的操作  
第二部分需要优化一个矩阵转置函数 使其缓存不命中最小化  

### ***PART A***  
该部分需要先安装软件 valgrind(内存泄露检测工具)  
系统自带了valgrind 省去了该步骤  
valgrind 得出的形式如下  
\[ ]操作符 地址,大小  
```  
I 0400d7d4,8
M 0421c7f0,4
L 04f6b868,8
S 7ff0005c8,8
```
I 表示指令加载(instruction) 由于只对数据缓存性能做检测 因此编写的程序中可以忽略I  
L 表示数据加载(load)  
S 表示数据存储(store)  
M 表示数据修改(modify)  

该部分需要在 csim.c 里编写一个使用LRU策略的 cache 模拟器  
lab提供了一个程序csim-ref 需要写出一个和它功能一样的程序  
```
Example 
Usage: ./csim-ref \[-hv] -s <s> -E <E> -b <b> -t <tracefile>
-h: Optional help flag that prints usage info
-v: Optional verbose flag that displays trace info
-s <s>: Number of set index bits (S = 2^s is the number of sets)
-E <E>: Associativity (number of lines per set)
-b <b>: Number of block bits (B = 2^b is the block size)
-t <tracefile>: Name of the valgrind trace to replay
 ```
第一部分理解后相对简单 将需求拆分后一步步进行实现
1.读取输入的参数argv 并进行判断和处理
2.将参数存入设置好的结构中
3.利用参数计算出高速缓存的结构 并进行初始化
4.读取文件并保存
5.处理文件 将其保存至一个结构组中
6.模拟高速缓存的运行 该部分可能为难点 还没有想到好的办法处理
7.输出运行结果

