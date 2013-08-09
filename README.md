warmud
======

A Mud focused on Player Killing

A <a href="http://en.wikipedia.org/wiki/Merc_(MUD)">MERC</a> <a href="http://en.wikipedia.org/wiki/Player_killing#Player_killing">PK</a> <a href="http://en.wikipedia.org/wiki/MUD">MUD</a> named Warmud.
<br>
I created this mud with inspiration from PKmud.  It is based on random types of 'Wars' that players partake in.  
There are pit wars, hot potato wars, regular PK fest, etc.  
All the mobiles are easy to kill for their equipment, and once a player chooses a class he gets all of that class' 
abilities.  The idea is to have all players on equal footing so the fights are fair -- but the smartest/most skilled 
player wins!
<br>
The mud is not fully bug-free and in fact there are probably a couple crash bugs, but they should be easy to fix.
<br>
<br>
War types:
<ul>
  <li>Pit War - everyone is thrown in a really small area to duke it out</li>
  <li>Regular War - explore the mud and find powerful items to kill other players with</li>
  <li>City War - Similar to Pit, but everyone is stuck in a city</li>
  <li>All vs One War - Someone is 'it' and gets a huge buff.  If you kill 'it' you become 'it'!</li>
  <li>Artifact War - A random player gets an incredibly powerful artifact</li>
  <li>Team War - Players are assigned random teams.  The last team standing wins!</li>
  <li>Capture Flag War - Same as Team but now there is a flag to capture.  Each team has a random starting base where the flag is located.</li>
  <li>BORG War - Borgs are invading!  If one kills you, you are assimilated!</li>
  <li>Fittest War - Honestly, I forget.</li>
  <li>Hot Skull - Like hot potato, only with death.  If you are holding on to the hot skull when it goes off, you explode!</li>
</ul>

War Flags:
<ul>
  <li>Scattered Equipment - equipment is scattered randomly around the mud</li>
  <li>Random Equipment - all the loaded equipment has random stats</li>
  <li>Loadall - all equipment is loaded (normally there's a % chance based on how good it is)</li>
  <li>Bloods - sacrificing a player corpse gives you a blood potion (full heal)</li>
  <li>Same Class - everyone starts as the same class</li>
  <li>Fountains - drinking from fountains gives you benefits, such as a small heal</li>
  <li>Hunters - hunters are randomly placed around the world.  when they see you, they hunt and kill you</li>
  <li>Wrath - if nobody is killed in a certain time period, someone is randomly slain</li>
</ul>

Other Features:
<ul>
  <li>Online customizable classes - no need to code a new class (still have to code skills/spells)</li>
  <li>Online editable socials (emotes)</li>
</ul>

<hr>
<h3>Setup and running the code</h3>
This code is designed to be compiled and run on linux.  cd to the /src directory and type 'make'.  
This should create the executable 'merc'.  There is a script meant to start the mud and keep it
running, called 'warup'.  Run ./warup
<br>
You can login with the user 'Admin', blank password.


