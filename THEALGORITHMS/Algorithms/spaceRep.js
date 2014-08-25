/** 
  WebRecommend.js
   - Owen Chen
  -----------------------
  
  Description:
  Tool to add websites you like into a json database file. Applies 
  Spaced repitition to generate websites that you like and learns 
  based on your quality of response. 

  0 = Love it, show it more. 5 = hate it push it back. 

  Brief outline of SM-2 SR algorithm:
  1. Split into smallest possible items. 
  2. E-Factor of 2.5 initialized to all variables. 
  3. OF Matrix and E-Factor categories. Algorithm:
          OF(1,EF) = 4
          for n > 1 OF(n, EF) := EF

          Where OF(n, EF) - optimal factor for n-th repitition  and EF 
  4. OF Matrix to determine inter-repetition intervals. 
          I(n, EF) = OF( n, EF) * I(n-1, EF)
          ... Code Onbline
  5. Assess quality of repetition responses in 0-5 scale
  6. Modify E-factor According to formula
  7. After each rep, modify relevant entry of OF matrix. 
  8. if quality respobse is lower than 3 start repetition for the item from the beginning with no E-factor change
  9. Repeat all items under four in quality assement.


  -V 1.0
  Algorithm based off SuperMemo SM-2 Learning Algorithm as described:
  http://www.supermemo.com/english/ol/sm2.htm

  Changes to the algorithm:
  - E-Factor (easiness factor) changed to hardness factor. The higher 
  q, the less likely you'd want to see it again. Changes to dislike
  factor. 

  -webSite files are seperated by topic. Spaced repitition applies to the
  sites inside each topic, new JSON file with attributes created with 
  each topic.

  Updates needed:
  -If none of the cards match current date and nothing is updated, do another 
    runthrough
  -Add in Date Functionality

  Bug:

  ***Javascript runs asynchronous and synchronous code!!!!!!!!!!

**/

var fs = require('fs');
var readline = require('readline');

var siteFile = 'sites.json',
    quizList = [],
    quizTimer = 500,
    today = 0, //ADD IN DATE FUNCTIONALITY LATER
    topicDict = [],
    sites = [], //All of the sites of the CURRENT topic. Each an object 

    newSiteObjArr = []; //same as sites array above, but with site objects
    siteJSONs = {}, //All of the JSON decks for each category
    curSiteKey = 0;

    cardCounter = 0;
    curKeyIndex = []; //Get the key name for index _ of this topic

    todaySiteCountTotal = 0;

// today.setHours(0,0,0,0);

//Begin welcome screen
console.log("Welcome to your personalized web generator!\n" +
  "After each site respond as follows:\n" +
  "(0) Absolutely friggin' love it \n" +
  "(1) It's sweet, but some parts bug me\n" +
  "(2) Meh, it's alright\n" +
  "(3) Hmmmmm\n" +
  "(4) Ok, I just wasted 5 minutes of my life\n" +
  "(5) Never take me here again.");



function startTopicFile(file){

    console.log(file);
    console.log(checkExists(file));
    topicDict = JSON.parse(fs.readFileSync(file));

    // topicDict = JSON.parse(file);
    console.log("passed JSON parse");
    //Possibily Split this into a new function
    //Count the number of topics
    // console.log(Object.keys(topicDict));
    var subjects = Object.keys(topicDict); //Number of subjects
    var subCount = subjects.length; //Subject Count
    // console.log(subCount);
    if (subCount) {
      console.log("There are " + subCount + " site topics you can pick");
      console.log("Here are the options: ");
      console.log(subjects);
      getUserInput("Pick a topic... ", pickTopic)
    } else {
      console.log("Error or something");
    }


}


/*
  pick the Topic from root bookmark JSON file sites.json. This will create new 
  objects for each of the websites under Topic, and assign them attributes for 
  the Algorithm.

  @methodName pickTopic
  @param {String} user Inputed string
  @return 

*/
function pickTopic(line){
  console.log(line);
  // console.log(topicDict[line]);

  //Check if JSON data exists or not
  curSiteKey = String(line) + ".json";
  var exists = checkExists(curSiteKey);
  var sites = 0;
  if (exists == true){
    newSiteObjArr = readsiteFile(curSiteKey);
    console.log("This JSON already exists, overwrite newSiteObjArr");
    console.log(newSiteObjArr.length);
    // sitesCount = newSiteObjArr.length;

  } else { //Doesn't exist, make new
    sites = topicDict[line];

    newSiteObjArr = []; //Empty out the array
    for (var i in sites){
      var newSiteObj = {
        name: sites[i]
        }

      newSiteObjArr.push(newSiteObj);
      // sitesCount = newSiteObjArr.length;

    }

    
  }

  //Populating new sites array with objects
  // console.log(sites);
  //NEED TO MAKE OBJECTS TO POPULATE SITES ARRAY
  //------------------------------------
 //Now the website is newSiteObjArr.name, and it has properties


  // for (var i in sites){
  //   var siteTitle = sites[i];

  //   sites[i] = new Object()
  //   sites[i].name = "Owen"; 
  //   console.log(sites[i]);
  //   // sites[i] = var siteKey = new Object();
  // }

  //---------------------------------

  // console.log(sites);

  // console.log("All sites newwww " + sites);

  sitesCount = newSiteObjArr.length;

  console.log("Sites dictionary length!!! " + sitesCount);

  siteJSONs[String(line) + ".json"] = sites;
  // console.log(siteJSONs);
  curSiteKey = line + ".json";
  console.log(curSiteKey + " This is curSite Key");



  if (sitesCount){
    console.log("You have " + sitesCount + " of sites here");
    getUserInput("Press enter to start your recommendations..",startStopQuiz);
  } else {
    console.log("There are no sites here today");
  }




}


/*
  get's User Input. Takes in a question to prompt, and a next function. 

  @method getUserInput
  @param {String} String prompt
  @param {Function} Function for next
  @param {String} What is the card 
  @return 

*/
function getUserInput(question, next, card) {
  var rl = readline.createInterface(process.stdin, process.stdout);
  rl.setPrompt(question);
  rl.prompt();
  rl.on('line', function(line) {
    rl.close();

    if (!card) {

      console.log("Goes into the not card");
      next(line);
    } else {

      console.log("goes into next");
      next(line, card);
    }
  });
}


/*
  Starts the Quizzing. Calls getNextCard

  @method startStopQuix
  @param {String} takes in user input
  @return
*/
function startStopQuiz(line) {

  console.log("Start STOP quiz line " + line);
  if (line.trim() === "exit") {
    return;
  } else {
    var count = sitesCount;
    todaySiteCountTotal = count;
    console.log("countttttt " + count);
    if (count) {
      cardCounter = 0;

      console.log("Going into getNextCard " + newSiteObjArr[0].name);
      getNextCard(newSiteObjArr[0]);
      
    }
  }
}

/*
  Returns the amount of website that is due today. Fix this

  @method cardQuizCount()
  @param 
  @return {Int} the Count
*/
function cardQuizCount() {
  var count = 0;
  for (var i=0; i<sites.length; i++) {
      var c = sites[i];
      var d = new Date(c.nextDate);
      if (c.interval === 0 || !c.interval || d.getTime() === today.getTime()) {
        count++;
      }
  }

  // console.log("COUNT IS "+ count);
  return count;
}

/*
  Get the next card. Calls cardQuizCount and getUserInput

  @method getNextCard()
  @param {String} card name
  @return 
*/
function getNextCard(card) {
    // console.log(jsonFile);
    // console.log(line);
    console.log(card.name + " this is the card");
    
    if (!(card.name)) {
      console.log("no card, write siteFile");
      writesiteFile(curSiteKey); //Save to file

      // var count = cardQuizCount(); //
      if (count) {
        getUserInput("Done. Hit enter to repeat " + count + " sites graded 3 or lower, or type exit to finish", startStopQuiz);
      } else {
        getUserInput("Done for today. Don't forget to come back tomorrow. :) (enter to exit)", startStopQuiz);
      }
      return;
    }
    //Set Defaults if new card. We need each card to have it's own
    //Properties somehow
    //Javascript objects have attributes. var card = {nextDate: 0}
    //DEBUG THIS
   //----------------------------

    if (!card.nextDate) { card.nextDate = 0; console.log(card.nextDate); }
    if (!card.prevDate) { card.prevDate = 0; }
    if (!card.interval) { card.interval = 0; }
    if (!card.reps) {  card.reps = 0; }
    if (!card.EF) { card.EF = 2.5; }
    if (!card.thisDate) {card.thisDate = 0; }

    //----------------------------
    console.log("Setting defaults");

    // var nextDate = new Date(card.nextDate); //convert to comparable date type
    var nextDate = card.nextDate; //convert to comparable date type
    var thisDate = card.thisDate
    console.log(nextDate + " next");
    console.log(today + " today");
    card.thisDate++;
    // console.log(nextDate + " the next Date");
    if (nextDate <= thisDate) {
      console.log("nextDate is less than today");
      quizCard(card);
    } else {
      cardCounter++; 
      console.log(cardCounter + " cardCounter for today");

      if (cardCounter >= todaySiteCountTotal){
        console.log("End of sites here");
        console.log(curSiteKey + " This is the site key");
        writesiteFile(curSiteKey); //Save to file
        return;      }



      console.log("Update cardCounter");
      console.log("sites [cardCounter] " + sites[cardCounter].name);
      getNextCard(sites[cardCounter]);
    }


}

/*

  Assesses the quality of a website's likeability according to the user

  @method quizCard
  @param {String} card name
  @return 
*/
function quizCard(card) {
    console.log("Site: " + card.name);
    setTimeout(function() {
      console.log("How did you like it?");
      getUserInput("Grade> ", updateCard, card);
    }, quizTimer);
}

/*
  Update card attributes for the website.

  @method updateCard
  @param {String} user input, card name
  @return 
*/
function updateCard(line, card) {
  var grade = parseInt(line, 10);
  if (grade <= 5 && grade >= 0) {
    calcIntervalEF(card, grade);
    cardCounter++;
    console.log(todaySiteCountTotal + " LASDKALSDKA  " + cardCounter)
    if (cardCounter >= todaySiteCountTotal){
      console.log("End of sites here");
      writesiteFile(curSiteKey); //Save to file
      return;
    } else {
      getNextCard(newSiteObjArr[cardCounter]);
    }

  } else { //Bad input
    getUserInput("Please enter 0-5 for... " + card.side2 + ": ", updateCard, card);
  }
}


/*
  Brief description of SM-2 Algorithm. EF (easiness factor is rating for how hard)
  This is changed to dislike factor.
  Grade on scale of 0-5.
  Update attributes according to spaced repitition. 
*/
function calcIntervalEF(card, grade) {
  var oldEF = card.EF,
      newEF = 0;
      // nextDate = new Date(today);

  console.log(card.nextDate + " The next date of card " + card.name);
  card.nextDate = card.thisDate; 



  if (grade >= 0 && grade <=5){ //Dont repeat based on the grade
    newEF = oldEF + (0.1 - (5-grade)*(0.08+(5-grade)*0.02));
    if (newEF < 1.3) { // 1.3 is the minimum E-F
      card.EF = 1.3;
    } else {
      card.EF = newEF;
    }

    card.reps = card.reps + 1;


    switch (card.reps) {
      case 1:
        card.interval = 1;
        break;
      case 2:
        card.interval = 6;
        break;
      default:
        card.interval = Math.ceil((card.reps - 1) * card.EF);
        break;
    }
  }

  card.nextDate = card.thisDate + card.interval;
  // card.nextDate = nextDate;
}


/* 
Asychronous version of readFile

@method checkExists
@param {String} fileName
@return {Boolean} true or false

*/



var checkExists = function(fileName){
    var exists = false;
    try {
        var contentSync = JSON.parse(fs.readFileSync(fileName));
        exists = true;
    } catch (err) {
        // console.error(err);
        console.log("This is an error");
    }
    // console.log(exists);
    return exists;
}



/*
  reads the file and parses JSON file into js readable variable.
  Stores the variable 

  @pmethod readsiteFile
  @param {String} file
  @return {Object} variable dictionary of JSON. 

*/
function readsiteFile(file) {


  try{
    var exists = checkExists(file);
    // console.log("Exists");
    if (file == siteFile){
      console.log("starting startTopicFile");
      startTopicFile(siteFile);

    } else { //Not the main topic file
      if (exists == true){
        //Subject already has spacedRep parameters
        sites = JSON.parse(fs.readFileSync(file));
        return sites
      } else {
        //No spaced repitition parameters
        //Create a new JSON file
        console.log("No existing JSON ");
      }
    }

  } catch (err){
    console.error(err);
    console.log("Caught an error");
  }


  //---------------------------------------

  // fs.readFileSync(file, function(err, data) { //Changed to readFileSync
  //   if (err) throw err;

  //   var exists = checkExists(file);

  //   if (file == siteFile){
  //     startTopicFile(siteFile);
  //   } else{ //Not the main topic file
  //     if (exists == true){
  //       //Subject already has spacedRep parameters
  //       sites = JSON.parse(data);
  //       return sites;
  //     } else {
  //       //No spaced Repitition happened here
  //       //Create new JSON file. 
  //       console.log("No existing JSON spacedRep data");
  //       console.log("It actually should never come here");
  //       return;
  //     }
  //   }

    //---------------------------------------

    // topicDict = JSON.parse(data);
    // //Possibily Split this into a new function
    // //Count the number of topics
    // // console.log(Object.keys(topicDict));
    // var subjects = Object.keys(topicDict); //Number of subjects
    // var subCount = subjects.length; //Subject Count
    // // console.log(subCount);


    // if (subCount) {
    //   console.log("There are " + subCount + " site topics you can pick");
    //   console.log("Here are the options: ");
    //   console.log(subjects);
    //   getUserInput("Pick a topic... ", pickTopic)
    // } else {
    //   console.log("Error or something");
    // }

  // });
}

readsiteFile(siteFile);




/*
  Writes back to specified JSON file.

  @method writesiteFile 
  @param {String} name of JSON file you wanna write into

*/
function writesiteFile(siteFile) {
  fs.writeFile(siteFile, JSON.stringify(newSiteObjArr, null, 2), function(err) {
    if (err) throw err;
    console.log("\nProgress saved back to file." + siteFile);
  });
}