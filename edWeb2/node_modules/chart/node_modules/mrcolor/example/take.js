var mr = require('mrcolor');
mr.take(18).forEach(function (color) {
    console.log('rgb(' + color.rgb().join(',') + ')');
});
