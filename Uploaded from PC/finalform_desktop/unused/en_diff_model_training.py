## Nice reference --> https://www.pluralsight.com/guides/regression-keras

### Import required libraries
import pandas as pd
import numpy as np 
from numpy import savetxt
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

dataset = pd.read_csv('collected_predicted_time_diff.csv')
x_col = ['out_humid','out_temp','next_humid','next_temp','minute_diff']
X = dataset[x_col]
X = X.iloc[:,:].values

y_col = ['energy_diff']
y = dataset[y_col]
y = y.iloc[:,:].values

# dataset_clean = dataset[dataset['max_max_power']>2000].drop(columns='out_light')

# X = dataset_clean.iloc[:, 5:-5].values
# y = dataset_clean.iloc[:, -1].values

X_train, X_test, y_train, y_test = train_test_split(X, y, test_size = 0.1, random_state = 1)

# Define model
model = Sequential()
model.add(Dense(28, input_dim=5, activation= "relu"))
#model.add(Dense(14, activation= "relu"))
model.add(Dense(1))
model.summary()
tf.keras.utils.plot_model(model, "en_diff_layers.png", show_shapes=True)

model.compile(loss= "mean_squared_error" , optimizer="adam", metrics=[tf.keras.metrics.RootMeanSquaredError()])
model.fit(X_train, y_train, batch_size=32, epochs=500)

pred_train= model.predict(X_train)
print(np.sqrt(mean_squared_error(y_train,pred_train)))
diff_pred = pred_train - y_train

pred_test= model.predict(X_test)
print(np.sqrt(mean_squared_error(y_test,pred_test))) 
diff_test = pred_test - y_test

# Save model
model.save('model/energy_model')

# buff = [np.zeros(10)]
# buff.append(np.zeros(10))
# buff.append(np.zeros(10))
# dummmy_temp = list(range(26,35))
# for hum in range(60,90):
#     pred = model.predict([[hum, 30, 70, 27]])
#     print(pred)

# plt.scatter(X['next_temp'], y, color="black")
# plt.scatter(X['next_temp'], y_pred, color="blue")

# plt.xticks(())
# plt.yticks(())

# plt.show()

savetxt('prediction/en_X_train.csv', X_train, delimiter=';')
savetxt('prediction/en_y_train.csv', y_train, delimiter=';')
savetxt('prediction/en_pred_train.csv', pred_train, delimiter=';')

savetxt('prediction/en_X_test.csv', X_test, delimiter=';')
savetxt('prediction/en_y_test.csv', y_test, delimiter=';')
savetxt('prediction/en_pred_test.csv', pred_test, delimiter=';')