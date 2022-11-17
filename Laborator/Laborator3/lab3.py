from flask import Flask
from flask import request, jsonify, Response
import json

app = Flask(__name__)

movies = []

def check_movie(id):
	for i in movies:
		if i['id'] == id:
			return i
	return None

def check_movie_name(movie, id):
	film_entry = None
	for i in movies:
		if i['nume'] == movie:
			return False
	return True	

@app.route("/movies", methods=["GET"])
def get_movies():
	print(movies)
	return jsonify(movies), 200


@app.route("/movies", methods=["POST"])
def post_movies():

	payload = request.get_json(silent=True)
	if not payload:
		return Response(status=400)

	# check if the movie is already in the collection
	if check_movie(payload['nume']):
		# Error handling
		return jsonify({'status': 'Filmul exista deja!'}), 400

	# get the next id from the list
	id = len(movies) + 1
	movies.append({'id': id, 'nume': payload['nume']})
	return jsonify({'status': 'created'}), 201


@app.route("/movie/<int:id>", methods=["PUT"])
def put_movie(id):
	payload = request.get_json(silent=True)
	if not payload:
		return Response(status=400)
	
	if payload.get('nume') is None:
		return Response(status=400)

	# check if the movie is in the collection
	mov = check_movie(id)
	if mov is None:	
		return jsonify({'status': 'Filmul nu exista!'}), 404

	if not check_movie_name(payload['nume'], id):
		return jsonify({'status': 'Filmul deja exista!'}), 404

	mov['nume'] = payload['nume']
	return jsonify({'status': 'movie modified'}), 200


@app.route("/movie/<int:id>", methods=["GET"])
def get_movie(id):
	# check if the movie is in the collection
	mov = check_movie(id)
	if mov is None:	
		return jsonify({'status': 'Filmul nu exista!'}), 404

	return jsonify(mov), 200

@app.route("/movie/<int:id>", methods=["DELETE"])
def delete_movie(id):
	# check if the movie is in the collection
	mov = check_movie(id)
	if mov is None:	
		return jsonify({'status': 'Filmul nu exista!'}), 404

	if mov is not None:
		movies.remove(mov)
		return jsonify(mov), 200

@app.route("/reset", methods=["DELETE"])
def reset_movies():
	while len(movies) > 0:
		movies.pop(0)

	return jsonify({'status': 'Filmele au fost sterse!'}), 200

if __name__ == '__main__':
    app.run('0.0.0.0', debug=True)