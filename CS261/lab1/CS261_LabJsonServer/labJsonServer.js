const uuid4 = require('uuid4');
const express = require('express');
const app = express();

app.use(express.json());
app.post('/lab', (req, res) => {
    if (req.headers["content-type"] != "application/json") {
        console.log(`lab: Received POST of non-JSON type ${req.headers["content-type"]}, returning 400`);
        res.sendStatus(400);
        return;
    }
    let output = {
        echo: req.body.input,
        id: uuid4()
    };
    console.log(`lab: returning `, output);
    res.json(output);
});

app.listen(4000);
console.log('listening on 4000...');