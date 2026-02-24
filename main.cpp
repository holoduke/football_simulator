#include <iostream>
#include <vector>

#include "lib/easylogginpp++.h"
#include "game.h"
#include "Util.h"
#include "include/rapidjson/rapidjson.h"
#include "include/rapidjson/document.h"
#include "include/rapidjson/writer.h"

INITIALIZE_EASYLOGGINGPP

using game::Game;
using game::GameOutputWriter;
using game::GameOptions;
using game::Communication;


int main(int argc, char* argv[]) {

    if (argv[3]) {
        LOG(INFO) << "data blob found parse it";

    };

    //el::Loggers::addFlag( el::LoggingFlag::DisableApplicationAbortOnFatalLog );
    el::Loggers::addFlag( el::LoggingFlag::ColoredTerminalOutput );
    el::Loggers::addFlag( el::LoggingFlag::AutoSpacing );
    el::Loggers::addFlag( el::LoggingFlag::LogDetailedCrashReason );

    LOG(INFO) << "main start";

    Lineup lineup = Lineup();

    rapidjson::Document d;

    bool winnerRequired = false;
    bool useAggregate = false;

    //used to determine winner
    int aggr_local_home_goals = 0;
    int aggr_visitor_home_goals = 0;
    int aggr_local_away_goals = 0;
    int aggr_visitor_away_goals = 0;

    if (argv[3]) {
        d.Parse(argv[3]);

        if (d.HasParseError()){
            const char* json = "{\"localteam\":[{\"id\":1,\"att\":50,\"def\":50,\"position\":[0,3],\"type\":\"goal\"},{\"id\":2,\"att\":50,\"def\":50,\"position\":[1,0],\"type\":\"def\"},{\"id\":3,\"att\":50,\"def\":50,\"position\":[1,2],\"type\":\"def\"},{\"id\":4,\"att\":50,\"def\":50,\"position\":[1,4],\"type\":\"def\"},{\"id\":5,\"att\":50,\"def\":50,\"position\":[1,6],\"type\":\"def\"},{\"id\":6,\"att\":50,\"def\":50,\"position\":[3,1],\"type\":\"mid\"},{\"id\":7,\"att\":50,\"def\":50,\"position\":[3,3],\"type\":\"mid\"},{\"id\":8,\"att\":50,\"def\":50,\"position\":[3,5],\"type\":\"mid\"},{\"id\":9,\"att\":50,\"def\":50,\"position\":[5,1],\"type\":\"att\"},{\"id\":10,\"att\":50,\"def\":50,\"position\":[5,3],\"type\":\"att\"},{\"id\":11,\"att\":50,\"def\":50,\"position\":[5,5],\"type\":\"att\"}],\"visitorteam\":[{\"id\":12,\"att\":50,\"def\":50,\"position\":[7,3],\"type\":\"goal\"},{\"id\":13,\"att\":50,\"def\":50,\"position\":[6,0],\"type\":\"def\"},{\"id\":14,\"att\":50,\"def\":50,\"position\":[6,2],\"type\":\"def\"},{\"id\":15,\"att\":50,\"def\":50,\"position\":[6,4],\"type\":\"def\"},{\"id\":16,\"att\":50,\"def\":50,\"position\":[6,6],\"type\":\"def\"},{\"id\":17,\"att\":50,\"def\":50,\"position\":[4,1],\"type\":\"mid\"},{\"id\":18,\"att\":50,\"def\":50,\"position\":[4,3],\"type\":\"mid\"},{\"id\":19,\"att\":50,\"def\":50,\"position\":[4,5],\"type\":\"mid\"},{\"id\":20,\"att\":50,\"def\":50,\"position\":[2,1],\"type\":\"att\"},{\"id\":21,\"att\":50,\"def\":50,\"position\":[2,3],\"type\":\"att\"},{\"id\":22,\"att\":50,\"def\":50,\"position\":[2,5],\"type\":\"att\"}]}";
            d.Parse(json);
            LOG(INFO) << " game data could not be parsed. use default";
        }
        else{
            winnerRequired = (d["requireWinner"].GetInt() == 1);
            useAggregate = (d["useAggregateHistory"].GetInt() == 1);

            if (useAggregate == 1){
                aggr_local_away_goals = d["aggr_history_local_away_goals"].GetInt();
                aggr_visitor_away_goals = d["aggr_history_visitor_away_goals"].GetInt();
                aggr_local_home_goals = d["aggr_history_local_home_goals"].GetInt();
                aggr_visitor_home_goals = d["aggr_history_visitor_home_goals"].GetInt();
            }
        }

        LOG(INFO) << " winner required " << winnerRequired;

        LOG(INFO) << " game data: " << argv[3];
    }

    const rapidjson::Value& localPlayers = d["localteam"];

    for (rapidjson::SizeType i = 0; i < localPlayers.Size(); i++){
        const rapidjson::Value& c = localPlayers[i];
        int playerId = c["id"].GetInt();
        int defense =c["def"].GetInt();
        int attack =c["att"].GetInt();
        int aggr = c.HasMember("aggr") ? c["aggr"].GetInt() : 50;
        string type = c["type"].GetString();
        int x = c["position"].GetArray()[0].GetInt();
        int y = c["position"].GetArray()[1].GetInt();

        Position * position = new Position(x,y);

        Player::Type playertype;
        if (type == "goal"){
            playertype = Player::Type::GoalKeeper;
        }
        else if (type == "def"){
            playertype = Player::Type::Defense;
        }
        else if (type == "mid"){
            playertype = Player::Type::Midfielder;
        }
        else if (type == "att"){
            playertype = Player::Type::Attack;
        }

        // LOG(INFO) << "add home player " << type << "id " << playerId << "pos " << x << "," << y << "att " << attack <<" def" << defense;

        Player * player = new Player(playerId,position,playertype,attack,defense,aggr);
        lineup.addPlayer(player,Player::Side::Home);
    }

    const rapidjson::Value& visitorPlayers = d["visitorteam"];

    for (rapidjson::SizeType i = 0; i < visitorPlayers.Size(); i++){
        const rapidjson::Value& c = visitorPlayers[i];
        int playerId = c["id"].GetInt();
        int defense =c["def"].GetInt();
        int attack =c["att"].GetInt();
        int aggr = c.HasMember("aggr") ? c["aggr"].GetInt() : 50;
        string type = c["type"].GetString();
        int x = c["position"].GetArray()[0].GetInt();
        int y = c["position"].GetArray()[1].GetInt();

        //position destroy in player destructor
        Position * position = new Position(x,y);

        Player::Type playertype;
        if (type == "goal"){
            playertype = Player::Type::GoalKeeper;
        }
        else if (type == "def"){
            playertype = Player::Type::Defense;
        }
        else if (type == "mid"){
            playertype = Player::Type::Midfielder;
        }
        else if (type == "att"){
            playertype = Player::Type::Attack;
        }

        //LOG(INFO) << "add away player " << type << "id " << playerId << "pos " << x << "," << y << "att " << attack <<" def" << defense;

        //player destroy in lineup destructor
        Player * player = new Player(playerId,position,playertype,attack,defense,aggr);
        lineup.addPlayer(player,Player::Side::Away);
    }

    Team localteam = Team(1,Team::FormationType::f_4_3_3);
    Team visitorteam = Team(2,Team::FormationType::f_4_3_3);

    Communication com = Communication();
    if (argv[1]) {
        com.path = argv[2];
    }
    else{
        com.path = "/tmp/null";
    }
    com.init();



    GameOptions options = GameOptions(lineup,localteam, visitorteam, com);

    if (argv[1]) {
        std::string multiplier = argv[1];
        LOG(INFO) << "set game speed multipler: " << multiplier;
        options.multiplier = atoi(multiplier.c_str());
    }

    options.useAggregate = useAggregate;
    options.winnerRequired = winnerRequired;
    options.aggr_local_home_goals = aggr_local_home_goals;
    options.aggr_visitor_home_goals = aggr_visitor_home_goals;
    options.aggr_local_away_goals = aggr_local_away_goals;
    options.aggr_visitor_away_goals = aggr_visitor_away_goals;

    LOG(INFO) << "multiplier speed = " << options.multiplier;

    Game game = Game(options);
    game.start();

    LOG(INFO) << "game ended";

    LOG(INFO) << "size players " << lineup.players.size();


    LOG(INFO) << "application ended";
    return 0;
}