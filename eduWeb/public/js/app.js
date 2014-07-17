'use strict';

// Declare app level module which depends on filters, and services

angular.module('myApp', [
  'myApp.controllers',
  'myApp.filters',
  'myApp.services',
  'myApp.directives'
]).
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
    when('/itemRes', {
      templateUrl: 'partials/itemResponse',
      controller: 'AlgCtrl'
    }).    
    otherwise({
      redirectTo: '/view2'
    });

  $locationProvider.html5Mode(true);
});
