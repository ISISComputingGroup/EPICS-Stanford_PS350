# test example python script for use in CSS/BOY
# accessing a file using environ variable to point
# to the location of the file, read from file and
# show how to access a PV linked to the script as
# PV trigger.
#
# make sure that DBHOME is defined to the location
# where the file hvdb.txt is locatate. 

from org.csstudio.opibuilder.scriptUtil import PVUtil
from org.csstudio.opibuilder.scriptUtil import ConsoleUtil
from org.csstudio.utility.pv import PVFactory
from org.csstudio.utility.pv import PVListener
#from org.csstudio.utility.pv.epics import EPICSPVFactory

import re
import os
import time
#from threading import Thread
from java.lang import Thread, Runnable

ConsoleUtil.writeInfo("cdcreadfromdatabase.py start!")
s1 = dir(PVFactory)
ConsoleUtil.writeInfo( str(s1) )
#l1 = PVFactory.getSupportedPrefixes()
#ConsoleUtil.writeInfo( str(l1) )

loc = os.getenv("DBHOME")
filename = loc+'/hvdb.txt'
inf = open(filename)

line = inf.readline()
line = inf.readline()
HVMainFrame = ['hv0','hv0','hv0','hv0','hv0','hv0','hv0']
HVModule = [0,0,0,0,0,0,0]
idx = 0;
lastline = line
line = inf.readline()
while (line):
	s = re.split("\s+",line)
	#ConsoleUtil.writeInfo(line+"  :"+s[0])
	if (re.match("CDC",s[0])):
		#ConsoleUtil.writeInfo(line)
		lastline = line
		if (s[5] == '23'):
			HVMainFrame[idx] = s[3]
 			HVModule[idx] = s[4]
			idx = idx+1
	line = inf.readline()

s = re.split("\s+",lastline)
HVMainFrame[6] = s[3]
HVModule[6] = s[4]

#for i in range(0,7) :
#	ConsoleUtil.writeInfo( HVMainFrame[i]+":"+str(HVModule[i]))

a1 = PVUtil.getString(pvs[0])
ConsoleUtil.writeInfo( "a1 is "+a1)

a2 = PVUtil.getLong(pvs[0])
ConsoleUtil.writeInfo( "a1 is "+str(a2))

class MyTask(Runnable):
	def run(self):
	#access an arbitrary pv not associated with the current widget
		str1 = "hv1:0:0:pwonoff"
		ConsoleUtil.writeInfo( "start thread with "+str1)
		pvN = PVFactory.createPV(str1)
		ConsoleUtil.writeInfo("after PVFactory")

		pvN.start()
		ConsoleUtil.writeInfo("after pvN.start()")
		cnt = 0
		time.sleep(0.1)
		status0 = pvN.getStateInfo()
		ConsoleUtil.writeInfo( "status of getStateInfo() is "+status0)
		status1 = pvN.isConnected()
		ConsoleUtil.writeInfo( "status of isConnected() is "+str(status1))

		while (( not (status1)) and (cnt<20) ):
			cnt += 1
			time.sleep(0.1)
			status1 = pvN.isConnected()
			ConsoleUtil.writeInfo(str(cnt)) 
			
		
		if (re.match("GotMetaData",status0)):
			ConsoleUtil.writeInfo( "epics variable is connected!")
			val = pvN.getValue()
			ConsoleUtil.writeInfo( "and the value is "+str(val))
		else:
			ConsoleUtil.writeInfo( "epics variable is NOT connected!")

		ConsoleUtil.writeInfo( "Stop thread now!")	
		pvN.stop()

#t = Thread(target=accpv, args=("hv1:0:0:status",))
#t.start()
thread = Thread(MyTask())
thread.start()
