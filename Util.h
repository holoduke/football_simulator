//
// Created by gillis on 13/02/2017.
//

#ifndef FOOTBALLSIMULATOR_UTIL_H
#define FOOTBALLSIMULATOR_UTIL_H


#include "Player.h"

namespace game {
    class Util {
    public:
        static double getRandom();
        static int getRandomBetween(int from, int to);
        static float getDistanceToLine(int x, int y, int x1, int y1, int x2, int y2);
        static float getDistance(int x, int y, int x1, int y1);
    };
}


#endif //FOOTBALLSIMULATOR_UTIL_H
