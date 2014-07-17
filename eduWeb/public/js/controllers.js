'use strict';

/* Controllers */

angular.module('myApp.controllers', []).
  controller('AppCtrl', function ($scope, $http) {
    // $scope.name = "Owen";

    $http({
      method: 'GET',
      url: '/api/name'
    }).
    success(function (data, status, headers, config) {
      $scope.name = data.name;
    }).
    error(function (data, status, headers, config) {
      $scope.name = 'Error!';
    });

  }).
  controller('CatCtrl', function ($scope, $http) {
    // write Ctrl here
    $scope.title = "PreTest";
    $scope.sites = "";
    $scope.formData = {};
    
// ============== SITES MODEL MONGO ====================

    // when landing on the page, get all todos and show them
    $http.get('/api/site')
      .success(function(data) {
        $scope.sites = data; //Adding the todos here
        console.log(data);
      })
      .error(function(data) {
        console.log('Error: ' + data);
      });

    // when submitting the add form, send the text to the node API
    $scope.createSite = function() {
      $http.post('/api/site', $scope.formData)
        .success(function(data) {
          $scope.formData = {}; // clear the form so our user is ready to enter another
          $scope.sites = data;
          console.log(data);
        })
        .error(function(data) {
          console.log('Error: ' + data);
        });
    };

    // delete a todo after checking it
    $scope.deleteSite = function(id) {
      $http.delete('/api/site/' + id)
        .success(function(data) {
          $scope.sites = data;
          console.log(data);
        })
        .error(function(data) {
          console.log('Error: ' + data);
        });
    };    
// ============== SITES MODEL MONGO ====================




  }).
  controller('MyCtrl2', function ($scope) {
    // write Ctrl here

  }).
  controller('AlgCtrl', function ($scope, $http) {
    // write Ctrl here
    $scope.title = "WebRec";


    $scope.formData = {};

// // ============== SITES MODEL MONGO ====================

//     // when landing on the page, get all todos and show them
//     $http.get('/api/site')
//       .success(function(data) {
//         $scope.sites = data; //Adding the todos here
//         console.log(data);
//       })
//       .error(function(data) {
//         console.log('Error: ' + data);
//       });


//     // when submitting the add form, send the text to the node API
//     $scope.createTodo = function() {
//       $http.post('/api/site', $scope.formData)
//         .success(function(data) {
//           $scope.formData = {}; // clear the form so our user is ready to enter another
//           $scope.sites = data;
//           console.log(data);
//         })
//         .error(function(data) {
//           console.log('Error: ' + data);
//         });
//     };

//     // delete a todo after checking it
//     $scope.deleteTodo = function(id) {
//       $http.delete('/api/site/' + id)
//         .success(function(data) {
//           $scope.sites = data;
//           console.log(data);
//         })
//         .error(function(data) {
//           console.log('Error: ' + data);
//         });
//     };    
// // ============== SITES MODEL MONGO ====================





    // when landing on the page, get all todos and show them
    $http.get('/api/todo')
      .success(function(data) {
        $scope.todos = data; //Adding the todos here
        console.log(data);
      })
      .error(function(data) {
        console.log('Error: ' + data);
      });


    // when submitting the add form, send the text to the node API
    $scope.createTodo = function() {
      $http.post('/api/todo', $scope.formData)
        .success(function(data) {
          $scope.formData = {}; // clear the form so our user is ready to enter another
          $scope.todos = data;
          console.log(data);
        })
        .error(function(data) {
          console.log('Error: ' + data);
        });
    };

    // delete a todo after checking it
    $scope.deleteTodo = function(id) {
      $http.delete('/api/todo/' + id)
        .success(function(data) {
          $scope.todos = data;
          console.log(data);
        })
        .error(function(data) {
          console.log('Error: ' + data);
        });
    };    



  }).  
  controller('LandCtrl', function ($scope) {
    // write Ctrl here    
    $scope.name = "Owen";
  

  });
