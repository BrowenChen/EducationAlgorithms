    
/**
 * Module dependencies
 */
 //Mongoose setup
require('./db');

var express = require('express'),
  bodyParser = require('body-parser'),
  methodOverride = require('method-override'),
  errorHandler = require('error-handler'),
  morgan = require('morgan'),
  routes = require('./routes'),
  api = require('./routes/api'),
  http = require('http'),
  mongoose = require('mongoose'),
  path = require('path');


var Schema = mongoose.Schema;

var app = module.exports = express();


/**
 * Configuration
 */


// define model =================

var siteSchema = new Schema({
    text: String,
    nextDate: Number,
    prevDate: Number,
    interval: Number,
    reps: Number,
    EF: Number,
    thisDate: Number

});

var Site = mongoose.model('Site', siteSchema);



var Todo = mongoose.model('Todo', {
    text : String

});


 //Setting up the database
 //Local host
 mongoose.connect('mongodb://localhost/adaptiveAlgorithms')
// mongoose.connect('mongodb://node:node@mongo.onmodulus.net:27017/uwO3mypu'); 



// all environments
app.set('port', process.env.PORT || 3000);
app.set('views', __dirname + '/views');
app.set('view engine', 'jade');
app.use(morgan('dev'));
app.use(bodyParser());
app.use(methodOverride());
app.use(express.static(path.join(__dirname, 'public')));



var env = process.env.NODE_ENV || 'development';

/**
 * Routes
 */

// serve index and view partials
app.get('/', routes.index);
app.get('/partials/:name', routes.partials);

// JSON API
app.get('/api/name', api.name);




// api ---------------------------------------------------------------------

// get all websites
app.get('/api/site', function(req, res) {
    // use mongoose to get all todos in the database
    Site.find(function(err, sites) {
        // if there is an error retrieving, send the error. nothing after res.send(err) will execute
        if (err)
            res.send(err)
        res.json(sites); // return all todos in JSON format
    });
});

// create site and send back all site after creation
app.post('/api/site', function(req, res) {
    // create a todo, information comes from AJAX request from Angular
    Site.create({
        text : req.body.text,
        done : false
    }, function(err, site) {
        if (err)
            res.send(err);
        // get and return all the todos after you create another
        Site.find(function(err, sites) {
            if (err)
                res.send(err)
            res.json(sites);
        });
    });

});

// delete a site
app.delete('/api/site/:site_id', function(req, res) {
    Site.remove({
        _id : req.params.site_id
    }, function(err, site) {
        if (err)
            res.send(err);

        // get and return all the sites after you create another
        Site.find(function(err, sites) {
            if (err)
                res.send(err)
            res.json(sites);
        });
    });
});


// ========================

// get all todos
app.get('/api/todo', function(req, res) {
    // use mongoose to get all todos in the database
    Todo.find(function(err, todos) {
        // if there is an error retrieving, send the error. nothing after res.send(err) will execute
        if (err)
            res.send(err)
        res.json(todos); // return all todos in JSON format
    });
});

// create todo and send back all todos after creation
app.post('/api/todo', function(req, res) {
    // create a todo, information comes from AJAX request from Angular
    Todo.create({
        text : req.body.text,
        done : false
    }, function(err, todo) {
        if (err)
            res.send(err);
        // get and return all the todos after you create another
        Todo.find(function(err, todos) {
            if (err)
                res.send(err)
            res.json(todos);
        });
    });

});

// delete a todo
app.delete('/api/todo/:todo_id', function(req, res) {
    Todo.remove({
        _id : req.params.todo_id
    }, function(err, todo) {
        if (err)
            res.send(err);

        // get and return all the todos after you create another
        Todo.find(function(err, todos) {
            if (err)
                res.send(err)
            res.json(todos);
        });
    });
});


// redirect all others to the index (HTML5 history)
app.get('*', routes.index);


/// error handlers

// development error handler
// will print stacktrace
if (app.get('env') === 'development') {
    app.use(function(err, req, res, next) {
        res.status(err.status || 500);
        res.render('error', {
            message: err.message,
            error: err
        });
    });
}

// production error handler
// no stacktraces leaked to user
app.use(function(err, req, res, next) {
    res.status(err.status || 500);
    res.render('error', {
        message: err.message,
        error: {}
    });
});


/**
 * Start Server
 */

http.createServer(app).listen(app.get('port'), function () {
  console.log('Express server listening on port ' + app.get('port'));
});
