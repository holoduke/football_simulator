//
// Created by gillis on 26/12/2016.
//

#ifndef FOOTBALLSIMULATOR_PLAYER_H
#define FOOTBALLSIMULATOR_PLAYER_H

#include "lib/easylogginpp++.h"
#include "Position.h"
//#include "Lineup.h"
#include <string>
using namespace std;

class PlayerStats{

    public:
    int blocks = 0;
    int successfullPasses = 0;
    int failedPasses = 0;
    int shotsOnGoal = 0;
    int goals = 0;
    int goalassists = 0;
    int fouls = 0;
    int corners = 0;
    int cornerGoals = 0;
    int yellowcard = 0;
    int yellowredcard = 0;
    int redcard = 0;
};

class Player {
    private:

    public:
    enum class Type {Attack,Midfielder,Defense,GoalKeeper};
    enum class Side {Home,Away,Undefined};
    PlayerStats stats;
    int attack;
    int defense;
    int corners = 30;
    bool moveProbabilitiesCalculated = false;

    Player(const int id, Position *p, Type t, int att, int def):id(id),position(p),type(t), attack(att), defense(def){
        stats = PlayerStats();
    }
    ~Player(){
        delete position;
    }
    Type type;
    Position * position;
    const int id;
    string name = "default";
    Side side;
    std::vector<Player*> teamMembers; //only in current lineup
    //Lineup::Side side;
};


#endif //FOOTBALLSIMULATOR_PLAYER_H
