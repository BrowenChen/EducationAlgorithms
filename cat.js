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
2. Find next item near difficulty, D.
3. Set D at the actual calibration of that item.

1. Administer that item.
2. Obtain a response.
3. Score that response.
4. Counttheitemstaken:L=L+1
5. Add the difficulties used: H = H + D
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
	H - 0,
	R = 0;


/**
Request next candidate
-----
Initialize variables of that user to 0
From test bank, find next item closest to the difficulty D.

Set user's D to the actually D value of that item.
@param User
**/



/**


**/



