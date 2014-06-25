var mr = require('mrcolor');
var $ = require('jquery-browserify');

$(window).ready(function () {
    var next = mr();
    
    $('#clicky').click(function () {
        var color = next();
        var rgb = color.rgb();
        var rgbStr = 'rgb(' + rgb.join(',') + ')';
        
        $('<div>')
            .css({
                'background-color' : rgbStr,
                color : (rgb[0] + rgb[1] + rgb[2]) / 3 > 128
                    ? 'black' : 'white',
                width : 500,
                height : 30
            })
            .text(rgbStr)
            .appendTo($('#colors'))
        ;
    });
});
