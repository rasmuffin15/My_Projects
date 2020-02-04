var express = require('express');
var crypto = require('crypto'), algorithm = 'aes-256-cbc', key = 'nerdy4birdy';
var pgp = require('pg-promise')();

var router = express.Router();
var supUser = '';

const dbConfig = {
    host: 'localhost',
    port: 5433,
    database: 'postgres',
    user: 'postgres',
    password: 'admin'
};

var db = pgp(dbConfig);


/* GET home page. */
router.get('/', function (req, res, next) {
    supUser  ='';
    res.render('signin', {title: 'Express'});
});

router.get('/signin', function (req, res, next) {
    supUser  ='';
    res.render('signin', {title: 'Express'});
});

router.post('/signin/login', function (req, res) {

    var cipher = crypto.createCipher(algorithm, key);
    var username = req.body.usernamefield;
    var tempPwd = req.body.passwordfield;

    var password = cipher.update(tempPwd, 'utf8', 'hex');
    password += cipher.final('hex');
    var check_signin = 'SELECT EXISTS(SELECT 1 FROM userLogin where username = \'' + username + '\' AND password = \'' + password + '\');';
    var getSightings = 'SELECT * FROM birdsightings INNER JOIN birds ON birdsightings.bird_id = birds.bird_id INNER JOIN users ON birdsightings.username = users.username ORDER BY sighting_number LIMIT 10;'
    console.log(check_signin)
    db.task('get-everything', task => {
        return task.batch([
            task.query(check_signin),
            task.query(getSightings)
        ]);
    })
        .then(info => {
            if (info[0].exists === true) {
                supUser = username;
                res.render('birdfeed', {
                    my_title: "My Bird Feed",
                    queryData: info[1]
                })
            } else if (info[0][0].exists === true) {
                supUser = username;
                res.render('birdfeed', {
                    my_title: "My Bird Feed",
                    queryData: info[1]
                })
            } else {
                res.render('signin', {
                    title: "Sign In",
                })
            }

        })
        .catch(error => {
            res.render('signin', {
                title: "Sign In",
            })
        });
});


router.get('/signin/registration', function (req, res, next) {
    res.render('registration', {title: 'Wingz Registration'});
});

router.get('/registration', function (req, res, next) {
    res.render('registration', {title: 'Wingz Registration'});
});

router.post('/registration/signup', function (req, res) {

    if (req.body.password !== req.body.cPassword) {
        res.render('registration', {
            title: 'Wingz Registration',
        });
        return;
    }

    var cipher = crypto.createCipher(algorithm, key);
    var username = '\'' + req.body.username + '\'';
    var tempPwd = req.body.password;
    console.log(tempPwd);
    var password = '\'' + cipher.update(tempPwd, 'utf8', 'hex') + cipher.final('hex') + '\'';
    var name = '\'' + req.body.name + '\'';
    var email = '\'' + req.body.email + '\'';
    var univ = '\'' + req.body.university + '\'';
    var bird = '\'' + req.body.favoriteBird + '\'';
    var signupinfo = 'insert into users(username, name, email_address, university, favorite_bird) values(' +
        username + ', ' +
        name + ', ' +
        email + ', ' +
        univ + ', ' +
        bird + '' +
        ') ON CONFLICT DO NOTHING;';
    var signuppwd = 'insert into userlogin values(' +
        username + ', ' +
        password + '' +
        ') ON CONFLICT DO NOTHING;';
    console.log(signupinfo);
    console.log(signuppwd);
    db.task('get-everything', task => {
        return task.batch([
            task.query(signupinfo),
            task.query(signuppwd)
        ]);
    })
        .then(info => {
            res.render('signin', {
                title: "Log In",
            })
        })
        .catch(error => {
            res.render('registration', {
                title: "Register",
            })
        });
});

router.get('/signin/birdFacts', function (req, res, next) {
    var get_bird_types = 'select common_name from birds;';
    db.any(get_bird_types)
        .then(function(rows){
            res.render('birdFacts', {
                title: "Bird Facts",
                username: supUser,
                birds: rows,
                bird_info: ''
            })
        })
        .catch(function (err) {
            request.flash('error',err);
            response.render('birdFacts', {
                title: 'Bird Facts',
                birds: '',
                bird_info: ''
            })
        })
});

router.get('/signin/birdFacts/birdInfo', function(req,res,next){
    var userchoice = req.query.fact_choice;
    var chosen_bird = "select * from birds where common_name ='" + userchoice + "';";
    var query = 'select common_name from birds;';


    db.task('get-everything', task => {
        return task.batch([
            task.any(query),
            task.any(chosen_bird)
        ]);
    })
        .then(info => {
            res.render('birdFacts',{
                my_title: "Bird Facts",
                birds: info[0],
                bird_info: info[1][0]
            })
        });
});

router.get('/signin/birdFeed', function (req, res, next) {
    var getSightings = 'SELECT * FROM birdsightings INNER JOIN birds ON birdsightings.bird_id = birds.bird_id INNER JOIN users ON birdsightings.username = users.username ORDER BY sighting_number LIMIT 10;';
    db.task('get-everything', task => {
        return task.batch([
            task.query(getSightings)
        ]);
    })
        .then(info => {
                res.render('birdfeed', {
                    my_title: "My Bird Feed",
                    queryData: info[0]
                })
        })
        .catch(error => {
            res.render('signin', {
                title: "Sign In",
            })
        });
});

router.get('/signin/birdReporting', function (req, res, next) {
    var getBirds = 'SELECT bird_id, common_name from birds ORDER BY common_name;';
    console.log(getBirds)
    db.task('get-everything', task => {
        return task.batch([
            task.query(getBirds),
        ]);
    })
        .then(info => {
                console.log(info);
                res.render('birdReporting', {
                    title: "Bird Reporting",
                    queryData: info[0]
                })

        })
        .catch(error => {
            res.render('signin', {
                title: "Sign In",
            })
        });
});

router.post('/signin/birdReporting', function (req, res, next) {

    var birdS = '\'' + req.body.birdSel + '\'';
    var loc = '\'' + req.body.Location + '\'';
    var user = '\'' + supUser + '\'';
    var birdSight = 'insert into birdsightings(sighting_number, username, bird_id, location) values(DEFAULT, ' +
        user + ', ' +
        birdS + ', ' +
        loc + ''+
        ') ON CONFLICT DO NOTHING;';
    var getSightings = 'SELECT * FROM birdsightings INNER JOIN birds ON birdsightings.bird_id = birds.bird_id INNER JOIN users ON birdsightings.username = users.username ORDER BY sighting_number DESC LIMIT 10;';

    console.log(birdSight)
    db.task('get-everything', task => {
        return task.batch([
            task.query(birdSight),
            task.query(getSightings)
        ]);
    })
        .then(info => {
            console.log(info);
            res.render('birdfeed', {
                title: "Bird Reporting",
                queryData: info[1]
            })

        })
        .catch(error => {
            res.render('birdReporting', {
                title: "Error",
            })
        });
});
module.exports = router;

