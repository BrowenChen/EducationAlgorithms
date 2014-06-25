var ee = require('events').EventEmitter;
var nodechart = require('../../index.js');
var datasource = new ee;
$(window).ready(function() {
    var myform = $('#myform');
    myform.change(function(ev) {
        var rendermode = ev.target.value;
        datasource.rendermode = rendermode;
    }); 
    var chart = new nodechart;
    datasource.rendermode = "line";
    chart.series(datasource);
    chart.to(document.getElementById('mycanvas'));
    var up = false;
    setInterval(function() {
        var val = Math.floor(Math.random()*100);
        datasource.emit('data',{y:val});
/*
        if (!up) {
            datasource.emit('data',{y:0});
            up = true;
        } else {
            datasource.emit('data',{y:100});
            up = false;
        }
*/
    },2500);
});
