//
// Created by gillis on 27/12/2016.
//

#ifndef FOOTBALLSIMULATOR_POSITION_H
#define FOOTBALLSIMULATOR_POSITION_H

#include "lib/easylogginpp++.h"

class Position {

private:

public:
    mutable int x;
    mutable int y;
    Position(int x,int y):x(x),y(y) {
        //LOG(INFO) << "position x:"<< this->x << " y:"<<this->y;
        positionHash = x<<16 | y;
    }
    ~Position() {
        //LOG(INFO) << "position destructed x:"<< this->x << " y:"<<this->y;
    }
    void set(int vx, int vy){
        x = vx;
        y = vy;
        positionHash = x<<16 | y;
    }
    int positionHash;
};

#endif //FOOTBALLSIMULATOR_POSITION_H
