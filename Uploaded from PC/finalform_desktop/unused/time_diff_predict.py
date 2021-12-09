### Import required libraries
import pandas as pd
# import numpy as np 
# from numpy import savetxt
# import matplotlib.pyplot as plt

### Import necessary modules
# from sklearn.model_selection import train_test_split
# from sklearn.metrics import mean_squared_error

### Keras specific
# from tensorflow import keras
# from keras.models import Sequential
# from keras.layers import Dense

import tensorflow as tf

dataset_exp = pd.read_csv('exp_complete_peak_table.csv')
dataset_exp.insert(len(dataset_exp.columns),'predicted_time_diff', 0.0)

x_col = ['out_humid','out_temp','next_humid','next_temp']
X_dataset_exp = dataset_exp[x_col]
X_exp = X_dataset_exp.iloc[:,:].values

# y_col = ['minute_diff']
# y = dataset[y_col]
# y = y.iloc[:,:].values

model = tf.keras.models.load_model('model/time_model')
predicted_time_exp = model.predict(X_exp)
dataset_exp['predicted_time_diff'] = predicted_time_exp

#savetxt('exp_predicted_time_diff.csv', predicted_time_exp, delimiter=';')
dataset_exp.to_csv('exp_predicted_time_diff.csv', index=True)

###############################
dataset_collected = pd.read_csv('collected_complete_peak_table.csv')
dataset_collected.insert(len(dataset_collected.columns),'predicted_time_diff', 0.0)

X_dataset_collected = dataset_collected[x_col]
X_collected = X_dataset_collected.iloc[:,:].values

predicted_time_collected = model.predict(X_collected)
dataset_collected['predicted_time_diff'] = predicted_time_collected

#savetxt('collected_predicted_time_diff.csv', predicted_time_collected, delimiter=';')
dataset_collected.to_csv('collected_predicted_time_diff.csv', index=True)

##############################
## We don't need this since prediction from EXP data, trained with collected data, will never match real time_diff of EXP data.
## Also, RMSE of predicted COLLECTED data have been evaluated right after training model.
# import math
# y_actual = [1,2,3,4,5]
# y_predicted = [1.6,2.5,2.9,3,4.1]
 
# MSE = np.square(np.subtract(y_actual,y_predicted)).mean() 
 
# RMSE = math.sqrt(MSE)
# print("Root Mean Square Error:\n")
# print(RMSE)