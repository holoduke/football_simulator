//
// Created by gillis on 03/03/2017.
//

#ifndef FOOTBALLSIMULATOR_MATCHREPORT_H
#define FOOTBALLSIMULATOR_MATCHREPORT_H


#include "Player.h"
#include "MoveOption.h"
#include "Communication.h"
#include "game.h"
#include "Util.h"

using game::Communication;
using game::Game;


class MatchStats {
public:

    int homescore = 0;
    int awayscore = 0;
    int homescore_before_et = 0;
    int awayscore_before_et = 0;
    bool useAggregate = false;
    int aggr_local_home_goals = 0;
    int aggr_visitor_home_goals = 0;
    int aggr_local_away_goals = 0;
    int aggr_visitor_away_goals = 0;
    int homePenaltyScore = 0;
    int awayPenaltyScore = 0;
    int homePenaltyMisses = 0;
    int awayPenaltyMisses = 0;

    int penaltyRound = 0;
    int penaltyTake = 0;
    int minute = 0;
    int afterExtraTime = 0;
    int afterPenaltySession = 0;

    int home_ballposession = 1;
    int away_ballposession = 1;
    int home_shotsgoal = 0;
    int away_shotsgoal = 0;
    int home_corners = 0;
    int home_corner_goals = 0;
    int away_corners = 0;
    int away_corner_goals = 0;
    float home_ballposession_perc = 0;
    float away_ballposession_perc = 0;
    float random = game::Util::getRandom() * 100000;

    /**
     * returns match report as json string
     * @param type
     * @return
     */
    string toString(string type){
        std::stringstream ss;
        ss << "{";
        ss << "\"type\":\"" << type << "\",";
        ss << "\"minute\":\"" << minute << "\",";
        ss << "\"localscore\":" << homescore << ",";
        ss << "\"visitorscore\":" << awayscore << ",";
        ss << "\"aggr_local_home_goals\":" << aggr_local_home_goals << ",";
        ss << "\"aggr_visitor_home_goals\":" << aggr_visitor_home_goals << ",";
        ss << "\"aggr_local_away_goals\":" << aggr_local_away_goals << ",";
        ss << "\"aggr_visitor_away_goals\":" << aggr_visitor_away_goals << ",";
        ss << "\"stats\":" << "{";
        ss << "\"home_ballposession\":" << home_ballposession << ",";
        ss << "\"away_ballposession\":" << away_ballposession << ",";
        ss << "\"home_shotsgoal\":" << home_shotsgoal << ",";
        ss << "\"away_shotsgoal\":" << away_shotsgoal << ",";
        ss << "\"home_corners\":" << home_corners << ",";
        ss << "\"away_corners\":" << away_corners << ",";
        ss << "\"home_corner_goal\":" << home_corner_goals << ",";
        ss << "\"away_corner_goal\":" << away_corner_goals << ",";
        ss << "\"home_ballposession_perc\":" << (((home_ballposession*1.0)/ (home_ballposession+away_ballposession)) * 100.0) << ",";
        ss << "\"away_ballposession_perc\":" << (((away_ballposession*1.0)/ (away_ballposession+home_ballposession)) * 100.0);
        ss << "}";
        ss << "}|||";

        return ss.str();
    }

    /**
 * returns match report as json string
 * @param type
 * @return
 */
    string toStringGoal(string type, Player::Side side, Player * player){
        std::stringstream ss;
        ss << "{";
        ss << "\"type\":\"" << type << "\",";
        ss << "\"goalBy\":" << player->id << ",";
        ss << "\"side\":\"" << (side == Player::Side::Home ? "home" : "away")  << "\",";
        ss << "\"minute\":\"" << minute << "\",";
        ss << "\"localscore\":" << homescore << ",";
        ss << "\"visitorscore\":" << awayscore << ",";;
        ss << "\"aggr_local_home_goals\":" << aggr_local_home_goals << ",";
        ss << "\"aggr_visitor_home_goals\":" << aggr_visitor_home_goals << ",";
        ss << "\"aggr_local_away_goals\":" << aggr_local_away_goals << ",";
        ss << "\"aggr_visitor_away_goals\":" << aggr_visitor_away_goals << ",";
        ss << "\"isPenalty\":" << 0;
        ss << "}|||";

        return ss.str();
    }

    /**
* returns match report as json string
* @param type
* @return
*/
    string toStringPenaltySession(string type, Player::Side side, Player * player, int round, int take){
        std::stringstream ss;
        ss << "{";
        ss << "\"type\":\"" << type << "\",";
        ss << "\"goalBy\":" << player->id << ",";
        ss << "\"side\":\"" << (side == Player::Side::Home ? "home" : "away")  << "\",";
        ss << "\"minute\":\"" << minute << "\",";
        ss << "\"localPenaltyScore\":" << homePenaltyScore << ",";
        ss << "\"visitorPenaltyScore\":" << awayPenaltyScore << ",";
        ss << "\"localPenaltyMiss\":" << homePenaltyMisses << ",";
        ss << "\"visitorPenaltyMiss\":" << awayPenaltyMisses << ",";
        ss << "\"round\":" << round << ",";
        ss << "\"take\":" << take << ",";
        ss << "\"isPenalty\":" << 0;
        ss << "}|||";

        return ss.str();
    }

    /**
     * sends match report as json string including player stats
     * @param game
     * @param type
     * @return
     */
    string toStringExtended(Game * game, string type){
        std::stringstream ss;
        ss << "{";
        ss << "\"sid\":" << random << ",";
        ss << "\"type\":\"" << type << "\",";
        ss << "\"minute\":\"" << minute << "\",";
        ss << "\"localscore\":" << homescore << ",";
        ss << "\"visitorscore\":" << awayscore << ",";

        if (afterExtraTime){
            ss << "\"localscore_before_et\":" << homescore_before_et << ",";
            ss << "\"visitorscore_before_et\":" << awayscore_before_et << ",";
        }

        ss << "\"aggr_local_home_goals\":" << aggr_local_home_goals << ",";
        ss << "\"aggr_visitor_home_goals\":" << aggr_visitor_home_goals << ",";
        ss << "\"aggr_local_away_goals\":" << aggr_local_away_goals << ",";
        ss << "\"aggr_visitor_away_goals\":" << aggr_visitor_away_goals << ",";


        if (homescore > awayscore){
            ss << "\"match_winner\":\"home\""<< ",";
        }
        else if (homescore < awayscore){
            ss << "\"match_winner\":\"away\""<< ",";
        }
        else{
            if (afterPenaltySession){
                if (homePenaltyScore > awayPenaltyScore){
                    ss << "\"match_winner\":\"home\""<< ",";
                }
                else{
                    ss << "\"match_winner\":\"away\""<< ",";
                }
            }
            else{
                ss << "\"match_winner\":\"draw\""<< ",";
            }
        }

        if (!useAggregate) {
            ss << "\"aggr_winner\":null" << ",";
        }
        else {
            Player::Side side = getAggregateWinner();

            //if aggregate winner side is not defined, we have a draw, but we can have penalties with a result
            if (side == Player::Side::Undefined) {
                if (afterPenaltySession) { //penalties sessions are always concluded with a winner. there must be a difference in home and away penalty score always
                    if (homePenaltyScore > awayPenaltyScore) {
                        ss << "\"aggr_winner\":\"home\"" << ",";
                    } else {
                        ss << "\"aggr_winner\":\"away\"" << ",";
                    }
                } else {
                    ss << "\"aggr_winner\":\"draw\"" << ",";
                }
            } else if (side == Player::Side::Home) {
                ss << "\"aggr_winner\":\"home\"" << ",";
            } else if (side == Player::Side::Away) {
                ss << "\"aggr_winner\":\"away\"" << ",";
            }
        }

        ss << "\"localPenaltyScore\":" << homePenaltyScore << ",";
        ss << "\"visitorPenaltyScore\":" << awayPenaltyScore << ",";
        ss << "\"afterExtraTime\":" << afterExtraTime << ",";
        ss << "\"afterPenaltySession\":" << afterPenaltySession << ",";
        ss << "\"stats\":" << "{";
        ss << "\"home_ballposession\":" << home_ballposession << ",";
        ss << "\"away_ballposession\":" << away_ballposession << ",";
        ss << "\"home_shotsgoal\":" << home_shotsgoal << ",";
        ss << "\"away_shotsgoal\":" << away_shotsgoal << ",";
        ss << "\"home_corners\":" << home_corners << ",";
        ss << "\"away_corners\":" << away_corners << ",";
        ss << "\"home_corner_goal\":" << home_corner_goals << ",";
        ss << "\"away_corner_goal\":" << away_corner_goals << ",";
        ss << "\"home_ballposession_perc\":" << (((home_ballposession*1.0)/ (home_ballposession+away_ballposession)) * 100.0) << ",";
        ss << "\"away_ballposession_perc\":" << (((away_ballposession*1.0)/ (away_ballposession+home_ballposession)) * 100.0)   ;
        ss << "},";
        ss << "\"playerSummary\":{";
        ss << "\"localteam\":[";
        for (int i=0; i < game->options.lineup.homeplayers.size();i++){

            if (i > 0) {
                ss << ",{";
            }
            else{
                ss << "{";
            }
            ss << "\"id\":" << game->options.lineup.homeplayers[i]->id << ",";
            ss << "\"goals\":" << game->options.lineup.homeplayers[i]->stats.goals << ",";
            ss << "\"successfullPasses\":" << game->options.lineup.homeplayers[i]->stats.successfullPasses << ",";
            ss << "\"failedPasses\":" << game->options.lineup.homeplayers[i]->stats.failedPasses << ",";
            ss << "\"shotsOnGoal\":" << game->options.lineup.homeplayers[i]->stats.shotsOnGoal << ",";
            ss << "\"blocks\":" << game->options.lineup.homeplayers[i]->stats.blocks << ",";
            ss << "\"cornerGoals\":" << game->options.lineup.homeplayers[i]->stats.cornerGoals << ",";
            ss << "\"corners\":" << game->options.lineup.homeplayers[i]->stats.corners << "";

            ss << "}";
        }
        ss << "],";
        ss << "\"visitorteam\":[";
        for (int i=0; i < game->options.lineup.awayplayers.size();i++){

            if (i > 0) {
                ss << ",{";
            }
            else{
                ss << "{";
            }
            ss << "\"id\":" << game->options.lineup.awayplayers[i]->id << ",";
            ss << "\"goals\":" << game->options.lineup.awayplayers[i]->stats.goals << ",";
            ss << "\"successfullPasses\":" << game->options.lineup.awayplayers[i]->stats.successfullPasses << ",";
            ss << "\"failedPasses\":" << game->options.lineup.awayplayers[i]->stats.failedPasses << ",";
            ss << "\"shotsOnGoal\":" << game->options.lineup.awayplayers[i]->stats.shotsOnGoal << ",";
            ss << "\"blocks\":" << game->options.lineup.awayplayers[i]->stats.blocks << ",";
            ss << "\"cornerGoals\":" << game->options.lineup.awayplayers[i]->stats.cornerGoals << ",";
            ss << "\"corners\":" << game->options.lineup.awayplayers[i]->stats.corners << "";

            ss << "}";
        }
        ss << "]";
        ss << ",\"sid\":" << random << "}";

        ss << "}|||";

        return ss.str();
    }

    Player::Side getAggregateWinner(){

        int totalHome = aggr_local_home_goals+aggr_local_away_goals;
        int totalAway = aggr_visitor_home_goals+aggr_visitor_away_goals;

        if (totalHome != totalAway){
            if (totalHome > totalAway){
                return Player::Side::Home;
            }
            else{
                return Player::Side::Away;
            }
        }
        else if (totalHome == totalAway && aggr_local_away_goals != aggr_visitor_away_goals){

            if (aggr_local_away_goals > aggr_visitor_away_goals){
                return Player::Side::Home;
            }
            else{
                return Player::Side::Away;
            }
        }
        return Player::Side::Undefined;
    }
};

class MatchReport {

public:

    MatchStats stats;

    bool isDrawGame(){
        return stats.homescore == stats.awayscore;
    }

    bool isPenaltyFinished(){

        if (stats.penaltyRound >= 5 && (stats.penaltyTake % 2 == 0) && stats.homePenaltyScore != stats.awayPenaltyScore){
            return true;
        }
        return false;
    }

    /**
     * first rule. winner is with most away goals, otherwise winner is with most goals. otherwise draw
     * @return
     */
    bool aggregateScoreIsDraw(){
        LOG(INFO) <<"aggr local_home: "<< stats.aggr_local_home_goals << ", local_away:" << stats.aggr_local_away_goals << ", visitor_home: "<< stats.aggr_visitor_home_goals << ", visitor_away:" << stats.aggr_visitor_away_goals;

        int totalHome = stats.aggr_local_home_goals+stats.aggr_local_away_goals;
        int totalAway = stats.aggr_visitor_home_goals+stats.aggr_visitor_away_goals;

        if (totalHome != totalAway){
            return false;
        }
        else if (totalHome == totalAway && stats.aggr_local_away_goals != stats.aggr_visitor_away_goals){
            return false;
        }
        return true;
    }

    void sendCom(string type){
        com->send(stats.toString(type).c_str());
    }

    void sendComGoal(string type, Player::Side side, Player * player){
        com->send(stats.toStringGoal(type,side,player).c_str());
    }

    void sendComPenaltySession(string type, PenaltyAction & penaltyAction){
        com->send(stats.toStringPenaltySession(type,penaltyAction.shooter->side,penaltyAction.shooter,penaltyAction.round,penaltyAction.take).c_str());
    }

    void sendComExtended(string type){
        com->send(stats.toStringExtended(game,type).c_str());
    }

    void onMatchStatusEvent(Game::MatchStatus status, int minute) {

        if (status == Game::MatchStatus::FIRST_HALF_PLAYING) {
            sendCom("start");
            LOG(INFO) << "match status changed to FIRST_HALF at m " << minute;
        } else if (status == Game::MatchStatus::HT) {
            sendCom("halftime_start");
            LOG(INFO) << "match status changed to HT at m " << minute;
        } else if (status == Game::MatchStatus::SECOND_HALF_PLAYING) {
            sendCom("halftime_finish");
            LOG(INFO) << "match status changed to SECOND_HALF at m " << minute;
        } else if (status == Game::MatchStatus::FT) {
            sendComExtended("finish");
            LOG(INFO) << "match status changed to FT at m " << minute << " score: "<<stats.homescore << " - "<<stats.awayscore;
        } else if (status == Game::MatchStatus::P) {
            stats.afterPenaltySession = true;
            sendComExtended("finish");
            LOG(INFO) << "match status changed to P at m " << minute << " score: "<<stats.homescore << " - "<<stats.awayscore;
        } else if (status == Game::MatchStatus::AET) {
            stats.afterExtraTime = true;
            sendComExtended("finish");
            LOG(INFO) << "match status changed to AET at m " << minute << " score: "<<stats.homescore << " - "<<stats.awayscore;
        } else if (status == Game::MatchStatus::BREAK1) {
            stats.homescore_before_et = stats.homescore;
            stats.awayscore_before_et = stats.awayscore;
            sendCom("break_before_et_start");
            LOG(INFO) << "match status changed to BREAK1 at m " << minute;
        } else if (status == Game::MatchStatus::ET_FIRST_HALF) {
            sendCom("extra_time_first_half_start");
            LOG(INFO) << "match status changed to ET_FIRST_HALF at m " << minute;
        }else if (status == Game::MatchStatus::ET_SECOND_HALF) {
            sendCom("extra_time_second_half_start");
            LOG(INFO) << "match status changed to ET_SECOND_HALF at m " << minute;
        }else if (status == Game::MatchStatus::BREAK2) {
            sendCom("break_between_et_start");
            LOG(INFO) << "match status changed to BREAK2 at m " << minute;
        } else if (status == Game::MatchStatus::BREAK3) {
            sendCom("break_before_penalty_start");
            LOG(INFO) << "match status changed to BREAK3 at m " << minute;
        } else if (status == Game::MatchStatus::PEN) {
            sendCom("penalty_session_start");
            LOG(INFO) << "match status changed to PEN at m " << minute << " score: "<<stats.homescore << " - "<<stats.awayscore;
        }
    }


    void onMinute(int minute){
        LOG(INFO) << "on minute " << minute;
        stats.minute = minute;
        sendCom("minute");
    }

    void onGoal(Player * player, int minute ){

        player->stats.goals++;

        if (player->side == Player::Side::Home){
            stats.aggr_local_home_goals++;
            stats.homescore++;
            LOG(INFO) << "goal home " << stats.homescore << "-"<< stats.awayscore << " - " << player->id << " -- " << minute;
            sendComGoal("goal",player->side,player);
        }
        else{
            stats.awayscore++;
            stats.aggr_visitor_away_goals++;
            LOG(INFO) << "goal away " << stats.homescore << "-"<<stats.awayscore << " - " << player->id << " m: " << minute;
            sendComGoal("goal",player->side,player);
        }
    }

    void onPenaltySessionScored(PenaltyAction & penaltyAction){
        stats.penaltyRound = penaltyAction.round;
        stats.penaltyTake = penaltyAction.take;
        if (penaltyAction.shooter->side == Player::Side::Home){
            stats.homePenaltyScore++;
            LOG(INFO) << "penalty scored home, round " << penaltyAction.round << ", take: " << penaltyAction.take << " -> " << stats.homePenaltyScore << "-"<< stats.awayPenaltyScore << " playerid " << penaltyAction.shooter->id;
            sendComPenaltySession("penalty_session_goal",penaltyAction);
        }
        else{
            stats.awayPenaltyScore++;
            LOG(INFO) << "penalty scored away, round " << penaltyAction.round << ", take: " << penaltyAction.take << " -> "  << stats.homePenaltyScore << "-"<< stats.awayPenaltyScore << " playerid " << penaltyAction.shooter->id;
            sendComPenaltySession("penalty_session_goal",penaltyAction);
        }
    }

    void onPenaltySessionMissed(PenaltyAction & penaltyAction){
        stats.penaltyRound = penaltyAction.round;
        stats.penaltyTake = penaltyAction.take;
        if (penaltyAction.shooter->side == Player::Side::Home){
            stats.homePenaltyMisses++;
            LOG(INFO) << "penalty missed home, round " << penaltyAction.round << ", take: " << penaltyAction.take << " -> " << stats.homePenaltyScore << "-"<< stats.awayPenaltyScore << " playerid " << penaltyAction.shooter->id;
            sendComPenaltySession("penalty_session_miss",penaltyAction);
        }
        else{
            stats.awayPenaltyMisses++;
            LOG(INFO) << "penalty missed away, round " << penaltyAction.round << ", take: " << penaltyAction.take << " -> "  << stats.homePenaltyScore << "-"<< stats.awayPenaltyScore << " playerid " << penaltyAction.shooter->id;
            sendComPenaltySession("penalty_session_miss",penaltyAction);
        }
    }

    void onCornerGoal(Player * player, int minute ){

        player->stats.cornerGoals++;

        if (player->side == Player::Side::Home){
            stats.homescore++;
            stats.home_corner_goals++;
            stats.aggr_local_home_goals++;
            LOG(INFO) << "goal home from corner " << stats.homescore << "-"<<stats.awayscore << " - " << player->id << " m: " << minute;
            sendComGoal("goal",player->side,player);
        }
        else{
            stats.awayscore++;
            stats.away_corner_goals++;
            stats.aggr_visitor_away_goals++;
            LOG(INFO) << "goal away from corner " << stats.homescore << "-"<<stats.awayscore << " - " << player->id << " m: " << minute;
            sendComGoal("goal",player->side,player);
        }
    }

    void onGoalShotAttempt(Player * player, int minute){

        player->stats.shotsOnGoal++;

        if (player->side == Player::Side::Home){
            //LOG(INFO) << "goal shot attempt from home - " << player->id << " m: " << minute;
            stats.home_shotsgoal++;
        }
        else{
            //LOG(INFO) << "goal shot attempt from away - " << player->id << " m: " << minute;
            stats.away_shotsgoal++;
        }
    }

    void onCorner(Player * player, int minute){

        player->stats.corners++;

        if (player->side == Player::Side::Home){
            //LOG(INFO) << "corner home - " << player->id << " m: " << minute;
            stats.home_corners++;
        }
        else{
            //LOG(INFO) << "corner away - " << player->id << " m: " << minute;
            stats.away_corners++;
        }
    }

    void onBallPassSuccess(Player * player, int minute){
        if (player->side == Player::Side::Home){
            stats.home_ballposession++;
        }
        else{
            stats.away_ballposession++;
        }

        player->stats.successfullPasses++;
    }

    void onBallPassFail(Player * player, Player * interceptor, int minute){

        if (player->side == Player::Side::Home){
            stats.away_ballposession++;
        }
        else{
            stats.home_ballposession++;
        }

        player->stats.failedPasses++;
        interceptor->stats.blocks++;
    }

    void onFoul(Player * source, Player * victim, int card){

    }

    Communication * com;
    Game * game;
};


#endif //FOOTBALLSIMULATOR_MATCHREPORT_H
