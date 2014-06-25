var lib = require('../lib/util');
var test = require('tap').test;
test('rangeY',function(t) {
    t.plan(1);
    var list = [{a:3,b:9},{a:-2,b:5}];
    var range = lib.rangeY(list);
    t.deepEqual(range,{min:-2,max:9,spread:11,shift:2}); 
});
test('cropdata',function(t) {
    t.plan(3);
    var x = [1,2,3,4,5,6];
    var a = lib.cropData(x,2);
    t.deepEqual(a,[5,6]);
    var b = lib.cropData(x,6);
    t.deepEqual(b,[1,2,3,4,5,6]);
    var c = lib.cropData(x,7);
    t.deepEqual(c,[1,2,3,4,5,6]);
});

test('spacing',function(t) {
    t.plan(1);
    var spacing = lib.getSpacing(5,200);
    t.equal(spacing,50);
});
test('startx',function(t) {
    t.plan(4);
    var spacing = lib.getSpacing(5,200);
    var startx = lib.getStartX(2,5,200);
    t.equal(startx,150);
    startx = lib.getStartX(1,5,200);
    t.equal(startx,200);
    startx = lib.getStartX(4,5,200);
    t.equal(startx,50);
    startx = lib.getStartX(5,5,200);
    t.equal(startx,0);
});
