## Nice reference --> https://www.pluralsight.com/guides/regression-keras

### Import required libraries
import pandas as pd
import numpy as np 
# from numpy import savetxt
# import matplotlib.pyplot as plt

### Import necessary modules
from sklearn.model_selection import train_test_split
from sklearn.metrics import mean_squared_error
#from sklearn.preprocessing import StandardScaler
#from math import sqrt

### Keras specific
import tensorflow as tf
#import keras
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import Dense

dataset = pd.read_csv('collected_complete_peak_table.csv')

x_col = ['out_humid','out_temp','next_humid','next_temp']
X = dataset[x_col]
X = X.iloc[:,:].values

y_col = ['minute_diff']
y = dataset[y_col]
y = y.iloc[:,:].values

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size = 0.2, random_state = 1)

# Define model
model = Sequential()
model.add(Dense(34, input_dim=4, activation= "relu"))
#model.add(Dense(17, activation= "relu"))
model.add(Dense(1))
# model.add(Dense(32, input_dim=4, activation= "relu"))
# model.add(Dense(16, activation= "relu"))
# model.add(Dense(8, activation= "relu"))
# model.add(Dense(1))
model.summary()
tf.keras.utils.plot_model(model, "en_diff_layers.png", show_shapes=True)

model.compile(loss= "mean_squared_error" , optimizer="adam", metrics=[tf.keras.metrics.RootMeanSquaredError()])
# Train model
model.fit(X_train, y_train, batch_size=32, epochs=500)

# Evaluate model
pred_train= model.predict(X_train)
print(np.sqrt(mean_squared_error(y_train,pred_train)))
diff_pred = pred_train - y_train

pred_test= model.predict(X_test)
print(np.sqrt(mean_squared_error(y_test,pred_test))) 
diff_test = pred_test - y_test

# Save model
model.save('model/time_model')