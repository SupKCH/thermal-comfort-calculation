from pythermalcomfort.models import pmv_ppd
from pythermalcomfort.utilities import v_relative, clo_dynamic
from pythermalcomfort.utilities import met_typical_tasks
from pythermalcomfort.utilities import clo_individual_garments
#from datetime import date
#from numpy import savetxt

### Import required libraries
import pandas as pd
import numpy as np 
# from numpy import savetxt
# import matplotlib.pyplot as plt

### Import necessary modules
from sklearn.model_selection import train_test_split
from sklearn.metrics import mean_squared_error
from sklearn.preprocessing import StandardScaler
#from math import sqrt
from joblib import dump

### Keras specific
import tensorflow as tf
#import keras
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense

## MRT curve fitting
a = -0.0391888
b = 1.2675646
c = 16.9690554

dataset = pd.read_csv('collected_summary_air_data.csv')
dataset.insert(len(dataset.columns),'PMV', 0.0)
dataset.insert(len(dataset.columns),'PPD', 0.0)

deci_hours = dataset['deci_hours'].values
tdb = dataset['main_temp'].values
rh = dataset['main_humid'].values

#tdb = 27  # dry bulb air temperature, [$^{\circ}$C]
#tr = 27.9  # mean radiant temperature, [$^{\circ}$C]
v = 0.2
#rh = 80
activity = "Typing"
garments = ["Sweatpants", "T-shirt", "Shoes or sandals"]
met = met_typical_tasks[activity]
icl = sum(
    [clo_individual_garments[item] for item in garments]
)
vr = v_relative(v=v, met=met)
clo = clo_dynamic(clo=icl, met=met)

# calculate PMV in accordance with the ASHRAE 55 2020
for i in range(len(dataset)):
    each_result = pmv_ppd(tdb=tdb[i], 
                          tr=a*deci_hours[i]**2 + b*deci_hours[i] + c, 
                          vr=vr, 
                          rh=rh[i],
                          met=met,
                          clo=clo,
                          standard="ASHRAE")
    dataset.at[i, "PMV"] = each_result['pmv']
    dataset.at[i, "PPD"] = each_result['ppd']

dataset.to_csv('collected_pmv.csv', index=True)

x_col = ['out_humid','out_temp','next_humid','next_temp','deci_hours']
X = dataset[x_col]
X = X.iloc[:,:].values

y_col = ['PMV']
y = dataset[y_col]
y = y.iloc[:,:].values

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size = 0.2, random_state = 1)

# Standardization
sc_x = StandardScaler()
X_train_std = sc_x.fit_transform(X_train)
X_test_std = sc_x.transform(X_test)

sc_y = StandardScaler()
y_train_std = sc_y.fit_transform(y_train)
y_test_std = sc_y.transform(y_test)

dump(sc_x, 'std_scaler_x_pmv.bin', compress=True)
dump(sc_y, 'std_scaler_y_pmv.bin', compress=True)

# Define model
model = Sequential()
model.add(Dense(48, input_dim=5, activation= "relu"))
#model.add(Dense(17, activation= "relu"))
model.add(Dense(1))
model.summary()
tf.keras.utils.plot_model(model, "pmv_layers.png", show_shapes=True)
######## Model can be improved by applying KerasTuner ########

model.compile(loss= "mean_squared_error" , optimizer="adam", metrics=[tf.keras.metrics.RootMeanSquaredError()])
# Train model
model.fit(X_train_std, y_train_std, batch_size=32, epochs=500)

# Evaluate model
pred_train_std = model.predict(X_train_std)
pred_train = sc_y.inverse_transform(pred_train_std)
print(np.sqrt(mean_squared_error(y_train,pred_train)))
diff_pred = pred_train - y_train
#diff_pred = ((pred_train - y_train)/y_train)*100

pred_test_std = model.predict(X_test_std)
pred_test = sc_y.inverse_transform(pred_test_std)
print(np.sqrt(mean_squared_error(y_test,pred_test))) 
diff_test = pred_test - y_test
#diff_test = ((pred_test - y_test)/y_test)*100

# Save model
model.save('model/pmv_model')