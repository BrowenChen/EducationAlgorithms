var mrcolor = require('mrcolor');
var Hash = require('hashish');
var hat = require('hat');
var util = require('./util');
var nextcolor = mrcolor();
var rack = hat.rack(128,10,2);

// foreach key in data add to hash axises 
// if new addition, create a color.
var colorToString = function(colorobj) {
    var color = colorobj.rgb();
    return 'rgb('+color[0]+','+color[1]+','+color[2]+')';
};
var update = function(list,linecolors) {
    var axishash = this.axishash;
    list.forEach(function(data) {
        var idx = 0;
        Hash(data)
            .filter(function(obj,key) {
                return key !== 'date'
            })
            .forEach(function(value,key) {
                if (axishash[key] === undefined) {
                    var color = undefined;
                    if ((linecolors !== undefined) && (linecolors[idx] !== undefined)) 
                        color = mrcolor.rgbhexToColorObj(linecolors[idx]);
                    else 
                        color = nextcolor();
                    idx++;
                    axishash[key] = {
                        color:color,
                        newarrival:true,
                        display:true
                    };
                } else {
                    axishash[key].newarrival = false;
                }
            })
        ;
    });
    return axishash;
};
var clear = function(legend_el) {
    this.axishash = {};
    $(legend_el).empty();   
};
var updateHTML = function(params) {
    if (params.el === undefined) {
        return;
    }
    var el = params.el;
    var axishash = this.axishash;
    Object.keys(axishash).forEach(function(axis) {
        if (axishash[axis].newarrival === true) {
            var legendlinestring = 'vertical-align:middle;display:inline-block;width:20px;border:thin solid '+colorToString(axishash[axis].color);
            var axisstring = 'padding:0;line-height:10px;font-size:10px;display:inline-block;margin-right:5px;';
            var legendid = '_'+rack(axis);
//            console.log(legendid);
            $(el)
                .append('<div class="legend" id="'+legendid+'"><input type=checkbox checked></input><div style="'+axisstring+'" class="axisname">' + axis + '</div><hr style="'+ legendlinestring+'" class="legendline" /></div>')
                .css('font-family','sans-serif');
            $('#'+legendid+' input[type="checkbox"]').click(function() {
                //
                //if ($('.legend input[type="checkbox"]:checked').length > 1) {
                    var legendname = rack.get(legendid.slice(1));
                    axishash[legendname].display = !axishash[legendname].display; // toggle boolean
                    $(this).attr('checked',axishash[legendname].display);
                    util.redraw({yaxises:axishash});  
               // }
            });
        }
    },this);
};
exports = module.exports = function() {
    this.axishash  = {};
    this.update = update;
    this.updateHTML = updateHTML;
    this.clear = clear;
};
