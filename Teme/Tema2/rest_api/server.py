import os
from flask import Flask
from flask import request, jsonify, Response
from flask_pymongo import PyMongo
import pymongo
from flask_wtf.csrf import CSRFProtect
import json

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

	if payload['nume'] is None or \
		payload['lat'] is None or \
		payload['lon'] is None:
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
		return Response(status=400)
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

	if payload['idTara'] is None or \
		payload['nume'] is None or \
		payload['lat'] is None or \
		payload['lon'] is None:
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
	pass

@app.route("/api/cities/country/<int:id_Tara>", methods=["GET"])
def get_city(id):
	pass

@app.route("/api/cities/<int:id>", methods=["PUT"])
def modify_city(id):
	pass

@app.route("/api/cities/<int:id>", methods=["DELETE"])
def delete_city(id):
	pass

"""
Functiile urmatoare realizeaza operatiile necesare
pentru temperatures
"""
@app.route("/api/temperatures", methods=["POST"])
def add_temperature():
	pass

@app.route("/api/temperatures", methods=["GET"])
def get_temperature():
	pass

@app.route("/api/temperatures/cities/<int:id_oras>", methods=["GET"])
def get_temperature_city(id_oras):
	pass

@app.route("/api/temperatures/countries/<int:id_tara>", methods=["GET"])
def get_temperature_country(id_tara):
	pass

@app.route("/api/temperatures/<int:id>", methods=["PUT"])
def modify_temperature():
	pass

@app.route("/api/temperatures/<int:id>", methods=["DELETE"])
def delete_temperature(id):
	pass

if __name__ == '__main__':
	app.run('0.0.0.0', port=6000, debug=True)