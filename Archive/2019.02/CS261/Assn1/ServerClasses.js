/** map of all our users as [name]:User(name, password, avatar, UID)*/


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
    constructor(name) {
        const IDRange_ = 10000
        this.IDRange = 5
        this.token = (Math.round(Math.random() * IDRange_) + IDRange_ + 1).toString() // [10001, 20001]
        this.username = name
        this.session = (Math.round(Math.random() * IDRange_)).toString()             // [0, 10000]
    }
}

// export to higher modules
module.exports = { User, Session }
