/** 
 * @brief holds all of the internal server functionality
 */

let { User, users, sessions } = require("./ServerClasses.js")

/**
 * @brief Authenticates a user's session and token against those issued
 * @param session : the incoming session ID
 * @param token   : the incoming token ID         
 */
function Authenticate(session, token) {
    if (sessions[session] == undefined) {
        console.log("Unauthenticated session " + session)
        return false
    }

    return sessions[session].token == token
}

/** 
 * @brief Authorize a user's name and password 
 * @param name     : the name this user gave
 * @param password : their password
 */
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

/** 
 * @brief Get user object given a sessionID 
 * @param session : an authenticated sessionID
 */
function GetUserBySID(session) {
    if (sessions[session] == undefined)
        return undefined
    else
        return users[sessions[session].username]
}

/** 
 * @brief Convert UserID to usernamee
 * @param userID : convert a userID into a key for lookup
 */
function ExtractName(userID) {
    let tempUser = new User()
    let userIDLength = tempUser.userIDRange

    return userID.substring(0, userID.length - userIDLength)
}

// export to higher modules
module.exports = { Authenticate, Authorize, GetUserBySID, ExtractName }
