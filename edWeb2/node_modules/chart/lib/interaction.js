
var colorToString = function(colorobj) {
    var color = colorobj.rgb();
    return 'rgb('+color[0]+','+color[1]+','+color[2]+')';
};
// get left and right neighbors of x
var getNeighbors = function(x,list) {
    var left = undefined;
    var right = undefined;
    for (var i = 0; i < list.length; i++) {
        var point = list[i];
        if (point.x <= x) 
            left = list[i];
        if (point.x > x)
            right = list[i];
        if (right !== undefined) 
            break;
    }
    
    return {left:left,right:right}
};
var equationY = function(point1,point2,x) {
    var m = (point2.y - point1.y) / (point2.x - point1.x);
    return (m * (x - point1.x)) + point1.y
}
var drawVerticalLine = function(params) {
    var ctx = params.ctx;
    var color = params.color;
    ctx.lineWidth = 1;
    ctx.strokeStyle = color;
    ctx.clearRect(0,0,params.width,params.height);
    ctx.beginPath();
    var x = params.x;
    if (params.x % 1 === 0) 
        x += 0.5;
    ctx.moveTo(x,params.height);
    ctx.lineTo(x,0);
    ctx.stroke();
};
var drawIntersections = function(params) {
    var sources = params.sources;
    var ctx = params.ctx;
    var x = params.x;
    sources.forEach(function(source) {
        var datahash = source.displayData;
        if (datahash !== undefined) {
            Object.keys(datahash).forEach(function(key) {
                var val = datahash[key];
                var neighbors = getNeighbors(x,val.list);
                if ((neighbors.left !== undefined) && (neighbors.right !== undefined)) {
                    var intersectY = equationY(neighbors.left,neighbors.right,x); 
                    ctx.beginPath();
                    var color = colorToString(val.yaxis.color);
                    ctx.fillStyle = color;
                    ctx.strokeStyle = '#FFFFFF';
                    ctx.arc(x, intersectY,4, 0, Math.PI*2, false);
                    ctx.fill();
                    ctx.stroke();
                }
            });
        }
    });
};
var mousemove = function(ev) {
    this.mouseisout = false;
    var offset = $('#'.concat(this.wrappingDivId)).offset();
    var x = ev.pageX - offset.left;
    var y = ev.pageY - offset.top;
    
    if (x < this.config.axispadding.left)
        return
    
    this.lastx = x; 
    drawVerticalLine({ctx:this.ctx,height:this.canvas.height,width:this.canvas.width,x:x+0.5,color:this.color.interactionline});
    drawIntersections({ctx:this.ctx,sources:this.sources,x:x});
    this.isCleared = false;
};

var redraw = function() {
    if (this.mouseisout === true) {
        if (this.isCleared === false) {
            this.ctx.clearRect(0,0,this.canvas.width,this.canvas.height);
            this.isCleared = true;
        }
        return;
    }
    if (this.lastx !== undefined) {
        var x = this.lastx;
        drawVerticalLine({ctx:this.ctx,height:this.canvas.height,width:this.canvas.width,x:x,color:this.color.interactionline});
        drawIntersections({ctx:this.ctx,sources:this.sources,x:x});
        this.isCleared = false;
    } 
};
var stop = function() {
    this.mouseisout = true;
};

var interaction = function (params) {
    this.isCleared = false;
    this.mouseisout = false;

    this.lastx = undefined;
   
    // these are exported to this for the test scripts
    this.getNeighbors = getNeighbors;
    this.equationY = equationY;

    this.drawVerticalLine = drawVerticalLine;
    this.drawIntersections = drawIntersections;

    this.mousemove = mousemove.bind(this);

    if (params !== undefined) {
        this.ctx = params.ctx;
        this.canvas = params.canvas;    
        this.sources = params.sources;
        this.wrappingDivId = params.wrappingDivId;
    }


    this.redraw = redraw.bind(this);
    this.stop = stop.bind(this);
    this.config = undefined;
    this.color = params.color
};
exports = module.exports = interaction;
