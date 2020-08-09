mkdir ../sample_output
mkdir ../sample_output/log_based
mkdir ../sample_output/log_based/top_aq40_horde_speed

cp ../items/have.txt start_with.txt

python3 ../Overheal/readers/read_health_changes_from_api.py mP1GvFZYzy3cfAX6 senseless_aq40_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  senseless_aq40_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l senseless_aq40_speed.txt | tee ../sample_output/log_based/top_aq40_horde_speed/senseless_aq40_speed_phase5.log

python3 ../Overheal/readers/read_health_changes_from_api.py dpkxBMG8aQYbN7c6 chocolate_milk_aq40_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  chocolate_milk_aq40_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l chocolate_milk_aq40_speed.txt | tee ../sample_output/log_based/top_aq40_horde_speed/chocolate_milk_aq40_speed_phase5.log

python3 ../Overheal/readers/read_health_changes_from_api.py TgxLzW2rAjnJtd1R dread_aq40_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  dread_aq40_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l dread_aq40_speed.txt | tee ../sample_output/log_based/top_aq40_horde_speed/dread_aq40_speed_phase5.log

python3 ../Overheal/readers/read_health_changes_from_api.py G7tXdq4N9AKWCvDM faint_aq40_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  faint_aq40_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l faint_aq40_speed.txt | tee ../sample_output/log_based/top_aq40_horde_speed/faint_aq40_speed_phase5.log

python3 ../Overheal/readers/read_health_changes_from_api.py qx3pBbYCHgVh8JDR nightlife_aq40_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  nightlife_aq40_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l nightlife_aq40_speed.txt | tee ../sample_output/log_based/top_aq40_horde_speed/nightlife_aq40_speed_phase5.log

python3 ../Overheal/readers/read_health_changes_from_api.py gamT9f71zKk23tjc onslaught_aq40_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  onslaught_aq40_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l onslaught_aq40_speed.txt | tee ../sample_output/log_based/top_aq40_horde_speed/onslaught_aq40_speed_phase5.log

