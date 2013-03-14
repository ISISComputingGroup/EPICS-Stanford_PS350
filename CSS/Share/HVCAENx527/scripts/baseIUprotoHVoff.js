importPackage(Packages.org.csstudio.opibuilder.scriptUtil);
importPackage(Packages.java.lang);


// Retrieve the value of the on/off variable
var trigVal = PVUtil.getLong( triggerPV );
var setVal = trigVal * 1.0;

// Loop through all PVs and set the values
for( i=0; i < pvs.length; i++ ) 
{	
	if( triggerPV != pvs[i] ) // Skip the trigger channel
	{ 
		var message = "Would have set value " + setVal ;
//		ConsoleUtil.writeInfo( message );     	
		pvs[i].setValue( setVal );
	} 	
}
