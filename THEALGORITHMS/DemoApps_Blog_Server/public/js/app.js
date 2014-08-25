'use strict';

// Declare app level module which depends on filters, and services

angular.module('myApp', [
  'myApp.controllers',
  'myApp.filters',
  'nvd3ChartDirectives',  
  'dangle',
  'calHeatmap',
  'myApp.services',
  'myApp.directives'
]).

directive('helloWorld', function() {
  return {
      restrict: 'AE',
      replace: 'true',
      template: '<h3>Hello World!!</h3>'
  };
}).

directive('donutChart', function() {
  return {
    restrict: 'E',
    link: link
  };
}).

directive('bars', function ($parse) {
  return {
     restrict: 'E',
     replace: true,
     template: '<div id="chart"> Hi </div>',
     link: function (scope, element, attrs) {
       var data = attrs.data.split(','),
       chart = d3.select('#chart')
         .append("div").attr("class", "chart")
         .selectAll('div')
         .data(data).enter()
         .append("div")
         .transition().ease("elastic")
         .style("width", function(d) { return d + "%"; })
         .text(function(d) { return d + "%"; });
     } 
  };
}).



directive("greenSlider", function() {

    var linkFun = function($scope, element, attrs) {
        var $slider = jQuery(element);
        var option = attrs;
        var tryPrseInt = function(key, option) {
            if (option[key]) {
                option[key] = parseInt(option[key]);
            }
        };

        tryPrseInt("min", option);
        tryPrseInt("max", option);
        tryPrseInt("step", option);

        option = jQuery.extend({
            value: $scope[option.ngModel],
            change: function(event, ui) {
                if (attrs.ngModel && ui.value != $scope[attrs.ngModel]) {
                    var express = attrs.ngModel + ' = ' + ui.value;
                    $scope.$apply(express);
                    if (attrs.ngChange) {
                        $scope.$eval(attrs.ngChange);
                    }
                }
            }
        }, option);
        $slider.slider(option);
        //back
        if (option.ngModel) {
            $scope.$watch(option.ngModel, function(val) {
                if (val != $slider.slider("value")) {
                    $slider.slider("value", val);
                }
            });
        }
        console.log(attrs);
    };
    return {
        restrict: 'E',
        replace: true,
        transclude: false,
        template: '<div />',
        link: linkFun
    };
}).

directive('slider', function ($parse) {
    return {
      restrict: 'E',
      replace: true,
      template: '<input type="text" />',
      link: function ($scope, element, attrs) {
        var model = $parse(attrs.model);
        var slider = $(element[0]).slider();

        slider.on('slide', function(ev) {
          model.assign($scope, ev.value);
          $scope.$apply();
        });
      }
    }
}).



config(function ($routeProvider, $locationProvider) {
  $routeProvider.
    when('/compAdapt', {
      templateUrl: 'partials/compAdapt',
      controller: 'CatCtrl'
    }).
    when('/view2', {
      templateUrl: 'partials/partial2',
      controller: 'MyCtrl2'
    }).
    when('/land', {
      templateUrl: 'partials/land',
      controller: 'LandCtrl'
    }).    
    when('/spacedRep', {
      templateUrl: 'partials/spacedRep',
      controller: 'AlgCtrl'
    }).
    when('/testing', {
      templateUrl: 'partials/testing',
      controller: 'AlgCtrl'
    }).   
    when('/irttesting', {
      templateUrl: 'partials/irtTesting',
      controller: 'IRTCtrl'
    }).      
    when('/cattesting', {
      templateUrl: 'partials/catTesting',
      controller: 'CatCtrl'
    }).        
    when('/zzish', {
      templateUrl: 'partials/zzish',
      controller: 'IRTCtrl'
    }).  
    when('/zzishtest', {
      templateUrl: 'partials/zzishtest',
      controller: 'IRTCtrl'
    }).                    
    when('/itemRes', {
      templateUrl: 'partials/itemResponse',
      controller: 'IRTCtrl'
    }).    

    otherwise({
      redirectTo: '/view2'
    });

  $locationProvider.html5Mode(true);
});




function link(scope, element) {
  // the D3 bits.. 
  // use `scope.data` as our data
  // and `element[0]` for the the directives containing DOM element
  var color = d3.scale.category10();
  var el = element[0];
  var width = el.clientWidth;
  var height = el.clientHeight;
  var min = Math.min(width, height);
  var pie = d3.layout.pie().sort(null);
  var arc = d3.svg.arc()
    .outerRadius(min / 2 * 0.9)
    .innerRadius(min / 2 * 0.5);
  var svg = d3.select(el).append('svg')
    .attr({width: width, height: height})
    .append('g')
      .attr('transform', 'translate(' + width / 2 + ',' + height / 2 + ')');
  
  // add the <path>s for each arc slice
  svg.selectAll('path').data(pie(scope.data))
    .enter().append('path')
      .style('stroke', 'white')
      .attr('d', arc)
      .attr('fill', function(d, i){ return color(i) });
}

