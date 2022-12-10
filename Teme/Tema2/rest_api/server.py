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

	if db.cities is None:
		return Response(status=400)

	index_cities = 0
	cities_list = list(db.cities.find())
	if len(cities_list) != 0:
		for city in cities_list:
			if city["_id"] > index_cities:
				index_cities = city["_id"]
		index_cities += 1

	city = {'_id': index_cities, 'name': payload['nume'], 'lat': payload['lat'], 'lon': payload['lon']}
	db.cities.insert_one(city)
	return jsonify({'id': index_cities}), Response(status=200)

@app.route("/api/countries", methods=["GET"])
def get_contries():
	cities_list = list(db.cities.find())
	return jsonify(cities_list), Response(status=200)

@app.route("/api/countries/<int:id>", methods=["PUT"])
def modify_country(id):
	pass

@app.route("/api/countries/<int:id>", methods=["DELETE"])
def delete_country(id):
	pass

"""
Functiile urmatoare realizeaza operatiile necesare
pentru cities
"""
@app.route("/api/cities", methods=["POST"])
def add_city(country, lat, lon):
	pass

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
	app.run('0.0.0.0', debug=True)