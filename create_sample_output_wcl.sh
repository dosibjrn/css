cat ../items/ban_after_p4.txt > ban_tmp_phase4.txt
cat ../items/not_for_priests.txt >> ban_tmp_phase4.txt

mkdir ../sample_output
mkdir ../sample_output/log_based

cp ../items/have.txt start_with.txt

# Top world bwl speedrun on 2020-06-02
python3 ../Overheal/readers/read_health_changes_from_api.py ytL9VY8DXfWdGRmJ progress_speed.txt

./css 4 ../items/phase5.csv ban_tmp_phase4.txt no-locks progress_speed_phase4 -a ../confs/wcl_bwl_speed_log.csv -l progress_speed.txt | tee ../sample_output/log_based/progress_speed_phase4.txt
./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks progress_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l progress_speed.txt | tee ../sample_output/log_based/progress_speed_phase5.txt

# Top Infamous (my guild) bwl speedrun on 2020-06-19
python3 ../Overheal/readers/read_health_changes_from_api.py Ra7k8ycG3VPDtbAH infamous_speed.txt

./css 4 ../items/phase5.csv ban_tmp_phase4.txt no-locks infamous_speed_phase4 -a ../confs/wcl_bwl_speed_log.csv -l infamous_speed.txt | tee ../sample_output/log_based/infamous_speed_phase4.txt
./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks infamous_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l infamous_speed.txt | tee ../sample_output/log_based/infamous_speed_phase5.txt

#Top world bwl speedrun on 2020-06-20
python3 ../Overheal/readers/read_health_changes_from_api.py Qy7wN89qG2ZFtdzf onslaught_speed.txt

./css 4 ../items/phase5.csv ban_tmp_phase4.txt no-locks onslaught_speed_phase4 -a ../confs/wcl_bwl_speed_log.csv -l onslaught_speed.txt | tee ../sample_output/log_based/onslaught_speed_phase4.txt
./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks onslaught_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l onslaught_speed.txt | tee ../sample_output/log_based/onslaught_speed_phase5.txt

#
