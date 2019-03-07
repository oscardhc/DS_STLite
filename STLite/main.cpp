//
//  main.cpp
//  STLite
//
//  Created by Haichen Dong on 2019/3/3.
//  Copyright © 2019 Haichen Dong. All rights reserved.
//

//#include <stdio.h>
//#include <stdlib.h>
//#include "SQLite.hpp"
//#include "priority_queue.hpp"
//
//int main(int argc, const char * argv[]) {
//    // insert code here...
//
//    return 0;
//}

// provided by 徐植天

#include<cstdio>
#include<algorithm>
#include<queue>
#include<vector>
#include<ctime>
#include<cstdlib>
#include<cstring>
#include<iostream>
#include<cctype>
#include<utility>
#include "priority_queue.hpp"
#include "deque.hpp"

using namespace sjtu;

int main() {
    Block<int> b1;
    for (int i = 1;i <= 10;i++) b1.insertAt(0,i);
    b1.print();
    b1.insertAt(2, 100);
    b1.print();
    return 0;
}
