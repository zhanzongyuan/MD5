
//  MD5.cpp
//  MD5
//
//  Created by applecz on 2017/10/13.
//  Copyright © 2017年 applecz. All rights reserved.
//

#include "MD5.hpp"
using namespace md5;
MD5::MD5(){
    inital();
}



//输出长度为16的字符串为最终压缩结果
string MD5::md5(string message){
    inital();
    Words words(message);
    uint64_t blockLength=words.getBlockSize();
    for (uint64_t i=0;i<blockLength; i++){
        Hmd5(words, i);
    }
    return print();
}

//T表
//const static uint32_t T_TABLE[256];
//a, b, c, d buffering
//uint32_t buffering[4];

void MD5::inital(){
    buffering[0]=INITIAL_VERTOR[0];
    buffering[1]=INITIAL_VERTOR[1];
    buffering[2]=INITIAL_VERTOR[2];
    buffering[3]=INITIAL_VERTOR[3];
}

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

//迭代函数
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
    state[0]=Words::CLS(state[0], CLS_S[mode][count]);
    state[0]=buffering[1]+state[0];
    
    buffering[0]=state[3];
    buffering[1]=state[0];
    buffering[2]=state[1];
    buffering[3]=state[2];
}

//funcMode模式：F G H I
//输入三个字；输出一个字
uint32_t MD5::F(uint32_t *bcd){
    return (bcd[0]&bcd[1])|((~bcd[0])&bcd[2]);
}
uint32_t MD5::G(uint32_t *bcd){
    return (bcd[0]&bcd[2])|(bcd[1]&(~bcd[2]));
}
uint32_t MD5::H(uint32_t *bcd){
    return bcd[0]^bcd[1]^bcd[2];
}
uint32_t MD5::I(uint32_t *bcd){
    return bcd[1]^(bcd[0]|(~bcd[2]));
}

//打印结果
string MD5::print(){
    string s="";
    for(int i=0; i<4; i++){
        uint32_t reBuf=Words::reverse(buffering[i]);
        s+=Words::toString(reBuf);
    }
    cout<<s<<endl;
    inital();
    return s;
}

