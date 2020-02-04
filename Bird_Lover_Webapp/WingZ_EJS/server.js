/***********************
  Load Components!

  Express      - A Node.js Framework
  Body-Parser  - A tool to help use parse the data in a post request
  Pg-Promise   - A database tool to help use connect to our PostgreSQL database
***********************/
var express = require('express'); //Ensure our express framework has been added
var app = express();
var bodyParser = require('body-parser'); //Ensure our body-parser tool has been added
app.use(bodyParser.json());              // support json encoded bodies
app.use(bodyParser.urlencoded({ extended: true })); // support encoded bodies

//Create Database Connection
var pgp = require('pg-promise')();

/**********************
  Database Connection information
  host: This defines the ip address of the server hosting our database.  We'll be using localhost and run our database on our local machine (i.e. can't be access via the Internet)
  port: This defines what port we can expect to communicate to our database.  We'll use 5432 to talk with PostgreSQL
  database: This is the name of our specific database.  From our previous lab, we created the football_db database, which holds our football data tables
  user: This should be left as postgres, the default user account created when PostgreSQL was installed
  password: This the password for accessing the database.  You'll need to set a password USING THE PSQL TERMINAL THIS IS NOT A PASSWORD FOR POSTGRES USER ACCOUNT IN LINUX!
**********************/
const dbConfig = {
	host: 'localhost',
	port: 5432,
	database: 'wingz_db',
	user: 'postgres',
	password: 'pwd'
};

var db = pgp(dbConfig);

// set the view engine to ejs
app.set('view engine', 'ejs');
app.use(express.static(__dirname + '/'));//This line is necessary for us to use relative paths and access our resources directory

app.get('/',function(req,res) {
  res.render('pages/home_page', {
    local_css: "my_style.css",
    my_title: "Home Page"
  });
});

app.get('/login', function(req,res) {
  res.render('pages/login', {
    local_css: "signin.css",
    my_title: "Login Page"
  })
});

app.get('/create_account', function(req,res) {
  res.render('pages/create_account', {
    local_css: "createaccount.css",
    my_title: "Create Account"
  });
});

app.post('/create_account', function(req, res) {
  var name = req.body.person_name;
  var separate = name.indexOf(" ");
  var first_name = name.substr(0, separate);
  var last_name = name.substr(separate);
  console.log(name);
  var email = req.body.person_email;
  var number = req.body.person_number;
  var fav = req.body.fav_bird;
  var pass = req.body.password;

  console.log(first_name);
  console.log(last_name);

  var insert_statement = "INSERT INTO accounts(first_name, last_name, phone_number, fav_bird, password, id)" +
                            "VALUES('" + first_name + "','" + last_name + "','" + number + "','" + fav + "','" 
                            + pass + "','" + 1 + "') ON CONFLICT DO NOTHING;";

  db.any(insert_statement)
    .then(function(rows){
      res.render('pages/home_page', {
        my_title: "Home Page"
      })
    })
})

app.get('/bird_posting', function(req,res) {
  
      res.render('pages/bird_posting', {
        my_title: "Registration Page",
      })
});

app.post('/bird_posting', function(req, res) {
  
  var r_date = req.body.report_date;
  var loc = req.body.bird_loc;
  var b_type = req.body.bird_type;
  var b_color = req.body.bird_color;
  var b_status = req.body.bird_status;

  var insert_statement = "INSERT INTO bird_report(report_date, location, bird_type, bird_color, bird_status) VALUES('" + r_date + "','" + loc + 
        "','" + b_type + "','" + b_color + "','" + b_status + "') ON CONFLICT DO NOTHING;";

  db.any(insert_statement)
    .then(function(rows){
      res.render('pages/bird_posting',{
        my_title: "Registration Page"
      })
    })
});

app.get('/bird_facts', function(req,res) {
  var query = 'select type from bird_facts;';
  db.any(query)
    .then(function (rows) {
      res.render('pages/bird_facts',{
        my_title: "Bird Facts",
        birds: rows,
        bird_info: '',
      })
    })
    .catch(function(err) {
      request.flash('error', err);
      response.render('pages/bird_facts', {
        title: 'Bird Facts',
        birds: ''
      })
    })
});

app.get('/bird_facts/bird_info', function(req, res) {

  var userchoice = req.query.fact_choice;

  var chosen_bird = "select * from bird_facts where type ='" + userchoice + "';";
  var query = 'select type from bird_facts;';


  db.task('get-everything', task => {
    return task.batch([
      task.any(query),
      task.any(chosen_bird)
    ]);
  })
  .then(info => {
    res.render('pages/bird_facts',{
      my_title: "Bird Facts",
      birds: info[0],
      bird_info: info[1][0]
    })
  })
});


app.listen(3000);
console.log('3000 is the magic port');