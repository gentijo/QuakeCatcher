Createing an ARM developement Environment for the ARM STM32 F4 using Eclipse on MaxOSX

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
Change the directories to Eclipse and Yagarto to match where you installed the products.

Run eclipse by clicking on the .command file in Finder.

Running this script will open a Terminal window, if you want Terminal to close after
launching Eclipse, go into preferences for Terminal and go to Settings / Shell.
Set the "When the shell exists" to "Close if the shell exited cleanly" and "Prompt 
before closing" to "Never"


Download the Eclipse ARM tools from http://sourceforge.net/projects/gnuarmeclipse/
Install the ARM tools into Eclispe by selecting from the Main Menu,
Help / Install New Software. Then in the dialog box click the Add button. In the
Add Repositiory dialog, click archive and select the zip file you just downloaded.

Next select the CDT GNU Cross Development tools and then click the Next button, agree
to the license and install the package. You will be prompted to restart Eclipse once it
is complete.

Next clone ChibiOS from https://github.com/mabl/ChibiOS

In Eclipse add the Demo project for the board.

Select File / New Makefile Project from Existing Code.
Type in the project name and browse for the subdirectory
chibios/trunk/demos/ARMCM4-STM32F407-DISCOVERY under the
directory where you cloned the project.

For toolchain, select ARM Mac OS X GCC (Yagarto)

Then click finsh.. The Project should show up in the Project Explorer.
Right click on the project and select "Clean Project"
Right click on the project and select "Build Project"

If all is installed correctly, the console log will show a bunch of Compiling statements.

Open main..c in the editor.. You may get a bunch of undefined referencess. 
If so, right click on the project and select "Index / Rebuild All"

