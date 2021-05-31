# LTE-handover-scenario
This code is implemented with NS3 as a simulation of 6 mobile nodes and 2 eNodeB (node 2 and 3) making a simple handover 
The two eNodeB are connected to a core network (node 0 and 1).

## Install ns3 and netanim
First, download the ns-3.29 compressed source code archive and extract it from: https://www.nsnam.org/releases/ns-3-29/download/ 

Then run the following commands:
- basic update 
  `sudo apt update` & `sudo apt upgrade`
- installing required libraries
  `sudo apt install build-essential autoconf automake libxmu-dev python-pygraphviz cvs mercurial bzr git cmake p7zip-full python-matplotlib python-tk python-dev python-kiwi    python-gnome2 qt4-dev-tools qt4-qmake qt4-qmake qt4-default gnuplot-x11 wireshark`

Next, ns3 builds so many packages which will be built using the following in the extracted archive
  `./build.py --enable-examples --enable-tests`




## Run build the ns3 code
Just copy the file **new_lte.cc** to the ns-allinone-3.29/ns-3.29/scratch folder then execute the build comand `./waf --run scratch/new_lte` in ns-allinone-3.29/ns-3.29 (No need to specify the extension).
a new file called **lte_handover_anim.xml** representing the animation and a trace file **lte_log.xml** which are the same files in this repo.

## View the simulation use Netanim
The Netanim file exists in ns-allinone-3.29/netanim-3.108 run it with `./Netanim`, a graphical interface appears, then you just need to import the **lte_handover_anim.xml** file and run the animation.a

## Show the performance chart
copy the **flow.py** file in the ns-allinone-3.29/ns-3.29 folder then run `python flow.py results.pdf` to get the chart.

