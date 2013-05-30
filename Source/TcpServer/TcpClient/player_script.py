#-*- coding: utf-8 -*-

import random
import time
import math
import PyEngine
import platform_script

random.seed(time.time())

class Position(object):
        def __init__(self, x, y, z):
                self.x = x
                self.y = y
                self.z = z

        def GetSelf(self):
                return float(self.x), float(self.y), float(self.z)

spawnpoints = []

def CreateSpawnPoint(x, y, z):
        global spawnpoints
        spawnpoints.append(Position(x+1, y+150, z+1))

def RandomizeSpawn():
        global spawnpoints
        index = random.randint(0, len(spawnpoints)-1)
        x = spawnpoints[index].x
        y = spawnpoints[index].y
        z = spawnpoints[index].z
        return x, y, z

def Solution():
        PyEngine.NotifyWhen("ToTheProblem", RandomizeSpawn, None)


def RandomSpawn():
        platforms = platform_script.platforms
        index = random.randint(0, len(platforms)-1)
        x = platforms[index].pos.x
        y = platforms[index].pos.y
        z = platforms[index].pos.z
        y += 100
        if(x == 0):
            x+=1
        if(z == 0):
            z+=1
        #if(platforms[index].type == 2):
            #z+=100
        elif(platforms[index].type == 3):
            z+=50

        print x,y,z, len(platforms), index
        return float(x), float(y), float(z)



def GetRandomSpawn():
        PyEngine.NotifyWhen("Spawnpoint", RandomSpawn, None)


random.seed(None)
