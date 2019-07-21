import os
from flask import Flask
from flask import jsonify
from flask import make_response
from flask import request
from flask import current_app
from flask import jsonify
from datetime import timedelta
from functools import update_wrapper

app = Flask(__name__)

def crossdomain(origin=None, methods=None, headers=None,
                max_age=21600, attach_to_all=True,
                automatic_options=True):
    if methods is not None:
        methods = ', '.join(sorted(x.upper() for x in methods))
    if headers is not None and not isinstance(headers, str):
        headers = ', '.join(x.upper() for x in headers)
    if not isinstance(origin, str):
        origin = ', '.join(origin)
    if isinstance(max_age, timedelta):
        max_age = max_age.total_seconds()

    def get_methods():
        if methods is not None:
            return methods

        options_resp = current_app.make_default_options_response()
        return options_resp.headers['allow']

    def decorator(f):
        def wrapped_function(*args, **kwargs):
            if automatic_options and request.method == 'OPTIONS':
                resp = current_app.make_default_options_response()
            else:
                resp = make_response(f(*args, **kwargs))
            if not attach_to_all and request.method != 'OPTIONS':
                return resp

            h = resp.headers

            h['Access-Control-Allow-Origin'] = origin
            h['Access-Control-Allow-Methods'] = get_methods()
            h['Access-Control-Max-Age'] = str(max_age)
            if headers is not None:
                h['Access-Control-Allow-Headers'] = headers
            return resp

        f.provide_automatic_options = False
        return update_wrapper(wrapped_function, f)
    return decorator

@app.route("/coffe", methods=['GET', 'POST'])
@crossdomain(origin='*')
def handle_coffe():
    weight = os.environ.get("WEIGHT", "0.0")
    weight = weight[2:-1]
    weight = float(weight)

    response = {
        "fill": weight
    }

    return jsonify(response), 200

@app.route("/weight", methods=['POST'])
def handle_weight():
    os.environ["WEIGHT"] = str(request.data)
    return "ok", 200

@app.route("/")
def half():
    return "Hello World!", 200

@app.errorhandler(404)
def not_found(error):
    return make_response(jsonify({"error": "Resource not found"}), 404)
