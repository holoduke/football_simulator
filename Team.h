//
// Created by gillis on 24/01/2017.
//

#ifndef FOOTBALLSIMULATOR_TEAM_H
#define FOOTBALLSIMULATOR_TEAM_H

#include "lib/easylogginpp++.h"

class Team {

public:
    int id;
    enum class FormationType {f_4_3_3,f_4_4_2};
    FormationType formationType;
    Team(int id, FormationType type):id(id),formationType(type){
    }
};
#endif //FOOTBALLSIMULATOR_TEAM_H