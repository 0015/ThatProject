# /////////////////////////////////////////////////////////////////
# /*
# OTA Solution - Build your own OTA platform (1/2, Preparing Python Flask API Server)
# Video Tutorial: https://youtu.be/_uTZad8RMZk
# Created by Eric N. (ThatProject)
# */
# /////////////////////////////////////////////////////////////////

import time
import hashlib
import os
from flask import Flask, send_file, json, request

app = Flask(__name__)

# ================================================
target_file_path = './firmware/test_firmware.bin'
company_name = 'ThatProject'
build_num = 2
build_date = '07-22-2021'
API_KEY = 'THIS_IS_MY_OWN_API_KEY'
# ================================================


@app.route('/api/post/update', methods=['POST'])
def api_update():
    if request.method == 'POST':
        key = request.form['api_key']
        target_path = request.form['target_path']
        if API_KEY == key and len(target_path) > 1:
            try:
                return send_file(target_path)
            except Exception as e:
                return str(e)


def version():
    size = os.path.getsize(target_file_path)
    print('Size of file is', size, 'bytes')

    with open(target_file_path, "rb") as file_to_check:
        data = file_to_check.read()
        md5_returned = hashlib.md5(data).hexdigest()
        print('md5_checksum is', md5_returned)

    value = {
        "companyName": company_name,
        "buildNum": build_num,
        "buildDate": build_date,
        "serverFilePath": target_file_path,
        "fileSize": size,
        "md5Checksum": md5_returned
    }
    return json.dumps(value)


@app.route('/api/get/version')
def api_version():
    return version()


@app.route('/api/get/time')
def api_localtime():
    currnet_local = time.localtime()
    gmt_offset = currnet_local.tm_gmtoff
    local_timestamp = time.mktime(currnet_local) + gmt_offset
    return json.dumps({"timestamp": "{}".format(local_timestamp)})


@app.route('/')
def hello_word():
    return "<p>Hello World!</p>"


@app.route('/favicon.ico')
def favicon():
    return send_file('./static/favicon.ico', mimetype='image/vnd.microsoft.icon')


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=9001, debug=True)
