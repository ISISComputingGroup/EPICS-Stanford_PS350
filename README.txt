Building on Windows
-------------------

* Initial Setup

You need to have perl installed - either strawberry perl (http://strawberryperl.com/) or active state perl (http://www.activestate.com/activeperl/downloads). The ISIS base/startup/win32.bat assumes strawberry perl
installed to "c:\strawberry\perl" so you may need to edit this file if this is not the case
 
You also need a Microsoft Visual C++ compiler - either full visual studio or the experss version.
You may need to edit the appropriate point in  base/startup/win32.bat to specify its location - the ISIS default works
for a 64bit computer compiling 64bit images.
  
checkl EPICS_HOST_ARCH in base/startup/win32.bat - default is windows-x64

* Building

  config_env
  make

(you may need to run “make” twice the very first time - "config_env.bat" only needs to be run once for a given command shell)

The above will build epics base + iocs - you do not need a separate copy of epics base on your computer. The PATH will also be set in your command shell, so caget etc. will be available. I’ll send out further instructions on how to run the IOCs etc. The galil driver does not currently build on windows, but the mclennan and “simulated motor” do. 

Simulated Motor IOC
-------------------

To start a simulated motor ioc do:

    cd \development\EPICS\support\motor\6-7-1\iocBoot\iocSim
    ..\..\bin\windows-x64\WithASyn.exe   st.cmd.unix

you can then type “dbl” at the epics> prompt to see your PVs and then then open another cmd, run config_env (to set paths) and then use e.g. caget to read them

Eurotherm Driver IOC
--------------------

basically,

•	cd \development\EPICS\support\eurotherm2k\1-11\iocs\example\iocBoot\iocexample
•	edit  stexample.src   and change the argument of "drvAsynSerialPortConfigure" from COM21 to whatever local COM port you have the eurotherm on
•	then run         ..\..\bin\windows-x64-debug\example.exe  stexample.src

There will be a couple of errors concerning EUROTHERM2K:MAN: that are displayed – these can be ignored

The eurotherm comes up “disabled” – you first need to enable it via:

    caput EUROTHERM2K:DISABLE 0

then you can e.g.

    caput EUROTHERM2K:SP  5
    caget EUROTHERM2K:SP:RBV

----
Freddie Akeroyd, 19/02/2013