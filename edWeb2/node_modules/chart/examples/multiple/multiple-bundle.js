(function(){var require = function (file, cwd) {
    var resolved = require.resolve(file, cwd || '/');
    var mod = require.modules[resolved];
    if (!mod) throw new Error(
        'Failed to resolve module ' + file + ', tried ' + resolved
    );
    var cached = require.cache[resolved];
    var res = cached? cached.exports : mod();
    return res;
};

require.paths = [];
require.modules = {};
require.cache = {};
require.extensions = [".js",".coffee"];

require._core = {
    'assert': true,
    'events': true,
    'fs': true,
    'path': true,
    'vm': true
};

require.resolve = (function () {
    return function (x, cwd) {
        if (!cwd) cwd = '/';
        
        if (require._core[x]) return x;
        var path = require.modules.path();
        cwd = path.resolve('/', cwd);
        var y = cwd || '/';
        
        if (x.match(/^(?:\.\.?\/|\/)/)) {
            var m = loadAsFileSync(path.resolve(y, x))
                || loadAsDirectorySync(path.resolve(y, x));
            if (m) return m;
        }
        
        var n = loadNodeModulesSync(x, y);
        if (n) return n;
        
        throw new Error("Cannot find module '" + x + "'");
        
        function loadAsFileSync (x) {
            x = path.normalize(x);
            if (require.modules[x]) {
                return x;
            }
            
            for (var i = 0; i < require.extensions.length; i++) {
                var ext = require.extensions[i];
                if (require.modules[x + ext]) return x + ext;
            }
        }
        
        function loadAsDirectorySync (x) {
            x = x.replace(/\/+$/, '');
            var pkgfile = path.normalize(x + '/package.json');
            if (require.modules[pkgfile]) {
                var pkg = require.modules[pkgfile]();
                var b = pkg.browserify;
                if (typeof b === 'object' && b.main) {
                    var m = loadAsFileSync(path.resolve(x, b.main));
                    if (m) return m;
                }
                else if (typeof b === 'string') {
                    var m = loadAsFileSync(path.resolve(x, b));
                    if (m) return m;
                }
                else if (pkg.main) {
                    var m = loadAsFileSync(path.resolve(x, pkg.main));
                    if (m) return m;
                }
            }
            
            return loadAsFileSync(x + '/index');
        }
        
        function loadNodeModulesSync (x, start) {
            var dirs = nodeModulesPathsSync(start);
            for (var i = 0; i < dirs.length; i++) {
                var dir = dirs[i];
                var m = loadAsFileSync(dir + '/' + x);
                if (m) return m;
                var n = loadAsDirectorySync(dir + '/' + x);
                if (n) return n;
            }
            
            var m = loadAsFileSync(x);
            if (m) return m;
        }
        
        function nodeModulesPathsSync (start) {
            var parts;
            if (start === '/') parts = [ '' ];
            else parts = path.normalize(start).split('/');
            
            var dirs = [];
            for (var i = parts.length - 1; i >= 0; i--) {
                if (parts[i] === 'node_modules') continue;
                var dir = parts.slice(0, i + 1).join('/') + '/node_modules';
                dirs.push(dir);
            }
            
            return dirs;
        }
    };
})();

require.alias = function (from, to) {
    var path = require.modules.path();
    var res = null;
    try {
        res = require.resolve(from + '/package.json', '/');
    }
    catch (err) {
        res = require.resolve(from, '/');
    }
    var basedir = path.dirname(res);
    
    var keys = (Object.keys || function (obj) {
        var res = [];
        for (var key in obj) res.push(key);
        return res;
    })(require.modules);
    
    for (var i = 0; i < keys.length; i++) {
        var key = keys[i];
        if (key.slice(0, basedir.length + 1) === basedir + '/') {
            var f = key.slice(basedir.length);
            require.modules[to + f] = require.modules[basedir + f];
        }
        else if (key === basedir) {
            require.modules[to] = require.modules[basedir];
        }
    }
};

(function () {
    var process = {};
    
    require.define = function (filename, fn) {
        if (require.modules.__browserify_process) {
            process = require.modules.__browserify_process();
        }
        
        var dirname = require._core[filename]
            ? ''
            : require.modules.path().dirname(filename)
        ;
        
        var require_ = function (file) {
            var requiredModule = require(file, dirname);
            var cached = require.cache[require.resolve(file, dirname)];

            if (cached && cached.parent === null) {
                cached.parent = module_;
            }

            return requiredModule;
        };
        require_.resolve = function (name) {
            return require.resolve(name, dirname);
        };
        require_.modules = require.modules;
        require_.define = require.define;
        require_.cache = require.cache;
        var module_ = {
            id : filename,
            filename: filename,
            exports : {},
            loaded : false,
            parent: null
        };
        
        require.modules[filename] = function () {
            require.cache[filename] = module_;
            fn.call(
                module_.exports,
                require_,
                module_,
                module_.exports,
                dirname,
                filename,
                process
            );
            module_.loaded = true;
            return module_.exports;
        };
    };
})();


require.define("path",function(require,module,exports,__dirname,__filename,process){function filter (xs, fn) {
    var res = [];
    for (var i = 0; i < xs.length; i++) {
        if (fn(xs[i], i, xs)) res.push(xs[i]);
    }
    return res;
}

// resolves . and .. elements in a path array with directory names there
// must be no slashes, empty elements, or device names (c:\) in the array
// (so also no leading and trailing slashes - it does not distinguish
// relative and absolute paths)
function normalizeArray(parts, allowAboveRoot) {
  // if the path tries to go above the root, `up` ends up > 0
  var up = 0;
  for (var i = parts.length; i >= 0; i--) {
    var last = parts[i];
    if (last == '.') {
      parts.splice(i, 1);
    } else if (last === '..') {
      parts.splice(i, 1);
      up++;
    } else if (up) {
      parts.splice(i, 1);
      up--;
    }
  }

  // if the path is allowed to go above the root, restore leading ..s
  if (allowAboveRoot) {
    for (; up--; up) {
      parts.unshift('..');
    }
  }

  return parts;
}

// Regex to split a filename into [*, dir, basename, ext]
// posix version
var splitPathRe = /^(.+\/(?!$)|\/)?((?:.+?)?(\.[^.]*)?)$/;

// path.resolve([from ...], to)
// posix version
exports.resolve = function() {
var resolvedPath = '',
    resolvedAbsolute = false;

for (var i = arguments.length; i >= -1 && !resolvedAbsolute; i--) {
  var path = (i >= 0)
      ? arguments[i]
      : process.cwd();

  // Skip empty and invalid entries
  if (typeof path !== 'string' || !path) {
    continue;
  }

  resolvedPath = path + '/' + resolvedPath;
  resolvedAbsolute = path.charAt(0) === '/';
}

// At this point the path should be resolved to a full absolute path, but
// handle relative paths to be safe (might happen when process.cwd() fails)

// Normalize the path
resolvedPath = normalizeArray(filter(resolvedPath.split('/'), function(p) {
    return !!p;
  }), !resolvedAbsolute).join('/');

  return ((resolvedAbsolute ? '/' : '') + resolvedPath) || '.';
};

// path.normalize(path)
// posix version
exports.normalize = function(path) {
var isAbsolute = path.charAt(0) === '/',
    trailingSlash = path.slice(-1) === '/';

// Normalize the path
path = normalizeArray(filter(path.split('/'), function(p) {
    return !!p;
  }), !isAbsolute).join('/');

  if (!path && !isAbsolute) {
    path = '.';
  }
  if (path && trailingSlash) {
    path += '/';
  }
  
  return (isAbsolute ? '/' : '') + path;
};


// posix version
exports.join = function() {
  var paths = Array.prototype.slice.call(arguments, 0);
  return exports.normalize(filter(paths, function(p, index) {
    return p && typeof p === 'string';
  }).join('/'));
};


exports.dirname = function(path) {
  var dir = splitPathRe.exec(path)[1] || '';
  var isWindows = false;
  if (!dir) {
    // No dirname
    return '.';
  } else if (dir.length === 1 ||
      (isWindows && dir.length <= 3 && dir.charAt(1) === ':')) {
    // It is just a slash or a drive letter with a slash
    return dir;
  } else {
    // It is a full dirname, strip trailing slash
    return dir.substring(0, dir.length - 1);
  }
};


exports.basename = function(path, ext) {
  var f = splitPathRe.exec(path)[2] || '';
  // TODO: make this comparison case-insensitive on windows?
  if (ext && f.substr(-1 * ext.length) === ext) {
    f = f.substr(0, f.length - ext.length);
  }
  return f;
};


exports.extname = function(path) {
  return splitPathRe.exec(path)[3] || '';
};
});

require.define("__browserify_process",function(require,module,exports,__dirname,__filename,process){var process = module.exports = {};

process.nextTick = (function () {
    var queue = [];
    var canPost = typeof window !== 'undefined'
        && window.postMessage && window.addEventListener
    ;
    
    if (canPost) {
        window.addEventListener('message', function (ev) {
            if (ev.source === window && ev.data === 'browserify-tick') {
                ev.stopPropagation();
                if (queue.length > 0) {
                    var fn = queue.shift();
                    fn();
                }
            }
        }, true);
    }
    
    return function (fn) {
        if (canPost) {
            queue.push(fn);
            window.postMessage('browserify-tick', '*');
        }
        else setTimeout(fn, 0);
    };
})();

process.title = 'browser';
process.browser = true;
process.env = {};
process.argv = [];

process.binding = function (name) {
    if (name === 'evals') return (require)('vm')
    else throw new Error('No such module. (Possibly not yet loaded)')
};

(function () {
    var cwd = '/';
    var path;
    process.cwd = function () { return cwd };
    process.chdir = function (dir) {
        if (!path) path = require('path');
        cwd = path.resolve(dir, cwd);
    };
})();
});

require.define("vm",function(require,module,exports,__dirname,__filename,process){module.exports = require("vm-browserify")});

require.define("/node_modules/vm-browserify/package.json",function(require,module,exports,__dirname,__filename,process){module.exports = {"main":"index.js"}});

require.define("/node_modules/vm-browserify/index.js",function(require,module,exports,__dirname,__filename,process){var Object_keys = function (obj) {
    if (Object.keys) return Object.keys(obj)
    else {
        var res = [];
        for (var key in obj) res.push(key)
        return res;
    }
};

var forEach = function (xs, fn) {
    if (xs.forEach) return xs.forEach(fn)
    else for (var i = 0; i < xs.length; i++) {
        fn(xs[i], i, xs);
    }
};

var Script = exports.Script = function NodeScript (code) {
    if (!(this instanceof Script)) return new Script(code);
    this.code = code;
};

Script.prototype.runInNewContext = function (context) {
    if (!context) context = {};
    
    var iframe = document.createElement('iframe');
    if (!iframe.style) iframe.style = {};
    iframe.style.display = 'none';
    
    document.body.appendChild(iframe);
    
    var win = iframe.contentWindow;
    
    forEach(Object_keys(context), function (key) {
        win[key] = context[key];
    });
     
    if (!win.eval && win.execScript) {
        // win.eval() magically appears when this is called in IE:
        win.execScript('null');
    }
    
    var res = win.eval(this.code);
    
    forEach(Object_keys(win), function (key) {
        context[key] = win[key];
    });
    
    document.body.removeChild(iframe);
    
    return res;
};

Script.prototype.runInThisContext = function () {
    return eval(this.code); // maybe...
};

Script.prototype.runInContext = function (context) {
    // seems to be just runInNewContext on magical context objects which are
    // otherwise indistinguishable from objects except plain old objects
    // for the parameter segfaults node
    return this.runInNewContext(context);
};

forEach(Object_keys(Script.prototype), function (name) {
    exports[name] = Script[name] = function (code) {
        var s = Script(code);
        return s[name].apply(s, [].slice.call(arguments, 1));
    };
});

exports.createScript = function (code) {
    return exports.Script(code);
};

exports.createContext = Script.createContext = function (context) {
    // not really sure what this one does
    // seems to just make a shallow copy
    var copy = {};
    if(typeof context === 'object') {
        forEach(Object_keys(context), function (key) {
            copy[key] = context[key];
        });
    }
    return copy;
};
});

require.define("events",function(require,module,exports,__dirname,__filename,process){if (!process.EventEmitter) process.EventEmitter = function () {};

var EventEmitter = exports.EventEmitter = process.EventEmitter;
var isArray = typeof Array.isArray === 'function'
    ? Array.isArray
    : function (xs) {
        return Object.prototype.toString.call(xs) === '[object Array]'
    }
;

// By default EventEmitters will print a warning if more than
// 10 listeners are added to it. This is a useful default which
// helps finding memory leaks.
//
// Obviously not all Emitters should be limited to 10. This function allows
// that to be increased. Set to zero for unlimited.
var defaultMaxListeners = 10;
EventEmitter.prototype.setMaxListeners = function(n) {
  if (!this._events) this._events = {};
  this._events.maxListeners = n;
};


EventEmitter.prototype.emit = function(type) {
  // If there is no 'error' event listener then throw.
  if (type === 'error') {
    if (!this._events || !this._events.error ||
        (isArray(this._events.error) && !this._events.error.length))
    {
      if (arguments[1] instanceof Error) {
        throw arguments[1]; // Unhandled 'error' event
      } else {
        throw new Error("Uncaught, unspecified 'error' event.");
      }
      return false;
    }
  }

  if (!this._events) return false;
  var handler = this._events[type];
  if (!handler) return false;

  if (typeof handler == 'function') {
    switch (arguments.length) {
      // fast cases
      case 1:
        handler.call(this);
        break;
      case 2:
        handler.call(this, arguments[1]);
        break;
      case 3:
        handler.call(this, arguments[1], arguments[2]);
        break;
      // slower
      default:
        var args = Array.prototype.slice.call(arguments, 1);
        handler.apply(this, args);
    }
    return true;

  } else if (isArray(handler)) {
    var args = Array.prototype.slice.call(arguments, 1);

    var listeners = handler.slice();
    for (var i = 0, l = listeners.length; i < l; i++) {
      listeners[i].apply(this, args);
    }
    return true;

  } else {
    return false;
  }
};

// EventEmitter is defined in src/node_events.cc
// EventEmitter.prototype.emit() is also defined there.
EventEmitter.prototype.addListener = function(type, listener) {
  if ('function' !== typeof listener) {
    throw new Error('addListener only takes instances of Function');
  }

  if (!this._events) this._events = {};

  // To avoid recursion in the case that type == "newListeners"! Before
  // adding it to the listeners, first emit "newListeners".
  this.emit('newListener', type, listener);

  if (!this._events[type]) {
    // Optimize the case of one listener. Don't need the extra array object.
    this._events[type] = listener;
  } else if (isArray(this._events[type])) {

    // Check for listener leak
    if (!this._events[type].warned) {
      var m;
      if (this._events.maxListeners !== undefined) {
        m = this._events.maxListeners;
      } else {
        m = defaultMaxListeners;
      }

      if (m && m > 0 && this._events[type].length > m) {
        this._events[type].warned = true;
        console.error('(node) warning: possible EventEmitter memory ' +
                      'leak detected. %d listeners added. ' +
                      'Use emitter.setMaxListeners() to increase limit.',
                      this._events[type].length);
        console.trace();
      }
    }

    // If we've already got an array, just append.
    this._events[type].push(listener);
  } else {
    // Adding the second element, need to change to array.
    this._events[type] = [this._events[type], listener];
  }

  return this;
};

EventEmitter.prototype.on = EventEmitter.prototype.addListener;

EventEmitter.prototype.once = function(type, listener) {
  var self = this;
  self.on(type, function g() {
    self.removeListener(type, g);
    listener.apply(this, arguments);
  });

  return this;
};

EventEmitter.prototype.removeListener = function(type, listener) {
  if ('function' !== typeof listener) {
    throw new Error('removeListener only takes instances of Function');
  }

  // does not use listeners(), so no side effect of creating _events[type]
  if (!this._events || !this._events[type]) return this;

  var list = this._events[type];

  if (isArray(list)) {
    var i = list.indexOf(listener);
    if (i < 0) return this;
    list.splice(i, 1);
    if (list.length == 0)
      delete this._events[type];
  } else if (this._events[type] === listener) {
    delete this._events[type];
  }

  return this;
};

EventEmitter.prototype.removeAllListeners = function(type) {
  // does not use listeners(), so no side effect of creating _events[type]
  if (type && this._events && this._events[type]) this._events[type] = null;
  return this;
};

EventEmitter.prototype.listeners = function(type) {
  if (!this._events) this._events = {};
  if (!this._events[type]) this._events[type] = [];
  if (!isArray(this._events[type])) {
    this._events[type] = [this._events[type]];
  }
  return this._events[type];
};
});

require.define("/package.json",function(require,module,exports,__dirname,__filename,process){module.exports = {"main":"index.js"}});

require.define("/index.js",function(require,module,exports,__dirname,__filename,process){var lib = require('./lib');
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
});

require.define("/lib/index.js",function(require,module,exports,__dirname,__filename,process){var util = require('./util');
var Hash = require('hashish');
var interaction = undefined;

var config = {
    padding : {
        left : 10,
        top : 20,
        bottom : 30
    },
    axispadding : {
        left : 50,  // yaxis
        bottom : 20 // xaxis
    }
};
exports.displayConfig = function(params) {
    if (params !== undefined) {
        Hash(config).update(params);
    }
};
exports.setInteraction = function(obj) {
    interaction = obj;
    interaction.config = config;
}
exports.setCanvas = function(el,that) {
    that.canvas = el;
    // transfer inline style to wrapping div
    var style = $(el).attr('style');
    var wrappingDiv = document.createElement('div');
    $(wrappingDiv).attr('style',style);
    $(el).removeAttr('style');

    wrappingDiv.id = that.wrappingDivId;
    wrappingDiv.height = that.canvas.height;
    $(that.canvas).wrap(wrappingDiv);
    that.ctx = el.getContext('2d');
    that.ctx.fillStyle = that.color.bg;
    that.ctx.fillRect(0,0,that.canvas.width,that.canvas.height);
};
exports.legendClear = function() {
    legend.clear(this.legend_el);
};
exports.setSource = function(source) {
    var id = source.id;    
    this.buffer[id].width = this.canvas.width;
    this.buffer[id].height = this.canvas.height;
    $(this.buffer[id]).css('position','absolute');
    $(this.canvas).before(this.buffer[id]);
    var onDataGraph = function(data,flags) {
        // timestamp
        data.date = new Date().getTime(); // actual timestamp

        if ((source.dataset === undefined) || (flags && (flags.multiple == true) && (flags.clear && flags.clear == true))) {
            source.dataset = [];
            
        }
        source.dataset.push(data); 
        if (this.pause === true) 
            return

        var windowsize = source.windowsize || data.windowsize || 10;
        var datatodisplay = (this.custom.cropFn) ? this.custom.cropFn(source.dataset,windowsize,this.custom.boundaries) : util.cropData(source.dataset,windowsize);
        var startx = util.getStartX(datatodisplay.length,windowsize,this.canvas.width); 
        var spacing = util.getSpacing(windowsize,this.canvas.width);

        var yaxises = this.legendobj.update(datatodisplay,this.color.line);
        if (this.legend_el !== undefined) 
            this.legendobj.updateHTML({el:this.legend_el});

        this.ctx.fillStyle = this.color.bg;
        this.ctx.fillRect(0,0,this.canvas.width,this.canvas.height);    

        if (flags && flags.multiple && (flags.multiple === true)) {
            Hash(yaxises).forEach(function(axis,key) {
                axis.range = util.rangeY(datatodisplay,key); 
            });    
            util.drawYaxisMultiple(this.canvas,this.ctx,yaxises,config);
//            util.drawHorizontalGrid(this.canvas.width,this.canvas.height,this.ctx);
//            util.drawVerticalGrid(datatodisplay,this.ctx,spacing,startx,this.canvas.height);
            
            util.draw_multiple({startx:startx,datatodisplay:datatodisplay,spacing:spacing,buffer:this.buffer[id],bufferctx:this.bufferctx[id],yaxises:yaxises});
        } else {
            var range = util.filterDynamicRangeY(datatodisplay,yaxises);
//            util.drawHorizontalGrid(this.canvas.width,this.canvas.height,this.ctx);
            util.drawXaxis({datatodisplay:datatodisplay,ctx:this.ctx,spacing:spacing,startx:startx,height:this.canvas.height,width:this.canvas.width,config:config,gridcolor:this.color.grid,xlabel:this.color.xlabel,xline:this.color.xline,doVertGrid:true});
            util.draw({startx:startx,datatodisplay:datatodisplay,spacing:spacing,buffer:this.buffer[id],bufferctx:this.bufferctx[id],yaxises:yaxises,config:config,rendermode:source.rendermode || this.rendermode || "line", range:range});
            util.clip({ctx:this.bufferctx[id],config:config,height:this.buffer[id].height,type:'clear',clipcolor:this.color.bg});
            util.clip({ctx:this.ctx,config:config,height:this.canvas.height,type:'fill',clipcolor:this.color.bg});
            util.drawYaxis({canvas:this.canvas,ctx:this.ctx,range:range,config:config,yline:this.color.yline,ylabel:this.color.ylabel});
    
            source.displayData = util.getDisplayPoints({startx:startx,datatodisplay:datatodisplay,spacing:spacing,height:this.buffer[id].height,yaxises:yaxises,config:config,range:range});
        }
    
        if (interaction !== undefined) {
            interaction.redraw();
        }        
    };
    source.on('data',onDataGraph.bind(this));
};
});

require.define("/lib/util.js",function(require,module,exports,__dirname,__filename,process){var Hash = require('hashish');
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
});

require.define("/node_modules/hashish/package.json",function(require,module,exports,__dirname,__filename,process){module.exports = {"main":"./index.js"}});

require.define("/node_modules/hashish/index.js",function(require,module,exports,__dirname,__filename,process){module.exports = Hash;
var Traverse = require('traverse');

function Hash (hash, xs) {
    if (Array.isArray(hash) && Array.isArray(xs)) {
        var to = Math.min(hash.length, xs.length);
        var acc = {};
        for (var i = 0; i < to; i++) {
            acc[hash[i]] = xs[i];
        }
        return Hash(acc);
    }
    
    if (hash === undefined) return Hash({});
    
    var self = {
        map : function (f) {
            var acc = { __proto__ : hash.__proto__ };
            Object.keys(hash).forEach(function (key) {
                acc[key] = f.call(self, hash[key], key);
            });
            return Hash(acc);
        },
        forEach : function (f) {
            Object.keys(hash).forEach(function (key) {
                f.call(self, hash[key], key);
            });
            return self;
        },
        filter : function (f) {
            var acc = { __proto__ : hash.__proto__ };
            Object.keys(hash).forEach(function (key) {
                if (f.call(self, hash[key], key)) {
                    acc[key] = hash[key];
                }
            });
            return Hash(acc);
        },
        detect : function (f) {
            for (var key in hash) {
                if (f.call(self, hash[key], key)) {
                    return hash[key];
                }
            }
            return undefined;
        },
        reduce : function (f, acc) {
            var keys = Object.keys(hash);
            if (acc === undefined) acc = keys.shift();
            keys.forEach(function (key) {
                acc = f.call(self, acc, hash[key], key);
            });
            return acc;
        },
        some : function (f) {
            for (var key in hash) {
                if (f.call(self, hash[key], key)) return true;
            }
            return false;
        },
        update : function (obj) {
            if (arguments.length > 1) {
                self.updateAll([].slice.call(arguments));
            }
            else {
                Object.keys(obj).forEach(function (key) {
                    hash[key] = obj[key];
                });
            }
            return self;
        },
        updateAll : function (xs) {
            xs.filter(Boolean).forEach(function (x) {
                self.update(x);
            });
            return self;
        },
        merge : function (obj) {
            if (arguments.length > 1) {
                return self.copy.updateAll([].slice.call(arguments));
            }
            else {
                return self.copy.update(obj);
            }
        },
        mergeAll : function (xs) {
            return self.copy.updateAll(xs);
        },
        has : function (key) { // only operates on enumerables
            return Array.isArray(key)
                ? key.every(function (k) { return self.has(k) })
                : self.keys.indexOf(key.toString()) >= 0;
        },
        valuesAt : function (keys) {
            return Array.isArray(keys)
                ? keys.map(function (key) { return hash[key] })
                : hash[keys]
            ;
        },
        tap : function (f) {
            f.call(self, hash);
            return self;
        },
        extract : function (keys) {
            var acc = {};
            keys.forEach(function (key) {
                acc[key] = hash[key];
            });
            return Hash(acc);
        },
        exclude : function (keys) {
            return self.filter(function (_, key) {
                return keys.indexOf(key) < 0
            });
        },
        end : hash,
        items : hash
    };
    
    var props = {
        keys : function () { return Object.keys(hash) },
        values : function () {
            return Object.keys(hash).map(function (key) { return hash[key] });
        },
        compact : function () {
            return self.filter(function (x) { return x !== undefined });
        },
        clone : function () { return Hash(Hash.clone(hash)) },
        copy : function () { return Hash(Hash.copy(hash)) },
        length : function () { return Object.keys(hash).length },
        size : function () { return self.length }
    };
    
    if (Object.defineProperty) {
        // es5-shim has an Object.defineProperty but it throws for getters
        try {
            for (var key in props) {
                Object.defineProperty(self, key, { get : props[key] });
            }
        }
        catch (err) {
            for (var key in props) {
                if (key !== 'clone' && key !== 'copy' && key !== 'compact') {
                    // ^ those keys use Hash() so can't call them without
                    // a stack overflow
                    self[key] = props[key]();
                }
            }
        }
    }
    else if (self.__defineGetter__) {
        for (var key in props) {
            self.__defineGetter__(key, props[key]);
        }
    }
    else {
        // non-lazy version for browsers that suck >_<
        for (var key in props) {
            self[key] = props[key]();
        }
    }
    
    return self;
};

// deep copy
Hash.clone = function (ref) {
    return Traverse.clone(ref);
};

// shallow copy
Hash.copy = function (ref) {
    var hash = { __proto__ : ref.__proto__ };
    Object.keys(ref).forEach(function (key) {
        hash[key] = ref[key];
    });
    return hash;
};

Hash.map = function (ref, f) {
    return Hash(ref).map(f).items;
};

Hash.forEach = function (ref, f) {
    Hash(ref).forEach(f);
};

Hash.filter = function (ref, f) {
    return Hash(ref).filter(f).items;
};

Hash.detect = function (ref, f) {
    return Hash(ref).detect(f);
};

Hash.reduce = function (ref, f, acc) {
    return Hash(ref).reduce(f, acc);
};

Hash.some = function (ref, f) {
    return Hash(ref).some(f);
};

Hash.update = function (a /*, b, c, ... */) {
    var args = Array.prototype.slice.call(arguments, 1);
    var hash = Hash(a);
    return hash.update.apply(hash, args).items;
};

Hash.merge = function (a /*, b, c, ... */) {
    var args = Array.prototype.slice.call(arguments, 1);
    var hash = Hash(a);
    return hash.merge.apply(hash, args).items;
};

Hash.has = function (ref, key) {
    return Hash(ref).has(key);
};

Hash.valuesAt = function (ref, keys) {
    return Hash(ref).valuesAt(keys);
};

Hash.tap = function (ref, f) {
    return Hash(ref).tap(f).items;
};

Hash.extract = function (ref, keys) {
    return Hash(ref).extract(keys).items;
};

Hash.exclude = function (ref, keys) {
    return Hash(ref).exclude(keys).items;
};

Hash.concat = function (xs) {
    var hash = Hash({});
    xs.forEach(function (x) { hash.update(x) });
    return hash.items;
};

Hash.zip = function (xs, ys) {
    return Hash(xs, ys).items;
};

// .length is already defined for function prototypes
Hash.size = function (ref) {
    return Hash(ref).size;
};

Hash.compact = function (ref) {
    return Hash(ref).compact.items;
};
});

require.define("/node_modules/hashish/node_modules/traverse/package.json",function(require,module,exports,__dirname,__filename,process){module.exports = {"main":"index.js"}});

require.define("/node_modules/hashish/node_modules/traverse/index.js",function(require,module,exports,__dirname,__filename,process){var traverse = module.exports = function (obj) {
    return new Traverse(obj);
};

function Traverse (obj) {
    this.value = obj;
}

Traverse.prototype.get = function (ps) {
    var node = this.value;
    for (var i = 0; i < ps.length; i ++) {
        var key = ps[i];
        if (!Object.hasOwnProperty.call(node, key)) {
            node = undefined;
            break;
        }
        node = node[key];
    }
    return node;
};

Traverse.prototype.has = function (ps) {
    var node = this.value;
    for (var i = 0; i < ps.length; i ++) {
        var key = ps[i];
        if (!Object.hasOwnProperty.call(node, key)) {
            return false;
        }
        node = node[key];
    }
    return true;
};

Traverse.prototype.set = function (ps, value) {
    var node = this.value;
    for (var i = 0; i < ps.length - 1; i ++) {
        var key = ps[i];
        if (!Object.hasOwnProperty.call(node, key)) node[key] = {};
        node = node[key];
    }
    node[ps[i]] = value;
    return value;
};

Traverse.prototype.map = function (cb) {
    return walk(this.value, cb, true);
};

Traverse.prototype.forEach = function (cb) {
    this.value = walk(this.value, cb, false);
    return this.value;
};

Traverse.prototype.reduce = function (cb, init) {
    var skip = arguments.length === 1;
    var acc = skip ? this.value : init;
    this.forEach(function (x) {
        if (!this.isRoot || !skip) {
            acc = cb.call(this, acc, x);
        }
    });
    return acc;
};

Traverse.prototype.paths = function () {
    var acc = [];
    this.forEach(function (x) {
        acc.push(this.path); 
    });
    return acc;
};

Traverse.prototype.nodes = function () {
    var acc = [];
    this.forEach(function (x) {
        acc.push(this.node);
    });
    return acc;
};

Traverse.prototype.clone = function () {
    var parents = [], nodes = [];
    
    return (function clone (src) {
        for (var i = 0; i < parents.length; i++) {
            if (parents[i] === src) {
                return nodes[i];
            }
        }
        
        if (typeof src === 'object' && src !== null) {
            var dst = copy(src);
            
            parents.push(src);
            nodes.push(dst);
            
            forEach(objectKeys(src), function (key) {
                dst[key] = clone(src[key]);
            });
            
            parents.pop();
            nodes.pop();
            return dst;
        }
        else {
            return src;
        }
    })(this.value);
};

function walk (root, cb, immutable) {
    var path = [];
    var parents = [];
    var alive = true;
    
    return (function walker (node_) {
        var node = immutable ? copy(node_) : node_;
        var modifiers = {};
        
        var keepGoing = true;
        
        var state = {
            node : node,
            node_ : node_,
            path : [].concat(path),
            parent : parents[parents.length - 1],
            parents : parents,
            key : path.slice(-1)[0],
            isRoot : path.length === 0,
            level : path.length,
            circular : null,
            update : function (x, stopHere) {
                if (!state.isRoot) {
                    state.parent.node[state.key] = x;
                }
                state.node = x;
                if (stopHere) keepGoing = false;
            },
            'delete' : function (stopHere) {
                delete state.parent.node[state.key];
                if (stopHere) keepGoing = false;
            },
            remove : function (stopHere) {
                if (isArray(state.parent.node)) {
                    state.parent.node.splice(state.key, 1);
                }
                else {
                    delete state.parent.node[state.key];
                }
                if (stopHere) keepGoing = false;
            },
            keys : null,
            before : function (f) { modifiers.before = f },
            after : function (f) { modifiers.after = f },
            pre : function (f) { modifiers.pre = f },
            post : function (f) { modifiers.post = f },
            stop : function () { alive = false },
            block : function () { keepGoing = false }
        };
        
        if (!alive) return state;
        
        function updateState() {
            if (typeof state.node === 'object' && state.node !== null) {
                if (!state.keys || state.node_ !== state.node) {
                    state.keys = objectKeys(state.node)
                }
                
                state.isLeaf = state.keys.length == 0;
                
                for (var i = 0; i < parents.length; i++) {
                    if (parents[i].node_ === node_) {
                        state.circular = parents[i];
                        break;
                    }
                }
            }
            else {
                state.isLeaf = true;
                state.keys = null;
            }
            
            state.notLeaf = !state.isLeaf;
            state.notRoot = !state.isRoot;
        }
        
        updateState();
        
        // use return values to update if defined
        var ret = cb.call(state, state.node);
        if (ret !== undefined && state.update) state.update(ret);
        
        if (modifiers.before) modifiers.before.call(state, state.node);
        
        if (!keepGoing) return state;
        
        if (typeof state.node == 'object'
        && state.node !== null && !state.circular) {
            parents.push(state);
            
            updateState();
            
            forEach(state.keys, function (key, i) {
                path.push(key);
                
                if (modifiers.pre) modifiers.pre.call(state, state.node[key], key);
                
                var child = walker(state.node[key]);
                if (immutable && Object.hasOwnProperty.call(state.node, key)) {
                    state.node[key] = child.node;
                }
                
                child.isLast = i == state.keys.length - 1;
                child.isFirst = i == 0;
                
                if (modifiers.post) modifiers.post.call(state, child);
                
                path.pop();
            });
            parents.pop();
        }
        
        if (modifiers.after) modifiers.after.call(state, state.node);
        
        return state;
    })(root).node;
}

function copy (src) {
    if (typeof src === 'object' && src !== null) {
        var dst;
        
        if (isArray(src)) {
            dst = [];
        }
        else if (isDate(src)) {
            dst = new Date(src);
        }
        else if (isRegExp(src)) {
            dst = new RegExp(src);
        }
        else if (isError(src)) {
            dst = { message: src.message };
        }
        else if (isBoolean(src)) {
            dst = new Boolean(src);
        }
        else if (isNumber(src)) {
            dst = new Number(src);
        }
        else if (isString(src)) {
            dst = new String(src);
        }
        else if (Object.create && Object.getPrototypeOf) {
            dst = Object.create(Object.getPrototypeOf(src));
        }
        else if (src.constructor === Object) {
            dst = {};
        }
        else {
            var proto =
                (src.constructor && src.constructor.prototype)
                || src.__proto__
                || {}
            ;
            var T = function () {};
            T.prototype = proto;
            dst = new T;
        }
        
        forEach(objectKeys(src), function (key) {
            dst[key] = src[key];
        });
        return dst;
    }
    else return src;
}

var objectKeys = Object.keys || function keys (obj) {
    var res = [];
    for (var key in obj) res.push(key)
    return res;
};

function toS (obj) { return Object.prototype.toString.call(obj) }
function isDate (obj) { return toS(obj) === '[object Date]' }
function isRegExp (obj) { return toS(obj) === '[object RegExp]' }
function isError (obj) { return toS(obj) === '[object Error]' }
function isBoolean (obj) { return toS(obj) === '[object Boolean]' }
function isNumber (obj) { return toS(obj) === '[object Number]' }
function isString (obj) { return toS(obj) === '[object String]' }

var isArray = Array.isArray || function isArray (xs) {
    return Object.prototype.toString.call(xs) === '[object Array]';
};

var forEach = function (xs, fn) {
    if (xs.forEach) return xs.forEach(fn)
    else for (var i = 0; i < xs.length; i++) {
        fn(xs[i], i, xs);
    }
};

forEach(objectKeys(Traverse.prototype), function (key) {
    traverse[key] = function (obj) {
        var args = [].slice.call(arguments, 1);
        var t = new Traverse(obj);
        return t[key].apply(t, args);
    };
});
});

require.define("/node_modules/mrcolor/package.json",function(require,module,exports,__dirname,__filename,process){module.exports = {"main":"index.js"}});

require.define("/node_modules/mrcolor/index.js",function(require,module,exports,__dirname,__filename,process){var convert = require('color-convert');

var mr = module.exports = function () {
    var used = [];
    var num = 0;
    var last = [];
    
    return function next () {
        var angle;
        if (num < 6) {
            angle = 60 * num;
            used.push(angle);
            if (num === 5) used.push(360);
        }
        else {
            var dxs = used.slice(1).map(function (u, i) {
                return (u - used[i]) * last.every(function (x) {
                    return Math.abs(u - x) > 60
                        && Math.abs((u - 360) - x) > 60
                    ;
                });
            });
            var ix = dxs.indexOf(Math.max.apply(null, dxs));
            
            var x = used[ix];
            var y = used[ix+1];
            angle = Math.floor(x + (y - x) / 2);
            used.splice(ix + 1, 0, angle);
        }
        
        num ++;
        last = [angle].concat(last).slice(0,4);
        
        return mr.fromHSL(
            angle,
            100 - Math.min(80, 1 / Math.sqrt(1 + Math.floor(num / 12)))
                * Math.random(),
            50 + Math.min(80, (Math.floor(num / 6) * 20))
                * (Math.random() - 0.5)
        );
    };
};

mr.fromHSL = function (h, s, l) {
    if (!s) s = 100;
    if (!l) l = 50;
    var hsl = [ h, s, l ];
    
    return {
        rgb : function () {
            return convert.hsl2rgb(hsl);
        },
        hsl : function () {
            return hsl;
        },
        hsv : function () {
            return convert.hsl2hsv(hsl)
        },
        cmyk : function () {
            return convert.hsl2cmyk(hsl)
        },
        xyz : function () {
            return convert.hsl2xyz(hsl)
        }
    };
};

mr.take = function (n) {
    if (n <= 0) return [];
    
    var res = [];
    var next = mr();
    
    for (var i = 0; i < n; i++) {
        res.push(next());
    }
    
    return res;
};

mr.lighten = function(color,by) {
    var hsv = color.hsv().map(function(val,idx) {
        return (idx == 1) ? (by || 0.2) * val : val
    });
    return mr.fromHSL.apply(undefined,convert.hsv2hsl(hsv));
};

mr.rgbhexToColorObj = function(color) {
    var cutHex = function (h) {return (h.charAt(0)=="#") ? h.substring(1,7):h}
    var hexToR = function (h) {return parseInt((cutHex(h)).substring(0,2),16)}
    var hexToG = function (h) {return parseInt((cutHex(h)).substring(2,4),16)}
    var hexToB = function (h) {return parseInt((cutHex(h)).substring(4,6),16)}
   
    var rgb = [hexToR(color),hexToG(color),hexToB(color)]; 
    return mr.fromHSL.apply(undefined,convert.rgb2hsl(rgb))
}
});

require.define("/node_modules/mrcolor/node_modules/color-convert/package.json",function(require,module,exports,__dirname,__filename,process){module.exports = {"main":"./index"}});

require.define("/node_modules/mrcolor/node_modules/color-convert/index.js",function(require,module,exports,__dirname,__filename,process){var conversions = require("./conversions");

var exports = {};
module.exports = exports;

for (var func in conversions) {
  // export rgb2hslRaw
  exports[func + "Raw"] =  (function(func) {
    // accept array or plain args
    return function(arg) {
      if (typeof arg == "number")
        arg = Array.prototype.slice.call(arguments);
      return conversions[func](arg);
    }
  })(func);

  var pair = /(\w+)2(\w+)/.exec(func),
      from = pair[1],
      to = pair[2];

  // export rgb2hsl and ["rgb"]["hsl"]
  exports[from] = exports[from] || {};

  exports[from][to] = exports[func] = (function(func) { 
    return function(arg) {
      if (typeof arg == "number")
        arg = Array.prototype.slice.call(arguments);
      
      var val = conversions[func](arg);
      if (typeof val == "string" || val === undefined)
        return val; // keyword

      for (var i = 0; i < val.length; i++)
        val[i] = Math.round(val[i]);
      return val;
    }
  })(func);
}});

require.define("/node_modules/mrcolor/node_modules/color-convert/conversions.js",function(require,module,exports,__dirname,__filename,process){/* MIT license */

module.exports = {
  rgb2hsl: rgb2hsl,
  rgb2hsv: rgb2hsv,
  rgb2cmyk: rgb2cmyk,
  rgb2keyword: rgb2keyword,
  rgb2xyz: rgb2xyz,
  rgb2lab: rgb2lab,

  hsl2rgb: hsl2rgb,
  hsl2hsv: hsl2hsv,
  hsl2cmyk: hsl2cmyk,
  hsl2keyword: hsl2keyword,

  hsv2rgb: hsv2rgb,
  hsv2hsl: hsv2hsl,
  hsv2cmyk: hsv2cmyk,
  hsv2keyword: hsv2keyword,

  cmyk2rgb: cmyk2rgb,
  cmyk2hsl: cmyk2hsl,
  cmyk2hsv: cmyk2hsv,
  cmyk2keyword: cmyk2keyword,
  
  keyword2rgb: keyword2rgb,
  keyword2hsl: keyword2hsl,
  keyword2hsv: keyword2hsv,
  keyword2cmyk: keyword2cmyk,
  
  xyz2rgb: xyz2rgb,
}


function rgb2hsl(rgb) {
  var r = rgb[0]/255,
      g = rgb[1]/255,
      b = rgb[2]/255,
      min = Math.min(r, g, b),
      max = Math.max(r, g, b),
      delta = max - min,
      h, s, l;

  if (max == min)
    h = 0;
  else if (r == max) 
    h = (g - b) / delta; 
  else if (g == max)
    h = 2 + (b - r) / delta; 
  else if (b == max)
    h = 4 + (r - g)/ delta;

  h = Math.min(h * 60, 360);

  if (h < 0)
    h += 360;

  l = (min + max) / 2;

  if (max == min)
    s = 0;
  else if (l <= 0.5)
    s = delta / (max + min);
  else
    s = delta / (2 - max - min);

  return [h, s * 100, l * 100];
}

function rgb2hsv(rgb) {
  var r = rgb[0],
      g = rgb[1],
      b = rgb[2],
      min = Math.min(r, g, b),
      max = Math.max(r, g, b),
      delta = max - min,
      h, s, v;

  if (max == 0)
    s = 0;
  else
    s = (delta/max * 1000)/10;

  if (max == min)
    h = 0;
  else if (r == max) 
    h = (g - b) / delta; 
  else if (g == max)
    h = 2 + (b - r) / delta; 
  else if (b == max)
    h = 4 + (r - g) / delta;

  h = Math.min(h * 60, 360);

  if (h < 0) 
    h += 360;

  v = ((max / 255) * 1000) / 10;

  return [h, s, v];
}

function rgb2cmyk(rgb) {
  var r = rgb[0] / 255,
      g = rgb[1] / 255,
      b = rgb[2] / 255,
      c, m, y, k;
      
  k = Math.min(1 - r, 1 - g, 1 - b);
  c = (1 - r - k) / (1 - k);
  m = (1 - g - k) / (1 - k);
  y = (1 - b - k) / (1 - k);
  return [c * 100, m * 100, y * 100, k * 100];
}

function rgb2keyword(rgb) {
  return reverseKeywords[JSON.stringify(rgb)];
}

function rgb2xyz(rgb) {
  var r = rgb[0] / 255,
      g = rgb[1] / 255,
      b = rgb[2] / 255;

  // assume sRGB
  r = r > 0.04045 ? Math.pow(((r + 0.055) / 1.055), 2.4) : (r / 12.92);
  g = g > 0.04045 ? Math.pow(((g + 0.055) / 1.055), 2.4) : (g / 12.92);
  b = b > 0.04045 ? Math.pow(((b + 0.055) / 1.055), 2.4) : (b / 12.92);
  
  var x = (r * 0.4124) + (g * 0.3576) + (b * 0.1805);
  var y = (r * 0.2126) + (g * 0.7152) + (b * 0.0722);
  var z = (r * 0.0193) + (g * 0.1192) + (b * 0.9505);

  return [x * 100, y *100, z * 100];
}

function rgb2lab(rgb) {
  var xyz = rgb2xyz(rgb),
        x = xyz[0],
        y = xyz[1],
        z = xyz[2],
        l, a, b;

  x /= 95.047;
  y /= 100;
  z /= 108.883;

  x = x > 0.008856 ? Math.pow(x, 1/3) : (7.787 * x) + (16 / 116);
  y = y > 0.008856 ? Math.pow(y, 1/3) : (7.787 * y) + (16 / 116);
  z = z > 0.008856 ? Math.pow(z, 1/3) : (7.787 * z) + (16 / 116);

  l = (116 * y) - 16;
  a = 500 * (x - y);
  b = 200 * (y - z);
  
  return [l, a, b];
}


function hsl2rgb(hsl) {
  var h = hsl[0] / 360,
      s = hsl[1] / 100,
      l = hsl[2] / 100,
      t1, t2, t3, rgb, val;

  if (s == 0) {
    val = l * 255;
    return [val, val, val];
  }

  if (l < 0.5)
    t2 = l * (1 + s);
  else
    t2 = l + s - l * s;
  t1 = 2 * l - t2;

  rgb = [0, 0, 0];
  for (var i = 0; i < 3; i++) {
    t3 = h + 1 / 3 * - (i - 1);
    t3 < 0 && t3++;
    t3 > 1 && t3--;

    if (6 * t3 < 1)
      val = t1 + (t2 - t1) * 6 * t3;
    else if (2 * t3 < 1)
      val = t2;
    else if (3 * t3 < 2)
      val = t1 + (t2 - t1) * (2 / 3 - t3) * 6;
    else
      val = t1;

    rgb[i] = val * 255;
  }
  
  return rgb;
}

function hsl2hsv(hsl) {
  var h = hsl[0],
      s = hsl[1] / 100,
      l = hsl[2] / 100,
      sv, v;
  l *= 2;
  s *= (l <= 1) ? l : 2 - l;
  v = (l + s) / 2;
  sv = (2 * s) / (l + s);
  return [h, s * 100, v * 100];
}

function hsl2cmyk(args) {
  return rgb2cmyk(hsl2rgb(args));
}

function hsl2keyword(args) {
  return rgb2keyword(hsl2rgb(args));
}


function hsv2rgb(hsv) {
  var h = hsv[0] / 60,
      s = hsv[1] / 100,
      v = hsv[2] / 100,
      hi = Math.floor(h) % 6;

  var f = h - Math.floor(h),
      p = 255 * v * (1 - s),
      q = 255 * v * (1 - (s * f)),
      t = 255 * v * (1 - (s * (1 - f))),
      v = 255 * v;

  switch(hi) {
    case 0:
      return [v, t, p];
    case 1:
      return [q, v, p];
    case 2:
      return [p, v, t];
    case 3:
      return [p, q, v];
    case 4:
      return [t, p, v];
    case 5:
      return [v, p, q];
  }
}

function hsv2hsl(hsv) {
  var h = hsv[0],
      s = hsv[1] / 100,
      v = hsv[2] / 100,
      sl, l;

  l = (2 - s) * v;  
  sl = s * v;
  sl /= (l <= 1) ? l : 2 - l;
  l /= 2;
  return [h, sl * 100, l * 100];
}

function hsv2cmyk(args) {
  return rgb2cmyk(hsv2rgb(args));
}

function hsv2keyword(args) {
  return rgb2keyword(hsv2rgb(args));
}

function cmyk2rgb(cmyk) {
  var c = cmyk[0] / 100,
      m = cmyk[1] / 100,
      y = cmyk[2] / 100,
      k = cmyk[3] / 100,
      r, g, b;

  r = 1 - Math.min(1, c * (1 - k) + k);
  g = 1 - Math.min(1, m * (1 - k) + k);
  b = 1 - Math.min(1, y * (1 - k) + k);
  return [r * 255, g * 255, b * 255];
}

function cmyk2hsl(args) {
  return rgb2hsl(cmyk2rgb(args));
}

function cmyk2hsv(args) {
  return rgb2hsv(cmyk2rgb(args));
}

function cmyk2keyword(args) {
  return rgb2keyword(cmyk2rgb(args));
}


function xyz2rgb(xyz) {
  var x = xyz[0] / 100,
      y = xyz[1] / 100,
      z = xyz[2] / 100,
      r, g, b;

  r = (x * 3.2406) + (y * -1.5372) + (z * -0.4986);
  g = (x * -0.9689) + (y * 1.8758) + (z * 0.0415);
  b = (x * 0.0557) + (y * -0.2040) + (z * 1.0570);

  // assume sRGB
  r = r > 0.0031308 ? ((1.055 * Math.pow(r, 1.0 / 2.4)) - 0.055)
    : r = (r * 12.92);

  g = g > 0.0031308 ? ((1.055 * Math.pow(g, 1.0 / 2.4)) - 0.055)
    : g = (g * 12.92);
        
  b = b > 0.0031308 ? ((1.055 * Math.pow(b, 1.0 / 2.4)) - 0.055)
    : b = (b * 12.92);

  r = (r < 0) ? 0 : r;
  g = (g < 0) ? 0 : g;
  b = (b < 0) ? 0 : b;

  return [r * 255, g * 255, b * 255];
}


function keyword2rgb(keyword) {
  return cssKeywords[keyword];
}

function keyword2hsl(args) {
  return rgb2hsl(keyword2rgb(args));
}

function keyword2hsv(args) {
  return rgb2hsv(keyword2rgb(args));
}

function keyword2cmyk(args) {
  return rgb2cmyk(keyword2rgb(args));
}

var cssKeywords = {
  aliceblue:  [240,248,255],
  antiquewhite: [250,235,215],
  aqua: [0,255,255],
  aquamarine: [127,255,212],
  azure:  [240,255,255],
  beige:  [245,245,220],
  bisque: [255,228,196],
  black:  [0,0,0],
  blanchedalmond: [255,235,205],
  blue: [0,0,255],
  blueviolet: [138,43,226],
  brown:  [165,42,42],
  burlywood:  [222,184,135],
  cadetblue:  [95,158,160],
  chartreuse: [127,255,0],
  chocolate:  [210,105,30],
  coral:  [255,127,80],
  cornflowerblue: [100,149,237],
  cornsilk: [255,248,220],
  crimson:  [220,20,60],
  cyan: [0,255,255],
  darkblue: [0,0,139],
  darkcyan: [0,139,139],
  darkgoldenrod:  [184,134,11],
  darkgray: [169,169,169],
  darkgreen:  [0,100,0],
  darkgrey: [169,169,169],
  darkkhaki:  [189,183,107],
  darkmagenta:  [139,0,139],
  darkolivegreen: [85,107,47],
  darkorange: [255,140,0],
  darkorchid: [153,50,204],
  darkred:  [139,0,0],
  darksalmon: [233,150,122],
  darkseagreen: [143,188,143],
  darkslateblue:  [72,61,139],
  darkslategray:  [47,79,79],
  darkslategrey:  [47,79,79],
  darkturquoise:  [0,206,209],
  darkviolet: [148,0,211],
  deeppink: [255,20,147],
  deepskyblue:  [0,191,255],
  dimgray:  [105,105,105],
  dimgrey:  [105,105,105],
  dodgerblue: [30,144,255],
  firebrick:  [178,34,34],
  floralwhite:  [255,250,240],
  forestgreen:  [34,139,34],
  fuchsia:  [255,0,255],
  gainsboro:  [220,220,220],
  ghostwhite: [248,248,255],
  gold: [255,215,0],
  goldenrod:  [218,165,32],
  gray: [128,128,128],
  green:  [0,128,0],
  greenyellow:  [173,255,47],
  grey: [128,128,128],
  honeydew: [240,255,240],
  hotpink:  [255,105,180],
  indianred:  [205,92,92],
  indigo: [75,0,130],
  ivory:  [255,255,240],
  khaki:  [240,230,140],
  lavender: [230,230,250],
  lavenderblush:  [255,240,245],
  lawngreen:  [124,252,0],
  lemonchiffon: [255,250,205],
  lightblue:  [173,216,230],
  lightcoral: [240,128,128],
  lightcyan:  [224,255,255],
  lightgoldenrodyellow: [250,250,210],
  lightgray:  [211,211,211],
  lightgreen: [144,238,144],
  lightgrey:  [211,211,211],
  lightpink:  [255,182,193],
  lightsalmon:  [255,160,122],
  lightseagreen:  [32,178,170],
  lightskyblue: [135,206,250],
  lightslategray: [119,136,153],
  lightslategrey: [119,136,153],
  lightsteelblue: [176,196,222],
  lightyellow:  [255,255,224],
  lime: [0,255,0],
  limegreen:  [50,205,50],
  linen:  [250,240,230],
  magenta:  [255,0,255],
  maroon: [128,0,0],
  mediumaquamarine: [102,205,170],
  mediumblue: [0,0,205],
  mediumorchid: [186,85,211],
  mediumpurple: [147,112,219],
  mediumseagreen: [60,179,113],
  mediumslateblue:  [123,104,238],
  mediumspringgreen:  [0,250,154],
  mediumturquoise:  [72,209,204],
  mediumvioletred:  [199,21,133],
  midnightblue: [25,25,112],
  mintcream:  [245,255,250],
  mistyrose:  [255,228,225],
  moccasin: [255,228,181],
  navajowhite:  [255,222,173],
  navy: [0,0,128],
  oldlace:  [253,245,230],
  olive:  [128,128,0],
  olivedrab:  [107,142,35],
  orange: [255,165,0],
  orangered:  [255,69,0],
  orchid: [218,112,214],
  palegoldenrod:  [238,232,170],
  palegreen:  [152,251,152],
  paleturquoise:  [175,238,238],
  palevioletred:  [219,112,147],
  papayawhip: [255,239,213],
  peachpuff:  [255,218,185],
  peru: [205,133,63],
  pink: [255,192,203],
  plum: [221,160,221],
  powderblue: [176,224,230],
  purple: [128,0,128],
  red:  [255,0,0],
  rosybrown:  [188,143,143],
  royalblue:  [65,105,225],
  saddlebrown:  [139,69,19],
  salmon: [250,128,114],
  sandybrown: [244,164,96],
  seagreen: [46,139,87],
  seashell: [255,245,238],
  sienna: [160,82,45],
  silver: [192,192,192],
  skyblue:  [135,206,235],
  slateblue:  [106,90,205],
  slategray:  [112,128,144],
  slategrey:  [112,128,144],
  snow: [255,250,250],
  springgreen:  [0,255,127],
  steelblue:  [70,130,180],
  tan:  [210,180,140],
  teal: [0,128,128],
  thistle:  [216,191,216],
  tomato: [255,99,71],
  turquoise:  [64,224,208],
  violet: [238,130,238],
  wheat:  [245,222,179],
  white:  [255,255,255],
  whitesmoke: [245,245,245],
  yellow: [255,255,0],
  yellowgreen:  [154,205,50]
};

var reverseKeywords = {};
for (var key in cssKeywords) {
  reverseKeywords[JSON.stringify(cssKeywords[key])] = key;
}
});

require.define("/lib/legend.js",function(require,module,exports,__dirname,__filename,process){var mrcolor = require('mrcolor');
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
});

require.define("/node_modules/hat/package.json",function(require,module,exports,__dirname,__filename,process){module.exports = {"main":"index.js"}});

require.define("/node_modules/hat/index.js",function(require,module,exports,__dirname,__filename,process){var hat = module.exports = function (bits, base) {
    if (!base) base = 16;
    if (bits === undefined) bits = 128;
    if (bits <= 0) return '0';
    
    var digits = Math.log(Math.pow(2, bits)) / Math.log(base);
    for (var i = 2; digits === Infinity; i *= 2) {
        digits = Math.log(Math.pow(2, bits / i)) / Math.log(base) * i;
    }
    
    var rem = digits - Math.floor(digits);
    
    var res = '';
    
    for (var i = 0; i < Math.floor(digits); i++) {
        var x = Math.floor(Math.random() * base).toString(base);
        res = x + res;
    }
    
    if (rem) {
        var b = Math.pow(base, rem);
        var x = Math.floor(Math.random() * b).toString(base);
        res = x + res;
    }
    
    var parsed = parseInt(res, base);
    if (parsed !== Infinity && parsed >= Math.pow(2, bits)) {
        return hat(bits, base)
    }
    else return res;
};

hat.rack = function (bits, base, expandBy) {
    var fn = function (data) {
        var iters = 0;
        do {
            if (iters ++ > 10) {
                if (expandBy) bits += expandBy;
                else throw new Error('too many ID collisions, use more bits')
            }
            
            var id = hat(bits, base);
        } while (Object.hasOwnProperty.call(hats, id));
        
        hats[id] = data;
        return id;
    };
    var hats = fn.hats = {};
    
    fn.get = function (id) {
        return fn.hats[id];
    };
    
    fn.set = function (id, value) {
        fn.hats[id] = value;
        return fn;
    };
    
    fn.bits = bits || 128;
    fn.base = base || 16;
    return fn;
};
});

require.define("/lib/interaction.js",function(require,module,exports,__dirname,__filename,process){
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
});

require.define("/examples/multiple/multiple.js",function(require,module,exports,__dirname,__filename,process){var ee = require('events').EventEmitter;
var nodechart = require('../../index.js');
var datasource = new ee;
$(window).ready(function() {
    var values1 = [500,492,488,440,400,390,420,390,350,200,200,100,96,94,95,92,80,81,50,49,48,47,44];
    var values2 = [1,2,2,3,3,4,4,3,3,4,6,7,6,5,5,5,4,3,3,2,2,1,1];
    var chart = new nodechart;
    chart.series(datasource);
    chart.to(document.getElementById('mycanvas'));
    chart.legend(document.getElementById('mylegend'));
    setInterval(function() {
        if ((values1.length > 0) && (values2.length > 0)) {
            var v1 = values1.shift();
            var v2 = values2.shift();
            datasource.emit('data',{higher:v1,lower:v2},{multiple:true});
        }
    },1000);
});
});
require("/examples/multiple/multiple.js");
})();
