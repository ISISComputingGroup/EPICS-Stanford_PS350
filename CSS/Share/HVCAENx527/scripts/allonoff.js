importPackage(Packages.org.csstudio.opibuilder.scriptUtil);

var stat = PVUtil.getString(pvs[0]);

//ConsoleUtil.writeInfo( "pvs[0] value is "+stat );
var state1 = "On";
var status1 = 1;
if (stat == 0){
	state1 = "Off";
	status1 = 0;
}


for (var i=1;i<pvs.length;i++){
	var val1 = " "+pvs[i].getValue();
	var s = val1.split(/\s+/);
	//ConsoleUtil.writeInfo( "pvs["+i+"] value is "+s[3] );
	 if (val1 != state1) {
            pvs[i].setValue( status1 );
     }
}