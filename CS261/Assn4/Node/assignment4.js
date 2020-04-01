/** require all modules */
let express = require('express');
let {User, Session } = require("./ServerClasses.js")
let { ExtractName } = require("./ServerFunctions.js")

/** PM2 variables */
let NODE_ENV         = process.env.NODE_ENV        
let MONGO_SERVER     = process.env.MONGO_SERVER    
let MONGO_DB_NAME    = process.env.MONGO_DB_NAME   
let REDIS_SERVER     = process.env.REDIS_SERVER    
let REDIS_PORT       = process.env.REDIS_PORT      
let EXPRESS_PORT     = process.env.EXPRESS_PORT    
let SESSION_DURATION = process.env.SESSION_DURATION
if(NODE_ENV == null){
    NODE_ENV         = 'testing mode',
    MONGO_SERVER     = 'mongodb://localhost:27017',
    MONGO_DB_NAME    = 'userDB_test',
    REDIS_SERVER     = '127.0.0.1',
    REDIS_PORT       = 6379,
    EXPRESS_PORT     = 3100,
    SESSION_DURATION = 10
}
/** set up server traits */
let app = express();
app.listen(EXPRESS_PORT)
app.use(express.json())

/** mongo db */
const MongoClient = require('mongodb').MongoClient;
const mongoClient = new MongoClient(MONGO_SERVER, { useUnifiedTopology : true });

/** redis ... db */
const redis = require('redis')
const redisClient = redis.createClient(REDIS_PORT, REDIS_SERVER)

console.log("Login Server Listening on Port " + EXPRESS_PORT)

mongoClient.connect(() => {
    
    const db = mongoClient.db(MONGO_DB_NAME);
    const users = db.collection('users');
    users.createIndex({ name: 1}, {unique: true});
    
    /** 
     * @brief Create user, no authentication 
     * @param req : must contain username, password, avatar
     * @param res : will contain new user data
     * MONGO no redis
     */
    app.post("/api/v1/users/", (req, res) => {
        let key = `ID${req.body.username}`
        users.findOne(
            {name : key},
            (err, dbUser) => {
                if(err != null){
                    console.log(err)
                    res.sendStatus(500)
                }
                else if(dbUser != null) {
                    console.log(req.body.username + " already exists")
                    res.sendStatus(409);
                }
                else {
                    users.findOneAndUpdate(
                        { name : key },
                        { $set : { [key] : new User(req.body.username, req.body.password, req.body.avatar)} }, 
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
                            else {
                                console.log("created new user " + req.body.username)
                                newUser = result.value[key]
                                res.send(newUser)
                            }
                        });
                }
            })
        
    })

    /** 
     * @brief login user, no authentication 
     * @param req : must contain username, password
     * @param res : will contain new session data
     * MONGO redis
     */
    app.post("/api/v1/login", (req, res) => {
        let result = null
        users.find().forEach(function(doc){

            if(doc[`${doc.name}`].username == req.body.username) {
                result = doc[`${doc.name}`]
            }
        }).then(function () {

            if(result == null){
                console.log("user does not exist: " + req.body.username)
                res.sendStatus(400)
            }
            else if(result.username != req.body.username || result.password != req.body.password) {
                console.log("invalid password")
                res.sendStatus(403)
            }
            else {
                console.log(result.username + " authorized")

                // create session
                let newSession = new Session("ID" + req.body.username)
                console.log("new session " + newSession.session + " for user " + newSession.key)
                redisClient.keys("*", (err, sessions) =>{
                    for(session in sessions) {
                        redisClient.hgetall(sessions[session], (err, ses) =>{
                            if(ses.key == newSession.key){
                                redisClient.del(sessions[session])
                            }
                        })
                    }
                })
                redisClient.hmset(
                    newSession.session,
                    "token",
                    newSession.token
                )
                redisClient.hmset(
                    newSession.session,
                    "key",
                    newSession.key
                )

                redisClient.expire(newSession.session, SESSION_DURATION)
                res.session = newSession.session
                res.token = newSession.token
                let time = new Date()
                time.getHours() + ":" + time.getMinutes() + "." + time.getSeconds()
                console.log("logging in user: " + req.body.username + " at " + time)
                res.send(newSession)
            }
        })
    })

    /** 
     * @brief get user by ID, authentication 
     * @param req : must contain session, token
     * @param res : will contain user data
     * MONGO redis
     */
    app.get("/api/v1/users/:id", (req, res) => {
        let auth = false
        redisClient.ttl(req.body.session, (err2, ttl) => {

            redisClient.hgetall(req.body.session, (err, ses) => {
                if(ses != null && req.body.token == ses.token){
                   auth = true
                   console.log(ses.key + " authenticated for " + ttl) 
                }
                else{
                    console.log("unathenticated user")
                }
                // unauthenticated
                if ( auth == false) {
                    console.log("bad authentication in GetByUserID")
                    res.sendStatus(401)
                }
                
                // authenticated
                else {
                    let findUser = ExtractName(req.params.id)
                    let key = `ID${findUser}`
                    users.findOne(
                        {name : key},
                        (err, result) => {
                            if(err != null){
                                console.log(err)
                                res.sendStatus(500)
                            }
                            
                            // couldnt find the sought after user
                            if(result == null){
                                
                                console.log("user not found: " + findUser)
                                res.sendStatus(404)
                            }
                            else {
                                let foundUser = Object.assign({}, result[key])
                                
                                redisClient.hgetall(req.body.session, (err, ses) => {
                                    if(ses == null){
                                        res.sendStatus(401)
                                    }
                                    else{
                                        const myKey = ses.key
                                        // that user isnt you
                                        if (myKey != key) {
                                            foundUser.password = undefined
                                        }
                                        console.log("returning user: " + findUser + " to user: " + myKey)
                                        res.send(foundUser)
                                    }
                                })
                            }
                        })
                    }
                })
        })

        }
    )


    /** 
     * @brief get user, authentication 
     * @param req : must contain session, token
     * @param res : will contain user data
     * MONGO redis
     */
    app.get("/api/v1/users", (req, res) => {
        
        let auth = false
        redisClient.ttl(req.body.session, (err2, ttl) => {
            redisClient.hgetall(req.body.session, (err, ses) => {
                if(ses != null && req.body.token == ses.token){
                   auth = true
                   let time = new Date()
                   time.getHours() + ":" + time.getMinutes() + "." + time.getSeconds()
                   console.log(ses.key + " authenticated for " + ttl + " at " + time) 
                   
    
                    // no user query provided
                    if (req.query.username == undefined) {
                    res.sendStatus(400)
                    }
                    else {
                        let myName = ses.key
                        let findUser =req.query.username
                        let key = `ID${findUser}`
                        users.findOne(
                            {name : key},
                            (err, result) => {
                                if(err != null){
                                    console.log(err)
                                    res.sendStatus(500)
                                }
            
                                // bad user request
                                else if(result == null) {
                                    users.find().forEach(function(doc){
            
                                        if(doc[`${doc.name}`].username == findUser) {
                                            result = doc[`${doc.name}`]
                                        }
                                    }).then(function () {
            
                                        if(result == null){
                                            console.log("couldn't find user " + findUser)
                                            res.sendStatus(404)
                                        }
                                        else {
                                            let foundUser = Object.assign({}, result)
            
                                            // that user isnt you
                                            if (myName != key) {
                                                foundUser.password = undefined
                                            }
                                            console.log("returning user: " + foundUser.username + " to user: " + myName)
                                            res.send(foundUser)
                                        }
                                    })
                                }
                                else {
                                    let foundUser = Object.assign({}, result[key])
            
                                    // that user isnt you
                                    if (myName != key) {
                                        foundUser.password = undefined
                                    }
                                    console.log("returning user: " + foundUser.username + " to user: " + myName)
                                    res.send(foundUser)
                                }
                            }
                        )
                    }
                }
                else{
                    console.log("unathenticated user")
                    res.sendStatus(401)
                }
            })
        })

    })

    /** 
     * @brief update user, authentication 
     * @param req : must contain username, password, session, token
     * @param res : will contain found user data
     * MONGO redis
     */
    app.put("/api/v1/users/:id", (req, res) => {
        redisClient.hgetall(req.body.session, (err, ses) => {
            if(ses != null && req.body.token == ses.token){
                console.log(ses.key + " authenticated") 

                let myKey = ses.key
                let findUser = ExtractName(req.params.id)
                let key = `ID${findUser}`
    
                users.findOne(
                    {name : key},
                    (err, result) => {
                        if(err != null){
                            console.log(err)
                            res.sendStatus(500)
                        }
                        
                        // no user
                        else if(result == null) {
                            console.log("no such user: " + findUser)
                            res.sendStatus(404)
                        }
                        
                        // unauthorized to make changes to this user
                        else if (myKey != key) {
                            console.log("access denied on updating other user info")
                            res.sendStatus(403)
                        }
    
                        // you are authorized - change your user as you see fit
                        else {
                            let id = result[key].id
                            newUser = new User(req.body.username, req.body.password, req.body.avatar)
                            newUser.id = id
                            users.findOneAndUpdate(
                                { name : key },
                                { $set : { [key] : newUser} }, 
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
                                    else if (result != null) {
                                        console.log("updated user " + myKey + " to " + req.body.username)
                                        res.send(result.value[key])
                                    }
                                }
                            )
                        }
                            
                    }
                )
            }
            else{
                console.log("bad authentication when updating user")
                res.sendStatus(401)
            }
        })
    })
                
                

    /** 
     * @brief connect user, authentication 
     * @param req : must contain session, token
     * @param res : will contain server data
     * MONGO redis wip
     */
    app.post("/api/v1/connect", (req, res) => {
        redisClient.hgetall(req.body.session, (err, ses) => {
            if(ses != null && req.body.token == ses.token){
                console.log(ses.key + " authenticated") 

                let key = ses.key
                users.findOne(
                    {name : key},
                    (err, result) => {
                        if(err != null){
                            console.log(err)
                            res.sendStatus(500)
                        }
                        else {
                            result[key].server = "127.0.0.1:3300"
                            res.send(result[key])
                        }
                    }
                )
            }
            else{
                console.log("bad authentication when connecting user")
                res.sendStatus(401)
            }
        })
    })
})