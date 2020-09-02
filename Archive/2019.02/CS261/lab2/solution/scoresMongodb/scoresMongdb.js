const express = require('express');
const app = express();
app.use(express.json());

const MongoClient = require('mongodb').MongoClient;
const mongoClient = new MongoClient('mongodb://localhost:27017', { useUnifiedTopology : true });

mongoClient.connect(() => {
    const db = mongoClient.db('cs261lab');
    const gameCollection = db.collection('games');
    gameCollection.createIndex({ name: 1}, {unique: true});

    app.post('/scoresLab/games/:gameName/scores/:playerName', (req, res) => {

        gameCollection.findOneAndUpdate(
            { name : req.params.gameName },
            { $set : { [`scores.${req.params.playerName}`] : req.body.score } }, 
            { 
                upsert : true, 
                returnOriginal : false 
            }, 
            (err, result) => {
                if (err != null) {
                    console.log(err);
                    res.sendStatus(500);    
                    return;
                }
                res.send(result.value);
            });
    });
    
    app.get('/scoresLab/games/:gameName/scores', async (req, res) => {

        gameCollection.findOne(
            { name : req.params.gameName }, 
            (err, result) => {
            if (err != null) {
                console.log(err);
                res.sendStatus(500);    
                return;
            }
            if (result == null) {
                res.sendStatus(404);
                return;
            }
            res.send(result);
        });
    });

    app.listen(4000, () => {
        console.log("Listening on 4000...");
    });
});