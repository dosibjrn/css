for stat in Int Spirit Healing Mp5 T1 T2 Darkmoon Hazza; do
  val=$(grep "| Upgrades at best in slot level, from start item to candidate, partial bonuses: 1: |" -B 40 "$@" | grep "Based on last: " -A 9 | grep $stat | awk -F: '{ total += $2 } END { print total/NR }')
  echo $stat: $val
done

# Int: 86.8049
# Spirit: 58.4913
# Sp / Healing: 100
# Mp5: 40.0633
# Crit: 485.17
# T1 3p/piece: 1085.62
# T2 3p/piece: 2943.19
# Hazza'rah's: 25809.4
# Darkmoon: 1197.69

