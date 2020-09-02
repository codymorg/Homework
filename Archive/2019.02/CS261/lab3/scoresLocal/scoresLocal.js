const express = require('express');
const app = express();
app.use(express.json());

const redis = require("redis")
const client = redis.createClient()

const allGames = {};
app.post('/scoresLab/games/:gameName/scores/:playerName', (req, res) => {

    let game = allGames[req.params.gameName];
    if (game == undefined) {
        game = {
            name: req.params.gameName,
            scores: {}
        };
        allGames[req.params.gameName] = game;
    }
    game.scores[req.params.playerName] = req.body.score;
    res.send(game);
});

app.get('/scoresLab/games/:gameName/scores', (req, res) => {

    let game = allGames[req.params.gameName];
    if (game == undefined) {
        res.sendStatus(404);
        return;
    }
    res.send(game);
});

app.listen(4000, () => {
    console.log("Listening on 4000...");
});