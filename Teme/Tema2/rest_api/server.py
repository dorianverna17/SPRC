import os
from flask import Flask
from flask import request, jsonify, Response
from flask_pymongo import PyMongo
import pymongo
from flask_wtf.csrf import CSRFProtect
import json
from datetime import datetime
from datetime import date

app = Flask(__name__)
app.config["MONGO_URI"] = \
	'mongodb://' + os.environ['MONGODB_USERNAME'] + \
	':' + os.environ['MONGODB_PASSWORD'] + '@' + \
	os.environ['MONGODB_HOSTNAME'] + ':27017/mongo?authSource=admin&retryWrites=true&w=majority'
app.config["WTF_CSRF_CHECK_DEFAULT"] = False

myclient = pymongo.MongoClient(app.config["MONGO_URI"])
db = myclient["mongo"]

"""
Functiile urmatoare realizeaza operatiile necesare
pentru countries
"""
@app.route("/api/countries", methods=["POST"])
def add_country():
	payload = request.get_json(silent=True)
	if not payload:
		return Response(status=400)

	if not 'nume' in payload or \
		not 'lat' in payload or \
		not 'lon' in payload:
		return Response(status=400)

	if db.countries is None:
		return Response(status=400)

	index_countries = 0
	countries_list = list(db.countries.find())
	if len(countries_list) != 0:
		for country in countries_list:
			if country["_id"] > index_countries:
				index_countries = country["_id"]
		index_countries += 1

	country = {'_id': index_countries, 'nume': payload['nume'], 'lat': payload['lat'], 'lon': payload['lon']}
	db.countries.insert_one(country)
	return jsonify({'id': index_countries}), 201

@app.route("/api/countries", methods=["GET"])
def get_contries():
	countries_list = list(db.countries.find())
	countries_list = [{'id': country['_id'], 'nume': country['nume'], 'lat': country['lat'], \
		'lon': country['lon']} for country in countries_list]
	return jsonify(countries_list), 200

@app.route("/api/countries/<int:id>", methods=["PUT"])
def modify_country(id):
	update_data = request.json
	if update_data is None:
		return Response(status=400)
	db.countries.find_one_and_update({'_id': id}, \
		{"$set": update_data})
	return Response(status=200)

@app.route("/api/countries/<int:id>", methods=["DELETE"])
def delete_country(id):
	if db.countries.count_documents({"_id": id}) == 0:
		return Response(status=404)
	db.countries.delete_one({'_id': id})
	return Response(status=200)

"""
Functiile urmatoare realizeaza operatiile necesare
pentru cities
"""
@app.route("/api/cities", methods=["POST"])
def add_city():
	payload = request.get_json(silent=True)
	if not payload:
		return Response(status=400)

	if not 'idTara' in payload or \
		not 'nume' in payload or \
		not 'lat' in payload or \
		not 'lon' in payload:
		return Response(status=400)

	if db.countries is None:
		return Response(status=400)

	if db.cities is None:
		return Response(status=400)

	# Check existence of country
	if db.countries.count_documents({"_id": payload['idTara']}) == 0:
		return Response(status=400)

	index_cities = 0
	cities_list = list(db.cities.find())
	if len(cities_list) != 0:
		for city in cities_list:
			if city["_id"] > index_cities:
				index_cities = city["_id"]
		index_cities += 1

	city = {'_id': index_cities, 'idTara': payload['idTara'], 'nume': payload['nume'], \
		'lat': payload['lat'], 'lon': payload['lon']}
	db.cities.insert_one(city)
	return jsonify({'id': index_cities}), 201

@app.route("/api/cities", methods=["GET"])
def get_cities():
	cities_list = list(db.cities.find())
	cities_list = [{'id': city['_id'], 'nume': city['nume'], 'lat': city['lat'], \
		'lon': city['lon']} for city in cities_list]
	return jsonify(cities_list), 200

@app.route("/api/cities/country/<int:id_Tara>", methods=["GET"])
def get_cities_by_country(id_Tara):
	cities_list = list(db.cities.find({"idTara": id_Tara}))
	cities_list = [{'id': city['_id'], 'nume': city['nume'], 'lat': city['lat'], \
		'lon': city['lon']} for city in cities_list]
	return jsonify(cities_list), 200

@app.route("/api/cities/<int:id>", methods=["PUT"])
def modify_city(id):
	update_data = request.json
	if update_data is None:
		return Response(status=400)
	db.cities.find_one_and_update({'_id': id}, \
		{"$set": update_data})
	return Response(status=200)

@app.route("/api/cities/<int:id>", methods=["DELETE"])
def delete_city(id):
	if db.cities.count_documents({"_id": id}) == 0:
		return Response(status=404)
	db.cities.delete_one({'_id': id})
	return Response(status=200)

"""
Functiile urmatoare realizeaza operatiile necesare
pentru temperatures
"""
@app.route("/api/temperatures", methods=["POST"])
def add_temperature():
	payload = request.get_json(silent=True)
	if not payload:
		return Response(status=400)

	if not 'idOras' in payload or \
		not 'valoare' in payload:
		return Response(status=400)

	if type(payload['valoare']) == str:
		return Response(status=400)

	if db.countries is None:
		return Response(status=400)

	if db.cities is None:
		return Response(status=400)

	if db.temperatures is None:
		return Response(status=400)

	# Check existence of country
	if db.cities.count_documents({"_id": payload['idOras']}) == 0:
		return Response(status=400)

	index_temperatures = 0
	temperatures_list = list(db.temperatures.find())
	if len(temperatures_list) != 0:
		for temperature in temperatures_list:
			if temperature["_id"] > index_temperatures:
				index_temperatures = temperature["_id"]
		index_temperatures += 1

	timestamp = datetime.today().isoformat()

	temperature = {'_id': index_temperatures, 'idOras': payload['idOras'], \
		'valoare': payload['valoare'], 'timestamp': timestamp}
	db.temperatures.insert_one(temperature)
	return jsonify({'id': index_temperatures}), 201

@app.route("/api/temperatures", methods=["GET"])
def get_temperature():
	lat = request.args.get('lat')
	lon = request.args.get('lon')
	from_date = request.args.get('from')
	until_date = request.args.get('until')

	temp = db.temperatures

	cities_ids_lat = []
	cities_ids_lon = []
	cities_list = list(db.cities.find())

	if not lat is None:
		for city in cities_list:
			if city["lat"] == float(lat):
				cities_ids_lat.append(city["_id"])

	if not lon is None:
		cities_ids = []
		for city in cities_list:
			if city["lon"] == float(lon):
				cities_ids_lon.append(city["_id"])

	# join cities lat with cities lon
	if (not lon is None) and (not lat is None):
		cities_ids = [id for id in cities_ids_lat if id in cities_ids_lon]
	elif (not lon is None):
		cities_ids = cities_ids_lon
	else:
		cities_ids = cities_ids_lat

	if from_date is None:
		from_date = datetime(1,1,1).isoformat()
	if until_date is None:
		until_date = datetime(9999,1,1).isoformat()

	if len(cities_ids) > 0:
		temp = temp.find({'idOras': { '$in': cities_ids }, \
			'timestamp': { '$gt': from_date, '$lt': until_date}})
	else:
		temp = temp.find({'timestamp': { '$gt': from_date, \
			'$lt': until_date}})

	temp = list(temp)
	modified_temp = [{'id': t['_id'], 'valoare': t['valoare'], \
		'timestamp': t['timestamp']} for t in temp]
	return jsonify(modified_temp), 200

@app.route("/api/temperatures/cities/<int:id_oras>", methods=["GET"])
def get_temperature_city(id_oras):
	from_date = request.args.get('from')
	until_date = request.args.get('until')

	temp = db.temperatures

	if from_date is None:
		from_date = datetime(1,1,1).isoformat()
	if until_date is None:
		until_date = datetime(9999,12,31).isoformat()

	temp = temp.find({'idOras': id_oras, \
			'timestamp': { '$gt': from_date, '$lt': until_date}})

	temp = list(temp)
	modified_temp = [{'id': t['_id'], 'valoare': t['valoare'], \
		'timestamp': t['timestamp']} for t in temp]
	return jsonify(modified_temp), 200

@app.route("/api/temperatures/countries/<int:id_tara>", methods=["GET"])
def get_temperature_country(id_tara):
	from_date = request.args.get('from')
	until_date = request.args.get('until')

	temp = db.temperatures

	cities_ids = []
	cities_list = list(db.cities.find())

	for city in cities_list:
		if city["idTara"] == id_tara:
			cities_ids.append(city["_id"])

	if from_date is None:
		from_date = datetime(1,1,1).isoformat()
	if until_date is None:
		until_date = datetime(9999,1,1).isoformat()

	if len(cities_ids) > 0:
		temp = temp.find({'idOras': { '$in': cities_ids }, \
			'timestamp': { '$gt': from_date, '$lt': until_date}})
	else:
		temp = temp.find({'timestamp': { '$gt': from_date, \
			'$lt': until_date}})

	temp = list(temp)
	modified_temp = [{'id': t['_id'], 'valoare': t['valoare'], \
		'timestamp': t['timestamp']} for t in temp]
	return jsonify(modified_temp), 200

@app.route("/api/temperatures/<int:id>", methods=["PUT"])
def modify_temperature(id):
	update_data = request.json
	if update_data is None:
		return Response(status=400)
	db.temperatures.find_one_and_update({'_id': id}, \
		{"$set": update_data})
	return Response(status=200)

@app.route("/api/temperatures/<int:id>", methods=["DELETE"])
def delete_temperature(id):
	if db.temperatures.count_documents({"_id": id}) == 0:
		return Response(status=404)
	db.temperatures.delete_one({'_id': id})
	return Response(status=200)

if __name__ == '__main__':
	app.run('0.0.0.0', port=6000, debug=True)