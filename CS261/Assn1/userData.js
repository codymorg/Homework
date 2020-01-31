/** map of all our users as [name]:User(name, password, avatar, UID)
 *  ans Sessions as [SessionID]:Session(token, name)
*/

var users = new Map()
class User {
    constructor(name, password, avatar) {
        this.name = name
        this.password = password
        this.avatar = avatar
        this.UID = name
    }
}

var sessions = new Map()
class Session {
    constructor(name) {
        this.token = Math.round(Math.random * 10000)
        this.name = name
    }
}

// export to higher modules
module.exports = { users, sessions }
module.exports = User
module.exports = Session

