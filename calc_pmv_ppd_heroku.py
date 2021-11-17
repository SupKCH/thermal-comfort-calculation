from pythermalcomfort.models import pmv_ppd
from pythermalcomfort.utilities import v_relative, clo_dynamic
from pythermalcomfort.utilities import met_typical_tasks
from pythermalcomfort.utilities import clo_individual_garments
from flask import Flask, request
from datetime import datetime, timedelta


## MRT curve fitting
a = -0.0391888
b = 1.2675646
c = 16.9690554

## look-up table
lookup_table = {23: 26.04044586, 
                 24: 26.81464088, 
                 25: 27.43148148, 
                 26: 28.09216867, 
                 27: 29.19605263}

#tdb = 27
#tr = 26
v = 0.2
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
        t -= 0.1
    for key, value in lookup_table.items():
        if t >= value:
            selected_t = key
        else:
            break
    room_result = pmv_ppd(tdb=task['tdb'], tr=fit_MRT, vr=vr, rh=task['rh'], met=met, clo=clo, standard="ASHRAE")
    return {'tdb': task['tdb'], 'rh': task['rh'], 'suggested_tdb': t, 'room_result': room_result, 'iterated_result': result, 'fit_MRT': fit_MRT, 'selected_t': selected_t}, 201  ## remove mrt/suggested_tdb in real case or update JSON size
  
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
        t -= 0.1
    for key, value in lookup_table.items():
        if t >= value:
            selected_t = key
        else:
            break
    return  {'suggested_tdb': t, 'rh': 70, 'result': result, 'time': current_time, 'fit_MRT': fit_MRT, 'selected_t': selected_t}, 200
