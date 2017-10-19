
//  MD5.hpp
//  MD5
//
//  Created by applecz on 2017/10/13.
//  Copyright © 2017年 applecz. All rights reserved.
//

#ifndef MD5_hpp
#define MD5_hpp

#include <stdio.h>
#include <cstdint>
#include <string>
#include <iostream>
#include "Words.hpp"
using namespace std;

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
    
    class MD5{
    public:
        MD5();
        //输出长度为16的字符串为最终压缩结果
        string md5(string message);
    private:
        //a, b, c, d buffering
        uint32_t buffering[4];
        
        //初始化
        void inital();
        
        //4轮循环每轮16次迭代，4轮用4个函数（模式），16次迭代用16个31位字
        //主体Hmd5函数 输入16个字，4个字的向量；输出为修改MD5内部的buffering
        //一轮Hmd5后则将buffering作为下一次Hmd5的输入向量
        void Hmd5(Words messageBlocks, uint64_t blockIndex);
        
        
        //迭代函数
        //输入四个字，第count次迭代，使用中间函数根据mode模式确定；更新buffering用于下一轮迭代
        void iterativeFunc(uint32_t* messageBlock, int count, int mode);
        
        //funcMode模式：F G H I
        //输入三个字；输出一个字
        uint32_t F(uint32_t *bcd);
        uint32_t G(uint32_t *bcd);
        uint32_t H(uint32_t *bcd);
        uint32_t I(uint32_t *bcd);
        
        
        //print 打印压缩结果，最终压缩结果于buffering中
        string print();
        
    };
};

#endif /* MD5_hpp */


