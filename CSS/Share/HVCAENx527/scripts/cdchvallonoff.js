importPackage(Packages.org.csstudio.opibuilder.scriptUtil);
importPackage(Packages.java.lang);
importPackage(Packages.org.csstudio.utility.pv);

var pvs0 = widget.getPV();
var OnOff = PVUtil.getLong(pvs0);
//ConsoleUtil.writeInfo("value of InOff is "+OnOff);
OnOff++;
pvs0.setValue(OnOff);

runnable = {

    run:function()

        {    

		for (var k=0;k<24;k++){
			var str = "hv1:0:"+k+":pwonoff"
            var  pv = PVFactory.createPV( str );

            pv.start();
			var cnt = 0;
            var status = pv.isConnected();
            while ((!(status))&&(cnt<20)){
                  Thread.sleep(100);
                  status = pv.isConnected();
                  cnt++;
			}
				
			if (cnt<20){
				
                  var value = pv.getValue();

                  //System.out.println( pv.getName() + " connection status is " + status );                

                  //System.out.println( pv.getName() + " = " + value );
				  var string1 = " "+value;
				  var s = string1.split(/\s+/);
				  if(OnOff%2){
				      pv.setValue(1);  
				  } else {
				     pv.setValue(0);
				  }				
			}
            pv.stop();

        }    
       }
    };        

new Thread(new Runnable(runnable)).start();
