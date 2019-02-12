var net = require('net');
var fs = require('fs');
var path = '/tmp/match.sock';
var child_process = require('child_process');

var playMatch = function (binary, data, events) {

    var socketPath = path + data.id;
    var allowConnection = true;

    function processData() {

        if (!dataToProcess || dataToProcess == "") return;
        try {
            dataparsed = JSON.parse(dataToProcess);
        }

        catch (e) {
            console.log("error parsing " + e, dataToProcess);
            process.exit();
            return;
        }
        if (dataparsed.type == "start") {
            events.onStart(dataparsed);
        }
        else if (dataparsed.type == "halftime_start") {
            events.onHalfTimeStart(dataparsed);
        }
        else if (dataparsed.type == "halftime_finish") {
            events.onHalfTimeFinish(dataparsed);
        }
        else if (dataparsed.type == "minute") {
            events.onMinute(dataparsed);
        }
        else if (dataparsed.type == "goal") {
            events.onGoal(dataparsed);
        }
        else if (dataparsed.type == "foul") {
            events.foul(dataparsed);
        }
        else if (dataparsed.type == "break_before_et_start") {
            events.break_before_et_start(dataparsed);
        }
        else if (dataparsed.type == "extra_time_first_half_start") {
            events.extra_time_first_half_start(dataparsed);
        }
        else if (dataparsed.type == "break_between_et_start") {
            events.break_between_et_start(dataparsed);
        }
        else if (dataparsed.type == "extra_time_second_half_start") {
            events.extra_time_second_half_start(dataparsed);
        }
        else if (dataparsed.type == "break_before_penalty_start") {
            events.break_before_penalty_start(dataparsed);
        }
        else if (dataparsed.type == "penalty_session_start") {
            events.penalty_session_start(dataparsed);
        }
        else if (dataparsed.type == "penalty_session_goal") {
            events.penalty_session_goal(dataparsed);
        }
        else if (dataparsed.type == "penalty_session_miss") {
            events.penalty_session_miss(dataparsed);
        }
        else if (dataparsed.type == "finish") {
            events.onFinish(dataparsed);
        }
    }

    fs.unlink(socketPath, function () {
        var server = net.createServer(function (c) {


            if (!allowConnection) {
                console.log("ignore processing. process can be connected only once");
                c.destroy();
                return;
            }

            allowConnection = false;

            c.on('end', function () {
                dataToProcess = null;
                server.close();
                fs.unlink(socketPath, function () {

                });
                c.destroy();
                c = null;
                server = null;
            });

            var internalBuffer = "";

            c.on('data', function (data) {

                data = data.toString();

                if (data && data.slice(-3) == "|||") {

                    if (internalBuffer != "") {

                        data = internalBuffer + data;

                        var dataParts = data.split("|||");

                        for (var i = 0; i < dataParts.length; i++) {
                            dataToProcess = dataParts[i];
                            processData();
                        }
                        internalBuffer = "";
                    }
                    else {
                        var dataParts = data.split("|||");

                        for (var i = 0; i < dataParts.length; i++) {
                            dataToProcess = dataParts[i];
                            processData();
                        }
                    }
                }
                else if (data) {
                    internalBuffer += data;
                }
            });

            c.on('error', function (err) {
                console.log("incoming error", err);
            });
        });console
        try {
            server.listen(socketPath, function () {
                //console.log('server bound on %s', socketPath);
                var command = binary + " " + data.matchSpeed + " " + socketPath + " \"" + JSON.stringify(data).replace(/\"/g, "\\\"") + "\"";

                var options = {
                    timeout: 20000
                }
                //console.log("execute command ",command);
                child_process.exec(command, options, function (error, stdout, stderr) {
                    //console.log(socketPath,error,stdout,stderr);
                });
            });
        }
        catch(e){
            console.log("critical error "+e);
        }
    });
}

var stats = {
    runsDone : 0,
    homeWin : 0,
    awayWin : 0,
    draw : 0,
    penaltyEnded : 0,
    afterExtraTimeEnded : 0,
    avg_goals_per_match : 0,
    total_goals :0,
    highest_goal_count : 0,
    longest_penalty_session:0,
    total_penalty_session_rounds:0,
    avg_penalty_session_length :0
}


var matchId = 1;

module.exports = {

    playMatch : function(binary,data,events){
        playMatch(binary,data, events);
    },

    runTest : function(binary,speed,runs,completecb){

        var runsDone = 0;

        function doneOrNot(){
            stats.runsDone++;
            runsDone++;
            if (runsDone == runs){
                console.log("run "+stats.runsDone+" matches. homewin:"+stats.homeWin+" - awaywin:"+stats.awayWin+" - draw:"+stats.draw+" - AET:"+stats.afterExtraTimeEnded+" - PEN:"+stats.penaltyEnded
                    +" - highest goal score: "+stats.highest_goal_count+" total goals: "+stats.total_goals+" - avg goals per match: "+stats.avg_goals_per_match+" - longest penalty session:"+stats.longest_penalty_session
                );
                if (completecb){
                    completecb();
                }
            }
        }

        for (var i=0; i < runs;i++) {

            matchId++;

            var data =
            {
                "id" : matchId+Math.random()*10000000, //unique identifier for a match. must be unique when using paralell
                "matchSpeed" : speed, ///multplier of match speed. 1 = 90 minutes. 10 = 9 minutes etc
                "requireWinner" : 1, //if 1 a match will end with score difference or penalties
                "useAggregateHistory" : 0,
                "aggr_history_local_away_goals" :0, //Aggregation score of a previous match
                "aggr_history_visitor_away_goals" :0,
                "aggr_history_local_home_goals" :0,
                "aggr_history_visitor_home_goals" :0,
                "localteam":
                    [{"id":1,"att":50,"def":50,"position":[0,3],"type":"goal"},
                        {"id":2,"att":150,"def":50,"position":[1,0],"type":"def"},
                        {"id":3,"att":150,"def":50,"position":[1,2],"type":"def"},
                        {"id":4,"att":50,"def":50,"position":[1,4],"type":"def"},
                        {"id":5,"att":50,"def":50,"position":[1,6],"type":"def"},
                        {"id":6,"att":50,"def":50,"position":[3,1],"type":"mid"},
                        {"id":7,"att":50,"def":50,"position":[3,3],"type":"mid"},
                        {"id":8,"att":150,"def":50,"position":[3,5],"type":"mid"},
                        {"id":9,"att":150,"def":50,"position":[5,1],"type":"att"},
                        {"id":10,"att":50,"def":50,"position":[5,3],"type":"att"},
                        {"id":11,"att":50,"def":50,"position":[5,5],"type":"att"},
                    ],
                "visitorteam":
                    [{"id":12,"att":50,"def":50,"position":[7,3],"type":"goal"},
                        {"id":13,"att":50,"def":50,"position":[6,0],"type":"def"},
                        {"id":14,"att":50,"def":50,"position":[6,2],"type":"def"},
                        {"id":15,"att":50,"def":50,"position":[6,4],"type":"def"},
                        {"id":16,"att":50,"def":50,"position":[6,6],"type":"def"},
                        {"id":17,"att":50,"def":50,"position":[4,1],"type":"mid"},
                        {"id":18,"att":50,"def":50,"position":[4,3],"type":"mid"},
                        {"id":19,"att":50,"def":50,"position":[4,5],"type":"mid"},
                        {"id":20,"att":50,"def":50,"position":[2,1],"type":"att"},
                        {"id":21,"att":50,"def":50,"position":[2,3],"type":"att"},
                        {"id":22,"att":50,"def":50,"position":[2,5],"type":"att"},
                    ]
            }

            var events = {
                onStart : function(result){
                    //console.log("on start","aggregate score: local-home:"+result.aggr_local_home_goals,", local-away:"+result.aggr_local_away_goals+", visitor-home:"+result.aggr_visitor_home_goals,", visitor-away:"+result.aggr_visitor_away_goals);
                },
                onHalfTimeStart : function(result){
                    //console.log("on halftime start");
                },
                onHalfTimeFinish : function(result){
                    //console.log("on halftime finish");
                },
                onGoal : function(result){
                    //console.log("on goal ",result.localscore+" - "+result.visitorscore);
                },
                onFoul : function(result){
                    //console.log("on foul");
                },
                break_before_et_start : function(result){
                    //console.log("break before et");
                },
                extra_time_first_half_start : function(result){
                    //console.log("extra_time_first_half_start");
                },
                break_between_et_start : function(result){
                    //console.log("break_between_et_start");
                },
                extra_time_second_half_start : function(result){
                    //console.log("extra_time_second_half_start");
                },
                break_before_penalty_start : function(result){
                    //console.log("break_before_penalty_start");
                },
                penalty_session_start : function(result){
                    //console.log("penalty_session_start");
                },
                penalty_session_goal : function(result){
                    //console.log("on penalty_session_goal",result.localPenaltyScore+" - "+result.visitorPenaltyScore);
                    stats.longest_penalty_session = Math.max(stats.longest_penalty_session,result.round);
                    //stats.avg_penalty_session_length = Math.round((stats.total_penalty_session_rounds/stats.penaltyEnded)*100)/100;
                },
                penalty_session_miss : function(result){
                    //console.log("on penalty_session_miss",result.side);
                    stats.longest_penalty_session = Math.max(stats.longest_penalty_session,result.round);
                },
                onFinish : function (result) {

                    if (result.aggr_winner == "home"){
                        var whowins = "Aggregate Winner: HOME"
                    }
                    else if (result.aggr_winner == "away"){
                        whowins = "Aggregate Winner: AWAY"
                    }
                    else if (result.aggr_winner == null){
                        whowins = "Aggregate Winner: NULL"
                    }
                    else{
                        whowins = "Aggregate Winner: DRAW"
                    }

                    var whowins2;
                    if (result.match_winner == "home"){
                        var whowins2 = "Match Winner: HOME";
                    }
                    else if (result.match_winner == "away"){
                        var whowins2 = "Match Winner: AWAY";
                    }
                    else{
                        var whowins2 = "Match Winner: DRAW";
                    }

                    var results ="";
                    if (result.afterPenaltySession){
                        results = "PEN "+result.localscore+" - "+result.visitorscore+" - (PEN Score: "+result.localPenaltyScore+" - "+result.visitorPenaltyScore+") - ";
                    }
                    else if (result.afterExtraTime){
                        results = "AET "+result.localscore_before_et+" - "+result.visitorscore_before_et+" - (ET  Score: "+result.localscore+" - "+result.visitorscore+") - ";
                    }
                    else{
                        results = "FT  "+result.localscore+" - "+result.visitorscore+" - ";
                    }

                    var previousMatchScore = "Previous Aggregate Score: " +(result.aggr_visitor_home_goals)+ " - "+(result.aggr_local_away_goals);
                    var previousMatchScore = "";
                    //var previousMatchScore = "Aggregate Score: lh:"+result.aggr_local_home_goals+", la:"+result.aggr_local_away_goals+", vh:"+result.aggr_visitor_home_goals+", va:"+result.aggr_visitor_away_goals;


                    //AGGR: LH:"+result.aggr_local_home_goals,", LA:"+result.aggr_local_away_goals+", VH:"+result.aggr_visitor_home_goals,", VA:"+result.aggr_visitor_away_goals

                    console.log("on finish",results,whowins2," - ",whowins," - ",previousMatchScore);

                    //console.log("match finished",result.localscore + "-"+result.visitorscore,result);
                    // console.log("player summary ",result.playerSummary);


                    if (!result.aggr_winner){ //no aggregate score is used to determine winner
                        if (result.localscore > result.visitorscore){
                            stats.homeWin++;
                        }
                        else if (result.localscore < result.visitorscore){
                            stats.awayWin++;
                        }
                        else if (result.afterPenaltySession){
                            if (result.localPenaltyScore > result.visitorPenaltyScore){
                                stats.homeWin++;
                            }
                            else{
                                stats.awayWin++;
                            }
                        }
                        else{
                            stats.draw++;
                        }
                    }
                    else{
                        if (result.aggr_winner == "home"){
                            stats.homeWin++;
                        }
                        else if (result.aggr_winner == "away"){
                            stats.awayWin++;
                        }
                        else if (result.aggr_winner == "draw"){
                            stats.draw++;
                        }
                        else{
                            console.log("error no aggr winner known");
                        }
                    }

                    if (result.afterExtraTime){
                        stats.afterExtraTimeEnded++;
                    }
                    if (result.afterPenaltySession){
                        stats.penaltyEnded++;
                    }

                    stats.total_goals += result.localscore + result.visitorscore;
                    stats.avg_goals_per_match = Math.round((stats.total_goals / stats.runsDone)*100)/100;

                    stats.highest_goal_count = Math.max(stats.highest_goal_count,result.localscore);
                    stats.highest_goal_count = Math.max(stats.highest_goal_count,result.visitorscore);

                    doneOrNot();
                },
                onMinute : function(result){
                    //console.log("on minute ",result.minute);
                }
            }


            //var m = new MatchPlayer();
            //m.playMatch(binary,data,events);
            playMatch(binary, data, events);
        }

    }
}

if (process.argv[2] == "test"){
    var speed = 1000000000000;
    var paralell = 500;
    var runs = 1;
    var binary = "./bin/footballsimulator";
    if (process.argv[3]){
        speed = process.argv[3];
    }
    if (process.argv[4]){
        paralell = process.argv[4];
    }
    if (process.argv[5]){
        binary = process.argv[5];
    }

    var currentRun = 1;
    var start = function() {
        module.exports.runTest(binary, speed, paralell, function () {
            if (currentRun == runs) {
                process.exit();
            }
            else {
                currentRun++;

                setTimeout(function(){
                    start();
                },100)
            }
        });
    }
    start();
}
