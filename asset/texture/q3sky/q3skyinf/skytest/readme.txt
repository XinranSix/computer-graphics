Using the map 'skytest' for skybox sun direction/elevation check
----------------------------------------------------------------

Copyright (C) Speaker (aka Laszlo Menczel) 2008

Principle
---------

The map contains a simple platform with a pole in the center, inside a
skybox. The pole is surrounded by directional markers for the eight main geographical directions (N, NE, E, SE, etc.). Please note that the markers
are kind of reversed: e.g. the Western marker is actually to the east
relative to the pole. If you move to a position in which the sun is lined up
with the pole and notice that you are standing on the marker labeled 'W' for
West, then the sun is actually in the West at direction 180. The elevation
can be fairly accurately estimated by the relative positions of the pole's
tip and the sun image. You should stand at a position where your distance
from the pole is the same as the pole's height (then the tip is at 45 degrees). The directional markers are positioned at such a distance. Dig it? :-)

Usage
-----

1. Set up GtkRadiant to compile the test map:
  - copy 'skytest.map' to 'baseq3/maps'
  - copy the files in 'textures/common' to 'baseq3/textures/common'
  - copy the (skeleton) skybox shader to 'baseq3/scripts'
  
2. Open the map in Radiant, select the skybox surfaces and apply your
   skybox texture

3. Compile the map using the fastest test settings of Radiant

4. Run the map and take notes of the direction and elevation of the sun

5. Modify the skybox shader using the observed values
