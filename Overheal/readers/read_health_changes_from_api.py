"""
Read all health change data from WCL API.

By: Kalle Karhu (Paisti/dosibjrn), 2020
    Greatly helped by examples by: Filip Gokstorp (Saintis), 2020
"""
import requests
import read_from_api
from json.decoder import JSONDecodeError
from datetime import datetime

def get_damage_taken(code, start=0, end=None, names=None, for_player=None):
    """
    Gets damage taken events for specified log code.

    :param code: the WarcraftLogs code.
    :param start: the start time, in milliseconds, to get heals for.
    :param end: the end time, in milliseconds, to get heals for. If None, gets logs for up to 3 hours.

    :returns damage_taken, lists of friendly damage taken events
    """
    if end is None:
        end = start + 3 * 60 * 60 * 1000  # look at up to 3h of data

    if names is None:
        names = dict()

    damage_taken = []

    next_start = start

    print("Fetching damage taken data from WCL...")
    progress_bar = read_from_api.ProgressBar(end - start, length=70)

    # will have to loop to get results
    request_more = True
    while request_more:
        url = f"{read_from_api.API_ROOT}/report/events/damage-taken/{code}?start={next_start}&end={end}&api_key={read_from_api.API_KEY}"

        print(progress_bar.render(next_start - start), end="\r")

        data = read_from_api._get_api_request(url)
        events = data["events"]
        if "nextPageTimestamp" in data:
            next_start = data["nextPageTimestamp"]
        else:
            request_more = False

        for e in events:
            try:
                timestamp = e["timestamp"]
                timestamp = datetime.fromtimestamp(timestamp / 1000.0).time()
                spell_id = str(e["ability"]["guid"])

                if "targetID" not in e:
                    # damage not to player, skipping
                    continue
                source = ""
                target = e["targetID"]
                target = names.get(target, f"[pid {target}]")
                hp_diff = -1.0*float(e["amount"])
                # print(timestamp,",",source,",",target,",", hp_diff)
                damage_taken.append(
                    (
                        timestamp,
                        source,
                        target,
                        hp_diff,
                    )
                )
            except Exception as ex:
                print("Exception while handling line", e)
                print(ex)

    print(progress_bar.render(end - start))

    return damage_taken

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser("Tests the api reading code.")

    parser.add_argument("source")
    parser.add_argument("file_out")
    args = parser.parse_args()

    source = args.source
    code = source

    if "https://" in source or "http://" in source:
        code = source.split("#")[0].split("/")[-1]

    start = 0
    end = None
    names = read_from_api.get_player_names(code)
    damage_taken = get_damage_taken(code, start, end, names)

    print("Another round to get healing, absorbs and periodics.")
    heals, periodics, absorbs = read_from_api.get_heals(code)

    print("Heals")
    for h in heals[:20]:
        print("  ", *h)

    print("Periodics")
    for h in periodics[:20]:
        print("  ", *h)

    print("Absorbs")
    for h in absorbs[:20]:
        print("  ", *h)

    print("Damage taken events")
    for h in damage_taken[:20]:
        print("  ", *h)


    file_out = args.file_out
    print("Saving results as csv to: ", file_out)

    with open(file_out, 'w') as f:
        for h in heals:
            line = str(h[0]) + ", " + str(h[1]) + ", " + str(h[3]) + ", " + str(h[4]) + "\n"
            f.write(line)
        for h in absorbs:
            line = str(h[0]) + ", " + str(h[1]) + ", " + str(h[3]) + ", " + str(h[4]) + "\n"
            f.write(line)
        for h in periodics:
            line = str(h[0]) + ", " + str(h[1]) + ", " + str(h[3]) + ", " + str(h[4]) + "\n"
            f.write(line)
        for h in damage_taken:
            line = str(h[0]) + ", " + str(h[1]) + ", " + str(h[2]) + ", " + str(h[3]) + "\n"
            f.write(line)
    print("Saving done.")

