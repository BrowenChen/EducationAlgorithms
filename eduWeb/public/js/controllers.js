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
  controller('AlgCtrl', function ($scope, $http) {
    // write Ctrl here
    $scope.title = "WebRec";
    $scope.sites = [];
    $scope.formData = {};


    $scope.testSite = {
    "name": "http://deadspin.com",
    "nextDate": 16,
    "prevDate": 0,
    "interval": 6,
    "reps": 3,
    "EF": 2.8000000000000003,
    "thisDate": 10
  };

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
  controller('CatCtrl', function ($scope, $http) {
    // write Ctrl here
    $scope.title = "PreTest";

    $scope.promptWait = true;

    $scope.formData = {};
    $scope.ansData = {};

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


//Algorithm from CAT starting here ======================================================================
    $scope.D = 0;
    $scope.L = 0,
    $scope.H = 0,
    $scope.R = 0,
    $scope.testStandard = 0, //Pass/Fail Standard
    $scope.tLength = 0;

    $scope.testLength = 5; //Amount of questions to ask

    $scope.question = "No Question Yet";
    $scope.response = "";
    $scope.closestItem = null;
    $scope.nextCandidate = null;

// DUMMY DATA *************************************************
    $scope.itemBank = [ 
      {
        question: "test1",
        answer: "1",
        difficulty: 22
      },
      {
        question: "test3",
        answer: "1",    
        difficulty: 21
      },
      {
          question: "test4: This is a question",
          answer: "1",    
          difficulty: 20
      },    
      {
          question: "test5: This is a question",
          answer: "1",    
          difficulty: 19
      },  

      {
          question: "test7: This is a question",
          answer: "1",    
          difficulty: 18
      },  
      {
          question: "test8: This is a question",
          answer: "1",    
          difficulty: 17
      },  

      {
        question: "test2: What is the square root of pie",
        answer: "1",  
        difficulty: 16
      }
    ]; // Item Bank to choose from


    $scope.candidateBank = [
      {
        name: "Owen",
        ability: 18
      },

      {
        name: "Chris",
        ability: 15
      } 

    ]; //student Test bank

// DUMMY DATA *************************************************

    $scope.newCandidate = function(){
      this.D = 0;
      this.L = 0;
      this.H = 0;
      this.R = 0;

      this.nextCandidate = this.candidateBank[0];
      this.D = this.nextCandidate.ability;
      this.testStandard = 5; // Initializing testStandard
      // this.testLength = this.testLength;
      alert(this.D);
      this.begin(this.nextCandidate);
    };

    $scope.begin = function(nextCandidate){
      alert(nextCandidate.name);
      this.closestItem = this.findItemInBank(this.D);
      // this.D = closestItem.difficulty;
      // alert("asdasdasd");
      // alert(this.D);
      // alert(closestItem.difficulty);

      this.D = this.closestItem.difficulty;

      this.response = this.administerItem(this.closestItem);

      //Dont continue until we get a response 
      // CONTINUE STEPS IN GETUSERPROMPT==================================

    }

    $scope.findItemInBank = function(D){
      // alert(this.itemBank[0].difficulty);
      for (var item in this.itemBank){
        var minVal = 999;
        var minItem = "";

        var difference = Math.abs(D - this.itemBank[item].difficulty)

        if (difference < minVal){
          minVal = difference;
          minItem = this.itemBank[item];
        }

      }

      console.log(minVal);
      console.log(minItem);

      return minItem;
    }

    $scope.administerItem = function(item){
      
      
      this.question = item.question;
      alert(this.question);
      alert("Waiting for response");
      // return null
      // var answer = this.getUserPrompt();
      
    }

    $scope.getUserPrompt = function(){
      if (this.closestItem == null){
        alert("Start testing first");
        //Disable this button
      }

      else {
        this.response = $scope.ansData.text;
        alert(this.response);


        // CONTINUE HERE ============================

        //1 or 0

        var score = this.scoreResponse(this.response, this.closestItem) 
        alert(score);

        this.L += 1;

        this.H = this.H + this.D;

        if (score == 0){
          this.D = this.D - 2/(this.L);
          // alert("score incorrect  and " + this.D);
        }

        if (score == 1){
          this.D = this.D + 2/(this.L);
          this.R += 1;
        }

        if (this.testLength > 0){
          this.testLength -= 1;

          //Test again
          this.begin(this.nextCandidate);


        }

        if (this.testLength <= 0){
          alert("ready to give final ability level");
          var W = this.L - this.R;

        //Check if a variable is zero and set Measure. Cant log(0)
        if (this.L == 0 || this.R == 0){
          var measure = this.H/this.L;
        }
        else {
          var measure = this.H/this.L + Math.log(this.R/W);
        }

        // var standardError 

        alert("Final ability level of the child is " + measure);

        //Maybe change the child ability level


        }

        return null;
      }

    //   return answer;
    }

    $scope.scoreResponse = function(res, cItem){
      alert(cItem.answer);
      if (res == cItem.answer){
        alert("Correct");
        return 1
      }
      else {
        return 0
      }
      
    }


//Algorithm from CAT ending here ======================================================================


  }).  
  controller('LandCtrl', function ($scope) {
    // write Ctrl here    
    $scope.name = "Owen";
  

  });