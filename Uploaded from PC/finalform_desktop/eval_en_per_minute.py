# -*- coding: utf-8 -*-
"""
Created on Wed Dec  1 23:08:46 2021

@author: SC
"""

import pandas as pd
import tensorflow as tf
from joblib import load
import numpy as np

dataset_col = pd.read_csv('collected_complete_peak_table.csv')
dataset_col.insert(len(dataset_col.columns),'predicted_en_per_minute', 0.0)

x_col = ['out_humid','out_temp','next_humid','next_temp']
X_dataset_col = dataset_col[x_col]
X_col = X_dataset_col.iloc[:,:].values

sc_x = load('std_scaler_x.bin')
sc_y = load('std_scaler_y.bin')

X_col_std = sc_x.transform(X_col)

model = tf.keras.models.load_model('model/en_per_minute_model')
predicted_en_col_std = model.predict(X_col_std)
predicted_en_col = sc_y.inverse_transform(predicted_en_col_std)
dataset_col['predicted_en_per_minute'] = predicted_en_col

minute_diff = np.array(dataset_col['minute_diff'].values)
minute_diff = np.reshape(minute_diff, (len(minute_diff),1))

dataset_col.to_csv('collected_predicted_en_per_minute.csv', index=False)