# Run from e.g. css/build
cp ../items/have.txt start_with.txt

cat ../items/ban_after_p4.txt > ban_tmp_phase4.txt
cat ../items/not_for_priests.txt >> ban_tmp_phase4.txt

mkdir ../sample_output
mkdir ../sample_output/log_based

./css 4 ../items/phase5.csv ban_tmp_phase4.txt no-locks -a ../confs/bwl_log_based_atiy.csv -l bwl_atiy_04-12.txt | tee ../sample_output/log_based/bwl_atiy_04-12_phase4.txt
./css 4 ../items/phase5.csv ban_tmp_phase4.txt no-locks -a ../confs/bwl_log_based_atiy_holy.csv -l bwl_atiy_04-12.txt | tee ../sample_output/log_based/bwl_atiy_holy_04-12_phase4.txt
./css 4 ../items/phase5.csv ban_tmp_phase4.txt no-locks -a ../confs/bwl_log_based_atiy_deficit_change.csv -l bwl_atiy_04-12.txt | tee ../sample_output/log_based/bwl_atiy_deficit_change_04-12_phase4.txt
./css 4 ../items/phase5.csv ban_tmp_phase4.txt no-locks -a ../confs/bwl_log_based_atiy_festival_dumplings.csv -l bwl_atiy_04-12.txt | tee ../sample_output/log_based/bwl_atiy_festival_dumplings_04-12_phase4.txt

./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks -a ../confs/bwl_log_based_atiy.csv -l bwl_atiy_04-12.txt | tee ../sample_output/log_based/bwl_atiy_04-12_phase5.txt


./css 4 ../items/phase5.csv ban_tmp_phase4.txt ../items/hazz.txt -a ../confs/bwl_log_based_atiy.csv -l bwl_atiy_04-12.txt | tee ../sample_output/log_based/bwl_atiy_log_hazz_locked_phase4.txt
./css 4 ../items/phase5.csv ban_tmp_phase4.txt start_with.txt -a ../confs/bwl_log_based_atiy.csv -l bwl_atiy_04-12.txt | tee ../sample_output/log_based/bwl_atiy_log_my_current_gear.txt
./css 4 ../items/phase5.csv ban_tmp_phase4.txt no-locks -a ../confs/bwl_2020-04-09.csv -l bwl.txt | tee ../sample_output/log_based/bwl_02-26_phase4.txt
./css 4 ../items/phase5.csv ban_tmp_phase4.txt no-locks -a ../confs/bwl_2020-04-09_reaction_1s.csv -l bwl.txt | tee ../sample_output/log_based/bwl_02-26_reaction_1s_phase4.txt
./css 4 ../items/phase5.csv ban_tmp_phase4.txt no-locks -a ../confs/bwl_2020-04-09.csv -l nefarian.txt | tee ../sample_output/log_based/nefarian_02-26_phase4.txt
./css 4 ../items/phase5.csv ban_tmp_phase4.txt no-locks -a ../confs/bwl_2020-04-09.csv -l WoWCombatLog_infamous_mc_and_bwl-archive-2020-02-27T05-49-49.582Z.txt | tee ../sample_output/log_based/mc_and_bwl_02-26_phase4.txt
./css 4 ../items/phase5.csv ban_tmp_phase4.txt start_with.txt -a ../confs/bwl_2020-04-09.csv -l WoWCombatLog_infamous_mc_and_bwl-archive-2020-02-27T05-49-49.582Z.txt | tee ../sample_output/log_based/mc_and_bwl_02-26_current.txt

./css 4 ../items/phase5.csv ban_tmp_phase4.txt no-locks -a ../confs/bwl_2020-04-09_deficit_change.csv -l WoWCombatLog_infamous_mc_and_bwl-archive-2020-02-27T05-49-49.582Z.txt | tee ../sample_output/log_based/mc_and_bwl_02-26_deficit_change_phase4.txt

cp start_with.txt bkb.txt
cp ../items/8pt2.txt start_with.txt

./css 4 ../items/phase5.csv ban_tmp_phase4.txt start_with.txt -a ../confs/bwl_2020-04-09_transc8.csv -l WoWCombatLog_infamous_mc_and_bwl-archive-2020-02-27T05-49-49.582Z.txt | tee ../sample_output/log_based/mc_and_bwl_02-26_transc8_phase4.txt

cp bkb.txt start_with.txt

./css 4 ../items/phase5.csv no-bans start_with.txt dme_duo_current -a ../confs/dme_duo.csv | tee ../sample_output/dme_duo_current.txt

./css 3 ../items/phase5.csv no-bans ../items/items_have.txt pvp_heal_current | tee ../sample_output/pvp_heal_current.txt
./css 3 ../items/phase5.csv ban_tmp_phase4.txt no-lock pvp_heal_phase4 | tee ../sample_output/pvp_heal_phase4.txt
./css 3 ../items/phase5.csv ../items/not_for_priests.txt no-lock pvp_heal_phase5 | tee ../sample_output/pvp_heal_phase5.txt

./css 2 ../items/phase5.csv ../items/bwl.txt no-lock shadow_pvp_phase4 | ../sample_output/shadow_pvp_phase4.txt
./css 2 ../items/phase5.csv ../items/not_for_priests.txt no-lock shadow_pvp_phase5 | ../sample_output/shadow_pvp_phase5.txt

./css 5 ../items/phase5.csv ban_tmp_phase4.txt no-locks pve_heal_full_buff_phase4 | tee ../sample_output/pve_heal_full_buff_phase4.txt
./css 4 ../items/phase5.csv ban_tmp_phase4.txt no-locks pve_heal_normal_buff_phase4 -a ../confs/average_raiding.csv | tee ../sample_output/pve_heal_normal_buff_phase4.txt

for mode in 2 3 4 5; do
  ./css $mode ../items/phase5.csv ../items/not_for_priests.txt ../items/have.txt | tee ../sample_output/css_${mode}_current.txt
  ./css $mode ../items/phase5.csv ../items/not_for_priests.txt | tee ../sample_output/css_${mode}_phase5.txt
  ./css $mode ../items/phase5.csv ban_tmp_phase4.txt | tee ../sample_output/css_${mode}_phase4.txt
done

mkdir ../sample_output/css5_buffs_phase5
for buffs in 0.0 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0; do
  ./css 5 ../items/phase5.csv -b $buffs -r 0.3 | tee ../sample_output/css5_buffs_phase5/css5_b_${buffs}_r_0.3.txt;
done

mkdir ../sample_output/snipe_bwl_phase4
for snipe in 0.0 0.2 0.4 0.6 0.8 1.0; do
  ./css 4 ../items/phase5.csv ban_tmp_phase4.txt no-locks bwl_snipe${snipe} -a ../confs/bwl_snipe${snipe}f | tee ../sample_output/snipe_bwl_phase4/css_4_snipe${snipe}.txt
done

mkdir ../sample_output/tags/
cp *.pawn_tag.txt ../sample_output/tags/. 
tail -n 1 ../sample_output/tags/*.pawn_tag.txt > ../sample_output/tags/all_latest_tags.txt
