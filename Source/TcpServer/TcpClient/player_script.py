#-*- coding: utf-8 -*-

import random
import time
import math
import PyEngine

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
