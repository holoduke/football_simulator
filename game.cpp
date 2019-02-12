//
// Created by gillis on 25/12/2016.
//

#include "game.h"
#include "Util.h"
#include "MatchReport.h"
#include "PenaltyAction.h"

namespace game {


    /**
     * initalization of values needed to run the game loop
     */
    void Game::initStartValues(){
        totalSteps = options.duration*options.stepsPerMinute;
        halfTimeSteps = options.halftimeDuration*options.stepsPerMinute;
        extraTimeSteps = options.extraTimeDuration*options.stepsPerMinute;

        LOG(INFO) << "extra time duration " << options.extraTimeDuration << " full duration " << options.duration;

        firstBreakSteps = options.firstBreakDuration*options.stepsPerMinute;
        secondBreakSteps = options.secondBreakDuration*options.stepsPerMinute;
        thirdBreakSteps = options.thirdBreakDuration*options.stepsPerMinute;

        timeBetweenPenalty = options.breakBetweenPenalty*options.stepsPerMinute;

        currentHalfTimeStep = 0;
        currentFullTimeStep = 0;
        currentFirstBreakStep = 0;
        currentExtraTimeStep = 0;
        currentSecondBreakStep = 0;
        currentThirdBreakStep = 0;
        stepTimeout = ((1000 * 60) / options.stepsPerMinute) / options.multiplier;

        //define random start position of ball
        homeCanStart = rand() % 2 == 0;

        LOG(INFO) << "initializing game: Ball start: " << (homeCanStart ? "home" : "away")  << "Steps per minute:" << options.stepsPerMinute << "Gamespeed:" << options.multiplier << "Steptimeout:" <<  stepTimeout;


        if (homeCanStart) {
            if (options.localteam.formationType == Team::FormationType::f_4_4_2){
                ballPosition.set(3,2);
            }
            else if (options.localteam.formationType == Team::FormationType::f_4_3_3) {
                ballPosition.set(3,3);
            }
        }
        else{
            if (options.visitorteam.formationType == Team::FormationType::f_4_4_2){
                ballPosition.set(4,2);
            }
            else if (options.visitorteam.formationType == Team::FormationType::f_4_3_3) {
                ballPosition.set(4,3);
            }
        }
    }

    void Game::resetBallPosition(Player::Side side){
        if (side == Player::Side::Home) {
            LOG(INFO) << "set ball postion to home";
            if (options.localteam.formationType == Team::FormationType::f_4_3_3) {
                ballPosition.set(3,3);
            }
            else if (options.localteam.formationType == Team::FormationType::f_4_4_2){
                ballPosition.set(3,2);
            }
        }
        else{
            LOG(INFO) << "set ball postion to away";
            if (options.visitorteam.formationType == Team::FormationType::f_4_3_3) {
                ballPosition.set(4,3);
            }
            else if (options.visitorteam.formationType == Team::FormationType::f_4_4_2){
                ballPosition.set(4,2);
            }
        }
    };

    void Game::runLoop() {

        MatchReport matchReport;
        matchReport.com = &options.communicator;
        matchReport.game = this;
        PenaltyAction penaltyAction;

        matchReport.stats.useAggregate = options.useAggregate;

        if (options.useAggregate) {
            matchReport.stats.aggr_local_home_goals = options.aggr_local_home_goals;
            matchReport.stats.aggr_visitor_home_goals = options.aggr_visitor_home_goals;
            matchReport.stats.aggr_local_away_goals = options.aggr_local_away_goals;;
            matchReport.stats.aggr_visitor_away_goals = options.aggr_visitor_away_goals;
        }

        //ballPosition.set(0,3); // give ball to original goal keeper

        int currentMinute = 0;

        double afterSecondHalfExtraTime = (totalSteps/11) * game::Util::getRandom(); //max 4 minutes;
        double afterExtraTimeSecondHalfExtraTime = (totalSteps/9) * game::Util::getRandom(); //max 4 minutes;

        while (true) {

            int currentMinuteTemp = currentFullTimeStep / options.stepsPerMinute;
            if (currentMinuteTemp != currentMinute){
                matchReport.onMinute(currentMinuteTemp);
            }
            currentMinute = currentMinuteTemp;

            if (status == P) {
                LOG(INFO) << "penalties finished";
                matchReport.onMatchStatusEvent(P,currentMinute);
                break;
            }
            else if (status == PEN_BREAK) {
                if (currentPenaltyBetweenBreakStep < timeBetweenPenalty) {
                    currentPenaltyBetweenBreakStep++;
                    std::this_thread::sleep_for(std::chrono::milliseconds(stepTimeout));
                    continue;
                }
                else{
                    LOG(INFO) << "penalty break ended. go take next penalty";
                    status = PEN;
                    continue;
                }
            }
            else if (status == PEN){

                options.lineup.goPenalty(penaltyAction);

                bool penaltyScored = game::Util::getRandom() <= penaltyAction.hitChance;

                //LOG(INFO) << "penalty taken with chance " << penaltyAction.hitChance <<  " scored: " << penaltyScored;

                if (penaltyScored){
                    matchReport.onPenaltySessionScored(penaltyAction);
                }
                else{
                    matchReport.onPenaltySessionMissed(penaltyAction);
                }

                if (matchReport.isPenaltyFinished()){
                    status = P;
                    continue;
                }

                currentPenaltyBetweenBreakStep = 0;
                status = PEN_BREAK;
                continue;
            }
            else if (status == BREAK3) { //after extra time first half
                if (currentThirdBreakStep< thirdBreakSteps) {
                    currentThirdBreakStep++;
                    std::this_thread::sleep_for(std::chrono::milliseconds(stepTimeout));
                    continue;
                }
                else{
                    LOG(INFO) << "third brake ended. go to status PEN";
                    status = PEN;
                    continue;
                }
            }
            else if (status == ET_SECOND_HALF){
                if (currentExtraTimeStep >= extraTimeSteps){
                    LOG(INFO) << "extra time second half ended";
                    currentExtraTimeStep = 0;

                    if ((!options.useAggregate && options.winnerRequired && matchReport.isDrawGame()) || (options.useAggregate && matchReport.aggregateScoreIsDraw())){
                        LOG(INFO) << "game is still a draw. go for penalties";
                        //gooo do penly
                        matchReport.onMatchStatusEvent(PEN,currentMinute);
                        status = BREAK3;
                        continue;
                    }
                    else{
                        status = AET;
                        matchReport.onMatchStatusEvent(AET,currentMinute);
                        break;
                    }
                }
                currentExtraTimeStep++;
            }
            else if (status == BREAK2) { //after extra time first half
                if (currentSecondBreakStep< secondBreakSteps) {
                    currentSecondBreakStep++;
                    std::this_thread::sleep_for(std::chrono::milliseconds(stepTimeout));
                    continue;
                }
                else{
                    LOG(INFO) << "second brake ended. go to status ET second half";
                    matchReport.onMatchStatusEvent(ET_SECOND_HALF,currentMinute);
                    status = ET_SECOND_HALF;
                    extraTimeSteps += afterExtraTimeSecondHalfExtraTime;
                    if (homeCanStart){
                        resetBallPosition(Player::Side::Away);
                    }
                    else{
                        resetBallPosition(Player::Side::Home);
                    }
                    continue;
                }
            }
            else if (status == ET_FIRST_HALF){
                if (currentExtraTimeStep >= extraTimeSteps){
                    LOG(INFO) << "extra time first half ended";
                    currentExtraTimeStep = 0;
                    matchReport.onMatchStatusEvent(BREAK2,currentMinute);
                    status = BREAK2;
                    continue;
                }
                currentExtraTimeStep++;
            }
            else if (status == BREAK1){ //after fulltime

                if (currentFirstBreakStep < firstBreakSteps) {
                    currentFirstBreakStep++;
                    std::this_thread::sleep_for(std::chrono::milliseconds(stepTimeout));
                    continue;
                }
                else{
                    LOG(INFO) << "first brake ended. go to status ET first half";
                    matchReport.onMatchStatusEvent(ET_FIRST_HALF,currentMinute);
                    status = ET_FIRST_HALF;
                    if (homeCanStart){
                        resetBallPosition(Player::Side::Home);
                    }
                    else{
                        resetBallPosition(Player::Side::Away);
                    }
                    continue;
                }
            }
            else if (status == FT) {
                matchReport.onMatchStatusEvent(FT,currentMinute);
                break;
            }
            else if ((status == FIRST_HALF_PLAYING || status == SECOND_HALF_PLAYING) && currentHalfTimeStep > totalSteps) {

                if (status == FIRST_HALF_PLAYING) {
                    status = HT;
                    currentHalfTimeStep = 0;
                    matchReport.onMatchStatusEvent(HT,currentMinute);
                } else if (status == SECOND_HALF_PLAYING) {

                    //if match is not aggregate and we require a winner let the match continue.  if the match is aggregate and current aggregate standings is draw also continue;
                    if ((!options.useAggregate && options.winnerRequired && matchReport.isDrawGame()) || (options.useAggregate && matchReport.aggregateScoreIsDraw())){
                        LOG(INFO) << "match ended as a draw, Winner required, so extra time";
                        matchReport.onMatchStatusEvent(BREAK1,currentMinute);
                        status = BREAK1;
                        continue;
                    }
                    else {
                        status = FT;
                        continue;
                    }
                }
            }
            else if (status == HT && currentHalfTimeStep > halfTimeSteps) {
                status = SECOND_HALF_PLAYING;
                currentHalfTimeStep = 0;
                totalSteps += afterSecondHalfExtraTime;
                if (homeCanStart){
                    resetBallPosition(Player::Side::Away);
                }
                else{
                    resetBallPosition(Player::Side::Home);
                }
                matchReport.onMatchStatusEvent(SECOND_HALF_PLAYING,currentMinute);
            }
            else if (status == READY_TO_START) {
                status = FIRST_HALF_PLAYING;
                matchReport.onMatchStatusEvent(FIRST_HALF_PLAYING,currentMinute);
            }

            currentHalfTimeStep++;

            if (status == HT){
                std::this_thread::sleep_for(std::chrono::milliseconds(stepTimeout));
                continue;
            }

            currentFullTimeStep++;









            Player * p = options.lineup.getPlayerAt(ballPosition);
            std::vector<MoveOption *> moveoptions = options.lineup.getMoveOptions(p);
            if (!p->moveProbabilitiesCalculated) {
                options.lineup.calculateMoveProbabilities(moveoptions); //TODO cache this as well
                p->moveProbabilitiesCalculated = true;
            }
            MoveOption * choice = options.lineup.pickChoice(p, moveoptions);
            options.lineup.performChoice(choice);

            //if we have a goalshot
            if (choice->shotType == MoveOption::ShotTypes::Goal_short_shot || choice->shotType == MoveOption::ShotTypes::Goal_long_shot){

                // sumPlayer.shotsOnGoal++;
                matchReport.onGoalShotAttempt(choice->fromPlayer,currentMinute);

                //goal shot, but goal is missed. we look at the chance of score with a corner
                if (!choice->result && choice->missedShotResultedInCornerGoalChance > 0){

                    bool isCorner = game::Util::getRandom() >= ((100-choice->cornerChance)/100.0);
                    bool isCornerGoal = game::Util::getRandom() >= ((100-choice->missedShotResultedInCornerGoalChance)/100.0);

                    choice->isCorner = isCorner;
                    choice->isCornerGoal = isCornerGoal;

                    if (isCorner){

                        matchReport.onCorner(choice->cornerShooter,currentMinute);

                        if (isCornerGoal){

                            matchReport.onCornerGoal(choice->cornerGoalShooter,currentMinute);

                            if (choice->fromPlayer->side == Player::Side::Home){
                                resetBallPosition(Player::Side::Away);
                            }
                            else{
                                resetBallPosition(Player::Side::Home);
                            }
                        }
                        else{
                            //missed corner (do nothing here for sake if simplicity) the keeper basically blocked the ball or the ball went out. no need to simulate.
                            ballPosition.set(choice->targetPlayer->position->x,choice->targetPlayer->position->y); // give ball to original goal keeper
                        }
                    }
                    else{
                        ballPosition.set(choice->targetPlayer->position->x,choice->targetPlayer->position->y); // give ball to original goal keeper
                    }
                }
                if (choice->result){

                    //sumPlayer.goals++;
                    matchReport.onGoal(choice->fromPlayer, currentMinute);

                    if (choice->fromPlayer->side == Player::Side::Home){
                        //homescore++;
                        resetBallPosition(Player::Side::Away);
                        //options.onEvent({type:"goal",player:choice.fromPlayer,team:"local","fromCorner":0,stats:getGameInfo()});
                    }
                    else{
                        //awayscore++;
                        resetBallPosition(Player::Side::Home);
                        //options.onEvent({type:"goal",player:choice.fromPlayer,team:"visitor","fromCorner":0,stats:getGameInfo()});
                    }

                    //if (!topscorers[choice.fromPlayer.name]) topscorers[choice.fromPlayer.name] = 1;
                    //topscorers[choice.fromPlayer.name]++;

                    //events.push({"minute":Math.round((currentFullTimeStep/stepsPerMinute)),"type":"goal","player":choice.fromPlayer,"score":homescore+"-"+awayscore});
                }
                else{
                    ballPosition.set(choice->targetPlayer->position->x,choice->targetPlayer->position->y); // give ball to original goal keeper
                }

            }
            else if (choice->result){ //just a succesfull pass
                //LOG(INFO) << "just pass " << choice->targetPlayer->position->x << " " << choice->targetPlayer->position->y;
                ballPosition.set(choice->targetPlayer->position->x,choice->targetPlayer->position->y);
                matchReport.onBallPassSuccess(choice->fromPlayer, currentMinute);
            }
            else{
                if (choice->whenLostPassTo != NULL){

                    //when ball is lost it can also be a foul. calculate that here.
                    if (game::Util::getRandom() > 0.9){
                        LOG(INFO) << "ball is lost, but we have a foul";
                        if (game::Util::getRandom() > 0.9){
                            LOG(INFO) << "and a yellowcard";
                        }
                    }

                    matchReport.onBallPassFail(choice->fromPlayer,choice->whenLostPassTo, currentMinute);
                    ballPosition.set(choice->whenLostPassTo->position->x,choice->whenLostPassTo->position->y); // give ball to interceptor
                }
                else{
                    LOG(INFO) << "error no whenlosttopassto palyer";
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(stepTimeout));
        }
    }
}
