__author__ = 'katharine'

import csv
import struct
import time
import datetime


def generate_files(source_dir, target_dir):
    stops_txt = [x for x in csv.DictReader(open("%s/stops.txt" % source_dir, 'rb')) if x['location_type'] == '0']
    print "%d stops" % len(stops_txt)

    name_replacements = (
        ('Caltrain', ''),
        ('Station', ''),
        ('Mt View', 'Mountain View'),
        ('So. San Francisco', 'South SF'),
        ('South San Francisco', 'South SF'),
    )

    stop_parent_map = {}
    stop_name_map = {}
    stop_map = {}
    stops = []
    for s in stops_txt:
        if s['parent_station'] != '' and s['parent_station'] in stop_parent_map:
            stop_map[int(s['stop_code'])] = stop_parent_map[s['parent_station']]
            continue
        for replacement in name_replacements:
            s['stop_name'] = s['stop_name'].replace(*replacement)
        s['stop_name'] = s['stop_name'].rstrip()
        if s['stop_name'] in stop_name_map:
            stop_map[int(s['stop_code'])] = stop_name_map[s['stop_name']]
            continue
        stop_map[int(s['stop_code'])] = len(stops)
        stop_parent_map[s['parent_station']] = len(stops)
        stop_name_map[s['stop_name']] = len(stops)
        stops.append({
            'name': s['stop_name'],
            'zone': int(s['zone_id']) if s['zone_id'] != '' else 0,
            'lat': float(s['stop_lat']),
            'lon': float(s['stop_lon'])
        })

    with open('%s/stops.dat' % target_dir, 'wb') as f:
        f.write(struct.pack('<B', len(stops)))
        for stop in stops:
            f.write(struct.pack('<B18sii', stop['zone'], stop['name'], int(stop['lat'] * 1000000), int(stop['lon'] * 1000000)))

    calendar_txt = list(csv.DictReader(open("%s/calendar.txt" % source_dir, 'rb')))

    cal = []
    cal_map = {}
    for i, x in enumerate(calendar_txt):
        cal_map[x['service_id']] = len(cal)
        end_time = datetime.datetime.strptime(x['end_date'], '%Y%m%d') + datetime.timedelta(1, hours=2)
        cal.append({
            'id': cal_map[x['service_id']],
            'start': time.mktime(time.strptime(x['start_date'], '%Y%m%d')),
            'end': time.mktime(end_time.timetuple()),
            'days': (
                (int(x['monday'])    << 0) |
                (int(x['tuesday'])   << 1) |
                (int(x['wednesday']) << 2) |
                (int(x['thursday'])  << 3) |
                (int(x['friday'])    << 4) |
                (int(x['saturday'])  << 5) |
                (int(x['sunday'])    << 6)
            )
        })

    calendar_dates_txt = list(csv.DictReader(open("%s/calendar_dates.txt" % source_dir, 'rb')))
    for i, x in enumerate(calendar_dates_txt):
        if x['service_id'] in cal_map:
            # XXX: Would be nice to find a way to mark special dates.  But
            # we can't, right now.  Oh well.
            continue

        cal_map[x['service_id']] = len(cal)
        start_time = datetime.datetime.strptime(x['date'], '%Y%m%d')
        end_time = start_time + datetime.timedelta(1, hours=2)
        cal.append({
            'id': cal_map[x['service_id']],
            'start': time.mktime(start_time.timetuple()),
            'end': time.mktime(end_time.timetuple()),
            'days': 0x7F,
        })

    with open('%s/calendar.dat' % target_dir, 'wb') as f:
        f.write(struct.pack('<B', len(cal)))
        for c in cal:
            f.write(struct.pack('<IIB', int(c['start']), int(c['end']), c['days']))

    trips_txt = list(csv.DictReader(open("%s/trips.txt" % source_dir, "rb")))

    tr = []
    tr_map = {}

    # These shouldn't be hardcoded, and should instead be inferred from routes.txt.
    route_map = {
        "BABY BULLET": 0,
        "LIMITED": 1,
        "LOCAL": 2,
        "SHUTTLE": 3,
        "Bu-130": 0,
        "Li-130": 1,
        "Lo-130": 2,
        "TaSj-130": 3,
        "Sp-130": 2, # XXX: Special Event Extra Service
    }

    short_name_replacements = (
      ('Tamien SJ shuttle', ''),
      ('S', ''),
      ('shuttle', ''),
    )
    
    for i, trip in enumerate(trips_txt):
        for replacement in short_name_replacements:
            trip['trip_short_name'] = trip['trip_short_name'].replace(*replacement)
        tr.append({
            'direction': int(not int(trip['direction_id'])),  # We picked opposing values for north/south.
            'route': route_map[trip['route_id']],
            'service': cal_map[trip['service_id']],
            'trip_name': int(trip['trip_short_name'])}),
        tr_map[trip['trip_id']] = i

    with open('%s/trips.dat' % target_dir, 'wb') as f:
        f.write(struct.pack('<H', len(tr)))
        for t in tr:
            f.write(struct.pack('<HBBB', t['trip_name'], t['direction'], t['route'], t['service']))

    times_txt = list(csv.DictReader(open("%s/stop_times.txt" % source_dir)))

    tm = sorted([{
        'time': (int(x['arrival_time'].split(':')[0])*60 + int(x['arrival_time'].split(':')[1])),
        'stop': stop_map[int(x['stop_id'])],
        'sequence': int(x['stop_sequence']),
        'trip': tr_map[x['trip_id']]
    } for x in times_txt], key=lambda y: y['time'])

    with open('%s/times.dat' % target_dir, 'wb') as f:
        f.write(struct.pack('<H', len(tm)))
        for t in tm:
            f.write(struct.pack('<HHBB', t['trip'], t['time'], t['stop'], t['sequence']))

    stop_times = [sorted([i for i, x in enumerate(tm) if x['stop'] == stop], key=lambda t: tm[t]['time']) for stop, s in enumerate(stops)]
    lengths = [len(x) for x in stop_times]

    with open('%s/stop_index.dat' % target_dir, 'wb') as f:
        f.write(struct.pack('<B', len(lengths)))
        counter = len(lengths)*4 + 1
        for l in lengths:
            f.write(struct.pack('<HH', counter, l))
            counter += l*2

        for s in stop_times:
            for x in s:
                f.write(struct.pack('<H', x))


    trip_stops = [sorted([i for i, x in enumerate(tm) if x['trip'] == trip], key=lambda k: tm[k]['stop']) for trip, s in enumerate(tr)]
    lengths = map(len, trip_stops)
    with open('%s/trip_index.dat' % target_dir, 'wb') as f:
        f.write(struct.pack('<H', len(lengths)))
        counter = len(lengths) * 3 + 2
        data_start = counter
        for l in lengths:
            f.write(struct.pack('<HB', counter, l))
            counter += l*2

        if data_start != f.tell():
            raise Exception("%d != %d" % (counter, f.tell()))

        for s in trip_stops:
            for x in s:
                f.write(struct.pack('<H', x))

        if f.tell() != counter:
            raise Exception("Not the expected length!")


if __name__ == "__main__":
    import sys
    generate_files(sys.argv[1], sys.argv[2])
