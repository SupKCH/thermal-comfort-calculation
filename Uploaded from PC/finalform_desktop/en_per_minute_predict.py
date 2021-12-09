# -*- coding: utf-8 -*-
"""
Created on Thu Nov 25 15:46:44 2021

@author: SC
"""

import pandas as pd
import tensorflow as tf
from joblib import load
import numpy as np

dataset_exp = pd.read_csv('exp_complete_peak_table.csv')
dataset_exp.insert(len(dataset_exp.columns),'predicted_en_per_minute', 0.0)
dataset_exp.insert(len(dataset_exp.columns),'cal_energy_diff', 0.0)

x_col = ['out_humid','out_temp','next_humid','next_temp']
X_dataset_exp = dataset_exp[x_col]
X_exp = X_dataset_exp.iloc[:,:].values

sc_x = load('std_scaler_x.bin')
sc_y = load('std_scaler_y.bin')

X_exp_std = sc_x.transform(X_exp)

model = tf.keras.models.load_model('model/en_per_minute_model')
predicted_en_exp_std = model.predict(X_exp_std)
predicted_en_exp = sc_y.inverse_transform(predicted_en_exp_std)
dataset_exp['predicted_en_per_minute'] = predicted_en_exp

minute_diff = np.array(dataset_exp['minute_diff'].values)
minute_diff = np.reshape(minute_diff, (len(minute_diff),1))
dataset_exp['cal_energy_diff'] = predicted_en_exp * minute_diff

dataset_exp.to_csv('exp_predicted_en_per_minute.csv', index=True)