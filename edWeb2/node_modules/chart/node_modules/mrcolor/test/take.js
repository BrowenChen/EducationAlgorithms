var assert = require('assert');
var mr = require('../');

exports.take6 = function () {
    var xs = mr.take(6);
    assert.equal(xs.length, 6);
    for (var i = 0; i < xs.length - 1; i++) {
        assert.equal(xs[i+1].hsl()[0] - xs[i].hsl()[0], 60);
    }
};

exports.take24 = function () {
    var xs = mr.take(24);
    assert.equal(xs.length, 24);
    for (var i = 0; i < 5; i++) {
        assert.equal(xs[i+1].hsl()[0] - xs[i].hsl()[0], 60);
    }
    
    for (var i = 6; i < 11; i++) {
        var dx = Math.abs(xs[i+1].hsl()[0] - xs[i].hsl()[0]);
        assert.ok(dx >= 15);
    }
    
    for (var i = 12; i < 17; i++) {
        var dx = xs[i+1].hsl()[0] - xs[i].hsl()[0];
        if (dx < 0) dx = 360 - dx;
        assert.ok(dx >= 15);
    }
    
    for (var i = 18; i < 23; i++) {
        var dx = xs[i+1].hsl()[0] - xs[i].hsl()[0];
        if (dx < 0) dx = 360 - dx;
        assert.ok(dx >= 7);
    }
};
