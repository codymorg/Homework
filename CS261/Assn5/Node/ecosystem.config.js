module.exports = {
  apps : [{
    name: 'assignment5',
    script: 'assignment5.js',

    // Options reference: https://pm2.keymetrics.io/docs/usage/application-declaration/
    instances: 1,
    autorestart: true,
    watch: false,
    max_memory_restart: '1G',
    env: {
      NODE_ENV:         'development',
      MONGO_SERVER:     'mongodb://localhost:27017',
      MONGO_DB_NAME:     'userDB_dev',
      REDIS_SERVER:     '127.0.0.1',
      REDIS_PORT:       6379,
      EXPRESS_PORT:     3100,
      SESSION_DURATION: 10,
      SHARED_SECRET: 'CS261S20'
      

    },
    env_production: {
      NODE_ENV: 'Production',
      MONGO_SERVER : 'mongodb://localhost:27017',
      MONGO_DB_NAME : 'userDB_pro',
      REDIS_SERVER : '127.0.0.1',
      REDIS_PORT : 6379,
      EXPRESS_PORT : 3100,
      SESSION_DURATION : 10,
      SHARED_SECRET : 'CS261S20'
      
    }
  }],

};
