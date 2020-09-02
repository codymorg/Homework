/** require all modules */
let express = require('express');
let bodyParser = require('body-parser');
let { User, Session } = require("./ServerClasses.js")

/** set up server traits */
let app = express();
app.listen(3100)
app.use(bodyParser.json())
console.log("Server Ready")


/** Declarations */
let users = new Map()
let sessions = new Map()

/** Authenticate user's session and token */
function Authenticate(session, token) {
    if (sessions[session] == undefined) {
        return false
    }

    return sessions[session].token == token
}

/** Authorize a user's name and password */
function Authorize(name, password) {
    // bad name
    if (users[name] == undefined) {
        console.log("user does not exists: " + name)
        return 400
    }

    // bad password
    else if (users[name].password != password) {
        console.log("invalid password")
        return 403
    }

    // good name good password
    else if (users[name].username = name) {
        console.log("user Authorized")
        return 200
    }
}

/** Get user object given a sessionID */
function GetUserBySID(session) {
    if (sessions[session] == undefined)
        return undefined
    else
        return users[sessions[session].username]
}

/** Convert UserID to username */
function ExtractName(userID) {
    let tempUser = new User()
    let userIDLength = tempUser.userIDRange

    return userID.substring(0, userID.length - userIDLength)
}

/** Create user, no authentication */
app.post("/api/v1/users/", (req, res) => {
    // fill map with new user data
    if (users[req.body.username] == undefined) {
        users[req.body.username] = new User(req.body.username, req.body.password, req.body.avatar)

        console.log("Creating new user:" + users[req.body.username].username)
        res.send(users[req.body.username])
    }

    // user already exists - throw error 409
    else {
        console.log("user exists: " + req.body.username)
        res.sendStatus(409)
    }
})

/** Login with user no authentication */
app.post("/api/v1/login", (req, res) => {

    // unauthorized requests return error
    let outCode = Authorize(req.body.username, req.body.password)
    if (outCode != 200) {
        res.sendStatus(outCode)
    }

    // authorized requests may login with a new session
    else {
        // create session
        let newSession = new Session(req.body.username)
        sessions[newSession.session] = newSession
        res.session = newSession.session
        res.token = newSession.token

        console.log("logging in user: " + req.body.username)
        res.send(newSession)
    }
})

/** Get user by userID authenticated */
app.get("/api/v1/users/:id", (req, res) => {

    // unauthenticated
    if (Authenticate(req.body.session, req.body.token) == false) {

        console.log("bad authentication in GetByUserID")
        res.sendStatus(401)
    }

    // authenticated
    else {
        let myName = sessions[req.body.session].username
        let findUser = ExtractName(req.params.id)

        // bad find
        if (users[findUser] == undefined) {
            console.log("no such user in GetUserByID: " + findUser)
            res.sendStatus(404)
        }

        // user found
        else {
            let foundUser = Object.assign({}, users[findUser])

            // that user isnt you
            if (myName != findUser) {
                foundUser.password = undefined
            }

            console.log("returning user: " + findUser + " to user: " + myName)
            res.send(foundUser)
        }
    }
})

/** Get user by name authenticated */
app.get("/api/v1/users", (req, res) => {
    // not authenticated
    if (Authenticate(req.body.session, req.body.token) == false) {
        res.sendStatus(401)
    }

    // no user query provided
    else if (req.query.username == undefined) {
        res.sendStatus(400)
    }

    // bad user request
    else if (users[req.query.username] == undefined) {
        res.sendStatus(404)
    }

    // valid user
    else {
        let foundUser = Object.assign({}, users[req.query.username])
        let myName = sessions[req.body.session].username

        // this is not your user
        if (foundUser.username != myName) {
            foundUser.password = undefined
        }

        console.log("returning user: " + foundUser.username + " to user: " + myName)
        res.send(foundUser)
    }

})

/** Update user authenticated */
app.put("/api/v1/users/:id", (req, res) => {
    if (Authenticate(req.body.session, req.body.token) == false) {
        console.log("bad authentication when updating user")
        res.sendStatus(401)
    }

    // authenticated
    else {
        let myUser = GetUserBySID(req.body.session).username
        let findUser = ExtractName(req.params.id)

        // no user
        if (users[findUser] == undefined) {
            console.log("no such user: " + findUser)
            res.sendStatus(404)
        }

        // unauthorized to make changes to this user
        else if (myUser != findUser) {
            console.log("access denied on updating other user info")
            res.sendStatus(403)
        }

        // you are changing your own account
        else {
            users[findUser].username = req.body.username
            users[findUser].password = req.body.password
            users[findUser].avatar = req.body.avatar

            console.log("user data changed: " + users[findUser])
            res.send(users[findUser])
        }
    }
})
