var Db = require('mongodb').Db;
var Connection = require('mongodb').Connection;
var Server = require('mongodb').Server;
var BSON = require('mongodb').BSON;
var ObjectID = require('mongodb').ObjectID;

ArticleProvider = function(host, port) {
	if (process.env.MONGOHQ_URL) { // connect to mongoHQ
    //Testing
		this.db = Db.connect('mongodb://localhost/blog');
		this.db.open(function(err, db) {
			if (err == null) {
				var regex = new RegExp("^mongo(?:db)?://(?:|([^@/]*)@)([^@/]*)(?:|/([^?]*)(?:|\\?([^?]*)))$");
				var match = process.env.MONGOHQ_URL.match(regex);
				var auth = match[1].split(':', 2);
				auth[0] = decodeURIComponent(auth[0]);
				auth[1] = decodeURIComponent(auth[1]);
				db.authenticate(auth[0], auth[1], function(err, success) {
					if (err) {
						console.error(err);
					} else {
						// worked >:O
					}
				});
			} else {
				console.error(err);
			}
		});
	}
	else {
		this.db= new Db('node-mongo-blog', new Server(host, port, {auto_reconnect: true}, {}));
		this.db.open(function(){});
	}
};


ArticleProvider.prototype.getCollection= function(callback) {
  this.db.collection('articles', function(error, article_collection) {
    if( error ) callback(error);
    else callback(null, article_collection);
  });
};

// find all blog posts in database
ArticleProvider.prototype.findAll = function(callback) {
    this.getCollection(function(error, article_collection) {
      if( error ) callback(error)
      else {
        article_collection.find().toArray(function(error, results) {
          if( error ) callback(error)
          else callback(null, results)
        });
      }
    });
};

// find a blog post in the database by its object ID
ArticleProvider.prototype.findById = function(id, callback) {
    this.getCollection(function(error, article_collection) {
      if( error ) callback(error)
      else {
        article_collection.findOne({_id: article_collection.db.bson_serializer.ObjectID.createFromHexString(id)}, function(error, result) {
          if( error ) callback(error)
          else callback(null, result)
        });
      }
    });
};

// save a (new) blog post
ArticleProvider.prototype.save = function(articles, callback) {
    this.getCollection(function(error, article_collection) {
      if( error ) callback(error)
      else {
        if( typeof(articles.length)=="undefined")
          articles = [articles];
        for( var i =0;i< articles.length;i++ ) {
          article = articles[i];
					text_to_replace = articles[i].body;
					var paragraphs = article.body.split('\r\n\r\n');
					article.body = paragraphs;
          article.created_at = new Date();
          if( article.comments === undefined ) article.comments = [];
          for(var j =0;j< article.comments.length; j++) {
            article.comments[j].created_at = new Date();
          }
        }

        article_collection.insert(articles, function() {
          callback(null, articles);
        });
      }
    });
};

// update a blog post
ArticleProvider.prototype.update = function(articleId, articles, callback) {
    this.getCollection(function(error, article_collection) {
      if( error ) callback(error);
      else {
        for( var i =0;i< articles.length;i++ ) {
          article = articles[i];
					text_to_replace = articles[i].body;
					var paragraphs = article.body.split('\r\n\r\n');
					article.body = paragraphs;
				}
	
        article_collection.update(
					{_id: article_collection.db.bson_serializer.ObjectID.createFromHexString(articleId)},
					articles,
					function(error, articles) {
						if(error) callback(error);
						else callback(null, articles)       
					});
      }
    });
};
///////////////////////////////////////////////////////////////////////////////////////////////

// remove a blog post from the database
ArticleProvider.prototype.delete = function(articleId, callback) {
	this.getCollection(function(error, article_collection) {
		if(error) callback(error);
		else {
			article_collection.remove(
				{_id: article_collection.db.bson_serializer.ObjectID.createFromHexString(articleId)},
				function(error, article){
					if(error) callback(error);
					else callback(null, article)
				});
			}
	});
};

ArticleProvider.prototype.addCommentToArticle = function(articleId, comment, callback) {
  this.getCollection(function(error, article_collection) {
    if( error ) callback( error );
    else {
      article_collection.update(
        {_id: article_collection.db.bson_serializer.ObjectID.createFromHexString(articleId)},
        {"$push": {comments: comment}},
        function(error, article){
          if( error ) callback(error);
          else callback(null, article)
        });
    }
  });
};

exports.ArticleProvider = ArticleProvider;