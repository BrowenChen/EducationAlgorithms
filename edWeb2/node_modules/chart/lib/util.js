var Hash = require('hashish');
var mr = require('mrcolor');

var getSpacing = function(windowsize,canvaswidth) {
    return Math.floor(canvaswidth / (windowsize-1));
}
exports.getSpacing = getSpacing;
exports.getStartX = function(length,windowsize,canvaswidth) {
    var x = undefined;
    var spacing = getSpacing(windowsize,canvaswidth);
    if (length <= windowsize) {
        x = canvaswidth - (spacing * (length-1));
    } else 
        x = 0;
    return x;
};
exports.cropData = function(list,windowsize) {
    if (list.length < windowsize)
        return list
    else return list.slice(list.length - windowsize)
};
var colorToString = function(colorobj,alpha) {
    var color = colorobj.rgb();
    if (alpha !== undefined)
        return 'rgba('+color[0]+','+color[1]+','+color[2]+','+alpha+')';
    else 
        return 'rgb('+color[0]+','+color[1]+','+color[2]+')';
};
exports.colorToString = colorToString;
var drawDot = function(params) {
    params.ctx.beginPath();
    params.ctx.strokeStyle = colorToString(params.color);
    params.ctx.arc(params.x, params.y, params.radius, 0, Math.PI*2, false);
    params.ctx.stroke();
};
exports.drawDot = drawDot;
exports.drawLine = function(params) {
    params.ctx.beginPath();
    params.ctx.arc(params.x, params.y, params.radius, 0, Math.PI*2, false);
    params.ctx.strokeStyle = params.color;
    params.ctx.stroke();
};
exports.drawHorizontalGrid = function(width,height,ctx,color){
    var heightchunks = Math.floor(height / 10);
    for (var i = 0; i < heightchunks; i++) {
        ctx.strokeStyle = color;
        ctx.beginPath();
        ctx.moveTo(0,i*heightchunks);
        ctx.lineTo(width,i*heightchunks);
        ctx.stroke();
    }
}
var getDateString = function(ms) {
    var date = new Date(ms);

    var pad = function(str) {
        if (str.length == 1) 
            return '0'.concat(str)
        if (str.length === 0) 
            return '00'
        else 
            return str
    };  
    var hours = date.getHours() % 12;
    if (hours === 0) 
        hours = '12';
    var seconds = pad(date.getSeconds());
    var minutes = pad(date.getMinutes());
    var meridian = date.getHours() >= 12 ? 'pm' : 'am';
    return hours +':'.concat(minutes) + ':'.concat(seconds) + meridian;
};
// if specialkey is defined, then we only look at members of list are specialkey
// i.e. list = [{foo:3,bar:9},{foo:4,bar:19}] rangeY(list,'foo'), gets range for just foo.
exports.rangeY = function(list,specialkey) {
    // % to top pad so the "peak" isn't at the top of the viewport, but we allow some extra space for better visualization
//    var padding = 0.10; // 0.10 = 10%;
    var padding = 0;

    var minY = undefined;
    var maxY = undefined;
    for (var i = 0; i < list.length; i++) {
        Hash(list[i])
            .filter(function(val,key) { 
                if (specialkey !== undefined) 
                    return (key == specialkey) 
                return (key !== 'date')
             })
            .forEach(function(val,key) {
            if (minY == undefined) 
                minY = val;
            if (maxY == undefined)
                maxY = val;
            if (val < minY)
                minY = val;
            if (val > maxY)
                maxY = val;
        });
    }
    maxY = (1 + padding)*maxY;
    var spread = undefined;
    if ((minY!== undefined) && (maxY !== undefined)) {
        spread = maxY - minY;
    }
    // shift is the amount any value in the interval needs to be shifted by to fall with the interval [0,spread]
    var shift = undefined;
    if ((minY < 0) && (maxY >= 0)) {
        shift = Math.abs(minY);
    }
    if ((minY < 0) && (maxY < 0)) {
        shift = Math.abs(maxY) + Math.abs(minY);
    }
    if (minY > 0) {
        shift = -minY;
    }
    if (minY == 0) 
        shift = 0;
    return {min:minY,max:maxY,spread:spread,shift:shift}
};
var tick = function() {
    var dash = function(ctx,x,y,offset,value,linecolor,labelcolor) {
        ctx.fillStyle = labelcolor;
        ctx.strokeStyle = linecolor;
        ctx.beginPath()
        ctx.moveTo(x-offset,y)
        ctx.lineTo(x+offset,y);
        ctx.stroke();
        ctx.fillText(value.toFixed(2),x-40,y+3);
    }
    var large = function(ctx,x,y,value,linecolor,labelcolor) {
        dash(ctx,x,y,6,value,linecolor,labelcolor);
    }
    var small = function(ctx,x,y,value,linecolor,labelcolor) {
        dash(ctx,x,y,2,value,linecolor,labelcolor);
    }
    return {
        large: large,
        small: small
    }
};
exports.drawYaxis = function(params) {
    var canvas = params.canvas;
    var ctx = params.ctx;
    var range = params.range;
    var config = params.config;
    var yline = params.yline;
    var ylabel = params.ylabel;
    
    var availableHeight = canvas.height - config.padding.top - config.padding.bottom;
    ctx.strokeStyle = yline;
    ctx.beginPath();
    ctx.moveTo(config.axispadding.left,canvas.height-config.padding.bottom);
    ctx.lineTo(config.axispadding.left,config.padding.top);
    ctx.stroke();
    var majordivisions = 4;
    var step = range.spread / majordivisions;
    for (var i = 0; i <= majordivisions; i++) {
        var ticky = (availableHeight) - ((i / majordivisions) * availableHeight);
        ticky += config.padding.top;
        var value = range.min + (i*step);
        tick().large(ctx,config.axispadding.left,ticky,value,yline,ylabel);
    }
};
exports.drawYaxisMultiple = function(canvas,ctx,yaxises) { 
    var idx = 0;
    Hash(yaxises).forEach(function(axis,key) {
        var x = 5 + (35*idx);
        ctx.fillStyle = '#FFF';
        ctx.font = '10px sans-serif';
        ctx.fillText(axis.range.min.toFixed(2),x,canvas.height);
        ctx.fillText(axis.range.max.toFixed(2),x,10);
        ctx.strokeStyle = colorToString(axis.color);
        ctx.beginPath();
        ctx.moveTo(x,canvas.height);
        ctx.lineTo(x,0);
        ctx.stroke();

        var majordivisions = 4;
        var step = axis.range.spread / majordivisions;
        for (var i = 0; i < majordivisions; i++) {
            var ticky = (canvas.height) - ((i / majordivisions) * canvas.height);
            var value = axis.range.min + (i*step);
            tick().large(ctx,x,ticky,value);
        }
        idx++;
    });
};
exports.clip = function(params) {
    var ctx = params.ctx;
    var height = params.height;
    var config = params.config;
    var clipcolor = params.clipcolor;
    if (params.type == 'clear') 
        ctx.clearRect(0,0,config.axispadding.left,height);
    if (params.type == 'fill') {
        ctx.fillStyle = clipcolor;
        ctx.fillRect(0,0,config.axispadding.left,height);
    }
};
exports.drawXaxis = function(params) {
    var datatodisplay = params.datatodisplay;
    var ctx = params.ctx;
    var spacing = params.spacing;
    var startx = params.startx;
    var height = params.height;
    var width = params.width;
    var config = params.config;
    var gridcolor = params.gridcolor;
    var xlabel = params.xlabel;
    var xline = params.xline;
    var doVertGrid = params.doVertGrid || false;
    // draw x-axis
    ctx.strokeStyle = params.xline;
    ctx.beginPath();
    ctx.moveTo(0,height - config.padding.bottom);
    ctx.lineTo(width,height - config.padding.bottom);
    ctx.stroke();
    // draw vertical grid
    if (doVertGrid === true) {
        ctx.fillStyle = xlabel;
        ctx.lineWidth = 1;
        for (var i = 0; i < datatodisplay.length;i++) {
            ctx.strokeStyle = gridcolor;
            ctx.beginPath();
            var x = startx+i*spacing;
            x += 0.5;
            ctx.moveTo(x,0);
            ctx.lineTo(x,height);
            ctx.stroke();
            var datestring = getDateString(datatodisplay[i].date);
            ctx.fillText(datestring,startx+i*spacing,height-5);
        }
    }
};
var lastsavedparams = {};
exports.getDisplayPoints = function(params) {
    var datatodisplay = params.datatodisplay;
    var startx = params.startx;
    var spacing = params.spacing;
    var height = params.height;
    var yaxises = params.yaxises;
    var range = params.range;
    var config = params.config;
    var displayPoints = {};
    Hash(yaxises)
        .filter(function(obj) {
            return (obj.display && obj.display === true)
        })
        .forEach(function(yaxis,key) {
            displayPoints[key] = {};
            displayPoints[key].yaxis = yaxis;
            displayPoints[key].list = [];
            datatodisplay.forEach(function(data,idx) {
                var yval = 0;
                var ratio = (data[key] + range.shift) / range.spread;
                var availableHeight = height - config.padding.top - config.padding.bottom;
                if (range.spread !== 0) {
                    yval = ratio * availableHeight;
                }
                var displayY = height - yval - config.padding.bottom;
                displayPoints[key].list.push({x:startx+(idx*spacing),y:displayY});
            },this);
        })
    ;
    return displayPoints;
};
// filters datatodisplay for dyanmic ranges based on legend select/deselect
exports.filterDynamicRangeY = function(datatodisplay,yaxises) {
    var filtered_list = []; // specifically for dynamic ranges
    for (var i = 0; i < datatodisplay.length; i++) {
        var item = Hash(datatodisplay[i])
        .filter(function(val,key) {
            return (key == 'date') || (yaxises[key].display == true)
        })
        .end;
        filtered_list.push(item);
    }
    var range = exports.rangeY(filtered_list);
    return range;
}
exports.draw = function (params) {
    lastsavedparams = params;
    var datatodisplay = params.datatodisplay;
    var startx = params.startx;
    var spacing = params.spacing;
    var buffer = params.buffer;
    var bufferctx = params.bufferctx;
    var yaxises = params.yaxises;
    var config = params.config;
    var rendermode = params.rendermode;

    bufferctx.clearRect(0,0,buffer.width,buffer.height);    
    
    var range = params.range;
    Hash(yaxises)
        .filter(function(obj) {
            return (obj.display && obj.display === true)
        })
        .forEach(function(yaxis,key) {
            // draw lines
            bufferctx.strokeStyle = colorToString(yaxis.color);
            bufferctx.fillStyle = colorToString(mr.lighten(yaxis.color),0.5);
            datatodisplay.forEach(function(data,idx) {
                var yval = 0;
                var ratio = (data[key] + range.shift) / range.spread;
                var availableHeight = buffer.height - config.padding.top - config.padding.bottom;
                if (range.spread !== 0) {
                    yval = ratio * availableHeight;
                }
                var displayY = buffer.height - yval - config.padding.bottom;

                if (rendermode == 'line' || rendermode == 'linefill') {
                    if (idx === 0) {
                        bufferctx.beginPath();
                        bufferctx.moveTo(startx+idx*spacing,displayY);
                    } else {
                        bufferctx.lineTo(startx+(idx*spacing),displayY);
                    }
                    if (idx == (datatodisplay.length -1)) {
                        if (rendermode == 'linefill') {
                            bufferctx.lineTo(startx+(idx*spacing),buffer.height-config.padding.bottom);
                            bufferctx.lineTo(startx,buffer.height-config.padding.bottom);
                            bufferctx.fill();
                        }
                        bufferctx.stroke();
                    }
                }
                if (rendermode == 'bar') {
                    bufferctx.beginPath();
                    var centerx = startx + idx*spacing;
                    bufferctx.moveTo(centerx-10,displayY);
                    bufferctx.lineTo(centerx+10,displayY);
                    bufferctx.lineTo(centerx+10,buffer.height-config.padding.bottom);
                    bufferctx.lineTo(centerx-10,buffer.height-config.padding.bottom);
                    bufferctx.lineTo(centerx-10,displayY);
                    bufferctx.stroke();
                    bufferctx.fill();
                }
            },this); 
            // draw dots
            datatodisplay.forEach(function(data,idx) {
                var yval = 0;
                var ratio = (data[key] + range.shift) / range.spread;
                var availableHeight = buffer.height - config.padding.top - config.padding.bottom;
                if (range.spread !== 0) {
                    yval = ratio * availableHeight;
                }
                var displayY = buffer.height - yval - config.padding.bottom;
                drawDot({
                    x:startx+(idx*spacing),
                    y:displayY, 
                    radius:3,
                    ctx:bufferctx,
                    color:yaxis.color
                });
            },this);
        })
    ;
};
exports.redraw = function(params) {
    lastsavedparams.yaxises = params.yaxises;
    exports.draw(lastsavedparams);
};



// completely parallel implementation for multiple y-axises.
// diff log
// changed functions/variables to _multiple
// commented out portions of code are there to indicate the strikethrus from the single axis

var lastsavedparams_multiple = {};
exports.draw_multiple = function (params) {
    lastsavedparams_multiple = params;
    var datatodisplay = params.datatodisplay;
    var startx = params.startx;
    var spacing = params.spacing;
    var buffer = params.buffer;
    var bufferctx = params.bufferctx;
    var yaxises = params.yaxises;

    bufferctx.clearRect(0,0,buffer.width,buffer.height);    

// commmented out because range now comes on the axis
//    var range = exports.rangeY(datatodisplay);
    Hash(yaxises)
        .filter(function(obj) {
            return (obj.display && obj.display === true)
        })
        .forEach(function(yaxis,key) {
            // draw lines
            bufferctx.strokeStyle = colorToString(yaxis.color);
            datatodisplay.forEach(function(data,idx) {
                var yval = 0;
//                var ratio = (data[key] + range.shift) / range.spread;
                var ratio = (data[key] + yaxis.range.shift) / yaxis.range.spread;
                if (yaxis.range.spread !== 0) {
                    yval = ratio * buffer.height;
                }
                if (idx === 0) {
                    bufferctx.beginPath();
                    bufferctx.moveTo(startx+idx*spacing,buffer.height - yval);
                } else {
                    bufferctx.lineTo(startx+(idx*spacing),buffer.height - yval);
                }
                if (idx == (datatodisplay.length -1)) {
                    bufferctx.stroke();
                }
            },this); 
            // draw dots
            datatodisplay.forEach(function(data,idx) {
                var yval = 0;
                if (yaxis.range.spread !== 0) {
                    yval = ((data[key] + yaxis.range.shift) / yaxis.range.spread) * buffer.height;
                }
                drawDot({
                    x:startx+(idx*spacing),
                    y:buffer.height - yval, 
                    radius:3,
                    ctx:bufferctx,
                    color:yaxis.color
                });
            },this);
        })
    ;
};
exports.redraw_multiple = function(params) {
    lastsavedparams_multiple.yaxises = params.yaxises;
    exports.draw_multiple(lastsavedparams_multiple);
};
