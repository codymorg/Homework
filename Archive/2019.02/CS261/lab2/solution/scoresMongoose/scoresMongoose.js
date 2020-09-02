const express = require('express');
const app = express();
app.use(express.json());

let mongoose = require('mongoose');
mongoose.connect('mongodb://localhost:27017/cs261lab', {
    useNewUrlParser : true,
    useCreateIndex: true,
    useUnifiedTopology : true,
    useFindAndModify : false,
});

let gameSchema = new mongoose.Schema({
    name : String,
    scores : mongoose.Schema.Types.Mixed
});
gameSchema.index({name : 1}, {unique: true});
let Game = mongoose.model('game', gameSchema);

app.post('/scoresLab/games/:gameName/scores/:playerName', (req, res) => {

    Game.findOneAndUpdate(
        { name: req.params.gameName },
        { $set : { [`scores.${req.params.playerName}`] : req.body.score } },
        {
            new : true, 
            upsert : true 
        }, 
        (err, model) => {
            if (err != null) {
                console.log(err);
                res.sendStatus(500);  
                return;  
            }
            res.send(model);
        });
});

app.get('/scoresLab/games/:gameName/scores', (req, res) => {

    Game.findOne(
        {name : req.params.gameName}, 
        (err, model) => {
            if (err != null) {
                console.log(error);
                res.sendStatus(500);   
                return; 
            }
            if (model == null) {
                res.sendStatus(404);
                return;
            }
            res.send(model);
        });
});

let db = mongoose.connection;
db.on('error', console.error.bind(console, 'connection error: '));
db.once('open', function() {

    app.listen(4000, () => {
        console.log("Listening on 4000...");
    });
});