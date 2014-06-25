var ee = require('events').EventEmitter;
var nodechart = require('../../index.js');
var datasource = new ee;
var datasource2 = new ee;
$(window).ready(function() {
    var chart = new nodechart;
    chart.series(datasource);
    chart.legend(document.getElementById('mylegend'));
    chart.to(document.getElementById('mycanvas'));

    var chart2 = new nodechart;
    chart2.series(datasource2);
    chart2.legend(document.getElementById('mylegend2'));
    chart2.to(document.getElementById('mycanvas2'));

    setInterval(function() {
        var a = Math.floor(Math.random()*30);
        var b = Math.floor(Math.random()*1000);
        var c = Math.floor(Math.random()*200);
        datasource.emit('data',{'php value':a,'nodejs value':b, 'fortran value':c});
        datasource2.emit('data',{'amd':a,'boston':b, 'california':c});
    },1000);

});
