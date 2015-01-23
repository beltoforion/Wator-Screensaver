######################################################################
#                                                                    #
#   _ _ _            _    _        ___   _ _ _        _              #
#  | | | | ___  _ _ | | _| |  ___ | | ' | | | | ___ _| |_ ___  _ _   #
#  | | | |/ . \| '_>| |/ . | / . \| |-  | | | |<_> | | | / . \| '_>  #
#  |__/_/ \___/|_|  |_|\___| \___/|_|   |__/_/ <___| |_| \___/|_|    # 
#                                                                    #									
#  					http://wator.beltoforion.de  #
# 					Version 1.1		     #
#                                                                    #									
######################################################################


Introduction:
-------------

This screensaver is an simulation based on a discrete simulation of predator-prey interaction. 
The simulation is called Wa-Tor. It was originally described by Alexander K. Dewdney in the 
Scientific American magazine. It simulates the hypothetical toroidal Planet Wa-Tor (Water Torus) 
whose surface is completely covered with water, occupied by two species: fish and sharks. The 
sharks are the predators. They eat the fish. The fish exist on a never ending supply of plankton. 
Both sharks and fish live according to a strict set of rules. This simulation of a simple ecology 
is highly dynamic as both species are walking a thin line between continuing life and extinction.

The screensaver is based on a dialog based MFC application combined with OpenGL for the 2D drawing. 
As usual, for OpenGL, using the binary without a hardware or drivers correctly supporting it will 
result in very low frame rates. The source code requires Microsoft Visual Studio V7.1 in order to 
compile, MSVC6 is not supported.

The ONLY official home page of this screensaver are:

	http://wator.beltoforion.de
	
	and 

	http://www.codeproject.com/opengl/Wator.asp


Installation:
-------------

Windows XP/Vista:
	Execute the file WorldOfWator.exe 

Note:  Users of the 64 bit Windows Vista versions need to manually install the screensaver.
  - Execute the setup
  - Go to the location where you installed the screen saver, right click the WorldOfWator 
    icon and select install.


Licence Statement:
------------------

    World of Wator - Screensaver
    Copyright 2009 Ingo Berg

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.



Acknowledgments:
----------------

This Software uses the CSetDPI class written by George Yohng in order 
to maintain window scaling regardless of the DPI settings.