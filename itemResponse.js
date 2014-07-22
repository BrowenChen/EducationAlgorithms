/*
Item Response Theory Algorithm
----------------
itemResponse.js

Intelligently provides a pre-assessment algorithm to determine where a student 
Should begin. 

*/

/*
@raschModel
@params itemDifficulty latentAbility

Person parameter is latentAbility

Item Response Theory Model for each item. One Parameter Model, Rasch. P(theta) is assessment of a learners probability of attaining a correct
answer to this specific problem given an ability level. 

Rasch Model IMplementation
Equation => exp(theta - Bi) / 1 + exp(theta - Bi)

===========================TEST EXAMPLES===========================

itemDif = 

*/
	
var raschModel = function(itemDifficulty, latentAbility){

	result = ( Math.exp(latentAbility - itemDifficulty) / (1 + Math.exp(latentAbility - itemDifficulty)) ) 
	return result;
}


console.log(raschModel(2, 3));
/*
@maximumLikelihood

Rasch model parameter updates

*/
