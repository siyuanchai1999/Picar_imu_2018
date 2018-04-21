import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
plt.ion()

pointsNum = 1000
radius = 2
theta = np.linspace(0,2*np.pi,pointsNum)

x = radius*np.sin(theta)  #circle
y = radius*np.cos(theta)

dfX = pd.DataFrame(x)
dfY = pd.DataFrame(y)
dfOutput = pd.concat([dfX,dfY],axis = 1)
dfOutput.to_csv("circle.csv")

xHeart = 16*(np.power(np.sin(theta),3))
yHeart = 13*np.cos(theta) - 5*np.cos(2*theta) - 2*np.cos(3*theta) - np.cos(4*theta)

dfXHeart = pd.DataFrame(xHeart)
dfYHeart = pd.DataFrame(yHeart)
dfOutputHeart = pd.concat([dfXHeart,dfYHeart], axis=1)
dfOutputHeart.to_csv("heart.csv")
plt.plot(xHeart,yHeart)

print("done")
