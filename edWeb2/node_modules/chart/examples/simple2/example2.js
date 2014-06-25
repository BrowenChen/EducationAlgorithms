var ee = require('events').EventEmitter;
var nodechart = require('../../index.js');
var datasource = new ee;
$(window).ready(function() {
    var chart = new nodechart;
    chart.series(datasource);
    chart.to(document.getElementById('mycanvas'));
    chart.color.line = ["#C45AEC","#C45AEC",undefined];
    var height = 400;
    setInterval(function() {
        var a = Math.floor(Math.random()*height);
        var b = Math.floor(Math.random()*height);
        var c = Math.floor(Math.random()*height);
        datasource.emit('data',{a:a,b:b,c:c});
    },1000);
});
