# Turn on/off all HVchannels of the CDC
#
# currently the HV main frame and HV board addresses
# for each HV channel are retreived form a database file 
# make sure that DBHOME is defined to the location
# where the database file hvdb.txt is locatate. 

from org.csstudio.opibuilder.scriptUtil import PVUtil
from org.csstudio.opibuilder.scriptUtil import ConsoleUtil
from org.csstudio.utility.pv import PVFactory
import re
import os
import time
#from threading import Thread
from java.lang import Thread, Runnable

#ConsoleUtil.writeInfo("cdchvallonoff.py start!")

# Note that DBHOME needs to be defined!!!!
loc = os.getenv("DBHOME")
filename = loc+'/hvdb.txt'
inf = open(filename)

# this part is because of the file format
line = inf.readline()
line = inf.readline()

# define the required variables
HVMainFrame = ['hv0','hv0','hv0','hv0','hv0','hv0','hv0']
HVModule = [0,0,0,0,0,0,0]
idx = 0;

# scan through the database file and get all CDC channels
lastline = line
line = inf.readline()
channeladdress = []
Ccounter = 0;
while (line):
	s = re.split("\s+",line)
	#ConsoleUtil.writeInfo(line+"  :"+s[0])
	if (re.match("CDC",s[0])):
		#ConsoleUtil.writeInfo(line)
		lastline = line
                channeladdress.append(s[3]+":"+s[4]+":"+s[5]+":pwonoff")
                Ccounter += 1
		if (s[5] == '23'):
			HVMainFrame[idx] = s[3]
 			HVModule[idx] = s[4]
			idx = idx+1
	line = inf.readline()

s = re.split("\s+",lastline)
HVMainFrame[6] = s[3]
HVModule[6] = s[4]

# state of the all on/off button
pvs0 = widget.getPV()
a1 = pvs0.getValue()
#ConsoleUtil.writeInfo(str(a1))
v1 = re.split("\s+",str(a1))
#ConsoleUtil.writeInfo( "v1[2] is  "+v1[2])

OldState = int(float(v1[2]))

NewState = 1
if (OldState == 1):
    NewState = 0

pvs0.setValue(NewState)

if (NewState):
    ConsoleUtil.writeInfo( "Turn all channels ON!")
else:
    ConsoleUtil.writeInfo( "Turn all channels OFF!") 

class MyTask(Runnable):
	def run(self):
            # loop over all CDC channels and switch the state to NewState
            #for k in range(0,Ccounter):
            for k in range(0,10):
		str1 = channeladdress[k]
		#ConsoleUtil.writeInfo( "Connect to "+str1)
		pvN = PVFactory.createPV(str1)
		pvN.start()
		cnt = 0
		#status0 = pvN.getStateInfo()
		#ConsoleUtil.writeInfo( "status of getStateInfo() is "+status0)
		status1 = pvN.isConnected()
		#ConsoleUtil.writeInfo( "status of isConnected() is "+str(status1))
		while (( not (status1)) and (cnt<5) ):
                    cnt += 1
                    time.sleep(0.1)
                    status1 = pvN.isConnected()
			#ConsoleUtil.writeInfo(str(cnt)) 
                    
		if (cnt<5):
			#ConsoleUtil.writeInfo( "epics variable is connected!")
			#val = pvN.getValue()
			#ConsoleUtil.writeInfo( "and the value is "+str(val))
                    pvN.setValue(NewState)

		else:
			ConsoleUtil.writeInfo( "epics channel "+str1+" is NOT connected!")

		pvN.stop()
            # this is the end of the loop
        # this is the end of the task thread

thread = Thread(MyTask())
thread.start()





