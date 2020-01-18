# Run from e.g. css/build

./css 4 ../items/more_raid.csv ../items/bwl.txt start_with.txt -a ../confs/dme_duo.csv | tee ../sample_output/dme_duo_current.txt
./css 4 ../items/more_raid.csv ../items/bwl.txt -a ../confs/our_mc.csv | tee ../sample_output/our_raids_mc_ony.txt
./css 4 ../items/more_raid.csv ../items/bwl.txt start_with.txt -a ../confs/our_mc.csv | tee ../sample_output/our_raids_current.txt

for mode in 2 3 4 5; do
  ./css $mode ../items/more_raid.csv | tee ../sample_output/css_${mode}_bwl.txt
  ./css $mode ../items/more_raid.csv ../items/banned_and_onyxia_mc.txt | tee ../sample_output/css_${mode}_pre-raid.txt
  ./css $mode ../items/more_raid.csv ../items/bwl.txt | tee ../sample_output/css_${mode}_mc_ony.txt
done

mkdir ../sample_output/css5_regen
for regen in 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0; do
  ./css 5 ../items/more_raid.csv -b 0.2 -r $regen | tee ../sample_output/css5_regen/css5_b_0.2_r_$regen.txt;
done

mkdir ../sample_output/css5_buffs
for buffs in 0.1 0.2 0.3 0.4 0.5 0.6 0.7 0.8 0.9 1.0; do
  ./css 5 ../items/more_raid.csv -b $buffs -r 0.3 | tee ../sample_output/css5_buffs/css5_b_${buffs}_r_0.3.txt;
done

#./css 5 ../items/more_raid.csv ../items/bwl.txt -b 0.2 -r 0.3 | tee ../sample_output/css_5_b_0.2_r_0.3_mc_ony.txt