var express = require('express');
//Express router. 
var router = express.Router();

/* GET home page. */
router.get('/', function(req, res) {
  res.render('index', { title: 'Express' });
});

/* Progress page route. http://localhost:8080/about)*/
router.get('/progress', function(req, res){
	res.render('progress', { title: 'Progress'});
})



module.exports = router;
