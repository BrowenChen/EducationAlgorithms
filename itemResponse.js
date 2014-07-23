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

	theta = scaleDifficulty(latentAbility);
	b = scaleDifficulty(itemDifficulty);


	result = ( Math.exp(theta - b) / (1 + Math.exp(theta - b)) ) 
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
@scaleDifficulty
@params value to convert, range1 [...], range2 [.....]

Scales item difficulty from 1-100 to -3 to 3 for raschModel.

*/

function scaleDifficulty( value) { 
	r1 = [1, 100];
	r2 = [-3, 3];
    return ( value - r1[ 0 ] ) * ( r2[ 1 ] - r2[ 0 ] ) / ( r1[ 1 ] - r1[ 0 ] ) + r2[ 0 ];
}


/*
@logOddsModel
@param raschModel

Returns logit units to estimate learner probability
Equation: e ^ ( probability of success / probability of failure) => e ^ (raschModel) / (1 - raschModel)

*/

var logOddsModel = function(raschProbability){
	logit = Math.exp( raschProbability / (1 - raschProbability));

	return logit;
}



// ================ ITEM BANK CALIBRATION UPDATE ========================

/*
@maximumLikelihood

Rasch model parameter updates
Page 55 of the book.
Maximum Likelihood Estimator, MLE of theta. 

*/




// ========================TESTING ================================

console.log("1 Parameter IRT");
console.log(raschModel(3, 2));

console.log("2 parameter IRT with variable discrimination");
console.log(brinbaumModel(3,2, 1));

console.log("3 parameter IRT ");





console.log("Rasch model testing ~~~~ ");
console.log(raschModel(70, 25));
console.log(raschModel(92,98));
console.log(raschModel(24,62));


console.log("scaled difficulty " + scaleDifficulty(80));

console.log('Rasch model of 72 and 70 converted probability ' +  raschModel(scaleDifficulty(80), scaleDifficulty(61)));

console.log("Testing logits ~~~~ ");

console.log(logOddsModel(raschModel(80, 61)) + " logits ");



