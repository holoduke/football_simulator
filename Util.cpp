//
// Created by gillis on 13/02/2017.
//
#include <iostream>
#include <math.h>
#include "Util.h"
#include "Player.h"
#include <iomanip>
#include <string>
#include <map>
#include <random>
#include <cmath>
#include "lib/pcg_random.hpp"

#include <random>
#include <iostream>

namespace game {

    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    std::uniform_real_distribution<> dis(0, 1);

    pcg_extras::seed_seq_from<std::random_device> seed_source;
    pcg32 rng(seed_source);


    double Util::getRandom() {
        std::uniform_real_distribution<> dis(0, 1);
        return dis(rng);
    }

    int Util::getRandomBetween(int from, int to){
        std::uniform_real_distribution<> dis(from, to+1);

        return dis(rng);
    }

    float Util::getDistanceToLine(int x, int y, int x1, int y1, int x2, int y2) {

        float A = x - x1;
        float B = y - y1;
        float C = x2 - x1;
        float D = y2 - y1;

        float dot = A * C + B * D;
        float len_sq = C * C + D * D;
        float param = -1;
        if (len_sq != 0) //in case of 0 length line
            param = dot / len_sq;

        float xx, yy;

        if (param < 0) {
            xx = x1;
            yy = y1;
        }
        else if (param > 1) {
            xx = x2;
            yy = y2;
        }
        else {
            xx = x1 + param * C;
            yy = y1 + param * D;
        }

        float dx = x - xx;
        float dy = y - yy;
        return sqrt(dx * dx + dy * dy);
    }

    float Util::getDistance(int x1, int y1, int x2, int y2) {
        return sqrt(pow(x2-x1,2) + pow(y2-y1,2));
    }
}