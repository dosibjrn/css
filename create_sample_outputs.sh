# Run from e.g. css/build
cp ../items/have_atiy.txt start_with.txt
./css 4 ../items/more_raid.csv ../items/not_for_priests.txt no-locks -a ../confs/bwl_log_based_atiy.csv -l bwl_atiy_04-12.txt | tee ../sample_output/log_based/bwl_atiy_04-12.txt
./css 4 ../items/more_raid.csv ../items/not_for_priests.txt no-locks -a ../confs/bwl_log_based_atiy_holy.csv -l bwl_atiy_04-12.txt | tee ../sample_output/log_based/bwl_atiy_holy_04-12.txt
./css 4 ../items/more_raid.csv ../items/not_for_priests.txt no-locks -a ../confs/bwl_log_based_atiy_deficit_change.csv -l bwl_atiy_04-12.txt | tee ../sample_output/log_based/bwl_atiy_deficit_change_04-12.txt

cp ../items/have.txt start_with.txt

./css 4 ../items/more_raid.csv ../items/not_for_priests.txt no-locks -a ../confs/bwl_log_based_atiy.csv -l bwl_atiy_04-12.txt | tee ../sample_output/log_based/bwl_atiy_log_my_start_gear.txt
./css 4 ../items/more_raid.csv ../items/not_for_priests.txt no-locks -a ../confs/bwl_2020-04-09.csv -l bwl.txt | tee ../sample_output/log_based/bwl_02-26.txt
./css 4 ../items/more_raid.csv ../items/not_for_priests.txt no-locks -a ../confs/bwl_2020-04-09_reaction_1s.csv -l bwl.txt | tee ../sample_output/log_based/bwl_02-26_reaction_1s.txt
./css 4 ../items/more_raid.csv ../items/not_for_priests.txt no-locks -a ../confs/bwl_2020-04-09.csv -l nefarian.txt | tee ../sample_output/log_based/nefarian_02-26.txt
./css 4 ../items/more_raid.csv ../items/not_for_priests.txt no-locks -a ../confs/bwl_2020-04-09.csv -l WoWCombatLog_infamous_mc_and_bwl-archive-2020-02-27T05-49-49.582Z.txt | tee ../sample_output/log_based/mc_and_bwl_02-26.txt

./css 4 ../items/more_raid.csv no-bans ../items/have.txt dme_duo_current -a ../confs/dme_duo.csv | tee ../sample_output/dme_duo_current.txt
./css 4 ../items/more_raid.csv ../items/bwl.txt no-lock pve_healing_mc_ony -a ../confs/our_mc.csv | tee ../sample_output/our_raids_mc_ony.txt
./css 4 ../items/more_raid.csv no-bans ../items/have.txt pve_healing_current -a ../confs/our_mc.csv | tee ../sample_output/our_raids_current.txt
./css 4 ../items/more_raid.csv no-bans no-lock pve_healing_bwl -a ../confs/bwl_0401.csv | tee ../sample_output/our_raids_bwl.txt
./css 3 ../items/more_raid.csv no-bans ../items_have.txt pvp_heal_current
./css 3 ../items/more_raid.csv ../items/bwl.txt no-lock pvp_heal_mc_ony
./css 2 ../items/more_raid.csv ../items/banned_and_onyxia_mc.txt no-lock shadow_pvp_pre-raid
./css 2 ../items/more_raid.csv ../items/bwl.txt no-lock shadow_pvp_mc_ony
./css 5 ../items/more_raid.csv ../items/bwl.txt no-lock pve_heal_mc_ony_full_buff
./css 4 ../items/more_raid.csv ../items/bwl.txt no-locks pve_average_raiding -a ../confs/average_raiding.csv | tee ../sample_output/average_raiding.txt
./css 4 ../items/more_raid.csv ../items/bwl.txt no-locks pve_avg_split_onyxia -a ../confs/avg_split_onyxia.csv | tee ../sample_output/average_split_onyxia.txt

mkdir ../sample_output/tags/
cp *.pawn_tag.txt ../sample_output/tags/. 
tail -n 1 *.pawn_tag.txt > ../sample_output/tags/all_latest_tags.txt

for mode in 2 3 4 5; do
  ./css $mode ../items/more_raid.csv | tee ../sample_output/css_${mode}_bwl.txt
  ./css $mode ../items/more_raid.csv ../items/banned_and_onyxia_mc.txt | tee ../sample_output/css_${mode}_pre-raid.txt
  ./css $mode ../items/more_raid.csv ../items/bwl.txt | tee ../sample_output/css_${mode}_mc_ony.txt
done

mkdir ../sample_output/css5_buffs
for buffs in 0.0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0; do
  ./css 5 ../items/more_raid.csv -b $buffs -r 0.3 | tee ../sample_output/css5_buffs/css5_b_${buffs}_r_0.3.txt;
done

#./css 5 ../items/more_raid.csv ../items/bwl.txt -b 0.2 -r 0.3 | tee ../sample_output/css_5_b_0.2_r_0.3_mc_ony.txt

mkdir ../sample_output/snipe_bwl
for snipe in 0.0 0.2 0.4 0.6 0.8 1.0; do
  ./css 4 ../items/more_raid.csv no-bans no-locks bwl_snipe${snipe} -a ../confs/bwl_snipe${snipe}f | tee ../sample_output/snipe_bwl/css_4_snipe${snipe}.txt
done


