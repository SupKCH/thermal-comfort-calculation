from pythermalcomfort.models import pmv_ppd
from pythermalcomfort.utilities import v_relative, clo_dynamic
from pythermalcomfort.utilities import met_typical_tasks
from pythermalcomfort.utilities import clo_individual_garments
from flask import Flask, request
from datetime import datetime, timedelta


## MRT curve fitting
a = -0.07961
b = 2.20962
c = 11.25989

## look-up table
setting_table = {23: 26.0, 
                 24: 26.5, 
                 25: 27.0, 
                 26: 28.0, 
                 27: 29.0}

#tdb = 27
tr = 26
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
    now = datetime.now() + timedelta(hours=7)
    hours = now.hour
    minutes = now.minute
    seconds = now.second
    deci_hours = hours + minutes/60 + seconds/3600
    fit_MRT = a*deci_hours**2 + b*deci_hours + c
    while True:
        result = pmv_ppd(tdb=t, tr=fit_MRT, vr=vr, rh=task['rh'], met=met, clo=clo, standard="ASHRAE")
        if result['pmv'] <= 0.5:
            break
        t -= 0.5
    room_result = pmv_ppd(tdb=task['tdb'], tr=fit_MRT, vr=vr, rh=task['rh'], met=met, clo=clo, standard="ASHRAE")
    return {'tdb': task['tdb'], 'rh': task['rh'], 't': t, 'room_result': room_result, 'result': result, 'fit_MRT': fit_MRT}, 201  ## remove mrt in real case
  
  elif request.method == 'GET':
    #message = 'Hey!, this is GET request. I want POST request'
    now = datetime.now() + timedelta(hours=7)
    current_time = now.strftime("%H:%M:%S")
    hours = now.hour
    minutes = now.minute
    seconds = now.second
    deci_hours = hours + minutes/60 + seconds/3600
    fit_MRT = a*deci_hours**2 + b*deci_hours + c
    t = 35
    while True:
        result = pmv_ppd(tdb=t, tr=fit_MRT, vr=vr, rh=70, met=met, clo=clo, standard="ASHRAE")
        if result['pmv'] <= 0.5:
            break
        t -= 0.5
    return  {'tdb': t, 'rh': 70, 'result': result, 'time': current_time, 'fit_MRT': fit_MRT}, 200
