#Creating an ARM developement Environment for the STM32F4 using Eclipse on MaxOSX

##Target: Compile the ChibiOS demo project for the STM32F4 Discovery Board.

###Basic Eclipse Setup

Download the Eclipse IDE for C/C++ Developers from http://www.eclipse.org/downloads.
Unzip and installed in a perfered location. (I chose /Java)

Download Yagarto ARM Toolchain from http://www.yagarto.de/, Open the DMG and drag the install
package (blue box) to your perfered install location (I chose /opt) and run the install. Once
the install is complete, you can remove the installer pacgage file.

Eclipse needs to have the Yagarto Toolchain on the Path in Eclispe. On the Mac I have found
only one reliable way to do this.. In the directory where you installed eclipse, create a
command file name "eclipse.juno.command", it can actually be named whatever you want but 
it needs the .command extension.. The contents of the file are as follows.
```bash
#!/bin/sh

PATH=$PATH:/opt/yagarto-4.7.1/bin
export PATH
/Java/eclipse-juno/eclipse&
```
Change the directories to Eclipse and Yagarto to match where you installed the respective products.

Run eclipse by clicking on the .command file in Finder.

Running this script will open a Terminal window, if you want Terminal to close after
launching Eclipse, go into preferences for Terminal and go to Settings / Shell.   
Set:   
**"When the shell exists" to "Close if the shell exited cleanly"**   
**"Prompt before closing" to "Never"**   

###Add in ARM tools for Eclipse

Download the Eclipse ARM tools from http://sourceforge.net/projects/gnuarmeclipse/
Install the ARM tools into Eclispe by selecting from the Main Menu,
Help / Install New Software. Then in the dialog box click the Add button. In the
Add Repositiory dialog, click archive and select the zip file you just downloaded.

Select the CDT GNU Cross Development tools and then click the Next button, agree
to the license and install the package. You will be prompted to restart Eclipse once it
is complete.

###Configure ChibiOS as an Eclipse Project.

Clone ChibiOS Source Code from https://github.com/mabl/ChibiOS

In Eclipse add the Demo project for the board.

From the main menu, select *File / New Makefile Project from Existing Code*.
Type in the project name and browse for the subdirectory
chibios/trunk/demos/ARMCM4-STM32F407-DISCOVERY under the
directory where you cloned the project.

For toolchain, select ARM Mac OS X GCC (Yagarto)

Click finsh.. The Project should show up in the Project Explorer.
Right click on the project and select "Clean Project"
Right click on the project and select "Build Project"

If all is installed correctly, the console log will show a bunch of Compiling statements.

Open main..c in the editor.. You may get a bunch of undefined referencess. 
If so, right click on the project and select "Index / Rebuild All"

##Configuring Break Point Debugging and Loading the Demo Project.

###Acquire ST Link tools
Put st-util and st-flash somewhere.

###Configuring ST-Util to run as a GDB Server from Eclipse
From Eclipse select (from the main menu) Run / External Tools / External Tools Configuration
Select "New Configiration" from the toolbar and configure an entry as the one is in the picture
below. Modify the path to st-util to match where you installed st-link. Choose apply, then close
once configured.

<img src="https://github.com/gentijo/QuakeCatcher/raw/master/pics/GDBServer-ExtToolConfig.png" />

###Install the Zylin GDB Debugger interface.

From the main menu on Eclipse, select Help / Install New Software
In the **Work With** line enter http://opensource.zylin.com/zylincdt 

Zylin Embedded CDT should show up in the list below, select it and choose *Next*
Keep following next and accept all licenses to install the module.

###Configure the Project to run the debugger
In the root of the Demo Project, create the file gdbinit.txt and add the following lines.
```code
file build/ch.elf
target remote 127.0.0.1:4242
load
```

From the main menu, select Run / Debug Configurations.
There should be an entry on the left side Zylin Embedded debug (Native)
Select it then choose *New Launch Configuration* from the toolbar above.

Configure the Main and Debugger tabs using the examples below.. Note the GDB Command
file should point to your gdbinit.txt
<img src="https://github.com/gentijo/QuakeCatcher/raw/master/pics/ARMProgDbgCfg_Main.png" />
<img src="https://github.com/gentijo/QuakeCatcher/raw/master/pics/ARMProgDbgCfg_DebuggerTab.png" />

