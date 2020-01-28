Classic Simple Stats

To build in e.g. Ubuntu terminal:

mkdir build

cd build

cmake ..

make -j

In case you don't feel like compiling, there's a compiled binary for x64 Windows under /bin folder.

For running the precompiled binary, you will need Microsoft Visual C++ Redistributable for Visual Studio 2015 - 2019. You can go grab one here: https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads


To run:

./css 4 ../items/more_raid.csv ../items/banned.txt

Ban mc and onyxia items from list:

./css 4 ../items/more_raid.csv ../items/banned_and_onyxia_mc.txt

The "4" is for item picking for pve healing, for disc priest.

For pvp healing, disc priest, use "./css 3" instead

For pvp shadow priest, use "./css 2".


This is developed and tested under Ubuntu 18.04, and Windows 10.

There's an example of run query for windows under /bin folder, which should be run in that folder, or another folder under css.

css.exe 4 ../items/more_raid.csv ../items/bwl.txt no-locks my_tag_name -a ../confs/our_mc.csv

Here:
 * 1st argument "4" selects pve healing without full buffs
 * 2nd argument ../items/more_raid.csv points to a file containing bunch of items from pre-raid to bwl
 * 3rd argument ../items/bwl.txt sets bans to anything with source "bwl", resulting in considering upgrades from pre-raid to ony and  mc
 * 4th argument would be for locked items (being only options for slot) during optimization. no-locks points to no file here, so no locks will be set
 * 5th argument "my_tag_name" is a name for a pawn tag that will be printed and saved as result of the optimization. This can be copy-pasted and imported to pawn addon.
 * 6th and 7th arguments "-a ../confs/our_mc.csv" points to comma separated file with "#" as comments, setting assumptions according to whatever is said in this file

The default assumptions and reading of assumptions from file are described in assumptions.h and assumptions.cpp

PvE healing has some assumptions and limitations.

The basic functionality optimizes regen pattern within given range (1-20 casts, then 0-10 ticks of regen).
The spell sequence is optimized as well: number of following spells is adjusted:
  * heal rank 2
  * heal rank 4
  * greater heal rank 1
  * greater heal rank 4
  * renew, max rank
  * poh, max rank

These spells have specific "max frequencies", limiting their relative amount. Max rank renews are limited to 20%,
max rank greater heals are limited to 20%, and PoH is limited to 10% due to the fact that it's not really realistic to
cast these all combat.
(i.e., 20% and 10% of total number of spells in sequence).

20% of critting heal is approximated to be non-overhealing on average.

Power infusion, if specified in talents for chosen character (hard coded as is) is popped when available,
      followed by max rank greater heals until duration ends / oom

Optimal items are picked slot at a time, going through items in given .csv for the slot, randomizing the slot order.
To avoid local maximas of the objective function (see below), in early stages of the optimization the current
best item is banned for the next round.

The PvE optimization function is average heal per second for 2, 3, 4, 5, 6 minute fights, each weighted equally.

To see effect of consumables, very similar option ./css 5 exists. This is same as 4, but with potions, runes, mana oil, mageblood and flask.


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

The pvp options for shadow and disc have fixed spell sequences.

The focus of this tool is mostly in PvP and PvE healing gear calculations.


./css can be called with "0" and "1" as well. These are some early implementations for leveling and really rough
stat weights which can be more or less disregarded.



Some set bonuses have been added to the system.
Current implementation splits bonuses to partial bonuses with squared relative weights (1 for 1 piece, 4 for 2 pieces etc). This allows the optimization to have some clue about the fact that collecting a set may be beneficial. Disabling the partial bonuses and using only the real bonuses at the end of the
optimization is a feature that is about to be implemented in near future.

The output you will get once the optimization is done (some minutes max usually) will contain all upgrades
agains the current list of items in start_items.txt in the binary folder (try copying items/have.txt here and modifying it accordingly),
as well as relative stat weights at the found best in slot level. Note that the relative stat weights depend heavily on
the current gear, so it's not really something that will work in a global sense.

The upgrades have points (unit corresponding to objective function) as well as precentages (relative improvement and bis level)
There's another upgrade listing at the start_item.txt level. E.g. sometimes int gear will be better than the bis +healing gear when your overall
gear level is lowish (e.g. 50% pre-raid bis level).

In the case of pve healing, you will also see the spell sequences and regen patterns used by the optimizer.


Couple more notes on updating assumptions via -a argument:

The entries here are read from non-empty lines. # can be used for commenting full or partial lines

The maxium consecutve regen ticks can be controlled by setting full_regen_limit to a suitable range in [0.0 - 1.0]
This limits the regen outside the five second rule to be under this relative amount.
You can additionally control the max_ticks entry, limiting maximum consecutive regen ticks to set value.
Moreover, you can adjust the target_hp, which should reflect the amount of hp in your healing pool you can let drop before continuing your heals.
The encounter dps that you're supposed to heal in combination with target_hp control the maximum regen time. Dps is set to be 0.8 * [your hps]. If your target_hp is 10000 and the incoming dps is 1000, you have 10 seconds to regen at a time.

You can additionally control the lengths of the encounters that are calculated, and their relative weights in the opimization by setting

pve_combat_lengths, 60, 120, 180 # lengths in seconds
pve_combat_weights, 1.5, 2, 1 # relative weights

or some other suitable valuesi.


