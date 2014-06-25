var express = require('express');
var app = express.createServer();
app.use(express.static(__dirname));

var browserify = require('browserify');
var bundle = browserify({
    entry : __dirname + '/main.js',
    watch : true
});
app.use(bundle);

app.get('/', function (req, res) {
    res.render('index.jade', {
        modified : bundle.modified,
        layout : false
    });
});

app.listen(8080);
console.log('Listening on :8080');
