# -*- coding: utf-8 -*-
"""
Spyder Editor

This is a temporary script file.
"""

import datetime
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt



data =[]
timeavail = False
with open("D:\MPULOG16.TXT","r") as f:
    lines = f.readlines()
    print(len(lines))

    for line in lines:
        # line can contain one of:
        #  MPU9250 Log , FIFO overflow! , 9:2:14.914 , 2597,153,-8491

        if line.count(":") == 2:
            timeavail = True
            ts = datetime.datetime.strptime("29/04/2016 "+line.strip(),"%d/%m/%Y %H:%M:%S.%f")
            #print(ts)
        if line.count(",") == 2:
            x,y,z = line.split(",")
            if timeavail:
                data.append({"ts":ts, "x": int(x), "y":int(y), "z":int(z)   } ) # ,{}".format(ts,line.strip()))
                timeavail = False
            else:
                data.append({ "x": int(x), "y":int(y), "z":int(z)   } ) # ,{}".format(ts,line.strip()))

print(len(data))

df = pd.DataFrame(data)

#print(df)
df.index = df['ts']
df=df.drop('ts',1)
df = df.resample("10L").mean()
df = df.interpolate()
print(len(df))
print(df)

df.plot()

#%%
quietmean = df['2016-04-29 09:07:50':'2016-04-29 09:09:20'].mean()
df = df-quietmean
#%%
df['m'] = np.sqrt(pow(df.x,2)+(df.y*df.y)+(df.z*df.z))
df.m.plot()
#%%
dflargewaves = df['2016-04-29 09:09:20':'2016-04-29 9:10:55'].copy()
dflargewaves.plot()
#%%
dfbroomhits = df['2016-04-29 09:10:55':'2016-04-29 09:16:00'].copy()
dfbroomhits.plot()
#%%
dfbroomhits['m'].plot()
#Stort Slag ved 9:12:20ish. Korresponderer med 04:24 på video p4290007.MOV
dfbroomhits['m'].argmax()
#Det er det største slaget i serien, og skjer altså ved 2016-04-29 09:12:24.110000
#To siste slag skjer ved 7:38 og 7:41 i videoen. Ser svakere ut på data enn ventet.
#De to slagene skjer framme på båten, og båten roterer, med lite bevegelse bak, der sensoren er montert.
#%%
dfbroomhits['x'].plot()
#Positiv x-retning i tyngdekraftretning. Positiv x korresponderer til kraft nedover
#%%
dfbroomhits['y'].plot()
#y-retning er styrbord-babord. Usikker hva som er positivt
#%%
dfbroomhits['z'].plot()
#z-retning er langs båten. Usikker hva som er positivt
#%%
from dateutil.parser import parse
parse('07:41')
delta = parse('00:07:41')-parse('00:04:24')
dfbroomhits['m'].argmax()+delta
#%%
dflasthits = df['2016-04-29 09:15:35':'2016-04-29 09:15:45'].copy()
dflasthits['y'].plot()
#Skjer faktisk 3 slag. Siste slaget skjer mot kamera og er litt vanskeligere å se, 
#men gir kraftigere utslag enn ventet, da det er nærmere midten av båten. Båten flytter seg i stedet for å rotere
len(dflasthits.y)
#%%
dflasthits.x.plot()
#%%  Hvorfor gir de store bølgene mindre utslag enn de små?
dflargewaves.x.plot()
dflargewaves.x.max()
#%%
#Utslaget fra de store bølgene er rundt G/10
#Bølgene er gitt ved funksjonen x=A*sin(wt+kz)
#Her er x høyderetning og z fartsretning til bølgen.
#For henholdsvis de store og små bølgene har vi
A = np.array([0.1, 0.02])
w = np.array([0.5,1.3])
w = 2*3.14*w
#k, som angir bølgelengden, er ukjent (og uinteresant)
#Vi måler akselerasjon, ikke forflyttelse, og kan regne ut akselerasjon ved å ta den tidsderiverte to ganger.
#Dette betyr å gange med -w^2 på utsiden
A*w*w
#Dette er utslaget (målt i m/s^2) vi forventer å se på sensoren fra de to bølgene.
#Vi ser at utslaget fra de små, hurtigere bølgene er større enn de store, og at 
#utslaget fra de store er, som ventet, 1/10 av 1g (1g er ~9000LSB)
