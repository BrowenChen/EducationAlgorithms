var lib = require('./lib');
var legend = require('./lib/legend');
var Interaction = require('./lib/interaction');
var hat = require('hat');
var rack = hat.rack();

var series = function() {
    var args = [].slice.call(arguments,0);
    for (var i = 0;i < args.length; i++) {
        var source = args[i];
        var id = rack();
        source.id = id; 
        this.buffer[id] = document.createElement('canvas');
        this.bufferctx[id] = this.buffer[id].getContext('2d');
        this.sources.push(source);
    }
};
var to = function(el) {
    // wrap canvas in a div, set this.canvas and this.ctx
    this.wrappingDivId = "_".concat(rack()).slice(0,10);
    lib.setCanvas(el,this)
    this.sources.forEach(lib.setSource.bind(this));
    this.sources.forEach(function(source) {
        var that = this;
        source.on('data',function(data) {
            that.currentdata = data;
        });
    },this);
    // this.interaction refers to the element created during new Chart
    $(this.interaction).css('position','absolute');
    this.interaction.width = el.width; 
    this.interaction.height = el.height;
    $(el).before(this.interaction);
    // wrappingDivId happens during setcanvas (TODO : correct for ref transparency)
    var interaction = new Interaction({ctx:this.interactionctx,canvas:this.interaction,sources:this.sources,color:this.color,wrappingDivId:this.wrappingDivId});
    lib.setInteraction(interaction);
    $('#'.concat(this.wrappingDivId)).mousemove(interaction.mousemove);
    $('#'.concat(this.wrappingDivId)).mouseout(interaction.stop);
};
var legendfn = function(el) {
    this.legend_el = el; 
    legendfn.clear = lib.legendClear.bind({legend_el:this.legend_el})
};
var inspect = function() {
    return this.currentdata;
};
var chart = function() {
    this.buffer = {};
    this.bufferctx = {};
    this.currentdata = undefined;
    this.sources = [];
    this.to = to;
    this.series = series;
    this.legend = legendfn;
    this.inspect = inspect;
    this.legendobj = new legend;
    this.interaction = document.createElement('canvas');
    this.interactionctx = this.interaction.getContext('2d');
    this.bgcolor = undefined;
    this.color = {grid:'#c9d6de',bg:'#FFF',xlabel:'#000',xline:'#000',ylabel:'#000',yline:'#000',interactionline:'#000',line:undefined};
    this.rendermode = "line"; // linefill, line, bar 
    
    this.custom = {boundaries : {left:undefined,right:undefined}, cropFn : undefined};
    this.pause = false;
};
exports = module.exports = chart;
