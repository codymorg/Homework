const express = require('express');
const app = express();
app.use(express.json());

const redis = require('redis');
const redisClient = redis.createClient(6379, '127.0.0.1');

function buildGameFromScores(gameName, scores) {
    let game = {
        name : gameName, 
        scores : scores
    };
    for (key in game.scores) {
        game.scores[key] = parseInt(game.scores[key]);
    }
    return game;
}

app.post('/scoresLab/games/:gameName/scores/:playerName', (req, res) => {

    redisClient.hmset(`games:${req.params.gameName}:scores`,
        req.params.playerName, req.body.score);

    redisClient.hgetall(`games:${req.params.gameName}:scores`, (err, scores) => {
        if (err != null) {
            console.log(err);
            res.sendStatus(500);
            return;
        }
        res.send(buildGameFromScores(req.params.gameName, scores));
    })
});

app.get('/scoresLab/games/:gameName/scores', (req, res) => {

    redisClient.hgetall(`games:${req.params.gameName}:scores`, (err, scores) => {
        if (err != null) {
            console.log(err);
            res.sendStatus(500);
            return;
        }
        if (scores == null) { 
            res.sendStatus(404);
            return;
        }
        res.send(buildGameFromScores(req.params.gameName, scores));
    })
});

app.listen(4000, () => {
    console.log("Listening on 4000...");
});