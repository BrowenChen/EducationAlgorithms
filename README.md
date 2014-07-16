Overview


Here at Zzish, we strive to fundamentally change the process of traditional learning. We attempt to maximize one’s learning potential in the most efficient way possible. We approach learning with a comprehensive holistic strategy that integrates principles of psychological and cognitive approaches to human memory, and we integrate that knowledge with the current advancing state of technology that is available to us in the hopes of creating a platform that can truly change the way people learn.

One of the three primary algorithms that we implement in our Adaptive Learning Module is Spaced Reptition:


Here’s Ebbinghaus’s famous Forgetting Curve lays out the exact rate at which the human mind forgets newly acquired facts. Thankfully, he painstakingly went through literally thousands of “nonsense syllables”, (combinations of random letters that have no meaning such as DAX), to step around the abstraction that ties a word to a visual meaning, and test the rate at which human memory retains and forgets information. 

Spaced Repitition is based on this fact to reinforce the knowledge in an optimal way to increase long term potentiation of facts. 

What this means is that instead of trying to cram a ton of information with equal exposure and weight into your head, you focus on your weak links at the right time to optimize the learning process. Think of the process of trying to learn 150 different concepts for an exam. In order to cover all 150 concepts with an equal level of mastery, we would need a strategic way to target the weak links and de-emphasize the ones we already know well. The adage “failure is the best teacher” rings loud and clear here. In order to see true progress and rapid growth, you must struggle and step out of a comfort zone. The SR algorithm applies this concept as a generalized template for one method of making the most out of your limited time. This is just one of the few algorithms that goes into the Adaptive Module, with the hopes of providing this platform and knowledge to all edTech developers to rid the inefficiencies of traditional learning. 


Mechanisms of the Algorithm


In brief, I will describe the breakdown of the SuperMemo SM-2 spaced repetition algorithm. (We plan to develop ours to the current SM-10+ algorithm)

We take a body of knowledge, and split it into individual pieces to be studied. Be it concepts in math, science, philosophy, etc, any topic can be broken down into it’s fundamental “atoms” that contribute to the holistic web of knowledge that constitutes a subject.

All of the items are now assigned an “E-Factor”, which represents the “easiness” of a certain piece of data according to you. This will be updated over time.

Now we assign intervals at which each piece of information should be shown to you according to this forgetting curve, and continuously update these intervals for each item with  a function that models the forgetting curve.

After each exposure to the student, the Q-Factor is the quality of the response, usually a measure of 1-5 that determines the student’s level of understanding on that item. We can take in metrics such as duration spent on problem, incorrect guesses, or even in the future advanced neurotracking software to determine the physiological signs of frustration in a students face.

The E-Factor is then assessed and recalculated, and the algorithm repeats to truly adapt to the learner. 

Computerized Adaptive Testing


Alright, now we have a method of determining the optimal sequencing of knowledge ordering. The new question is what is the optimal point for a student start learning?


This can be accomplished through the CAT (Computerized Adaptive Testing) algorithm, used by many large-scale testing organizations such as the GMAT. 

Adaptive testing focuses on minimizing the time for diagnostic pre-testing, and attempts to accurately predict a unknown student’s level of ability given no prior knowledge on the student. From that starting point, we are able to then utilized Spaced Repitition to gather more details on the user and improve our personalized learning model over time.

Brief Description of Algorithm
Say that the developer of a math educational app creates a test bank of 100 multiple choices questions for the game. Each question is then assigned a difficulty level, predesignated by the developer, and we choose a small subset of questions to pre-test the user. 

We include item difficulty units, and child ability units to begin testing. The algorithm then adjusts the estimated child ability and slightly adjusts the item difficulty units based on the response of the test and finds the optimal ability level for a student when his pass/rate deviance falls below a target threshold. Here is an image of CAT in action:




Item Response Theory


The next algorithm in this sequence of educational learning algorithms is the IRT. The idea here is to aggregate a continuous model for estimating the probabilistic outcome of each response based on a student user. We can utilize IRT to extract data to be used for recommendation systems in a interconnected graph of separated topics and subjects, providing key insight for where the student should go from here given the details that we extract. The key here is that it tests proficiency/ability, and is used for a deeper personalized learning experience. 

How the Algorithm Works
Basically, there is an item characteristic curve graph. Given the student ability, it will predict the probability for whether a student will understand or fail that question. There can be multiple item characteristics to be represented in the graph, but we will stick to the one parameter model for simplicity. 

Item Discrimination is the concept of how the difficulty of a problem is relative to ability level. If there is no discrimination, the value of the item difficulty is a constant flat line across all ability levels. If there is a discrimination, this type of problem can be more useful in distinguishing the high-ability students from the low-ability students (think of an exam with an average percentage of 90% versus an exam with an average of 50%. The latter exam will allow smart students a greater room to demonstrate their intellect and differentiate themselves more than the average)




Experiences with Math Safari at Queens Manor Primary School


There’s absolutely no question that iPad games engage a student’s interests far greater than the traditional method of teacher-blackboard lectures. There is also no question that the brain absorbs information at a much faster rate when studying a topic you truly enjoy versus a dull topic a learner has no interest in. The simple engagement and classroom social aspect of Math Safari really shown through here as an effective way to change fundamental classroom learning. The sheer authentic looks of giddy excitement in the children’s eyes reaffirms the belief that Zzish is heading in the right direction.

Aside from the social competition aspect of Math Safari (which made the students excited to the point where they would yell across the room if they beat a fellow classmate of theirs), the adaptive learning aspect of Math Safari can lead to very promising advances in the way children learn. Not only is it personalized for each student, it takes much of the tedious administration aspects out of a teachers job and refocuses it on their primary objective: to inspire and pass on knowledge to kids through the joys of teaching. 

This was one of the biggest ideas that math safari and Zzish’s potential really reflected. The casual remark from a teacher who wasn’t a part of our classroom trial experiment said that she loved teaching and that passion drove her to become a teacher. Over the years however, that passion declined due to the administrative and monotonous work involved in teaching and the joy was barely there anymore. This is where I see a huge potential of what Zzish can provide. Not only will it affect the fundamental ways people of all levels learn, it will also provide teachers a resource to refocus on what truly matters as a teacher in a school. 




Why holistic learning? 


The human mind is a complex structure. The neural network system is equivalent to how gigantic cities are built. We attach meaning to abstract symbols such as letters (cell bodies/buildings), and correlate each symbol to other pieces of knowledge (axons/roads). The more roads that are available to access a certain building, the easier it is to get to that destination. To reinforce our constructs (cities), we must continue building these neural highways to connect these information with each other, which reinforces the students ability to recall highly connected pieces of information with greater ease than if one were to simply attempt rote memorization on an item. 

Think of what you had for lunch 28 days ago, and think back to your first breakup with a girl you loved. Which experience do you typically have better recollection of?

The way the human mind allocates and classifies information in the brain is through the amygdala and hippocampus. Our brains and body have evolved to be highly adaptive and efficient at what we do, which includes information processing. Therefore, we recognize that the more emotional and sensory impact an event has on you, the more important the brain thinks it should be. Synesthete's are the greatest examples of this idea at work. They unconsciously correlate every number to a color and sometimes even an opinion, so memorizing a string of random numbers is far less work for them than it is to us. 

A short example in mnemonics: Memorize the sequence 15 43 23 00. Those numbers alone have no meaning in your head other them being symbols for math. Now, by using the Dominic’s system, we correlate the numbers into letter pairs and then to objects to create a story. For my Dominic system, it would correlate to the letters “AE DC BC OO”. I would then transform that sequence into a memorable and vivid story in my mind, incorporating as much sensory detail as I can to reinforce memory. Something like this - “an American Eagle flew into the window of the white house in washington DC, and Jesus Chris (BC = before Christ) came to save the eagle while Ozzy Osburn was playing a rock concert right outside”. With practice, the translation would be automatic, allowing you to memorize rapid strings of information.


