from pythermalcomfort.models import adaptive_ashrae
from pythermalcomfort.utilities import running_mean_outdoor_temperature
from flask import Flask, request

app = Flask(__name__)
@app.route('/', methods=['POST', 'GET'])
def cal():
  if request.method == 'POST':
    if not request.json or not 'title' in request.json or not 'tdb' in request.json or not 'tr' in request.json or not 'tod_list' in request.json or not 'v' in request.json:
        abort(400)
    task = {
      'title': request.json['title'],
      'tdb': request.json['tdb'],
      'tr': request.json['tr'],
      'tod_list': request.json['tod_list'],
      'alpha': request.json['alpha'],
      'v': request.json['v']
    }
    rmt_value = running_mean_outdoor_temperature(task['tod_list'], alpha=task['alpha'])
    result = adaptive_ashrae(tdb=task['tdb'], tr=task['tr'], t_running_mean=rmt_value, v=task['v'])
    return {'task': task, 'result': result}, 201
  
  elif request.method == 'GET':
    message = 'Hey!, this is GET request. I want POST request'
    return  message, 200
