//
// Created by Administrator on 2017/10/7.
//

#ifndef JNIDEMO_CCOUNTER_H
#define JNIDEMO_CCOUNTER_H


class CCounter{
    int n;

public :
    CCounter(int v){
        n = v;
    }
    int execute(){
        int i,sum = 0;
        for (i = 0; i <= n; ++i) {
            sum += i;
        }
        return sum;
    }
};


#endif //JNIDEMO_CCOUNTER_H
