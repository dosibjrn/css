# Run from e.g. css/build or css/bin

cp ../items/have_mod_locked.txt start_with.txt

for instance in bwl mc onyxia zg; do
  ./css 4 ../items/more_raid.csv ../items/not_for_priests.txt start_with.txt recent_${instance}_current -a ../confs/bwl_2020-04-09.csv -l /c/Program\ Files\ \(x86\)/World\ of\ Warcraft/_classic_/Logs/recent/$instance.txt | tee ../sample_output/log_based/recent_${instance}_current.txt
  ./css 4 ../items/more_raid.csv ../items/not_for_priests.txt no-locks recent_${instance}_bis -a ../confs/bwl_2020-04-09.csv -l /c/Program\ Files\ \(x86\)/World\ of\ Warcraft/_classic_/Logs/recent/$instance.txt | tee ../sample_output/log_based/recent_${instance}_bis.txt
done

mkdir ../sample_output/tags/
cp *.pawn_tag.txt ../sample_output/tags/. 
tail -n 1 *.pawn_tag.txt > ../sample_output/tags/all_latest_tags.txt

