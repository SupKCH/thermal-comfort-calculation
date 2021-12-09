# -*- coding: utf-8 -*-
"""
Created on Thu Nov 25 12:28:58 2021

@author: SC
"""

import pandas as pd
import tensorflow as tf

dataset_exp = pd.read_csv('exp_predicted_time_diff.csv')
dataset_exp.insert(len(dataset_exp.columns),'predicted_en_diff', 0.0)

x_col = ['out_humid','out_temp','next_humid','next_temp','minute_diff']
X_dataset_exp = dataset_exp[x_col]
X_exp = X_dataset_exp.iloc[:,:].values

# y_col = ['en_diff']
# y = dataset[y_col]
# y = y.iloc[:,:].values

## StandartScaler is needed!

model = tf.keras.models.load_model('model/energy_model')
predicted_en_exp = model.predict(X_exp)
dataset_exp['predicted_en_diff'] = predicted_en_exp
dataset_exp_filtered = dataset_exp[dataset_exp['out_humid']>51.1]

#savetxt('exp_predicted_time_diff.csv', predicted_time_exp, delimiter=';')
dataset_exp.to_csv('exp_predicted_en_diff.csv', index=True)