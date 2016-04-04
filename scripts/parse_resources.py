from __future__ import division
import struct
import time
import os
import datetime

def parse_resources(resources_dir):
	stops_dat = open(os.path.join(resources_dir, 'stops.dat'), 'rb').read()

	header_len = 1 # struct.calcsize('<B')
	record_len = 27 # struct.calcsize('<B18sii')
	stops_len = struct.unpack_from('<B', stops_dat)[0]
	stops = []
	for counter in range(stops_len):
		zone, name, lat, lon = struct.unpack_from('<B18sii', stops_dat, header_len + counter * record_len)
		stops.append({
			'id': counter,
			'name': name.rstrip('\x00'),
			'zone': zone,
			'lat': lat / 1000000, # They're stored in the struct as integers by multiplying by 1,000,000; I want them as floats (hence __future__ division import)
			'lon': lon / 1000000
		})

	calendar_dat = open(os.path.join(resources_dir, 'calendar.dat'), 'rb').read()

	header_len = 1 # struct.calcsize('<B')
	record_len = 9 # struct.calcsize('<IIB')
	calendar_len = struct.unpack_from('<B', calendar_dat)[0]
	calendar = []
	for counter in range(calendar_len):
		start, end, days = struct.unpack_from('<IIB', calendar_dat, header_len + counter * record_len)
		start = time.strftime('%Y-%m-%d', time.localtime(start))
		end = datetime.datetime.fromtimestamp(end) - datetime.timedelta(1, hours=2)
		end = time.strftime('%Y-%m-%d', end.timetuple())
		days_list = []
		# Simplest way to do this, not pretty...
		if days & (1 << 0):
			days_list.append('Monday')
		if days & (1 << 1):
			days_list.append('Tuesday')
		if days & (1 << 2):
			days_list.append('Wednesday')
		if days & (1 << 3):
			days_list.append('Thursday')
		if days & (1 << 4):
			days_list.append('Friday')
		if days & (1 << 5):
			days_list.append('Saturday')
		if days & (1 << 6):
			days_list.append('Sunday')
		calendar.append({
			'id': counter,
			'start': start,
			'end': end,
			'days': days_list
		})

	route_map = {
		0: 'Bullet',
		1: 'Limited',
		2: 'Local',
		3: 'Shuttle'
	}

	trips_dat = open(os.path.join(resources_dir, 'trips.dat'), 'rb').read()

	header_len = 2 # struct.calcsize('<H')
	record_len = 5 # struct.calcsize('<HBBB')
	trips_len = struct.unpack_from('<H', trips_dat)[0]
	trips = []
	for counter in range(trips_len):
		name, direction, route, service = struct.unpack_from('<HBBB', trips_dat, header_len + counter * record_len)
		direction = 'Southbound' if direction else 'Northbound'
		route = route_map[route]
		service = calendar[service]
		trips.append({
			'id': counter,
			'name': name,
			'direction': direction,
			'route': route,
			'service': service
		})

	times_dat = open(os.path.join(resources_dir, 'times.dat'), 'rb').read()

	header_len = 2 # struct.calcsize('<H')
	record_len = 6 # struct.calcsize('<HHBB')
	times_len = struct.unpack_from('<H', times_dat)[0]
	times = []
	for counter in range(times_len):
		trip, time_i, stop, sequence = struct.unpack_from('<HHBB', times_dat, header_len + counter * record_len)
		trip = trips[trip]
		time_str = '%d:%d' % (time_i // 60, time_i % 60)
		stop = stops[stop]
		times.append({
			'id': counter,
			'trip': trip,
			'time': time_str,
			'stop': stop,
			'sequence': sequence
		})

	stop_index_dat = open(os.path.join(resources_dir, 'stop_index.dat'), 'rb').read()

	header_len = 1 # struct.calcsize('<B')
	record_len = 4 # struct.calcsize('<HH')
	stop_index_len = struct.unpack_from('<B', stop_index_dat)[0]
	stop_index = []
	for counter1 in range(stop_index_len):
		position, length = struct.unpack_from('<HH', stop_index_dat, header_len + counter1 * record_len)
		stop_time_list = []
		for counter2 in range(length):
			time_i = struct.unpack_from('<H', stop_index_dat, position + counter2 * 2)[0]
			stop_time_list.append(times[time_i])
		stop_index.append({
			'stop': stops[counter1],
			'index': stop_time_list
		})

	trip_index_dat = open(os.path.join(resources_dir, 'trip_index.dat'), 'rb').read()

	header_len = 2 # struct.calcsize('<H')
	record_len = 3 # struct.calcsize('<HB')
	trip_index_len = struct.unpack_from('<H', trip_index_dat)[0]
	trip_index = []
	for counter1 in range(trip_index_len):
		position, length = struct.unpack_from('<HB', trip_index_dat, header_len + counter1 * record_len)
		trip_time_list = []
		for counter2 in range(length):
			time_i = struct.unpack_from('<H', trip_index_dat, position + counter2 * 2)[0]
			trip_time_list.append(times[time_i])
		trip_index.append({
			'trip': trips[counter1],
			'index': trip_time_list
		})

	return stops_len, stops, calendar_len, calendar, trips_len, trips, times_len, times, stop_index_len, stop_index, trip_index_len, trip_index

if __name__ == '__main__':
	import sys
	if len(sys.argv) > 1:
		print(repr(parse_resources(sys.argv[1])))
	else:
		print('Please provide the path to the resources directory, eg ~/git/pebble-caltrain/resources/data')
