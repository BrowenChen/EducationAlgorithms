/*
Item Response Theory Algorithm
----------------
itemResponse.js

Intelligently provides a pre-assessment algorithm to determine where a student 
Should begin. 

These models return P(theta), the probability that a user gets this item correct, given the laten ability
of that user and the parameters for the item.

*/

/*
@raschModel
@params itemDifficulty latentAbility

Person parameter is latentAbility

Item Response Theory Model for each item. One Parameter Model, Rasch. P(theta) is assessment of a learners probability of attaining a correct
answer to this specific problem given an ability level. 

Rasch Model IMplementation
Equation => exp(theta - Bi) / 1 + exp(theta - Bi)

References:
Book Rasch Models

===========================TEST EXAMPLES===========================

itemDif = 

*/
	
var raschModel = function(itemDifficulty, latentAbility){
	result = ( Math.exp(latentAbility - itemDifficulty) / (1 + Math.exp(latentAbility - itemDifficulty)) ) 
	return result;
}

/*
@brinbaumModel

Two parameter IRT with discrimination variable ai. 


*/

var brinbaumModel = function(itemDifficulty, latentAbility, discrimination){

	var numerator = (Math.exp(discrimination * (latentAbility - itemDifficulty)));
	var denominator = 1 + (Math.exp(discrimination * (latentAbility - itemDifficulty)));

	result = (numerator / denominator);
	return result;

}



/*
@threeParamModel

Three parameter IRT with third variable ci for pseudoChance. Lower bound to account for 
guessing.

*/

var threeParamModel = function(itemDifficulty, latentAbility, discrimination, psuedoChance){

	numerator = (1 - psuedoChance);
	denominator = 1 + (Math.exp(-1.7 * (discrimination) * (latentAbility - itemDifficulty)));

	result = psuedoChance + (numerator / denominator);

	return result;

}






/*
@maximumLikelihood

Rasch model parameter updates
Page 55 of the book.
Maximum Likelihood Estimator, MLE of theta. 

*/




// ========================TESTING ================================

console.log("1 Parameter IRT");
console.log(raschModel(3, 2));

console.log("2 parameter IRT with no discrimination");
console.log(brinbaumModel(3,2, 1));

