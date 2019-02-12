//
// Created by gillis on 26/12/2016.
//

#include "Lineup.h"
#include "Util.h"
#include "Player.h"
#include "MoveOption.h"
#include "PenaltyAction.h"
#include "lib/easylogginpp++.h"
#include <vector>
#include <algorithm>
#include <math.h>

float Lineup::distanceToSegement(Player * enemyPlayer, Player * playerA, Player * playerB){
    return game::Util::getDistanceToLine(enemyPlayer->position->x,enemyPlayer->position->y,playerA->position->x,playerA->position->y,playerB->position->x,playerB->position->y);
}

float Lineup::getDistance(Player * playerA, Player * playerB){
    return game::Util::getDistance(playerA->position->x,playerA->position->y,playerB->position->x,playerB->position->y);
}

Player * Lineup::getPlayerAt(Position & p){
    return playerByPositionLookup[p.positionHash];
}

Player * Lineup::getPlayerById(int id){
    return playerByIdLookup[id];
}

Player * Lineup::getRandomPlayerFromTeam(Player::Side side){
    if (side == Player::Side::Home){
        return homeplayers[std::floor(game::Util::getRandom()*homeplayers.size())];
    }
    return awayplayers[std::floor(game::Util::getRandom()*awayplayers.size())];
}

Player * Lineup::getRandomPlayerFromTeam(Player::Side side, Player * exclude){

    std::vector<Player*> pool = awayplayers;
    if (side == Player::Side::Home){
        pool = homeplayers;
    }

    //create ondemand teammembers
    if (exclude->teamMembers.size() == 0){
        vector<Player *>::iterator playerIter;
        for(playerIter = pool.begin(); playerIter != pool.end(); playerIter++) {
            if ((*playerIter)->id == exclude->id) continue;
            exclude->teamMembers.push_back((*playerIter));
        }
    }
    return exclude->teamMembers[std::floor(game::Util::getRandom()*exclude->teamMembers.size())];
}

void Lineup::addPlayer(Player * player, Player::Side side){
    playerByPositionLookup[player->position->positionHash] = player;
    //playerByIdLookup[player->id] = player;
    player->side = side;
    players.push_back(player);

    if (side == Player::Side::Home){
        homeplayers.push_back(player);
    }
    else{
        awayplayers.push_back(player);
    }
}

std::vector<Player*> Lineup::getTeamEnemyPlayers(Player * player){

    std::vector<Player*> enemyPlayers;

    for (int i=0; i < players.size();i++){
        if (players[i]->side == player->side) continue;
        enemyPlayers.push_back(players[i]);
    }

    return enemyPlayers;
}

void Lineup::performChoice(MoveOption * moveOption){
//console.log("----")
    moveOption->result = true;

    //first check goal
    if (moveOption->goalKeeperBlockChance > 0){
        moveOption->result = game::Util::getRandom() < ((100-(moveOption->goalKeeperBlockChance*100))/100.0);
        if (!moveOption->result){
            moveOption->whenLostPassTo = moveOption->targetPlayer;
        }
        return;
    }

    vector<MoveOptionInterceptor *>::iterator interceptIter;
    for(interceptIter = moveOption->interceptors.begin(); interceptIter != moveOption->interceptors.end(); interceptIter++) {

        MoveOptionInterceptor *interceptor = (*interceptIter);

        if (interceptor->blockChance > 0){
            moveOption->result = game::Util::getRandom() < ((100-(interceptor->blockChance*100))/100.0);

            if (!moveOption->result){
                moveOption->whenLostPassTo = interceptor->player;
                break;
            }
        }
    }
}

MoveOption * Lineup::pickChoice(Player * fromPlayer, std::vector<MoveOption *> moveOptions){

    MoveOption::MoveStyle style;

    bool useGoaly = (game::Util::getRandom() >= ((100-20)/100.0)); //20% that we pick goal

    if (fromPlayer->type == Player::Type::GoalKeeper){ //goalkeepers cannot play backwards
        style = MoveOption::MoveStyle::Attack;
        useGoaly = (game::Util::getRandom() >= ((100-3)/100.0)); //%2 that we pick goal
    }
    else if (fromPlayer->type == Player::Type::Attack && !useGoaly){ //this case is for an attacker. which cannot always attack
        if (game::Util::getRandom() >= ((100-80)/100.0)){
            style = MoveOption::MoveStyle::Neutral;
        }
        else{
            style = MoveOption::MoveStyle::Defense;
        }
    }
    else if (game::Util::getRandom() >= ((100-30)/100.0)){ //30% chance we go in attack
        style = MoveOption::MoveStyle::Attack;
    }
    else if (game::Util::getRandom() >= ((100-80)/100.0)){ //80 change go neutral
        style = MoveOption::MoveStyle::Neutral;
    }
    else{
        style = MoveOption::MoveStyle::Defense;
    }



    if (moveOptions.size() == 0){
        LOG(INFO) << "no move options available";
    }

    vector<MoveOption *>::iterator moveIter;

    vector<MoveOption *> newMoves;

    for(moveIter = moveOptions.begin(); moveIter != moveOptions.end(); moveIter++) {
        MoveOption * moveoption = (*moveIter);
        if (moveoption->moveStyle != style) continue;
        if (!useGoaly && style == MoveOption::MoveStyle::Attack && moveoption->targetPlayer->type == Player::Type::GoalKeeper) continue;
        newMoves.push_back(moveoption);
    }

    //LOG(INFO) << "get item from #moves" << newMoves.size() << " test: " << game::Util::getRandom() << "------" << std::floor(game::Util::getRandom()*newMoves.size());

    return newMoves[std::floor(game::Util::getRandom()*newMoves.size())];
}

void Lineup::calculateCornerProbablity(MoveOption * moveOption){
    moveOption->cornerShooter = getRandomPlayerFromTeam(moveOption->fromPlayer->side);
    moveOption->cornerGoalShooter = getRandomPlayerFromTeam(moveOption->fromPlayer->side,moveOption->cornerShooter);

    moveOption->cornerChance = 20; //default value for corner chance
    moveOption->cornerShooterChance = 40 * ((100-(100-moveOption->cornerShooter->corners))/100.0); //chance corner is shot succesfully
    moveOption->cornerGoalChance = 40 * ((100-(100-moveOption->cornerGoalShooter->corners))/100.0); //chance corner ends up in goal
    moveOption->missedShotResultedInCornerGoalChance = ((moveOption->cornerShooterChance/100.0) * (moveOption->cornerGoalChance/100.0)) * 100.0;
}

void Lineup::calculateMoveProbabilities(std::vector<MoveOption *> moveOptions){

    vector<MoveOption *>::iterator moveIter;

    for(moveIter = moveOptions.begin(); moveIter != moveOptions.end(); moveIter++) {

        MoveOption * moveoption = (*moveIter);

        if (moveoption->shotType == MoveOption::ShotTypes::Goal_long_shot){
            float CORRECTION_PARTIAL_PASS = 0.1;
            float CORRECTION_FRONTAL_PASS = 0.1; //increases overal chance of blocking and thus decreasing chance of scoring
            float CORRECTION_GOALKEEPER_BLOCK_CHANCE = 20;

            //example (100 / ((60/(0.1*60))+1))/100; = 0.17 because 0.1 makes it smaller. with 0.1 being 1, result would be 0.5 = 50% chance

            vector<MoveOptionInterceptor *>::iterator interceptIter;
            for(interceptIter = moveoption->interceptors.begin(); interceptIter != moveoption->interceptors.end(); interceptIter++) {

                MoveOptionInterceptor * interceptor = (*interceptIter);

                if (interceptor->positionType == MoveOptionInterceptor::InterceptorPosition::Behind_player) continue;

                if (interceptor->positionType == MoveOptionInterceptor::InterceptorPosition::In_front_player && interceptor->type == MoveOptionInterceptor::InterceptionType::Pass_heading_partly_in_direction_to){

                    interceptor->blockChance = (100 / ((moveoption->fromPlayer->attack/(CORRECTION_PARTIAL_PASS*interceptor->player->defense))+1))/100.0;
                }
                else if (interceptor->positionType == MoveOptionInterceptor::InterceptorPosition::In_front_player && interceptor->type == MoveOptionInterceptor::InterceptionType::Frontal_pass){

                    interceptor->blockChance = (100 / ((moveoption->fromPlayer->attack/(CORRECTION_FRONTAL_PASS*interceptor->player->defense))+1))/100.0;
                }
            }

            moveoption->goalKeeperBlockChance = (100 / ((moveoption->fromPlayer->attack/(moveoption->targetPlayer->defense*CORRECTION_GOALKEEPER_BLOCK_CHANCE))+1))/100.0;

            calculateCornerProbablity(moveoption);
        }
        else if (moveoption->shotType == MoveOption::ShotTypes::Goal_short_shot){
            float CORRECTION_PARTIAL_PASS = 0.2;
            float CORRECTION_FRONTAL_PASS = 0.2; //increases overal chance of blocking and thus decreasing chance of scoring
            float CORRECTION_GOALKEEPER_BLOCK_CHANCE = 10;

            vector<MoveOptionInterceptor *>::iterator interceptIter;
            for(interceptIter = moveoption->interceptors.begin(); interceptIter != moveoption->interceptors.end(); interceptIter++) {

                MoveOptionInterceptor * interceptor = (*interceptIter);

                if (interceptor->positionType == MoveOptionInterceptor::InterceptorPosition::Behind_player) continue;

                if (interceptor->positionType == MoveOptionInterceptor::InterceptorPosition::In_front_player && interceptor->type == MoveOptionInterceptor::InterceptionType::Pass_heading_partly_in_direction_to){

                    interceptor->blockChance = (100 / ((moveoption->fromPlayer->attack/(CORRECTION_PARTIAL_PASS*interceptor->player->defense))+1))/100.0;
                }
                else if (interceptor->positionType == MoveOptionInterceptor::InterceptorPosition::In_front_player && interceptor->type == MoveOptionInterceptor::InterceptionType::Frontal_pass){

                    interceptor->blockChance = (100 / ((moveoption->fromPlayer->attack/(CORRECTION_FRONTAL_PASS*interceptor->player->defense))+1))/100.0;
                }
            }

            moveoption->goalKeeperBlockChance = (100 / ((moveoption->fromPlayer->attack/(moveoption->targetPlayer->defense*CORRECTION_GOALKEEPER_BLOCK_CHANCE))+1))/100.0;

            calculateCornerProbablity(moveoption);
        }
        else if ((!moveoption->interceptors.empty() && moveoption->shotType == MoveOption::ShotTypes::Short_pass)
                 || (!moveoption->interceptors.empty() && moveoption->shotType == MoveOption::ShotTypes::Long_pass)
                ){

            float CORRECTION_PARTIAL_PASS = 0.05; //the lower the more chance a pass wil succeed
            float CORRECTION_FRONTAL_PASS = 0.1; //increases overal chance of blocking and thus decreasing chance of scoring

            vector<MoveOptionInterceptor *>::iterator interceptIter;
            for(interceptIter = moveoption->interceptors.begin(); interceptIter != moveoption->interceptors.end(); interceptIter++) {

                MoveOptionInterceptor * interceptor = (*interceptIter);

                if (interceptor->positionType == MoveOptionInterceptor::InterceptorPosition::Behind_player) continue;

                if (interceptor->positionType == MoveOptionInterceptor::InterceptorPosition::In_front_player && interceptor->type == MoveOptionInterceptor::InterceptionType::Pass_heading_in_direction_to){

                    interceptor->blockChance = (100 / ((moveoption->fromPlayer->attack/(CORRECTION_PARTIAL_PASS*interceptor->player->defense))+1))/100.0;
                }
                if (interceptor->positionType == MoveOptionInterceptor::InterceptorPosition::In_front_player && interceptor->type == MoveOptionInterceptor::InterceptionType::Pass_heading_partly_in_direction_to){

                    interceptor->blockChance = (100 / ((moveoption->fromPlayer->attack/(CORRECTION_PARTIAL_PASS*interceptor->player->defense))+1))/100.0;
                }
                else if (interceptor->positionType == MoveOptionInterceptor::InterceptorPosition::In_front_player && interceptor->type == MoveOptionInterceptor::InterceptionType::Frontal_pass){

                    interceptor->blockChance = (100 / ((moveoption->fromPlayer->attack/(CORRECTION_FRONTAL_PASS*interceptor->player->defense))+1))/100.0;
                }
            }
        }
    }
}

/**
 * get all move options for given player. move option means all team members plus goalkeeper of enemy team.
 * for each player we calculate the distance and shot type. We also determine per player a list of interceptors
 * @param player
 * @return
 */
std::vector<MoveOption *> Lineup::getMoveOptions(Player * player){

    if (cachedMoveOptions.count(player->id) > 0){
        return cachedMoveOptions[player->id];
    }

    int teamAttackDirection = player->side == Player::Side::Home ? 1 : -1;

    std::vector<MoveOption *> moveOptions;

    std::vector<Player*> enemyPlayers = getTeamEnemyPlayers(player);

    vector<Player*>::iterator playerIter;
    for(playerIter = players.begin(); playerIter != players.end(); playerIter++) {

        if ((*playerIter)->id == player->id) continue;
        if ((*playerIter)->side != player->side && (*playerIter)->type != Player::Type::GoalKeeper) continue;

        MoveOption * moveOption = new MoveOption();
        moveOption->attackDirection = teamAttackDirection;
        moveOption->fromPlayer = player;
        moveOption->targetPlayer = (*playerIter);
        moveOption->distance = getDistance((*playerIter),player);

        //determine move direction and type (attack, neutral or defense) for this player move option
        int moveDirection = moveOption->targetPlayer->position->x - moveOption->fromPlayer->position->x;

        if ((moveDirection < 0 && teamAttackDirection == -1) || (moveDirection > 0 && teamAttackDirection == 1)){
            moveOption->moveStyle = MoveOption::MoveStyle::Attack;
        }
        else if (moveDirection == 0){
            moveOption->moveStyle = MoveOption::MoveStyle::Neutral;
        }
        else{
            moveOption->moveStyle = MoveOption::MoveStyle::Defense;
        }
        moveOption->direction = moveDirection;

        //determine shot type based on distance
        //case for enemy goalkeeper (only enemy player the ball can go to
        if (moveOption->targetPlayer->side != moveOption->fromPlayer->side){
            if (moveOption->distance > 2){
                moveOption->shotType = MoveOption::ShotTypes::Goal_long_shot;
            }
            else{
                moveOption->shotType = MoveOption::ShotTypes::Goal_short_shot;
            }
        }
        else{
            if (moveOption->distance > 2.5){
                moveOption->shotType = MoveOption::ShotTypes::Long_pass;
            }
            else{
                moveOption->shotType = MoveOption::ShotTypes::Short_pass;
            }
        }

        //get ball direction
        float ballDirectionXf = moveOption->fromPlayer->position->x - moveOption->targetPlayer->position->x;
        int ballDirectionX = (ballDirectionXf > 0 ? -1 : (ballDirectionXf < 0 ? 1 : 0));
        float  ballDirectionYf = moveOption->fromPlayer->position->y - moveOption->targetPlayer->position->y;
        int ballDirectionY = (ballDirectionYf > 0 ? -1 : (ballDirectionYf < 0 ? 1 : 0));


        //loop over enemy players to determine interceptors
        vector<Player*>::iterator enemyPlayerIter;
        for(enemyPlayerIter = enemyPlayers.begin(); enemyPlayerIter != enemyPlayers.end(); enemyPlayerIter++) {

            if ((*enemyPlayerIter)->id == moveOption->targetPlayer->id) continue; //interceptors only other players. handles keeper case

            float distance = distanceToSegement((*enemyPlayerIter),player,(*playerIter));

            if (distance < 1.5){

                float playerFaceXf = moveOption->fromPlayer->position->x - (*enemyPlayerIter)->position->x;
                int playerFaceX = (playerFaceXf > 0 ? -1 : (playerFaceXf < 0 ? 1 : 0));
                float  playerFaceYf = moveOption->fromPlayer->position->y - (*enemyPlayerIter)->position->y;
                int playerFaceY = (playerFaceYf > 0 ? -1 : (playerFaceYf < 0 ? 1 : 0));

                MoveOptionInterceptor * moveInterceptor = new MoveOptionInterceptor();
                moveInterceptor->player = (*enemyPlayerIter);
                moveInterceptor->distanceToBall = distance;
                moveInterceptor->distanceToPlayer = getDistance((*enemyPlayerIter),player);

                if (distance == 0){
                    moveInterceptor->type = MoveOptionInterceptor::InterceptionType::Frontal_pass;
                    moveInterceptor->positionType = MoveOptionInterceptor::InterceptorPosition::In_front_player;
                }
                else{
                    if (playerFaceX == ballDirectionX && playerFaceY == ballDirectionY){
                        moveInterceptor->type = MoveOptionInterceptor::InterceptionType::Pass_heading_in_direction_to;
                        moveInterceptor->positionType = MoveOptionInterceptor::InterceptorPosition::In_front_player;
                        moveInterceptor->behindPlayer = false;
                    }
                    else if (playerFaceX == ballDirectionX){
                        moveInterceptor->type = MoveOptionInterceptor::InterceptionType::Pass_heading_partly_in_direction_to;
                        moveInterceptor->positionType = MoveOptionInterceptor::InterceptorPosition::In_front_player;

                        if ((moveOption->targetPlayer->position->y > moveOption->fromPlayer->position->y && moveInterceptor->player->position->y < moveOption->fromPlayer->position->y)
                            || 	(moveOption->targetPlayer->position->y < moveOption->fromPlayer->position->y && moveInterceptor->player->position->y > moveOption->fromPlayer->position->y)
                                ){
                            moveInterceptor->positionType = MoveOptionInterceptor::InterceptorPosition::Behind_player;
                            moveInterceptor->behindPlayer = true;
                        }

                        if ((moveOption->targetPlayer->position->x > moveOption->fromPlayer->position->x && moveInterceptor->player->position->x > moveOption->targetPlayer->position->x)
                            || 	(moveOption->targetPlayer->position->x < moveOption->fromPlayer->position->x && moveInterceptor->player->position->x < moveOption->targetPlayer->position->x)
                                ){
                            moveInterceptor->positionType = MoveOptionInterceptor::InterceptorPosition::Behind_target_player;
                            moveInterceptor->behindTargetPlayer = true;
                        }

                    }
                    else if (playerFaceY == ballDirectionY){
                        moveInterceptor->type = MoveOptionInterceptor::InterceptionType::Pass_heading_partly_in_direction_to;
                        moveInterceptor->positionType = MoveOptionInterceptor::InterceptorPosition::In_front_player;

                        if ((moveOption->targetPlayer->position->x > moveOption->fromPlayer->position->x && moveInterceptor->player->position->x < moveOption->fromPlayer->position->x)
                            || 	(moveOption->targetPlayer->position->x < moveOption->fromPlayer->position->x && moveInterceptor->player->position->x > moveOption->fromPlayer->position->x)
                                ){
                            moveInterceptor->positionType = MoveOptionInterceptor::InterceptorPosition::Behind_player;
                            moveInterceptor->behindPlayer = true;
                        }

                        if ((moveOption->targetPlayer->position->y > moveOption->fromPlayer->position->y && moveInterceptor->player->position->y > moveOption->targetPlayer->position->y)
                            || 	(moveOption->targetPlayer->position->y < moveOption->fromPlayer->position->y && moveInterceptor->player->position->y < moveOption->targetPlayer->position->y)
                                ){
                            moveInterceptor->positionType = MoveOptionInterceptor::InterceptorPosition::Behind_target_player;
                            moveInterceptor->behindTargetPlayer = true;
                        }
                    }
                    else{
                        //LOG(INFO) << "player " << (*enemyPlayerIter)->id << " is behind player" << player->id << " shooting to " << (*playerIter)->id;
                        moveInterceptor->type = MoveOptionInterceptor::InterceptionType::Pass_heading_not_in_direction_to;
                        moveInterceptor->positionType = MoveOptionInterceptor::InterceptorPosition::Behind_player;
                        moveInterceptor->behindPlayer = true;
                    }
                }

                moveOption->interceptors.push_back(moveInterceptor);
            }
        }
        if (moveOption->interceptors.size() > 0) {
            std::sort(moveOption->interceptors.begin(), moveOption->interceptors.end(),
                      MoveOptionInterceptorSort()); //sort to make sure the closest interceptor is handled first later
        }
        moveOptions.push_back(moveOption);
    }

    if (moveOptions.size() > 0) {
        std::sort(moveOptions.begin(), moveOptions.end(), MoveOptionSort());
        cachedMoveOptions[player->id] = moveOptions;
    }
    return moveOptions;
}


void Lineup::goPenalty(PenaltyAction & penaltyAction){

    if (currentPenaltyTake == 0){
        penaltyAction.currentSide = rand() % 2 == 0 ? Player::Side::Home : Player::Side::Away;

        penaltyPlayerIndexHome = game::Util::getRandomBetween(0, 10);
        penaltyPlayerIndexAway = game::Util::getRandomBetween(0, 10);

    }
    else{
        penaltyAction.currentSide = penaltyAction.currentSide == Player::Side::Home ? Player::Side::Away : Player::Side::Home;
    }

    Player * player;
    if (penaltyAction.currentSide == Player::Side::Home) {
        penaltyPlayerIndexHome = (penaltyPlayerIndexHome+1) % homeplayers.size();
        player = homeplayers[penaltyPlayerIndexHome];
    }
    else{
        penaltyPlayerIndexAway = (penaltyPlayerIndexAway+1) % awayplayers.size();
        player = awayplayers[penaltyPlayerIndexAway];
    }

    penaltyAction.hitChance = ((100-15)/100.0);
    penaltyAction.shooter = player;

    currentPenaltyTake++;

    currentPenaltyRound = ceil(((float)currentPenaltyTake)/2);
    penaltyAction.round = currentPenaltyRound;
    penaltyAction.take = currentPenaltyTake;
}
