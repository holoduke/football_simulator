//
// Created by gillis on 26/12/2016.
//

#ifndef FOOTBALLSIMULATOR_LINEUP_H
#define FOOTBALLSIMULATOR_LINEUP_H

#include <math.h>
#include <vector>
#include <unordered_map>
#include "Player.h"
#include "MoveOption.h"
#include "PenaltyAction.h"
#include "lib/easylogginpp++.h"
#include "Position.h"

class Lineup {

    private:
    float pDistance(int x, int y, int x1, int y1, int x2, int y2);

    public:
        Lineup(){

        }
        ~Lineup(){
            vector<MoveOption *>::iterator moveIter;
            //clear cached moveoptions
            for (int i=0; i < players.size();i++){
                std::vector<MoveOption *> moveOptions = cachedMoveOptions[players[i]->id];

                for (int i=0; i < moveOptions.size(); i++){
                    delete ((MoveOption *)moveOptions[i]);
                }
                moveOptions.clear();
            }

            for (int i=0; i < players.size();i++){
               delete players[i]; //position gets deleted in player destruct
            }


        }
        enum class Side {Home,Away};
        void addPlayer(Player * player, Player::Side side);
        float distanceToSegement(Player * enemyPlayer, Player * playerA, Player * playerB);
        float getDistance(Player * playerA, Player * playerB);
        Player * getPlayerAt(Position & p);
        Player * getPlayerById(int id);
        Player * getRandomPlayerFromTeam(Player::Side side);
        Player * getRandomPlayerFromTeam(Player::Side side,Player * exclude);
        std::vector<MoveOption *> getMoveOptions(Player * p);
        void calculateMoveProbabilities(std::vector<MoveOption *> moveOptions);
        void calculateCornerProbablity(MoveOption * moveOption);
        MoveOption * pickChoice(Player * fromPlayer, std::vector<MoveOption *> moveOptions);
        void performChoice(MoveOption * moveOption);
        std::vector<Player*> getTeamEnemyPlayers(Player * p);
        void goPenalty(PenaltyAction & PenaltyAction);
        bool isPenaltyFinished();

        std::unordered_map<int,Player*> playerByPositionLookup;
        std::unordered_map<int,Player*> playerByIdLookup;
        std::vector<Player*> players;
        std::vector<Player*> homeplayers;
        std::vector<Player*> awayplayers;
        std::unordered_map<int,std::vector<MoveOption *>> cachedMoveOptions;
        int currentPenaltyTake = 0;
        int currentPenaltyRound = 0;
        int penaltyPlayerIndexHome = 0;
        int penaltyPlayerIndexAway = 0;
};

#endif //FOOTBALLSIMULATOR_LINEUP_H