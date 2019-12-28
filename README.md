Classic Simple Stats

To build:

mkdir build
cd build
cmake ..
make -j

To run:

./css 4 ../items/more_raid.csv ../items/banned.txt

Ban mc and onyxia items from list:

./css 4 ../items/more_raid.csv ../items/banned_and_onyxia_mc.txt

The "4" is for item picking for pve healing, for disc priest.

For pvp healing, disc priest, use "./css 3" instead

For pvp shadow priest, use "./css 2".

This is developed and tested under Ubuntu 18.04, but should probably build and work in Windows as well.



PvE healing has some assumptions and limitations.

The basic functionality optimizes regen pattern within given range (1-100 casts, then 0-20 ticks of regen).
The spell sequence is optimized as well: number of following spells is adjusted:
  * heal rank 2
  * heal rank 4
  * greater heal rank 1
  * greater heal, max rank
  * renew, max rank
  * poh, max rank

These spells have specific "max frequencies", limiting their relative amount. Max rank renews, max rank greater heals
are limited to 0.2 due to the fact that it's not relly realistic to cast these two all combat. Poh is limited to 0.1.
(i.e., 20% and 10% of total number of spells in sequence).

Power infusion, if specified in talents for chosen character (hard coded as is) is popped when available,
      followed by max rank greater heals until duration ends / oom

Optimal items are picked slot at a time from, randomizing the slot order.
To avoid local maximas of the objective function (see below), in early stages of the optimization the current
best item is banned for the next round.

The PvE optimization function is average heal per second for 2, 3, 4, 5, 6 minute fights, each weighted equally.



The PvP healing item picking functions in quite similar fashion, only the objective function and spell sequence changes.
The PvP healing item picking objective function is:
 (hps * effective mana) * (hps * effective hit points) * effective hit points

HpS here comes from a pvp spell sequence with couple renews and mostly max rank flash heals.

 Reasoning here kind of goes back to the three functions of priest in PvP:
 * (hps * effective mana): freely heal your allies
 * (hps * effective hit points): throw out max heals as long as you stay alive taking a beating
 * effective hit points: survive rogues jumping on you and focus switches to you


 PvP shadow is quite similar to PvP healing, but the Hps is replaced by shadow dps with somewhat simple shadow
 damage rotation.


The focus of this tool is mostly in PvP and PvE healing gear calculations.


./css can be called with "0" and "1" as well. These are some early implementations for leveling and really rough
stat weights which can be more or less disregarded.
