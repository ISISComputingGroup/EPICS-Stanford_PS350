importPackage(Packages.org.csstudio.opibuilder.scriptUtil);
importPackage(Packages.java.lang);
importPackage(Packages.org.csstudio.utility.pv);

var hvvalue = PVUtil.getLong(pvs[0]);
var MFM = widget.getPropertyValue("name");

runnable = {

    run:function()

        {    

		for (var k=0;k<24;k++){
			var str = MFM+":"+k+":v0set"
            var  pv = PVFactory.createPV( str );
			//ConsoleUtil.writeInfo("channel is "+str+"    and hv is "+hvvalue);

            pv.start();
			var cnt = 0;
            var status = pv.isConnected();
            while ((!(status))&&(cnt<20)){
                  Thread.sleep(100);
                  status = pv.isConnected();
                  cnt++;
			      //ConsoleUtil.writeInfo("channel is "+str+"    and hv is "+hvvalue);
			}
				
			if (cnt<20){
			      //ConsoleUtil.writeInfo("channel is "+str+"    and hv is "+hvvalue);

				  hvvalue *= 1.0;  // this is needed to make a number
                  //var value = pv.getValue();

                  //System.out.println( pv.getName() + " connection status is " + status );                

                  //System.out.println( pv.getName() + " = " + value );
				  //var string1 = " "+value;
				  //var s = string1.split(/\s+/);
				  pv.setValue(hvvalue);
			}
            pv.stop();

        }    
       }
    };        

new Thread(new Runnable(runnable)).start();
