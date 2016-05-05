# -*- coding: utf-8 -*-
"""
Created on Fri May  6 00:04:35 2016

@author: Alaka

Script to parse the data produced by Arduino101curieSD.ino
See CURIE11.txt example file
"""
#%%
import datetime
import pandas as pd
import matplotlib.pyplot as plt


data =[]
with open("CURIE11.TXT","r") as f:
    lines = f.readlines()
    print(len(lines))

    for line in lines:
        # line can contain one of:
        # 66582,2597,153,-8491,16523,2056,3051 , # 39282 LED ON, # 39782 LED OFF
        # 
        if line.count("#") == 1:
            tsmilli = line.split(" ")[1]
            ts = datetime.datetime(2016,5,6+(int(tsmilli)//(24*60*60*1000)),
                                  (int(tsmilli) // (60*60*1000))%24,(int(tsmilli) // (60*1000))%60,
                                  (int(tsmilli) // (1000))%60, (int(tsmilli) % 1000)*1000)
            LED = line.strip().split(" ")[3]
            data.append({"ts":ts,"LED":LED})
           # print(ts)
        if line.count(",") == 6:
            tsmilli,ax,ay,az,gx,gy,gz = line.split(",")
            ts = datetime.datetime(2016,5,6+(int(tsmilli)//(24*60*60*1000)),
                                  (int(tsmilli) // (60*60*1000))%24,(int(tsmilli) // (60*1000))%60,
                                  (int(tsmilli) // (1000))%60, (int(tsmilli) % 1000)*1000)
            data.append({"ts":ts, "ax": int(ax), "ay":int(ay), "az":int(az), "gx":int(gx), "gy":int(gy), "gz":int(gz)   } ) # ,{}".format(ts,line.strip()))



print(len(data))

df = pd.DataFrame(data)

#print(df)
df.index = df['ts']
df=df.drop('ts',1)
#df = df.resample("s")
#df = df.interpolate()
df = df.ffill()
print(df)

df[['ax','ay','az']].plot()
#%%
df[['gx','gy','gz']].plot()