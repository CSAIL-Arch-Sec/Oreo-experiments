import pandas as pd
import matplotlib.pyplot as plt

filename = "../disk-image/experiments/LEBench-Sim/lebench_stats.csv"

plt.rcParams["figure.figsize"] = [7.50, 3.50]
plt.rcParams["figure.autolayout"] = True

df_base = pd.read_csv(filename).set_index('name')
df_eval = pd.read_csv(filename).set_index('name') * 2
df_rel = df_eval / df_base
df_one = df_base / df_base

df_merged = pd.merge(
    df_rel[['mean (ns)']], 
    df_one[['mean (ns)']], 
    on='name', 
    suffixes = (' eval', ' base'))
# print(df_merged)

df_merged.plot.bar()

plt.savefig('foo.png', bbox_inches='tight')