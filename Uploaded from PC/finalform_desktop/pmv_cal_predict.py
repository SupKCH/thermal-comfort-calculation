from pythermalcomfort.models import pmv_ppd
from pythermalcomfort.utilities import v_relative, clo_dynamic
from pythermalcomfort.utilities import met_typical_tasks
from pythermalcomfort.utilities import clo_individual_garments
import pandas as pd
import tensorflow as tf
from joblib import load

## MRT curve fitting
a = -0.0391888
b = 1.2675646
c = 16.9690554

dataset_exp = pd.read_csv('exp_summary_air_data.csv')
dataset_exp.insert(len(dataset_exp.columns),'PMV', 0.0)
dataset_exp.insert(len(dataset_exp.columns),'PPD', 0.0)
dataset_exp.insert(len(dataset_exp.columns),'predicted_PMV', 0.0)

deci_hours = dataset_exp['deci_hours'].values
tdb = dataset_exp['main_temp'].values
rh = dataset_exp['main_humid'].values
v = 0.2
activity = "Typing"
garments = ["Sweatpants", "T-shirt", "Shoes or sandals"]
met = met_typical_tasks[activity]
icl = sum(
    [clo_individual_garments[item] for item in garments]
)
vr = v_relative(v=v, met=met)
clo = clo_dynamic(clo=icl, met=met)

# calculate PMV in accordance with the ASHRAE 55 2020
for i in range(len(dataset_exp)):
    each_result = pmv_ppd(tdb=tdb[i], 
                          tr=a*deci_hours[i]**2 + b*deci_hours[i] + c, 
                          vr=vr, 
                          rh=rh[i],
                          met=met,
                          clo=clo,
                          standard="ASHRAE")
    dataset_exp.at[i, "PMV"] = each_result['pmv']
    dataset_exp.at[i, "PPD"] = each_result['ppd']

#dataset_exp.to_csv('collected_pmv.csv', index=True)

x_col = ['out_humid','out_temp','next_humid','next_temp','deci_hours']
X_dataset_exp = dataset_exp[x_col]
X_exp = X_dataset_exp.iloc[:,:].values

sc_x = load('std_scaler_x_pmv.bin')
sc_y = load('std_scaler_y_pmv.bin')

X_exp_std = sc_x.transform(X_exp)

model = tf.keras.models.load_model('model/pmv_model')
predicted_pmv_std = model.predict(X_exp_std)
predicted_pmv = sc_y.inverse_transform(predicted_pmv_std)
dataset_exp['predicted_PMV'] = predicted_pmv

dataset_exp.to_csv('exp_real_and_predicted_pmv.csv', index=True)