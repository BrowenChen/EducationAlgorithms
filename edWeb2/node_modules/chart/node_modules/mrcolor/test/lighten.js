var assert = require('assert');

var mr = require('../');
var next = mr();
var x = next();

//console.log(x.hsl());console.log(y.hsl());console.log(z.hsl());

exports.lighten = function () {
    
    var y = mr.lighten(x);
    var z = mr.lighten(x,0.5);

    assert.equal(Math.round(y.hsl()[1]), 20);
    assert.equal(Math.round(z.hsl()[1]), 50);
};
