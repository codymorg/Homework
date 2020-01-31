var express = require('express');
var bodyParser = require('body-parser');
var app = express();
app.use(bodyParser.json())

var users = new Map()
class User {
    constructor(name, password, avatar) {
        this.username = name
        this.password = password
        this.avatar = avatar
        this.id = name + Math.round(Math.random() * 1000)
    }
}

var sessions = new Map()
class Session {
    constructor(name) {
        this.token = Math.round(Math.random * 10000)
        this.name = name
    }
}



app.get('/', (req, res) => {
    console.log('Hit the root get handler!')
    res.send('get got')
})
app.listen(3100)
console.log('express2 listening')

/** Create user, no authentication */
app.post('/api/v1/users/', (req, res) => {
    console.log("posting")

    // fill map with new user data
    users[req.username] = new User(req.body.username, req.body.password, req.body.avatar)
    console.log(users[req.username])
    res.send(users[req.username])
})

