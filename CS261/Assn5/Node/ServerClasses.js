/** 
 * @brief base file contains users and sessions calsses and maps
 *        map of all our users as [name]:User(name, password, avatar, UID)
 */

let crypto = require('crypto')

class User {
    constructor(name, password, avatar) {
        const userRange_ = 1000
        this.userIDRange = 4
        this.username = name
        this.password = password
        this.avatar = avatar
        this.id = name + (Math.round(Math.random() * userRange_) + userRange_)
    }
}

class Session {
    constructor(name, message) {
        const IDRange_ = 10000
        this.IDRange = 5
        this.token = BuildToken(message + Math.round(Math.random() * 1000))
        this.key = name
        this.session = (Math.round(Math.random() * IDRange_)).toString()             // [0, 10000]
    }
}

function BuildToken(message) {

    // expanded to ease debuggability
    // could be compacted to: return crypto.createHash('sha256').update(message).digest('base64');

    let hash = crypto.createHash('sha256');
    hash.update(message);
    let token = hash.digest('base64');
    return token;
}

// export to higher modules
module.exports = { User, Session }
