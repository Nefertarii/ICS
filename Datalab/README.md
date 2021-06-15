```c
int bitXor(int x, int y) //1
{
    /*
    做表统计 x^y=~(x&y)&(x|y)
    而|运算，可以通过~和&构造出来
    即x|y=~(~x&~y)。
    */
    return (~(x & y)) & (~(~x & ~y));
}
```

```c
int tmin(void) //1
{
    /*
    最长32位字节 Tmin = 1000...0 直接逻辑位移
    */
    return 1 << 31;
}
```

```c
int isTmax(int x)
{
    /*
    利用异或性质0异或任何数等于其本身
    如果是Tmax tmp1为0 tmp为全0 取非后为1  return 0^1 = 1 
    如果是-1 tmp为1 tmp为0 取非后为1 return 1^1 = 0
    如果是其他任何数如 2  tmp1为0  tmp2不为0 取非后为0  return 0^0 = 0
    */
    int tmp1 = !(x + 1);
    int tmp2 = !((x + 1) ^ (~x));
    return tmp1 ^ tmp2;
}
```

```c
int allOddBits(int x)
{
    /*
    掩码1010...1010
    取得x的奇数位1 再与掩码异或
    若x奇数位全为1 异或得0
    返回时取非 上一步得0返回1 否则返回0
    */
    //难点在于不能直接使用 0xAAAAAAAA;
    int temp = 0xaa+(0xaa<<8) 
    return !x;
}
```

```c
int negate(int x) //1
{
    /*
    取反再+1
    */
    return (~x) + 1;
}
```

```c
int isAsciiDigit(int x)
{
    /*
    upper 0x39  0011 1001b 加上比0x39大的数后符号由正变负
    lower 0x30  0011 0000b 加上比0x30小的值时是负数
    返回取符号位 全0返回0 否则返回1
    通过两数相减结果的正负来判断两个数之间的大小关系
    不支持减法 但是a-b可以转化为a+(-b)
    -b == ~b+1，即 a-b == a+(~b+1)
    判断该结果最高有效位是否为1，来判断结果的正负
    */
    int sign = 0x1 << 31;
    int upper = ~(sign | 0x39);
    int lower = ~0x30;
    upper = sign & (upper + x) >> 31;
    lower = sign & (lower + 1 + x) >> 31;
    return !(upper | lower);
}
```

```c
int conditional(int x, int y, int z)
{
    /*
  x=true return y else return z
  先对x取非 构造成全0或者全1
  利用逻辑左移加算术右移，可以构造出一个0x0 或者是0xffffffff
  利用获得的x作为掩码 
  进行(z&x),(y&~x)操作 必定有一侧为全0
  利用或操作 (z|y) 选择出需要的数字
  */
    x = (!x << 31) >> 31;
    return (z & x) | (y & ~x);
}
```

```c
int isLessOrEqual(int x, int y)
{
    /*
    先取符号位 两个符号不同的补码相减可能会造成溢出
    符号位相同则两数相减 再根据符号位判断大小
    如果不同 可以很容易得到结果 如果相同 则需要相减来判断
    */
    int signx = (x >> 31) & 1;
    int signy = (y >> 31) & 1;
    int signdif = (!signy) & signx;
    int signsam = (!(signx ^ signy)) & (((x + (~y)) >> 31) & 1);
    return signdif | signsam;
}
```

```c
int logicalNeg(int x)
{
    /*
    利用其补码的性质 除了0和Tmin外其他数都是互为相反数关系
    0和最小数的补码是本身 不过0的符号位与其补码符号位位或为0 最小数的为1
    */
    return ((x | (~x + 1)) >> 31) + 1;
}
```

```c
int howManyBits(int x)
{
    /*
    ???
    */
    return 0;
}
```
