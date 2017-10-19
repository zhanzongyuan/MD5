# MD5算法设计实现

> 姓名：詹宗沅
>
> 学号：15331386
>
> 日期：2017-10-16

</br>

## 一、算法流程及c++描述

1. 算法基本架构

   1. ![屏幕快照 2017-10-16 下午1.47.15](/Users/applecz/Documents/hw/MD5/屏幕快照 2017-10-16 下午1.47.15.png)

2. **二进制补位使得（mod 512=0）,并将二进制转为uint32\_t位**

   1. 这里补位，string转uint32\_t和`words`的存储通过声明一个类`class Words;` 内实现（后面会有Words的方法具体介绍），代码如下

      1. ```c++
         class Words{
         public:
             Words();
             //分为512 bits的小块，这里用64 byte代表处理一个小块
             Words(string str);
             uint32_t toWord(string s);
             static string toString(uint32_t n);
             //二维重载Words w;
             //w[blockIndex][offset]
             uint32_t *operator[](uint64_t blockIndex);
             //求512 bits的分段数
             uint64_t getBlockSize();
             //print
             void print();
             static uint32_t reverse(uint32_t n);
         private:
             uint32_t *data;
             //总word的长度
             uint64_t len;
         };
         ```

   2. 输入字符串Str，分解为`strLen`长度的字节串（包含`strLen`个bytes）

   3. 计算`strLen` mod 64的余数`remain`（64bytes即为512bits）

      1. ```c++
         //原消息长度
         uint64_t size=str.size();

         //size%64
         int remain=size&0x3f;
         ```

   4. 通过`remain`需要补`b'1000....`，我们将`additionLen`视为补位的bytes数（即补bytes `char(128)char(0)char(0)…..`的数目）
      1. 如果`remain`<56，直接补`additionLen=56-remain`个bytes（`char(128)char(0)char(0)...`）

      2. 如果`remain`>=56，补`additionLen=56+(64-remain)`个bytes（`char(128)char(0)char(0)...`）

      3. 通过`remain`补位的原因是为了使得bytes串的长度（mod 64=56），对应二进制串的（mod 512=488）

      4. ```c++
         //计算需要补位的长度，补位到56 bytes，再用长度值的后64 bits补上
         int additionLen=56-remain;
         if (additionLen<=0)
             additionLen=64-remain+56;
         ```

   5. 补位后得到的新长度为`newLen=additionLen+strLen`，对每4个bytes转换为一个32位uint32_t
      1. 转换方法：将`str[0]str[1]str[2]str[3]`依次从uint32\_t 低位到uint32\_t 高位放

      2. 即：
        ```c++ 
        uint32_t toWord(string s){
            return ((uint(s[3])<<24)&0xff000000)
                |((uint(s[2])<<16)&0x00ff0000)
                |((uint(s[1]<<8)&0x0000ff00)
                  |((uint(s[0]))&0xff));
        }
        ```

      3. 处理结果为一个长度为`wordsLen=newLen/4`的uint32\_t串`words`

      4. ```c++
         //补位
         //补b'100000....
         str+=char(128);
         additionLen--;
         while(additionLen!=0){
             str+=char(0);
             additionLen--;
         }
         ```

   6. 最后对`words`补64bits，这64bits来自原始输入串的位数`bitsLen=strLen*8`

      1. 先取`bitsLen`低32bits加入`words`末端，`bitsLen`高32bits加入`words`末端

      2. 所以words的结尾是由`bitsLen`的低位高位表示`[x]...[x][bitsLen.low][bitsLen.high]` 

      3. ```c++
         //原长度值的低64位，即8个byte，直接补入data
         //[low][high] 不需要大小端转换
         uint32_t high=0;
         uint32_t low=(size<<3)&0xffffffff;
         if (low<(size<<3)) high++;
         high=(size>>29)&0xffffffff;
         data[len-1]=high;
         data[len-2]=low;
         ```

   7. 最终得到位数mod 512=0的uint32_t数组`words`

3. **迭代加密 `Hmd5()`** 

   1. ![屏幕快照 2017-10-16 下午1.46.56](/Users/applecz/Documents/hw/MD5/屏幕快照 2017-10-16 下午1.46.56.png)
   2. 计算输入：$CV_i$ 和$wordsBlock_i[0...15]$ 
      1. 其中$wordsBlock_i[0...15]$ 为`words`里第$i$ 个512位块，即为16个words，所以$wordsBlock_i$ 编号是$[0...15]$ ，其中$wordsBlock_i[k]$ 表示$wordsBlock_i$ 里的第$k$ 个word（`uint32_t`) 
   3. 输出：$CV_{i+1}$ 
   4. 4轮主循环，16轮子循环
      1. 当为第一次进行Hmd5时buffering有特定的初始向量
      2. 这里buffering是全局变量，是$CV_{i}$ 子循环中间操作数
      3. 最终得到的`buffering`要和`buffering`的初始值`state`相加，得到下一轮的$CV_{i+1}$ 

   ```c++
   //4轮循环每轮16次迭代，4轮用4个函数（模式），16次迭代用16个31位字
   //主体Hmd5函数 输入16个字，4个字的向量；输出为修改MD5内部的buffering
   //一轮Hmd5后则将buffering作为下一次Hmd5的输入向量
   void MD5::Hmd5(Words messageBlocks, uint64_t blockIndex){
       uint32_t state[4];
       state[0]=buffering[0];
       state[1]=buffering[1];
       state[2]=buffering[2];
       state[3]=buffering[3];
       for (int i=0; i<4; i++){
           for (int j=0; j<16; j++){
               iterativeFunc(messageBlocks[blockIndex], j, i);
           }
       }
       buffering[0]=buffering[0]+state[0];
       buffering[1]=buffering[1]+state[1];
       buffering[2]=buffering[2]+state[2];
       buffering[3]=buffering[3]+state[3];
   }
   ```
      4. 迭代更新buffering `void iterativeFunc(uint32_t *messageBlock, int turn, int mode);`

        1. ![屏幕快照 2017-10-16 下午1.46.42](/Users/applecz/Documents/hw/MD5/屏幕快照 2017-10-16 下午1.46.42.png)

        2. buffering[0..3]视为a,b,c,d：其中计算公式为

            1. `a=b+(a+T[mode][turn]+messageBlock[K[mode][turn]]+g(b,c,d))<<<CLS_S[mode][turn]`
              1. 公式说明：
                 1. T为$T[i] = int(2^{32}|sin(i)|)$ T以及预先计算通过查表得到，对应于4x16的每个循环
                   undefinedK满足如下，预先计算静态存储
                       1. mode==0: `K=count`
                       2. mode==1: `K=(1+5*count)%16`
                       3. mode==2: `K=(5+3*count)%16`
                       4. mode==3: `K=(7*count)%16`
                        undefinedg(b,c,d)对应不同mode有不同的函数：
                        5. mode==0: `g(b,c,d)=F(b, c, d)= (((b) & (c)) | ((~b) & (d)))`
                        6. mode==1:` g(b,c,d)=G(b, c, d)= (((b) & (d)) | ((c) & (~d)))`
                        7. mode==2: `g(b,c,d)=H(b, c, d)= ((b) ^ (c) ^ (d))`
                        8. mode==3: `g(b,c,d)=I(b, c, d)= ((c) ^ ((b) | (~d)))`
                 2. CLS_S则是一个4x16的静态表预先存储
            2. 在进行完a的计算后，需要进行一个错位
               1. `buffering[0]=d; buffering[1]=a; buffering[2]=b; buffering[3]=c;`
               2. 则完成一次buffering的更新

        3. ```c++
           //输入四个字，第count次迭代，使用中间函数根据mode模式确定；更新buffering用于下一轮迭代
            void MD5::iterativeFunc(uint32_t* messageBlock, int count, int mode){
             uint32_t state[4];
             state[0]=buffering[0];
             state[1]=buffering[1];
             state[2]=buffering[2];
             state[3]=buffering[3];
             uint32_t G_bcd;
             if (mode==0) G_bcd=F(&(buffering[1]));
             else if (mode==1) G_bcd=G(&(buffering[1]));
             else if (mode==2) G_bcd=H(&(buffering[1]));
             else G_bcd=I(&(buffering[1]));
             //compute
             state[0]=buffering[0]+G_bcd+messageBlock[K[mode][count]]+T[mode][count];
             state[0]=CLS(state[0], CLS_S[mode][count]);
             state[0]=buffering[1]+state[0];
             
             buffering[0]=state[3];
             buffering[1]=state[0];
             buffering[2]=state[1];
             buffering[3]=state[2];
            }
           ```
        </br>

## 二、数据结构

> 这里为了将字符串初始化处理，将字符串补位，并将其分为多个小的32位字，我们声明了一个Words类用于抽象化多个word段，作为输入处理的数据类型
>
> 另一方面定义了类MD5，表示md5压缩转换器，其中实现了md5的中间计算过程，以及包含md5使用的静态结构

![未命名文件](/Users/applecz/Documents/hw/MD5/未命名文件.png)

- 静态表：

  - ```c++
    //MD4.hpp
    namespace md5{
        //初始向量
        const static uint32_t INITIAL_VERTOR[4]={0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476};
        //循环左的量
        const static int CLS_S[4][16] = {
            { 7,12,17,22, 7,12,17,22, 7,12,17,22, 7,12,17,22 },
            { 5, 9,14,20, 5, 9,14,20, 5, 9,14,20, 5, 9,14,20 },
            { 4,11,16,23, 4,11,16,23, 4,11,16,23, 4,11,16,23 },
            { 6,10,15,21, 6,10,15,21, 6,10,15,21, 6,10,15,21 }};
        //16循环里取X的次序
        const static int K[4][16]={
            {0,1,2,3, 4,5,6,7, 8,9,10,11, 12,13,14,15},
            {1,6,11,0, 5,10,15,4, 9,14,3,8, 13,2,7,12},
            {5,8,11,14, 1,4,7,10, 13,0,3,6, 9,12,15,2},
            {0,7,14,5, 12,3,10,1, 8,15,6,13, 4,11,2,9}
        };
        //T表
        const static uint32_t T[4][16]={
            {0xd76aa478, 0xe8c7b756, 0x242070db, 0xc1bdceee, 0xf57c0faf, 0x4787c62a, 0xa8304613, 0xfd469501,
                0x698098d8, 0x8b44f7af, 0xffff5bb1, 0x895cd7be, 0x6b901122, 0xfd987193, 0xa679438e, 0x49b40821
            },
            {0xf61e2562, 0xc040b340, 0x265e5a51, 0xe9b6c7aa, 0xd62f105d,  0x2441453, 0xd8a1e681, 0xe7d3fbc8,
                0x21e1cde6, 0xc33707d6, 0xf4d50d87, 0x455a14ed, 0xa9e3e905, 0xfcefa3f8, 0x676f02d9, 0x8d2a4c8a
            },
            {0xfffa3942, 0x8771f681, 0x6d9d6122, 0xfde5380c, 0xa4beea44, 0x4bdecfa9, 0xf6bb4b60, 0xbebfbc70,
                0x289b7ec6, 0xeaa127fa, 0xd4ef3085,  0x4881d05, 0xd9d4d039, 0xe6db99e5, 0x1fa27cf8, 0xc4ac5665
            },
            {0xf4292244, 0x432aff97, 0xab9423a7, 0xfc93a039, 0x655b59c3, 0x8f0ccc92, 0xffeff47d, 0x85845dd1,
                0x6fa87e4f, 0xfe2ce6e0, 0xa3014314, 0x4e0811a1, 0xf7537e82, 0xbd3af235, 0x2ad7d2bb, 0xeb86d391
            }
        };
    }
    ```

</br>

## 三、实验结果

![屏幕快照 2017-10-16 下午1.36.48](/Users/applecz/Documents/hw/MD5/屏幕快照 2017-10-16 下午1.36.48.png)

![屏幕快照 2017-10-16 下午1.36.38](/Users/applecz/Documents/hw/MD5/屏幕快照 2017-10-16 下午1.36.38.png)

- 比较RFC文件测试结果

![屏幕快照 2017-10-16 下午1.29.23](/Users/applecz/Documents/hw/MD5/屏幕快照 2017-10-16 下午1.29.23.png)

结果一致

</br>

## 四、实验体会

这次实验真的坑很多，虽然仔细查看了RFC的md5论文，但是还是出了算法部分理解上的偏差，特别是在做初始化补位的时候。一开始没有理解算法的补位原则是小端规则，后来及时发现并改正。

再者是最后补原长度值的64位，没发现是低32位在前，高32位在后并且不用进行大小端转换

最后，通过这次实验确实提高了自己查阅文档，阅读他人代码的能力（原md5论文的代码真的写的很差！）