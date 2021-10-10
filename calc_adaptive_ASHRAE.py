from pythermalcomfort.models import adaptive_ashrae
from pythermalcomfort.utilities import running_mean_outdoor_temperature
#from pprint import pprint
from flask import Flask, request

app = Flask(__name__)
#books = []
@app.route('/', methods=['POST', 'GET'])
def cal():
  if request.method == 'POST':
    #body = request.get_json()
    #books.append(body)
    
    rmt_value = running_mean_outdoor_temperature([29, 28, 30, 29, 28, 30, 27], alpha=0.9)
    result = adaptive_ashrae(tdb=25, tr=25, t_running_mean=rmt_value, v=0.3)
    #pprint(result)
    
    return { "message": "Book already add to database", "body": result }, 201
  elif request.method == 'GET':
    rmt_value = running_mean_outdoor_temperature([29, 28, 30, 29, 28, 30, 27], alpha=0.9)
    result = adaptive_ashrae(tdb=25, tr=25, t_running_mean=rmt_value, v=0.3)
    return { "books": result }, 200
  
