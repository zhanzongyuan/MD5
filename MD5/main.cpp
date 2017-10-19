//
//  main.cpp
//  MD5
//
//  Created by applecz on 2017/10/13.
//  Copyright © 2017年 applecz. All rights reserved.
//

#include <iostream>
#include <string>
#include <cstring>
#include <cstdint>
#include "MD5.hpp"

using namespace std;

int main(int argc, const char * argv[]) {
    string s[]={"", "a", "abc", "message digest", "abcdefghijklmnopqrstuvwxyz",
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789",
        "12345678901234567890123456789012345678901234567890123456789012345678901234567890"
    };
    md5::MD5 md5er;
    for (int i=0; i<7; i++){
        cout<<"\nmd5(\""<<s[i]<<"\")=\n";
        md5er.md5(s[i]);
    }
    return 0;
}
