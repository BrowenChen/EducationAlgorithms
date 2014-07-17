/*
Computerized Algorithimic Testing
-----------------------------------
cat.js
@author Owen Chen

Generalized Algorithm where developer provides a test bank. Maybe json file

inputs: Test bank, json file with card data
	Card attributes:
		- difficulty
		- subject/section

	
Algorithm

1. Request next candidate. Set D=0, L=0, H=0, and R=0. 
	D = Item difficulty. L = items taken. H = difficultiesTotal R= response score
2. Find next item near difficulty, D.
	Search in item bank for closest item near D
3. Set D at the actual calibration of that item.
	Set d = that item. 
1. Administer that item.
	Prompt that question
2. Obtain a response.
	Get a response from user
3. Score that response.
	Score the response
4. Counttheitemstaken:L=L+1
	Global variable L. 
5. Add the difficulties used: H = H + D
	Global variable totalDifficulties
6. If response incorrect, update item difficulty: D = D - 2/L

7. If response correct, update item difficulty: D = D + 2/L

8. If response correct, count right answers: R = R + 1
9. If not ready to decide to pass/fail, Go to step 2.

10. If ready to decide pass/fail, calculate wrong answers: W = L - R
11. Estimate measure: B = H/L + log(R/W)
12. Estimate standard error of the measure: S = [L/(R*W)]
13. Compare B with pass/fail standard T.
14. If(T-S)<B<(T+S),gotostep2.
15. If(B-S)>T,thenpass.
16. If(B+S)<T,thenfail.


References:
- Computer-Adaptive Testing:
A Methodology Whose Time Has Come.

*/

var fs = require('fs');
var readline = require('fs');


//JSON card bank

//Variables
// D = difficulty
// L = items taken
// H = Difficulties used
// R = Right answers 

var D = 0,
	L = 0,
	H = 0,
	R = 0;


// var itemBank = []; // Item Bank to choose from

// Dummy Data
var itemBank = [ 
	{
		question: "test1",
		difficulty: 9
	},
	{
		question: "test3",
		difficulty: 92
	},
	{
	  	question: "test4",
	  	difficulty: 19
	},		
	{
		question: "test2",
		difficulty: 14
	}
]; // Item Bank to choose from


var student = {}; //Student Object
var item = "";


/**
@Item properties:
Json
{
	question: "String",
	difficulty: Integer,
}

**/




/**
Request next candidate
-----
Initialize variables of that user to 0
From test bank, find next item closest to the difficulty D.

Set user's D to the actual D value of that item.
@param User
**/

var begin = function(){
	//Prompt for next candidate 
};


/**
@findItemInBank
@param D = target difficulty
Returns the item with the closest difficulty as the passed in parameter D.
**/

var findItemInBank = function(D){
	minVal = 999;
	minItem = "";
	for (item in itemBank){
		var difference = Math.abs(D - itemBank[item].difficulty)
		if (difference < minVal){
			minVal = difference;
			minItem = itemBank[item];
		}
	}

	return minItem
}


/**
@IRTAlgorithm
Function is Pi(Theta) = P(Xi = 1 | Theta) = ((Theta - Di) / (1 + (Theta - Di))).

Theta = probability of getting question i correct
Xi = Question i
Di = difficulty of question i

**/


var IRTAlgorithm = function(){

}


// ======================TEST CASES========================= //

result = findItemInBank(23);
console.log("Item difficulty closest to 23");
console.log(result.difficulty);
console.log(result.question);




