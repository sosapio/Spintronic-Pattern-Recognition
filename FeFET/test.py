import pandas as pd
experimental_data = pd.read_csv('pzt_hyst.csv')
print(experimental_data)
#experimental_E = experimental_data[0].values
#experimental_P = experimental_data[1].values
print(experimental_data.values)