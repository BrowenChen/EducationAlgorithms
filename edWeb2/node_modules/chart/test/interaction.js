var Interaction = require('../lib/interaction');
var i = new Interaction();
var test = require('tap').test;
test('getNeighbors',function(t) {
    t.plan(5);
    var list = [{x:2,y:3},{x:3,y:9},{x:6,y:15}];
    var neighbors = i.getNeighbors(4,list);
    t.deepEquals(neighbors,{left:{x:3,y:9},right:{x:6,y:15}});
    neighbors = i.getNeighbors(1,list);
    t.deepEquals(neighbors,{left:undefined,right:{x:2,y:3}});
    neighbors = i.getNeighbors(2.5,list);
    t.deepEquals(neighbors,{left:{x:2,y:3},right:{x:3,y:9}});
    neighbors = i.getNeighbors(3,list);
    t.deepEquals(neighbors,{left:{x:3,y:9},right:{x:6,y:15}});
    neighbors = i.getNeighbors(7,list);
    t.deepEquals(neighbors,{left:{x:6,y:15},right:undefined});
});
