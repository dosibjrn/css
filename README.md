# Classic Simple Stats

## Using Warcraft Logs data (Windows cmd example)

This repo now contains a modified version of https://github.com/Saintis/Overheal

There is a new python script, read_health_change_from_api.py that is able to obtain a log file from warcraft logs, which is accepted as input to the simulator. To use this, just add key "wcl_log, 1" to your conf file and pass the downloade log file as normal with -l.

For the script to work, as mentioned in Overheal repo, you need to go to warcraftlogs, your account and copy the API key to apikey.txt in the folder you are running the abovementioned script. In addition, you need to give a name to your application using the API. This name does not matter.


In the following example, we load the current top bwl speedrun from WCL and run the simulator on that

`python3 ../Overheal/readers/read_health_changes_from_api.py ytL9VY8DXfWdGRmJ progress_speed.txt`

and then run

`css.exe 4 ../items/more_raid.csv ../items/not_for_priests.txt no-locks -a ../confs/wcl_bwl_speed_log.csv -l progress_speed.txt`

The python script accepts both the url or just the code, which is the suffix of the URL.

You may be requested to install python3 and may miss "requests". First can be solved by installing python3, latter can be solved by running

`pip install requests`


## Log Based PvE Healing

The `./css 4` and `./css 5` (more about other options below) can be combined with `-l mylogfile.txt`
where mylogfile.txt should be a proper wow classic combat log.
The log will be parsed to form heal deficits and their changes during combat. Spells used will be picked from flash heal ranks and heal and greater heal ranks, depending on current mana, relative time left of current combat, and optimized parameters. That is, the tool tries to find an optimal strategy
for maximizing hps. Output will show chosen spells, chosen parameter values controlling overheal and flash/slow heal tradeoff, as well as the usual output for 
a pve healing run. The optimization is a bit more prone to fit to local minimas, so the actual values for items seem to be a bit more noisy.
Due to this, we use Eigen to do a least squares fit to last 3000 "stat change -> hps change" data points, to obtain weights for stats
with currently optimized gear.
These alternative values are shown in the gear scores output at end as "Alt:". This alternative scoring does not take special effects, such as set bonuses into
account, so for set items, the original scores may be more telling.

The log based healing is still pretty experimental and some issues remain, e.g.:
* Spirit value resulting from least squares fit seems surprisingly low
* Hps seems a bit high / combat time a bit low: there have been cases when casting time sum was over 100% of combat time.

So all log based results should be taken with a pinch of salt for now.

Please note that for now this functionality is not available in the windows exe, just if you compile it yourself.


## Building and running (In Ubuntu context)

To build in e.g. Ubuntu terminal:

mkdir build

cd build

cmake ..

make -j

In case you don't feel like compiling, there's a compiled binary for x64 Windows under /bin folder.

For running the precompiled binary, you will need Microsoft Visual C++ Redistributable for Visual Studio 2015 - 2019. You can go grab one here: https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads


To run:

`./css 4 ../items/more_raid.csv ../items/banned.txt`

Ban mc and onyxia items from list:

`./css 4 ../items/more_raid.csv ../items/banned_and_onyxia_mc.txt`

The "4" is for item picking for pve healing, for disc priest.

For pvp healing, disc priest, use `./css 3` instead

For pvp shadow priest, use `./css 2`.


This is developed and tested under Ubuntu 18.04, and Windows 10.

There's an example of run query for windows under /bin folder, which should be run in that folder, or another folder under css.

`css.exe 4 ../items/more_raid.csv ../items/bwl.txt no-locks my_tag_name -a ../confs/our_mc.csv`

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
  * heal, rank 2
  * heal, rank 4
  * greater heal, rank 1
  * greater heal, rank 4
  * flash heal, rank 1
  * flash heal, rank 4
  * flash heal, rank 7
  * renew, rank 9
  * poh, rank 4
  * shield, rank 10

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


`./css` can be called with "0" and "1" as well. These are some early implementations for leveling and really rough
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

The maxium consecutive regen ticks can be controlled by setting full_regen_limit to a suitable range in [0.0 - 1.0]
This limits the regen outside the five second rule to be under this relative amount.
You can additionally control the max_ticks entry, limiting maximum consecutive regen ticks to set value.
Moreover, you can adjust the target_hp, which should reflect the amount of hp in your healing pool you can let drop before continuing your heals.
The encounter dps that you're supposed to heal in combination with target_hp control the maximum regen time. Dps is set to be 0.8 * [your hps]. If your target_hp is 10000 and the incoming dps is 1000, you have 10 seconds to regen at a time.

You can additionally control the lengths of the encounters that are calculated, and their relative weights in the opimization by setting

pve_combat_lengths, 60, 120, 180 # lengths in seconds
pve_combat_weights, 1.5, 2, 1 # relative weights

or some other suitable values.

Spell ranks are also controllable, by setting e.g.

spell_ranks, 2, 4, 1, 4, 1, 4, 7, 9, 4, 10

for default values. The order here is: 2 heals, 2 greater heals, 3 flash heals, renew, poh, shield.
