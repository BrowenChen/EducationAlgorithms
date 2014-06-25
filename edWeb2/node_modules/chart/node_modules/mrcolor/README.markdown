Mr. Color
=========

I just want some colors, you know?

![mr. color](http://substack.net/images/mrcolor.png)

This module gives you some colors which are all different but they get less
different the more colors you load.

The hue is deterministic but the saturation and luminosity vary increasingly
randomly the more colors are generated.

![mrcolor browser example screenshot](https://github.com/substack/mrcolor/raw/master/example/browser/screenshot.png)

examples
========

example/take.js
---------------

````javascript
var mr = require('mrcolor');
mr.take(18).forEach(function (color) {
    console.log('rgb(' + color.rgb().join(',') + ')');
});
````

output:

    $ node example/take.js
    rgb(254,1,1)
    rgb(255,255,0)
    rgb(0,255,0)
    rgb(1,254,254)
    rgb(0,0,255)
    rgb(255,47,255)
    rgb(226,113,0)
    rgb(214,54,0)
    rgb(109,217,1)
    rgb(0,243,121)
    rgb(4,129,255)
    rgb(84,1,167)
    rgb(198,0,99)
    rgb(255,206,62)
    rgb(112,254,65)
    rgb(0,181,135)
    rgb(0,39,154)
    rgb(213,89,255)

methods
=======

var mr = require('mrcolor');

var next = mr();
----------------

Call `next()` repeatedly to get more colors.

mr.take(n)
----------

Generate `n` colors as an array.

mr.lighten(color,[by])
----------

Return a lighter version of a color.

color objects
=============

Color objects have these methods:

* rgb()
* hsl()
* hsv()
* cmyk()
* xyz()

The return values are arrays with each of the color parameters.
See [color-convert](https://github.com/harthur/color-convert) for more info.

installation
============

With [npm](http://npmjs.org), just do:

    npm install mrcolor

This module also works with
[browserify](https://github.com/substack/node-browserify).
