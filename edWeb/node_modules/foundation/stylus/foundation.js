'use strict';

var path = require('path');
var packageInfo = require('../package.json');
var libPath = __dirname;
var dependencies = [require('stylus-type-utils')];

/**
 * Return the plugin callback for stylus.
 *
 * @return {Function}
 * @api public
 */
exports = module.exports = function plugin() {
  return function(stylus){
    stylus.include(libPath);
    dependencies.forEach(function(dep) {
      stylus.use(dep());
    });
  };
};


/**
 * Theme name.
 */
exports.theme = packageInfo.name;

/**
 * Library version.
 */
exports.version = packageInfo.version;

/**
 * Stylus path.
 */
exports.path = libPath;

/**
 * Dependent modules
 * 
 * @type {Array}
 */
exports.dependencies = dependencies;

/**
 * Assets paths
 * 
 * @type {Array}
 */
exports.assetPaths = [path.join(__dirname, '..', 'js')];
