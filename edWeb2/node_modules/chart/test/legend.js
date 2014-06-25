var lib = require('../lib/legend')();
var test = require('tap').test;
// test that keys are merged correctly
test('legend',function(t) {
    t.plan(1);
    var x1 = {a:1,b:2};
    var x2 = {c:3};
    var y = [x1,x2];
    var legend = {};
    legend = lib.update(y);
    var keys = Object.keys(legend);
    t.deepEqual(keys,['a','b','c']); 
});
