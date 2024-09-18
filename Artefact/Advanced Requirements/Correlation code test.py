import pandas as pd

df = pd.read_csv('applesdataset.csv')

correlation_matrix = df[['arousalindexcalc', 'rdinrempsg']].corr()
correlation_coefficient = correlation_matrix.iloc[0, 1]
print(correlation_coefficient)