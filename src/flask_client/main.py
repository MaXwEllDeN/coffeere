import logging
import json, os

from flask import request, Response, render_template, jsonify, Flask
from pywebpush import webpush, WebPushException

app = Flask(__name__)
#app.config['SECRET_KEY'] = '9OLWxND4o83j4K4iuopO'

DER_BASE64_ENCODED_PRIVATE_KEY_FILE_PATH = os.path.join(os.getcwd(), "private_key.txt")
DER_BASE64_ENCODED_PUBLIC_KEY_FILE_PATH = os.path.join(os.getcwd(), "public_key.txt")

VAPID_PRIVATE_KEY = open(DER_BASE64_ENCODED_PRIVATE_KEY_FILE_PATH, "r+").readline().strip("\n")
VAPID_PUBLIC_KEY = open(DER_BASE64_ENCODED_PUBLIC_KEY_FILE_PATH, "r+").read().strip("\n")

subscriptions = []

VAPID_CLAIMS = {
    "sub": "mailto:maxwell@lsd.ufcg.edu.br"
}

def send_web_push(subscription_information, message_body):
    return webpush(
        subscription_info=subscription_information,
        data=message_body,
        vapid_private_key=VAPID_PRIVATE_KEY,
        vapid_claims=VAPID_CLAIMS
    )

@app.route('/')
def index():
    return render_template('index.html')

@app.route("/subscription/", methods=["GET", "POST"])
def subscription():
    """
        POST creates a subscription
        GET returns vapid public key which clients uses to send around push notification
    """

    if request.method == "GET":
        return Response(response=json.dumps({"public_key": VAPID_PUBLIC_KEY}),
            headers={"Access-Control-Allow-Origin": "*"}, content_type="application/json")

    token = request.json.get('sub_token')

    try:
        token = json.loads(token)
    except Exception as e:
        print("error",e)
        return jsonify({'failed':str(e)})

    subscriptions.append(token)
    return jsonify({'success':1})

@app.route("/push_v1/", methods=['GET'])
def push_v1():
    message = "Cafezin cafezin"

    for token in subscriptions:
        try:
            send_web_push(token, message)
        except WebPushException as e:
            print(type(e))
            # if expiredSession: remove from database.
        except Exception as e:
            return jsonify({'failed':str(e)})

    return jsonify({'success':1})

if __name__ == "__main__":
    app.run(host="0.0.0.0",port=8080)
