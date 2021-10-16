from pythermalcomfort.models import pmv_ppd
from pythermalcomfort.utilities import v_relative, clo_dynamic
from pythermalcomfort.utilities import met_typical_tasks
from pythermalcomfort.utilities import clo_individual_garments
from flask import Flask, request

#tdb = 27
tr = 25
v = 0.1
#rh = 50
activity = "Typing"
garments = ["Sweatpants", "T-shirt", "Shoes or sandals"]
met = met_typical_tasks[activity]
icl = sum(
    [clo_individual_garments[item] for item in garments]
)
vr = v_relative(v=v, met=met)
clo = clo_dynamic(clo=icl, met=met)

app = Flask(__name__)
@app.route('/', methods=['POST', 'GET'])
def cal():
  if request.method == 'POST':
    if not request.json or not 'tdb' in request.json or not 'rh' in request.json:
        abort(400)
    task = {
      'tdb': request.json['tdb'],
      'rh': request.json['rh'],
    }
    t = 35
    while True:
        result = pmv_ppd(tdb=t, tr=tr, vr=vr, rh=task['rh'], met=met, clo=clo, standard="ASHRAE")
        if result['pmv'] <= 0.5:
            break
        t -= 0.5
    room_result = pmv_ppd(tdb=task['tdb'], tr=tr, vr=vr, rh=task['rh'], met=met, clo=clo, standard="ASHRAE")
    return {'tdb': task['tdb'], 'rh': task['rh'], 'room_result': room_result, 'result': result}, 201
  
  elif request.method == 'GET':
    #message = 'Hey!, this is GET request. I want POST request'
    t = 35
    while True:
        result = pmv_ppd(tdb=t, tr=tr, vr=vr, rh=70, met=met, clo=clo, standard="ASHRAE")
        if result['pmv'] <= 0.5:
            break
        t -= 0.5
    return  {'tdb': t, 'rh': 70, 'result': result}, 200
