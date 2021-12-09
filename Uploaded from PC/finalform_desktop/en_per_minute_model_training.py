## Nice reference --> https://www.pluralsight.com/guides/regression-keras

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

dataset = pd.read_csv('collected_complete_peak_table.csv')
dataset.insert(len(dataset.columns),'en_per_minute', 0.0)
dataset['en_per_minute'] = dataset['energy_diff'] / dataset['minute_diff']

x_col = ['out_humid','out_temp','next_humid','next_temp']
X = dataset[x_col]
X = X.iloc[:,:].values

y_col = ['en_per_minute']
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

dump(sc_x, 'std_scaler_x.bin', compress=True)
dump(sc_y, 'std_scaler_y.bin', compress=True)

# Define model
model = Sequential()
model.add(Dense(40, input_dim=4, activation= "relu"))
#model.add(Dense(17, activation= "relu"))
model.add(Dense(1))
# model.add(Dense(32, input_dim=4, activation= "relu"))
# model.add(Dense(16, activation= "relu"))
# model.add(Dense(8, activation= "relu"))
# model.add(Dense(1))
model.summary()
tf.keras.utils.plot_model(model, "en_per_minute_layers.png", show_shapes=True)
######## Model can be improved by applying KerasTuner ########

model.compile(loss= "mean_squared_error" , optimizer="adam", metrics=[tf.keras.metrics.RootMeanSquaredError()])
# Train model
model.fit(X_train_std, y_train_std, batch_size=32, epochs=500)

# Evaluate model
pred_train_std = model.predict(X_train_std)
pred_train = sc_y.inverse_transform(pred_train_std)
print(np.sqrt(mean_squared_error(y_train,pred_train)))
diff_pred = ((pred_train - y_train)/y_train)*100

pred_test_std = model.predict(X_test_std)
pred_test = sc_y.inverse_transform(pred_test_std)
print(np.sqrt(mean_squared_error(y_test,pred_test))) 
diff_test = ((pred_test - y_test)/y_test)*100

# Save model
model.save('model/en_per_minute_model')