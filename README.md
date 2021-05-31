# LTE-handover-scenario
this code is implemented with NS3 as a simulation of 6 mobile nodes and 2 eNodeB (node 2 and 3) making a simple handover 
the two eNodeB are connected to a core network (node 0 and 1)

## To install ns3 and netanim
First, download the ns-3.29 compressed source code archive and extract it from: https://www.nsnam.org/releases/ns-3-29/download/

Then run the following commands:
- basic update 
  `sudo apt update` & `sudo apt upgrade`
- installing required libraries
  `sudo apt install build-essential autoconf automake libxmu-dev python-pygraphviz cvs mercurial bzr git cmake p7zip-full python-matplotlib python-tk python-dev python-kiwi    python-gnome2 qt4-dev-tools qt4-qmake qt4-qmake qt4-default gnuplot-x11 wireshark`
Next, in the extracted archive run:
- ns3 builds so many packages which will be built using the following
  `./build.py --enable-examples --enable-tests`





## To run build the ns3 code
just copy the file **new_lte.cc** to the scratch folder the execute the build comand `./waf --run scratch/new_lte` 
a new file called **lte_handover_anim.xml** which is the same file in this repo.

## To view the simulation use Netanim. 
