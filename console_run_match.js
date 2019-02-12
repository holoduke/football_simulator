var fs = require('fs');

var db = require("../utils/db.js");
var dbi = null;
var log = require("../utils/log");
var redis = require('redis'); 
var match = require("../models/match.js");
var team = require("../models/team.js");
var squad = require("../models/squad.js");
var player = require("../models/player.js");
var matchEvent = require("../models/matchevent.js");
var matchStats = require("../models/matchstat.js");
var MatchFinder = require("../models/matchfinder.js");
var ExpTeam = require("../models/exp_team.js");
var ExpPlayer = require("../models/exp_player.js");
var simulator = require("./server.js");
var playernames = {};

//node console_run_match.js 1075 4-3-3 1064 4-3-3 2 2000

var loadConfig = function(cb){
	var fs = require("fs");
	fs.readFile("../config/config.json",function(error,data){
		config = null;
		if (error){
			console.log("error opening config");
		}
		
		try{
			config = JSON.parse(data); 
		}
		catch(e){
			console.log("error parsing config file",e); 
		}
		
		log.d("loaded configuration");
		
		if (cb){
			return cb(config);
		}
	});
}


loadConfig(function(config){

var gameinstance;    
    db.getInstance(config.dbConfiguration,function(db){
        dbi = db;
        
        match.setDbi(db);
        team.setDbi(db);
        squad.setDbi(dbi);
        matchEvent.setDbi(dbi);
        matchStats.setDbi(dbi);
        ExpTeam.setDbi(dbi);
        ExpPlayer.setDbi(dbi);
        config.db = db;
        
        config.redis = redis.createClient();
        
        var Team = team;
        Team.getExtended(process.argv[2],function(teama){
                Team.getExtended(process.argv[4],function(teamb){

                    //create match
                    m = match.getInstance();
                    m.localteam_id = process.argv[2];
                    m.localteam = teama;
                    m.visitorteam_id = process.argv[4];
                    m.visitorteam = teamb;
                    m.status = "SC";
                    m.save(function(result){
                    //    startGame(m,teama,teamb,function(){
                            console.log("finished");
                            process.exit();
                      //  });
                    });
                });
        });
    });


    var getPlayerFormation = function(p,formationType, side, handicap){

        console.log("get formation ",formationType,"handicap",handicap);
        for (var i=0; i < p.length; i++){
            p[i].stat_attack = parseInt(p[i].stat_attack*handicap);
        }

        if (side == "home"){
            if (formationType == "4-3-3"){
                return [
                    {"id":p[10].id,"att":p[10].stat_attack,"def":p[10].stat_defence,"talent":p[10].talent,"position":[0,3],"type":"goal"},
                    {"id":p[9].id,"att":p[9].stat_attack,"def":p[9].stat_defence,"talent":p[9].talent,"position":[1,0],"type":"def"},
                    {"id":p[8].id,"att":p[8].stat_attack,"def":p[8].stat_defence,"talent":p[8].talent,"position":[1,2],"type":"def"},
                    {"id":p[7].id,"att":p[7].stat_attack,"def":p[7].stat_defence,"talent":p[7].talent,"position":[1,4],"type":"def"},
                    {"id":p[6].id,"att":p[6].stat_attack,"def":p[6].stat_defence,"talent":p[6].talent,"position":[1,6],"type":"def"},
                    {"id":p[5].id,"att":p[5].stat_attack,"def":p[5].stat_defence,"talent":p[5].talent,"position":[3,1],"type":"mid"},
                    {"id":p[4].id,"att":p[4].stat_attack,"def":p[4].stat_defence,"talent":p[4].talent,"position":[3,3],"type":"mid"},
                    {"id":p[3].id,"att":p[3].stat_attack,"def":p[3].stat_defence,"talent":p[3].talent,"position":[3,5],"type":"mid"},
                    {"id":p[2].id,"att":p[2].stat_attack,"def":p[2].stat_defence,"talent":p[2].talent,"position":[5,1],"type":"att"},
                    {"id":p[1].id,"att":p[1].stat_attack,"def":p[1].stat_defence,"talent":p[1].talent,"position":[5,3],"type":"att"},
                    {"id":p[0].id,"att":p[0].stat_attack,"def":p[0].stat_defence,"talent":p[0].talent,"position":[5,5],"type":"att"}
                ];
            }
        }
        else if (side == "away"){
            if (formationType == "4-3-3"){
                return [
                    {"id":p[10].id,"att":p[10].stat_attack,"def":p[10].stat_defence,"talent":p[10].talent,"position":[7,3],"type":"goal"},
                    {"id":p[9].id,"att":p[9].stat_attack,"def":p[9].stat_defence,"talent":p[9].talent,"position":[6,0],"type":"def"},
                    {"id":p[8].id,"att":p[8].stat_attack,"def":p[8].stat_defence,"talent":p[8].talent,"position":[6,2],"type":"def"},
                    {"id":p[7].id,"att":p[7].stat_attack,"def":p[7].stat_defence,"talent":p[7].talent,"position":[6,4],"type":"def"},
                    {"id":p[6].id,"att":p[6].stat_attack,"def":p[6].stat_defence,"talent":p[6].talent,"position":[6,6],"type":"def"},
                    {"id":p[5].id,"att":p[5].stat_attack,"def":p[5].stat_defence,"talent":p[5].talent,"position":[4,1],"type":"mid"},
                    {"id":p[4].id,"att":p[4].stat_attack,"def":p[4].stat_defence,"talent":p[4].talent,"position":[4,3],"type":"mid"},
                    {"id":p[3].id,"att":p[3].stat_attack,"def":p[3].stat_defence,"talent":p[3].talent,"position":[4,5],"type":"mid"},
                    {"id":p[2].id,"att":p[2].stat_attack,"def":p[2].stat_defence,"talent":p[2].talent,"position":[2,1],"type":"att"},
                    {"id":p[1].id,"att":p[1].stat_attack,"def":p[1].stat_defence,"talent":p[1].talent,"position":[2,3],"type":"att"},
                    {"id":p[0].id,"att":p[0].stat_attack,"def":p[0].stat_defence,"talent":p[0].talent,"position":[2,5],"type":"att"}
                ];
            }
        }
    }


    var startGame = function(matchObj,teama,teamb,finishCb){

        var data = {};
        var events = {};

        var handicap = matchObj.getHandicap();

        data.id = Math.round(Math.random()*1000);
        data.matchSpeed = process.argv[6] || 9999999999;
        console.log("get formation for local");
        data.localteam = getPlayerFormation(teama.lineup,"4-3-3","home",1);
        console.log("get formation for visitor");
        data.visitorteam = getPlayerFormation(teamb.lineup,"4-3-3","away",1);


        var events = {
            onStart : function(result){
                console.log("on start");
                matchObj.status = "PL";
                matchObj.save();
            },
            onHalfTimeStart : function(result){
                console.log("on halftime start");
                matchObj.status = "HT";
                matchObj.save();
            },
            onHalfTimeFinish : function(result){
                console.log("on halftime finish");
                matchObj.status = "PL";
                matchObj.save();
            },
            onGoal : function(result){
                console.log("on goal",result);

                matchObj.localscore = result.localscore;
                matchObj.visitorscore = result.visitorscore;
                matchObj.save();

                me = matchEvent.getInstance();
                me.type = "goal"
                me.minute = result.minute;
                me.match_id = matchObj.id;
                me.side = result.side
                me.player_id = result.goalBy;
                me.save(function(res,matchevent){

                });

                //send to table calculation queue
                var data = {};
                data.matchId = matchObj.id;
                data.leagueId = matchObj.leagueId
                data.eventType = "goal";
                data.side = result.side;
                data.playerId = result.goalBy;
                data.localteamId=  matchObj.localteam.id;
                data.visitorteamId=  matchObj.visitorteam.id;
                data.localscore = result.localscore;
                data.visitorscore = result.visitorscore;

            },
            onFinish : function (result) {

                console.log("match finished",result.localscore + "-"+result.visitorscore);
                matchObj.status = "FT";
                matchObj.save();

                var data2 = {};
                data2.matchId = matchObj.id;
                data2.leagueId = matchObj.leagueId
                data2.eventType = "finish";
                data2.localteamId=  matchObj.localteam.id;
                data2.visitorteamId=  matchObj.visitorteam.id;
                data2.localscore = result.localscore;
                data2.visitorscore = result.visitorscore;

                try{;
                    var data = JSON.stringify(data2);

                    console.log("Worker " + cluster.worker.id +" - MatchId "+matchObj.id+"send finish event to Queue");
                    console.log("Worker " + cluster.worker.id +" - MatchId "+matchObj.id+"send Table calc event to Queue");
                }
                catch(e){
                    console.log("error send to queue",data);
                }


                //add some data to result blob
                result.home_team = matchObj.localteam;
                result.away_team = matchObj.visitorteam;

                //also for the players

                for (var i=0; i< result.playerSummary.localteam.length;i++){
                    for (var j=0; j < result.home_team.squad.length;j++){
                        if (result.home_team.squad[j].id == result.playerSummary.localteam[i].id){
                            result.playerSummary.localteam[i].lvl = result.home_team.squad[j].lvl;
                            result.playerSummary.localteam[i].talent = result.home_team.squad[j].talent;
                            result.playerSummary.localteam[i].pos = result.home_team.squad[j].position;
                            result.playerSummary.localteam[i].exp = result.home_team.squad[j].exp;
                            result.playerSummary.localteam[i].stat_attack = result.home_team.squad[j].stat_attack;
                            result.playerSummary.localteam[i].stat_defence = result.home_team.squad[j].stat_defence;
                        }
                    }
                }
                for (var i=0; i< result.playerSummary.visitorteam.length;i++){
                    for (var j=0; j < result.away_team.squad.length;j++){
                        if (result.away_team.squad[j].id == result.playerSummary.visitorteam[i].id){
                            result.playerSummary.visitorteam[i].lvl = result.away_team.squad[j].lvl;
                            result.playerSummary.visitorteam[i].talent = result.away_team.squad[j].talent;
                            result.playerSummary.visitorteam[i].pos = result.away_team.squad[j].position;
                            result.playerSummary.visitorteam[i].exp = result.away_team.squad[j].exp;
                            result.playerSummary.visitorteam[i].stat_attack = result.away_team.squad[j].stat_attack;
                            result.playerSummary.visitorteam[i].stat_defence = result.away_team.squad[j].stat_defence;
                        }
                    }
                }

                ExpTeam.update(result,function(updateResult){
                    if (updateResult && updateResult.type == "ok"){
                        //console.log("Worker " + cluster.worker.id +" - MatchId "+matchObj.id+" succesfully updated team stats");
                    }
                    else{
                        //console.log("Worker " + cluster.worker.id +" - MatchId "+matchObj.id+" error updating team stats TODO fallback ",error);
                    }

                    ExpPlayer.update(result,function(result2){
                        console.log("updated player stats -> match is finished ",result.localscore,"-",result.visitorscore);
                        finishCb(true); //send ack to let publisher know match is over and complete
                    });

                });
            },
            onMinute : function(result){
                // console.log("on minute ",result);

                matchObj.timer = result.minute;

                var ms = matchStats.getInstance();
                ms.match_id = matchObj.id;
                ms.home_ballposession = result.stats.home_ballposession_perc;
                ms.away_ballposession = result.stats.away_ballposession_perc;
                ms.home_shotsgoal = result.stats.home_shotsgoal;
                ms.away_shotsgoal = result.stats.away_shotsgoal;
                ms.home_corners = result.stats.home_corners;
                ms.away_corners = result.stats.away_corners;
                ms.home_corner_goals = result.stats.home_corner_goal;
                ms.away_corner_goals = result.stats.away_corner_goal;
                ms.save(function(res){})

                matchObj.save();
            },
            onError : function(){
                //DODO
            }
        }

        //console.log(data);
        simulator.playMatch("./footballsimulator2",data,events);


    }
})
