'use strict';

/* Controllers */

var app = angular.module('myApp.controllers', []).
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


  controller('IRTCtrl', function ($scope, $http) {



//AVATARS
		$scope.avatarName = "Avatar";

    $scope.avatarSelect = function(name){
    	this.avatarName = name;	
    }

//AVATARS

    // $scope.name = "Owen";
    $scope.test = function(){
      alert("testing");
    }

    $scope.title = "Predict";
    $scope.exampleData = [ [ 1025409600000 , 0] , [ 1028088000000 , -6.3382185140371] , [ 1030766400000 , -5.9507873460847] , [ 1033358400000 , -11.569146943813] , [ 1036040400000 , -5.4767332317425] , [ 1038632400000 , 0.50794682203014] , [ 1041310800000 , -5.5310285460542] , [ 1043989200000 , -5.7838296963382] , [ 1046408400000 , -7.3249341615649] , [ 1049086800000 , -6.7078630712489] , [ 1051675200000 , 0.44227126150934] , [ 1054353600000 , 7.2481659343222] , [ 1056945600000 , 9.2512381306992] , [ 1059624000000 , 11.341210982529] , [ 1062302400000 , 14.734820409020] , [ 1064894400000 , 12.387148007542] , [ 1067576400000 , 18.436471461827] , [ 1070168400000 , 19.830742266977] , [ 1072846800000 , 22.643205829887] , [ 1075525200000 , 26.743156781239] , [ 1078030800000 , 29.597478802228] , [ 1080709200000 , 30.831697585341] , [ 1083297600000 , 28.054068024708] , [ 1085976000000 , 29.294079423832] , [ 1088568000000 , 30.269264061274] , [ 1091246400000 , 24.934526898906] , [ 1093924800000 , 24.265982759406] , [ 1096516800000 , 27.217794897473] , [ 1099195200000 , 30.802601992077] , [ 1101790800000 , 36.331003758254] , [ 1104469200000 , 43.142498700060] , [ 1107147600000 , 40.558263931958] , [ 1109566800000 , 42.543622385800] , [ 1112245200000 , 41.683584710331] , [ 1114833600000 , 36.375367302328] , [ 1117512000000 , 40.719688980730] , [ 1120104000000 , 43.897963036919] , [ 1122782400000 , 49.797033975368] , [ 1125460800000 , 47.085993935989] , [ 1128052800000 , 46.601972859745] , [ 1130734800000 , 41.567784572762] , [ 1133326800000 , 47.296923737245] , [ 1136005200000 , 47.642969612080] , [ 1138683600000 , 50.781515820954] , [ 1141102800000 , 52.600229204305] , [ 1143781200000 , 55.599684490628] , [ 1146369600000 , 57.920388436633] , [ 1149048000000 , 53.503593218971] , [ 1151640000000 , 53.522973979964] , [ 1154318400000 , 49.846822298548] , [ 1156996800000 , 54.721341614650] , [ 1159588800000 , 58.186236223191] , [ 1162270800000 , 63.908065540997] , [ 1164862800000 , 69.767285129367] , [ 1167541200000 , 72.534013373592] , [ 1170219600000 , 77.991819436573] , [ 1172638800000 , 78.143584404990] , [ 1175313600000 , 83.702398665233] , [ 1177905600000 , 91.140859312418] , [ 1180584000000 , 98.590960607028] , [ 1183176000000 , 96.245634754228] , [ 1185854400000 , 92.326364432615] , [ 1188532800000 , 97.068765332230] , [ 1191124800000 , 105.81025556260] , [ 1193803200000 , 114.38348777791] , [ 1196398800000 , 103.59604949810] , [ 1199077200000 , 101.72488429307] , [ 1201755600000 , 89.840147735028] , [ 1204261200000 , 86.963597532664] , [ 1206936000000 , 84.075505208491] , [ 1209528000000 , 93.170105645831] , [ 1212206400000 , 103.62838083121] , [ 1214798400000 , 87.458241365091] , [ 1217476800000 , 85.808374141319] , [ 1220155200000 , 93.158054469193] , [ 1222747200000 , 65.973252382360] , [ 1225425600000 , 44.580686638224] , [ 1228021200000 , 36.418977140128] , [ 1230699600000 , 38.727678144761] , [ 1233378000000 , 36.692674173387] , [ 1235797200000 , 30.033022809480] , [ 1238472000000 , 36.707532162718] , [ 1241064000000 , 52.191457688389] , [ 1243742400000 , 56.357883979735] , [ 1246334400000 , 57.629002180305] , [ 1249012800000 , 66.650985790166] , [ 1251691200000 , 70.839243432186] , [ 1254283200000 , 78.731998491499] , [ 1256961600000 , 72.375528540349] , [ 1259557200000 , 81.738387881630] , [ 1262235600000 , 87.539792394232] , [ 1264914000000 , 84.320762662273] , [ 1267333200000 , 90.621278391889] , [ 1270008000000 , 102.47144881651] , [ 1272600000000 , 102.79320353429] , [ 1275278400000 , 90.529736050479] , [ 1277870400000 , 76.580859994531] , [ 1280548800000 , 86.548979376972] , [ 1283227200000 , 81.879653334089] , [ 1285819200000 , 101.72550015956] , [ 1288497600000 , 107.97964852260] , [ 1291093200000 , 106.16240630785] , [ 1293771600000 , 114.84268599533] , [ 1296450000000 , 121.60793322282] , [ 1298869200000 , 133.41437346605] , [ 1301544000000 , 125.46646042904] , [ 1304136000000 , 129.76784954301] , [ 1306814400000 , 128.15798861044] , [ 1309406400000 , 121.92388706072] , [ 1312084800000 , 116.70036100870] , [ 1314763200000 , 88.367701837033] , [ 1317355200000 , 59.159665765725] , [ 1320033600000 , 79.793568139753] , [ 1322629200000 , 75.903834028417] , [ 1325307600000 , 72.704218209157] , [ 1327986000000 , 84.936990804097] , [ 1330491600000 , 93.388148670744]];
    
    $scope.loadString = "Item Difficulty ";
    $scope.difVal = null;
    $scope.scaled = null;
    $scope.raschResult = null;
    $scope.raschData = [];

    $scope.scaleDifficulty = function(value){
      var r1 = [1, 100];
      var r2 = [-3, 3];
      var scaled = ( value - r1[ 0 ] ) * ( r2[ 1 ] - r2[ 0 ] ) / ( r1[ 1 ] - r1[ 0 ] ) + r2[ 0 ];
      return scaled;
    }

    $scope.raschModel = function(itemDifficulty, latentAbility){


      var theta = this.scaleDifficulty(latentAbility);
      var b = this.scaleDifficulty(itemDifficulty);     
      var raschResult = ( Math.exp(theta - b) / (1 + Math.exp(theta - b)) ) * 100 



      // alert(theta);
      // alert(b);       
      // alert(this.raschResult);

      return raschResult;

    }

    $scope.startRasch = function(){
      this.loadString += this.difVal;

      this.raschData.push(this.loadString);
      for (var i = 0; i < 100; i++){
        var pushResult = this.raschModel(this.difVal, i)
        
        this.raschData.push(pushResult)
      }   
      console.log(this.raschData[0]);  
    }


    $scope.loadChart = function(){ 
            alert(this.difVal);
            this.startRasch();
            chart.load({
              columns: [
              this.raschData

              ]
            });
}

    $scope.exampleDataHard = [   
         {
             "key": "Item Difficulty: 70",
             "values": [ [ 1, 0 ],
  [ 1, 1 ],
  [ 1, 2 ],
  [ 1, 3 ],
  [ 1, 4 ],
  [ 1, 5 ],
  [ 2, 6 ],
  [ 2, 7 ],
  [ 2, 8 ],
  [ 2, 9 ],
  [ 2, 10 ],
  [ 2, 11 ],
  [ 2, 12 ],
  [ 3, 13 ],
  [ 3, 14 ],
  [ 3, 15 ],
  [ 3, 16 ],
  [ 3, 17 ],
  [ 4, 18 ],
  [ 4, 19 ],
  [ 4, 20 ],
  [ 4, 21 ],
  [ 5, 22 ],
  [ 5, 23 ],
  [ 5, 24 ],
  [ 6, 25 ],
  [ 6, 26 ],
  [ 6, 27 ],
  [ 7, 28 ],
  [ 7, 29 ],
  [ 8, 30 ],
  [ 8, 31 ],
  [ 9, 32 ],
  [ 9, 33 ],
  [ 10, 34 ],
  [ 10, 35 ],
  [ 11, 36 ],
  [ 11, 37 ],
  [ 12, 38 ],
  [ 13, 39 ],
  [ 13, 40 ],
  [ 14, 41 ],
  [ 15, 42 ],
  [ 16, 43 ],
  [ 17, 44 ],
  [ 18, 45 ],
  [ 18, 46 ],
  [ 19, 47 ],
  [ 20, 48 ],
  [ 21, 49 ],
  [ 22, 50 ],
  [ 24, 51 ],
  [ 25, 52 ],
  [ 26, 53 ],
  [ 27, 54 ],
  [ 28, 55 ],
  [ 29, 56 ],
  [ 31, 57 ],
  [ 32, 58 ],
  [ 33, 59 ],
  [ 35, 60 ],
  [ 36, 61 ],
  [ 38, 62 ],
  [ 39, 63 ],
  [ 41, 64 ],
  [ 42, 65 ],
  [ 43, 66 ],
  [ 45, 67 ],
  [ 46, 68 ],
  [ 48, 69 ],
  [ 50, 70 ],
  [ 51, 71 ],
  [ 53, 72 ],
  [ 54, 73 ],
  [ 56, 74 ],
  [ 57, 75 ],
  [ 58, 76 ],
  [ 60, 77 ],
  [ 61, 78 ],
  [ 63, 79 ],
  [ 64, 80 ],
  [ 66, 81 ],
  [ 67, 82 ],
  [ 68, 83 ],
  [ 70, 84 ],
  [ 71, 85 ],
  [ 72, 86 ],
  [ 73, 87 ],
  [ 74, 88 ],
  [ 75, 89 ],
  [ 77, 90 ],
  [ 78, 91 ],
  [ 79, 92 ],
  [ 80, 93 ],
  [ 81, 94 ],
  [ 81, 95 ],
  [ 82, 96 ],
  [ 83, 97 ],
  [ 84, 98 ],
  [ 85, 99 ] ]
},

          {
             "key": "Item Difficulty: 90",
             "area": true,
             "values": [[ 0, 0 ],
  [ 0, 1 ],
  [ 0, 2 ],
  [ 0, 3 ],
  [ 0, 4 ],
  [ 0, 5 ],
  [ 0, 6 ],
  [ 0, 7 ],
  [ 0, 8 ],
  [ 0, 9 ],
  [ 0, 10 ],
  [ 0, 11 ],
  [ 0, 12 ],
  [ 0, 13 ],
  [ 0, 14 ],
  [ 1, 15 ],
  [ 1, 16 ],
  [ 1, 17 ],
  [ 1, 18 ],
  [ 1, 19 ],
  [ 1, 20 ],
  [ 1, 21 ],
  [ 1, 22 ],
  [ 1, 23 ],
  [ 1, 24 ],
  [ 1, 25 ],
  [ 2, 26 ],
  [ 2, 27 ],
  [ 2, 28 ],
  [ 2, 29 ],
  [ 2, 30 ],
  [ 2, 31 ],
  [ 2, 32 ],
  [ 3, 33 ],
  [ 3, 34 ],
  [ 3, 35 ],
  [ 3, 36 ],
  [ 3, 37 ],
  [ 4, 38 ],
  [ 4, 39 ],
  [ 4, 40 ],
  [ 4, 41 ],
  [ 5, 42 ],
  [ 5, 43 ],
  [ 5, 44 ],
  [ 6, 45 ],
  [ 6, 46 ],
  [ 6, 47 ],
  [ 7, 48 ],
  [ 7, 49 ],
  [ 8, 50 ],
  [ 8, 51 ],
  [ 9, 52 ],
  [ 9, 53 ],
  [ 10, 54 ],
  [ 10, 55 ],
  [ 11, 56 ],
  [ 11, 57 ],
  [ 12, 58 ],
  [ 13, 59 ],
  [ 13, 60 ],
  [ 14, 61 ],
  [ 15, 62 ],
  [ 16, 63 ],
  [ 17, 64 ],
  [ 18, 65 ],
  [ 18, 66 ],
  [ 19, 67 ],
  [ 20, 68 ],
  [ 21, 69 ],
  [ 22, 70 ],
  [ 24, 71 ],
  [ 25, 72 ],
  [ 26, 73 ],
  [ 27, 74 ],
  [ 28, 75 ],
  [ 29, 76 ],
  [ 31, 77 ],
  [ 32, 78 ],
  [ 33, 79 ],
  [ 35, 80 ],
  [ 36, 81 ],
  [ 38, 82 ],
  [ 39, 83 ],
  [ 41, 84 ],
  [ 42, 85 ],
  [ 43, 86 ],
  [ 45, 87 ],
  [ 46, 88 ],
  [ 48, 89 ],
  [ 50, 90 ],
  [ 51, 91 ],
  [ 53, 92 ],
  [ 54, 93 ],
  [ 56, 94 ],
  [ 57, 95 ],
  [ 58, 96 ],
  [ 60, 97 ],
  [ 61, 98 ],
  [ 63, 99 ] ]
}];


    //Data given in will be list of dictionaries. Values will be list of lists, x value y value.
    $scope.exampleDataEasy = [

          {
              "key": "Item Difficulty: 20",
              "values": [[ 22, 0 ],
  [ 24, 1 ],
  [ 25, 2 ],
  [ 26, 3 ],
  [ 27, 4 ],
  [ 28, 5 ],
  [ 29, 6 ],
  [ 31, 7 ],
  [ 32, 8 ],
  [ 33, 9 ],
  [ 35, 10 ],
  [ 36, 11 ],
  [ 38, 12 ],
  [ 39, 13 ],
  [ 41, 14 ],
  [ 42, 15 ],
  [ 43, 16 ],
  [ 45, 17 ],
  [ 46, 18 ],
  [ 48, 19 ],
  [ 50, 20 ],
  [ 51, 21 ],
  [ 53, 22 ],
  [ 54, 23 ],
  [ 56, 24 ],
  [ 57, 25 ],
  [ 58, 26 ],
  [ 60, 27 ],
  [ 61, 28 ],
  [ 63, 29 ],
  [ 64, 30 ],
  [ 66, 31 ],
  [ 67, 32 ],
  [ 68, 33 ],
  [ 70, 34 ],
  [ 71, 35 ],
  [ 72, 36 ],
  [ 73, 37 ],
  [ 74, 38 ],
  [ 75, 39 ],
  [ 77, 40 ],
  [ 78, 41 ],
  [ 79, 42 ],
  [ 80, 43 ],
  [ 81, 44 ],
  [ 81, 45 ],
  [ 82, 46 ],
  [ 83, 47 ],
  [ 84, 48 ],
  [ 85, 49 ],
  [ 86, 50 ],
  [ 86, 51 ],
  [ 87, 52 ],
  [ 88, 53 ],
  [ 88, 54 ],
  [ 89, 55 ],
  [ 89, 56 ],
  [ 90, 57 ],
  [ 90, 58 ],
  [ 91, 59 ],
  [ 91, 60 ],
  [ 92, 61 ],
  [ 92, 62 ],
  [ 93, 63 ],
  [ 93, 64 ],
  [ 93, 65 ],
  [ 94, 66 ],
  [ 94, 67 ],
  [ 94, 68 ],
  [ 95, 69 ],
  [ 95, 70 ],
  [ 95, 71 ],
  [ 95, 72 ],
  [ 96, 73 ],
  [ 96, 74 ],
  [ 96, 75 ],
  [ 96, 76 ],
  [ 96, 77 ],
  [ 97, 78 ],
  [ 97, 79 ],
  [ 97, 80 ],
  [ 97, 81 ],
  [ 97, 82 ],
  [ 97, 83 ],
  [ 97, 84 ],
  [ 98, 85 ],
  [ 98, 86 ],
  [ 98, 87 ],
  [ 98, 88 ],
  [ 98, 89 ],
  [ 98, 90 ],
  [ 98, 91 ],
  [ 98, 92 ],
  [ 98, 93 ],
  [ 98, 94 ],
  [ 98, 95 ],
  [ 99, 96 ],
  [ 99, 97 ],
  [ 99, 98 ],
  [ 99, 99 ]  ]
         },    
         {
              "key": "Item Difficulty: 50",
              "values": [[ 4, 0 ],
  [ 4, 1 ],
  [ 5, 2 ],
  [ 5, 3 ],
  [ 5, 4 ],
  [ 6, 5 ],
  [ 6, 6 ],
  [ 6, 7 ],
  [ 7, 8 ],
  [ 7, 9 ],
  [ 8, 10 ],
  [ 8, 11 ],
  [ 9, 12 ],
  [ 9, 13 ],
  [ 10, 14 ],
  [ 10, 15 ],
  [ 11, 16 ],
  [ 11, 17 ],
  [ 12, 18 ],
  [ 13, 19 ],
  [ 13, 20 ],
  [ 14, 21 ],
  [ 15, 22 ],
  [ 16, 23 ],
  [ 17, 24 ],
  [ 18, 25 ],
  [ 18, 26 ],
  [ 19, 27 ],
  [ 20, 28 ],
  [ 21, 29 ],
  [ 22, 30 ],
  [ 24, 31 ],
  [ 25, 32 ],
  [ 26, 33 ],
  [ 27, 34 ],
  [ 28, 35 ],
  [ 29, 36 ],
  [ 31, 37 ],
  [ 32, 38 ],
  [ 33, 39 ],
  [ 35, 40 ],
  [ 36, 41 ],
  [ 38, 42 ],
  [ 39, 43 ],
  [ 41, 44 ],
  [ 42, 45 ],
  [ 43, 46 ],
  [ 45, 47 ],
  [ 46, 48 ],
  [ 48, 49 ],
  [ 50, 50 ],
  [ 51, 51 ],
  [ 53, 52 ],
  [ 54, 53 ],
  [ 56, 54 ],
  [ 57, 55 ],
  [ 58, 56 ],
  [ 60, 57 ],
  [ 61, 58 ],
  [ 63, 59 ],
  [ 64, 60 ],
  [ 66, 61 ],
  [ 67, 62 ],
  [ 68, 63 ],
  [ 70, 64 ],
  [ 71, 65 ],
  [ 72, 66 ],
  [ 73, 67 ],
  [ 74, 68 ],
  [ 75, 69 ],
  [ 77, 70 ],
  [ 78, 71 ],
  [ 79, 72 ],
  [ 80, 73 ],
  [ 81, 74 ],
  [ 81, 75 ],
  [ 82, 76 ],
  [ 83, 77 ],
  [ 84, 78 ],
  [ 85, 79 ],
  [ 86, 80 ],
  [ 86, 81 ],
  [ 87, 82 ],
  [ 88, 83 ],
  [ 88, 84 ],
  [ 89, 85 ],
  [ 89, 86 ],
  [ 90, 87 ],
  [ 90, 88 ],
  [ 91, 89 ],
  [ 91, 90 ],
  [ 92, 91 ],
  [ 92, 92 ],
  [ 93, 93 ],
  [ 93, 94 ],
  [ 93, 95 ],
  [ 94, 96 ],
  [ 94, 97 ],
  [ 94, 98 ],
  [ 95, 99 ] ]
          }

         
     ];   







  }).

  controller('AlgCtrl', function ($scope, $http) {

		$scope.avatarName = "Avatar";

		$scope.avatarList = [
						{name: "'Dog'",
							order: 1,
							img: "/imgs/dog.png"
						},
						{name: "'Hippo'",
							order: 2,
							img: "/imgs/hippo.png"
						},
						{name: "'Monkey'",
							order: 3,
							img: "/imgs/monkey.png"
						},
						{name: "'Macaw'",
							order: 4,
							img: "/imgs/macaw.png"
						}						
		];
//def getValueOf(k, L):
  //  for d in L:
    //    if k in d:
      //      return d[k]


    $scope.avatarSelect = function(name){
    	
    	this.avatarName = name;	

    }

    //Change the order of the avatar
    $scope.changeValOf = function(avatarName, order){

    	
     		for (var d in this.avatarList){
     			

     			if (("'" + avatarName + "'") == this.avatarList[d]['name']){



     				
     				

     				var goUp = false;
     				var curOrder = this.avatarList[d]['order'];

     				if (this.avatarList[d]['order'] > order){
     					goUp = true;
     				}


     				console.log(this.avatarList[d]['order']);
     				//$scope.avatarList[d]['order'] = order;
     				//console.log($scope.avatarList[d]['order']);
     				
     				this.updateOrders(order, curOrder, goUp, d);
     				//Change order number,
     				// Update all the rest of the order numbers after 

     			}
     		}
     	}
    
    $scope.updateOrders = function(targetOrder, curOrder, goUp, curIndex){
    	//if true, 
    	
    	
    	if (goUp == true){

	    	for (var d in this.avatarList){
	    		if ((this.avatarList[d]['order'] >= targetOrder) && (this.avatarList[d]['order'] < curOrder) ){
	    			
	    			this.avatarList[d]['order'] += 1;
	    		}

	    		else if (this.avatarList[d]['order'] == curOrder){
	    			console.log("changing current Oder");
	    			while (this.avatarList[d]['order'] > targetOrder){
	    				this.avatarList[d]['order'] -= 1;
	    			}

	    		}
	    	}
	    }


    	else if (goUp == false){
		
	    	for (var d in this.avatarList){
	    		if ((this.avatarList[d]['order'] <= targetOrder) && (this.avatarList[d]['order'] > curOrder) ){
	    			
	    			this.avatarList[d]['order'] -= 1;
	    		}

	    		else if (this.avatarList[d]['order'] == curOrder){
	    			console.log("changing current Oder");
	    			while (this.avatarList[d]['order'] < targetOrder){
	    				this.avatarList[d]['order'] += 1;
	    			}

	    		}	    		
	    	}	    	

    	}


    	for (var i in this.avatarList){
    		console.log(this.avatarList[i]['order']);
    	}




    }

    $scope.sort = function(avatarName, interestLevel){
    	for (var i in this.avatarList){
    		console.log(this.avatarList[i]['order']);
    	}




	
	
      //if interest level is 0-2, position 4
      //if interest level is 3 position 3
      //if interest level is 4 position 2
      //ifinterest is 5, position 1
      switch(interestLevel){
        case 0 || 1:
          this.changeValOf(avatarName, 4);
          
          //Pass in new order number 
          break;
        case 2 || 3:
        	this.changeValOf(avatarName, 3);
        	

        	break;
        case 4:
        	this.changeValOf(avatarName, 2);
        	
        	//this.avatarList["'" + avatarName + "'"]['order'] = 2;
        	
        	break
        case 5: 
        	this.changeValOf(avatarName, 1);
        	
        	break
      }    




    }	



    // write Ctrl here
    $scope.start = true;

    //Slider Controls
    $scope.value = 10;
    $scope.value2 = 10;

    $scope.sliderVal = 2;

    $scope.change = function() {

        console.log("change", this.value);
        
    };
    $scope.response = function(avatarName, interest){
      this.sliderVal = interest;
      this.sort(avatarName, this.sliderVal)
      
      

    }
    //Slider Controls





    $scope.title = "WebRec";
    $scope.sites = [];
    $scope.sitesRemaining = 0;
    $scope.formData = {};

    $scope.showSite = "www.google.com";

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

    $scope.remover = function() {
        var arr = this.sites;
        var item = "";
        for(var i = arr.length; i--;) {
            if(arr[i] === item) {
                arr.splice(i, 1);
            }
        }
    }

    $scope.test = function(){
      console.log(this.sites)
      console.log("test");
    }

    // when landing on the page, get all todos and show them
    $http.get('/api/site')
      .success(function(data) {
        $scope.sites = data; //Adding the todos here
        $scope.sitesRemaining = data.length;
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

          $scope.sitesRemaining = data.length;
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

          // alert(id);
          $scope.sites = data;
          $scope.sitesRemaining = data.length;
          console.log(data);
        })
        .error(function(data) {
          console.log('Error: ' + data);
        });
    };    
// ============== SITES MODEL MONGO ====================


// ======= SPACED REPITITON ALOGIRHTM HERE  ======= 







  }).
  controller('MyCtrl2', function ($scope) {
    // write Ctrl here

  }).
  controller('CatCtrl', function ($scope, $http) {
    // write Ctrl here

    $scope.test = function(){
    	alert("Testing");
    }



    // CHART CONTROLLER

    var resultsA = {
            Sex: {
                _type: "terms",
                terms: [{
                    term: "Male",
                    count: 36
                }, {
                    term: "Female",
                    count: 148
                }]
            }
    };
    $scope.results = resultsA;



    $scope.data = {
        entries: [
          {time: 1, count: 26},
          {time: 2, count: 44},
          {time: 3, count: 1}
        ]
    };


    // CHART CONTROLLER END


    $scope.shared = { data: [1] };
    $scope.chartClicked = function(){
      var n = Math.round(Math.random() * 9) + 1;
      $scope.shared.data = d3.range(n).map(function(d){ return Math.random(); });
    }    

    // END DONUT CHART CONTROLLER

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
    $scope.childAbility = 50;
    $scope.ability = false;

// DUMMY DATA *************************************************
    $scope.itemBank = [ 
      {
        question: "Dubstep",
        answer: "1",
        difficulty: 82
      },
      {
        question: "MC^2",
        answer: "1",    
        difficulty: 71
      },
      {
          question: "There is no answer",
          answer: "1",    
          difficulty: 57
      },    
      {
          question: "Pop quiz",
          answer: "1",    
          difficulty: 49
      },  

      {
          question: "The meaning of life?",
          answer: "1",    
          difficulty: 38
      },  
      {
          question: "The answer is No",
          answer: "1",    
          difficulty: 27
      },  

      {
        question: "Is the sky blue",
        answer: "1",  
        difficulty: 16
      }
    ]; // Item Bank to choose from


    $scope.candidateBank = [
      {
        name: "Owen",
        ability: 41
      },

      {
        name: "Chris",
        ability: 70
      } 

    ]; //student Test bank

// DUMMY DATA *************************************************


    $scope.loadChartGauge = function(){ 
            var colData = ['ability', this.childAbility];
            chartGauge.load({
				columns: [colData]
            });
}

    $scope.loadDifGauge = function(){ 
            var colData = ['difficulty', this.D];
            difGauge.load({
				columns: [colData]
            });
}



    $scope.newCandidate = function(){

      this.D = 0;
      this.L = 0;
      this.H = 0;
      this.R = 0;

      this.nextCandidate = this.candidateBank[0];
      this.childAbility = this.nextCandidate.ability;
      this.testStandard = 5; // Initializing testStandard
      // this.testLength = this.testLength;
      this.begin(this.nextCandidate);
      
      this.loadChartGauge();
      this.loadDifGauge();
    };

    $scope.begin = function(nextCandidate){
      console.log(nextCandidate.name);
      this.closestItem = this.findItemInBank(nextCandidate.ability);
      // this.D = closestItem.difficulty;
      // console.log("asdasdasd");
      // console.log(this.D);
      // console.log(closestItem.difficulty);

      this.D = this.closestItem.difficulty;

      this.response = this.administerItem(this.closestItem);

      //Dont continue until we get a response 
      // CONTINUE STEPS IN GETUSERPROMPT==================================

    }

    $scope.minVal = 999;
    $scope.minItem = "";
    $scope.findItemInBank = function(D){
      // console.log(this.itemBank[0].difficulty);
      for (var item in this.itemBank){


        var difference = Math.abs(D - this.itemBank[item].difficulty)
        console.log(difference);
        console.log("The difference");

        if (difference < this.minVal){
          console.log("This is smaller");
          this.minVal = difference;
          this.minItem = this.itemBank[item];
          console.log("The min val is " + this.minVal);
          console.log(this.minItem.question);
        }

      }

      console.log(this.minVal);
      console.log(this.minItem);

      return this.minItem;
    }

    $scope.administerItem = function(item){
      
      
      this.question = item.question;
      console.log(this.question);
      console.log("Waiting for response");
      // return null
      // var answer = this.getUserPrompt();
      
    }

    $scope.getUserPrompt = function(){
      
      if (this.closestItem == null){
        console.log("Start testing first");
        //Disable this button
      }

      else {
        this.response = $scope.ansData.text;
        // console.log(this.response);

        this.ansData = {};

        // CONTINUE HERE ============================

        //1 or 0
        console.log("starting get User Prompt and score Response");
        var score = this.scoreResponse(this.response, this.closestItem) 
        // console.log(score);

        this.L += 1;

        this.H = this.H + this.D;

        if (score == 0){
          this.D = this.D - 2/(this.L);
          console.log("score incorrect  and " + this.D);
        }

        if (score == 1){
          this.D = this.D + 2/(this.L);
          this.R += 1;
        }


        //Update the Charts
        this.loadChartGauge();
        this.loadDifGauge();        

        if (this.testLength > 0){
          this.testLength -= 1;

          //Test again
          this.begin(this.nextCandidate);


        }

        if (this.testLength <= 0){
          console.log("ready to give final ability level");
          console.log(this.R);
          var W = this.L - this.R;
          console.log(" W, H and L, values. ")
          console.log(W);
          console.log(this.H);
          console.log(this.L);

          //Check if a variable is zero and set Measure. Cant log(0)
          if (this.L == 0 || this.R == 0){
            console.log("Child ability will be infinity")
            var measure = this.H/this.L;
          }
          else {
            var measure = this.H/this.L + Math.log(this.R/W);
          }

          // var standardError 
          if (W == 0){
            console.log("You need to retest");
            this.childAbility = "Re-test, you either got 100% or 0%"
          }
          else if (W != 0){
            this.ability = true; 
            this.childAbility = measure;
          }

          alert("Final ability level of the child is " + measure + " and D is " + this.D);

          //Update the Charts
          this.loadChartGauge();
          this.loadDifGauge();        
          //Maybe change the child ability level



        }

        return null;
      }

    //   return answer;
    }

    $scope.scoreResponse = function(res, cItem){
      console.log(cItem.answer);
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



// charts stuff ======




// app.
//    directive('bars', function ($parse) {
//       return {
//          restrict: 'E',
//          replace: true,
//          template: '<div id="chart"></div>',
//          link: function (scope, element, attrs) {
//            var data = attrs.data.split(','),
//            chart = d3.select('#chart')
//              .append("div").attr("class", "chart")
//              .selectAll('div')
//              .data(data).enter()
//              .append("div")
//              .transition().ease("elastic")
//              .style("width", function(d) { return d + "%"; })
//              .text(function(d) { return d + "%"; });
//          } 
//       };
//    });









  