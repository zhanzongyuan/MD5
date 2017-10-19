//
//  Words.hpp
//  MD5
//
//  Created by applecz on 2017/10/14.
//  Copyright © 2017年 applecz. All rights reserved.
//

#ifndef Words_hpp
#define Words_hpp

#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdint>
using namespace std;

class Words{
public:
    Words(){
        data=NULL;
        len=0;
    }
    //分为512 bits的小块，这里用64 byte代表处理一个小块
    Words(string str){
        //原消息长度
        uint64_t size=str.size();
        
        //size%64
        int remain=size&0x3f;
        
        //计算需要补位的长度，补位到56 bytes，再用长度值的后64 bits补上
        int additionLen=56-remain;
        if (additionLen<=0)
            additionLen=64-remain+56;
        
        //补位后Words的长度 size&0x3f==size%64
        len=(size+additionLen+8)>>2;
        data=new uint32_t[len];
        
        //补位
        //补b'100000....
        str+=char(128);
        additionLen--;
        while(additionLen!=0){
            str+=char(0);
            additionLen--;
        }
        //原长度值的低64位，即8个byte，直接补入data
        //[low][high] 不需要大小端转换
        uint32_t high=0;
        uint32_t low=(size<<3)&0xffffffff;
        if (low<(size<<3)) high++;
        high=(size>>29)&0xffffffff;
        data[len-1]=high;
        data[len-2]=low;
        
        uint64_t c=0;
        uint64_t index=0;
        size=str.length();
        while (size>0) {
            data[index]=toWord(str.substr(c, 4));
            index++;
            c+=4;
            size-=4;
        }
    }
    uint32_t toWord(string s){
        return ((uint(s[3])<<24)&0xff000000)
            |((uint(s[2])<<16)&0x00ff0000)
            |((uint(s[1]<<8)&0x0000ff00)
              |((uint(s[0]))&0xff));
    }
    static string toString(uint32_t n){
        ostringstream o;
        o<<hex;
        o<<((n>>28)&0xf)<<((n>>24)&0xf)<<((n>>20)&0xf)<<((n>>16)&0xf)
         <<((n>>12)&0xf)<<((n>>8)&0xf)<<((n>>4)&0xf)<<((n)&0xf);
        return o.str();
    }
    //二维重载Words w;
    //w[blockIndex][offset]
    uint32_t *operator[](uint64_t blockIndex){
        return &data[blockIndex*16];
    }
    //求512 bits的分段数
    uint64_t getBlockSize(){
        return len/16;
    }
    static uint32_t reverse(uint32_t n){
        return ((n&0xff)<<24)|((n&0xff00)<<8)|((n&0xff0000)>>8)|((n&0xff000000)>>24);
    }
    //循环左移
    static uint32_t CLS(uint32_t m, int n){
        return (m<<n)|(m>>(32-n));
    }

private:
    uint32_t *data;
    //总word的长度
    uint64_t len;
};


#endif /* Word_hpp */



