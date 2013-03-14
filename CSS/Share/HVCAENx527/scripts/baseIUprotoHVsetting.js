importPackage(Packages.org.csstudio.opibuilder.scriptUtil);
importPackage(Packages.java.lang);

// Global hashes to keep the calibration parameters for HV settings
var aPar, bPar;

// Function to initialize the hashes for HV calibration settings
function initMap()
{
	// Address list that I copied from Kei's ROOT macro
	var addrList= 	[	'0x732c79', '0x72c932', '0x72360e', '0x738545', '0x733d10',	// this row all horizontal bars
						'0x733e06', '0x72dacc', '0x72bb3f', '0x72579b', '0x738516',	// this row all vertical   bars
						'0x72f75e', '0x72a530', '0x72db49', '0x7256d9', '0x72a5c2',	// row 1 (bottom)
				 		'0x72361d', '0x7256ae', '0x72355b', '0x72d907', '0x72bb50',	// row 2
				 		'0x72c9e2', '0x738553', '0x7281f8', '0x7282ef', '0x72daa4',	// row 3
				 		'0x72a60a', '0x72c8e8', '0x733d39', '0x72bade', '0x7256f4',	// row 4
				 		'0x722bc0', '0x726e8f', '0x726dcc', '0x72d915', '0x7256cb' ];	// row 5 (top)			

	// Parameter A for all bases 
	var pA		=	[	1077.20, 1069.88, 1101.50, 1077.88, 1067.42,
						1072.25, 1091.35, 1022.64, 1070.95, 1058.38, 
						1013.65, 1029.88, 1004.50, 1070.68, 1013.29, 
						1064.80, 1091.22, 1048.86, 1087.06, 1064.46,
						1072.96, 1000.00, 1044.56, 1041.17, 1061.99, 
						1052.34, 1017.46, 1020.50, 1079.39, 1039.75,
			    		1058.06, 1064.54, 1079.63, 1043.96, 1044.79 ];
			    	
	// Parameter B for all bases 		    	
	var pB	= 	[ 		12.9054, 12.5077, 12.3997, 12.5126, 11.6277,
			      		11.8904, 11.6944, 11.7823, 11.8650, 11.9711, 
			      		10.9025, 11.6602, 10.6200, 12.5188, 12.3737, 
			      		12.1175, 12.7650, 13.0452, 12.7062, 12.2504,
			       		12.4227, 12.6281, 12.0193, 13.6443, 11.1453, 
			       		12.5427, 12.7099, 12.3328, 11.8016, 11.5700,
			       		13.2598, 13.2966, 12.7810, 12.3665, 12.3983 ];		    	

	var aHash = new Array();
	var bHash = new Array();

	for( i = 0; i < addrList.length; i++ )
	{
		var msg = "Setting " +  addrList[i] + " , "  + pA[i] + " , " + pB[i] ;
//		ConsoleUtil.writeInfo( msg );  
		
		var addr = addrList[i];
		aHash[addr] = pA[i];
		bHash[addr] = pB[i];
	}
	return [ aHash, bHash ];
}


// Function to calculate the HV value for 
// a given board address and the "threshold" value 
function getHV( addr, thr )
{
	// Functional form is also taken from Kei'c code
	var v0 = aPar[addr] * Math.pow( thr, 1.0 / bPar[addr] );
	
	var message = "Address is " + addr + " , threshold is " + thr + 
		" , A is " + aPar[addr] + " , B is " + bPar[addr] + " , V0 is " + v0 ; 
//	ConsoleUtil.writeInfo( message );  
	return ( v0 );
//	return ( thr * 10.0 );
}

// Initialize the fit parameters for HV settings
var hashArray = initMap();
var aPar = hashArray[0];
var bPar = hashArray[1]; 

// Retrieve the value of the on/off variable
var trigVal = PVUtil.getDouble( triggerPV );

// The valeus for the "threshold" in mV that correspond to mbbo values.
// This array and the mbbo fields need to be syncronized 
var threshArray = [ 0, 50, 75, 100, 125, 150 ];

// Find threshold value
//var threshVal = -1;
//if( threshArray.length > trigVal ) {
//	threshVal = threshArray[trigVal];
//} 

var threshVal = trigVal;

var message = "Selector is  " + trigVal + " , threshold is " + threshVal;
//	+ " : should have been " + threshArray[trigVal] + " : Length was " + threshArray.length ;
// ConsoleUtil.writeInfo( message );     


// Loop through all PVs and set the values
for( i=0; i < pvs.length; i++ ) 
{
	// Split the name of the HV field with ":" as the delimiter
	var pvName = pvs[i].getName();
	var strField = pvName.split( ":" );
//	var fieldMessage = "Fields are " + strField[0] + "|" +  strField[1] + "|" + strField[2]  ;
//	ConsoleUtil.writeInfo( fieldMessage );     
	
	// Third field (with index 2) is the address of the board
	// First is "epics", second is "//bus?" 
	var boardAddress = strField[2];
	if( triggerPV != pvs[i] )  // Skip the trigger channel
	{ 
		if( threshVal >= 0 )
		{
			var setVal = getHV( boardAddress, threshVal ); 
			pvs[i].setValue( setVal );
		}	
	} 	
}
