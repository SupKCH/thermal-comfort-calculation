from pythermalcomfort.models import adaptive_ashrae
from pythermalcomfort.utilities import running_mean_outdoor_temperature
#from pprint import pprint
from flask import Flask, request

app = Flask(__name__)
@app.route('/', methods=['POST', 'GET'])
def cal():
  if request.method == 'POST':
    
    #rmt_value = running_mean_outdoor_temperature([29, 28, 30, 29, 28, 30, 27], alpha=0.9)
    #result = adaptive_ashrae(tdb=25, tr=25, t_running_mean=rmt_value, v=0.3)
    #pprint(result)
    if not request.json or not 'title' in request.json:
        abort(400)
    task = {
        'title': request.json['title'],
        'description': request.json.get('description', ""),
        'done': False
    }
    
    #if not request.json or not 'title' in request.json or not 'tdb' in request.json or not 'tr' in request.json:
    #    abort(400)
    #task = {
    #    'title': request.json['title'],
    #    'tdb': request.json['tdb'],
    #    'tr': request.json['tr']
    #}
    
    return jsonify({'task': task}), 201
  elif request.method == 'GET':
    rmt_value = running_mean_outdoor_temperature([29, 28, 30, 29, 28, 30, 27], alpha=0.9)
    result = adaptive_ashrae(tdb=25, tr=25, t_running_mean=rmt_value, v=0.3)
    return  result, 200
