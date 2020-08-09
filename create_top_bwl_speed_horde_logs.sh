mkdir ../sample_output
mkdir ../sample_output/log_based
mkdir ../sample_output/log_based/top20_bwl_horde_speed

cp ../items/have.txt start_with.txt

python3 ../Overheal/readers/read_health_changes_from_api.py Qy7wN89qG2ZFtdzf onslaught_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  onslaught_bwl_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l onslaught_speed.txt | tee ../sample_output/log_based/top20_bwl_horde_speed/onslaught_bwl_speed_phase5.log
python3 ../Overheal/readers/read_health_changes_from_api.py 4cJ18djtXyTVnGQP resist_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  resist_bwl_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l resist_speed.txt | tee ../sample_output/log_based/top20_bwl_horde_speed/resist_bwl_speed_phase5.log
python3 ../Overheal/readers/read_health_changes_from_api.py LdVQ1z46BbJkKfG9 senseless_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  senseless_bwl_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l senseless_speed.txt | tee ../sample_output/log_based/top20_bwl_horde_speed/senseless_bwl_speed_phase5.log
python3 ../Overheal/readers/read_health_changes_from_api.py yqdb3C4gkwMX8HY9 fiend_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  fiend_bwl_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l fiend_speed.txt | tee ../sample_output/log_based/top20_bwl_horde_speed/fiend_bwl_speed_phase5.log
python3 ../Overheal/readers/read_health_changes_from_api.py LVtG8pcgXaKANv24 statement_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  statement_bwl_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l statement_speed.txt | tee ../sample_output/log_based/top20_bwl_horde_speed/statement_bwl_speed_phase5.log
python3 ../Overheal/readers/read_health_changes_from_api.py g2Lp7KaBmGrYZPAz zoo_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  zoo_bwl_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l zoo_speed.txt | tee ../sample_output/log_based/top20_bwl_horde_speed/zoo_bwl_speed_phase5.log
python3 ../Overheal/readers/read_health_changes_from_api.py 4c9p3RBZyHatnL2K dreamstate_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  dreamstate_bwl_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l dreamstate_speed.txt | tee ../sample_output/log_based/top20_bwl_horde_speed/dreamstate_bwl_speed_phase5.log
python3 ../Overheal/readers/read_health_changes_from_api.py yWdLxD3Fcva7GKkA innuendo_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  innuendo_bwl_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l innuendo_speed.txt | tee ../sample_output/log_based/top20_bwl_horde_speed/innuendo_bwl_speed_phase5.log
python3 ../Overheal/readers/read_health_changes_from_api.py B4N7bJ2hZkrgQFDX rampage_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  rampage_bwl_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l rampage_speed.txt | tee ../sample_output/log_based/top20_bwl_horde_speed/rampage_bwl_speed_phase5.log
python3 ../Overheal/readers/read_health_changes_from_api.py bBTNKh4MyPzRgGFZ calamity_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  calamity_bwl_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l calamity_speed.txt | tee ../sample_output/log_based/top20_bwl_horde_speed/calamity_bwl_speed_phase5.log
python3 ../Overheal/readers/read_health_changes_from_api.py Km37CpMzkyGnPFcT own_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  own_bwl_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l own_speed.txt | tee ../sample_output/log_based/top20_bwl_horde_speed/own_bwl_speed_phase5.log
python3 ../Overheal/readers/read_health_changes_from_api.py 7nLMCNZ1RktKvAb8 fika_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  fika_bwl_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l fika_speed.txt | tee ../sample_output/log_based/top20_bwl_horde_speed/fika_bwl_speed_phase5.log
python3 ../Overheal/readers/read_health_changes_from_api.py MnBKRtx6Xvjmdgpc who_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  who_bwl_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l who_speed.txt | tee ../sample_output/log_based/top20_bwl_horde_speed/who_bwl_speed_phase5.log
python3 ../Overheal/readers/read_health_changes_from_api.py 6FhknMKNyfHTbp7A scrubclub_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  scrubclub_bwl_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l scrubclub_speed.txt | tee ../sample_output/log_based/top20_bwl_horde_speed/scrubclub_bwl_speed_phase5.log
python3 ../Overheal/readers/read_health_changes_from_api.py Ra7k8ycG3VPDtbAH infamous_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  infamous_bwl_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l infamous_speed.txt | tee ../sample_output/log_based/top20_bwl_horde_speed/infamous_bwl_speed_phase5.log
python3 ../Overheal/readers/read_health_changes_from_api.py y9D1nWw2tFpr6L7P vevfabriken_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  vevfabriken_bwl_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l vevfabriken_speed.txt | tee ../sample_output/log_based/top20_bwl_horde_speed/vevfabriken_bwl_speed_phase5.log
python3 ../Overheal/readers/read_health_changes_from_api.py 4Hf7DGhaP3NTdm29 chocolate_milk_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  chocolate_milk_bwl_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l chocolate_milk_speed.txt | tee ../sample_output/log_based/top20_bwl_horde_speed/chocolate_milk_bwl_speed_phase5.log
python3 ../Overheal/readers/read_health_changes_from_api.py kpL7VZay9jCtXAx4 soon_tm_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  soon_tm_bwl_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l soon_tm_speed.txt | tee ../sample_output/log_based/top20_bwl_horde_speed/soon_tm_bwl_speed_phase5.log
python3 ../Overheal/readers/read_health_changes_from_api.py 3NZbT9cPJhMt8qWQ storm_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  storm_bwl_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l storm_speed.txt | tee ../sample_output/log_based/top20_bwl_horde_speed/storm_bwl_speed_phase5.log
python3 ../Overheal/readers/read_health_changes_from_api.py Lna6BhT4AqwR8QNb helix_speed.txt; ./css 4 ../items/phase5.csv ../items/not_for_priests.txt no-locks  helix_bwl_speed_phase5 -a ../confs/wcl_bwl_speed_log.csv -l helix_speed.txt | tee ../sample_output/log_based/top20_bwl_horde_speed/helix_bwl_speed_phase5.log
bash ../bin/grepping.sh ../sample_output/log_based/top20_bwl_horde_speed/*_phase5.log > ../sample_output/log_based/top20_bwl_horde_speed/weights_avg_phase5.txt
bash ../bin/grepping.sh ../sample_output/log_based/top20_bwl_horde_speed/*_phase4.log > ../sample_output/log_based/top20_bwl_horde_speed/weights_avg_phase4.txt

