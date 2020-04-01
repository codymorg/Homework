/** 
 * @brief holds all of the internal server functionality
 */

let { User, users, sessions } = require("./ServerClasses.js")



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
module.exports = { GetUserBySID, ExtractName }
