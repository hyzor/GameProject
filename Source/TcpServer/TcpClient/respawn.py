import time
import math
import random
import PyEngine

random.seed(time.time())

class SpawnPoint():
    
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

    def GetSelf(self):
        return float(self.x), float(self.y), float(self.z)

def GetSpawnTime():
    SpawnTime = 10
    return SpawnTime

def PlayerDied():
    PyEngine.NotifyWhen("spawn", GetSpawnTime, None)

spawnpoints = []

def RandomSpawn():
    global spawnpoints
    index = random.randint(0,len(spawnpoints)-1)
    return spawnpoints[index].GetSelf()

def getSpawnPos():
    PyEngine.NotifyWhen("spawn", RandomSpawn, None)

def CreateSpawnPos(x, y, z):
    global spawnpoints
    spawnpoints.append(SpawnPoint(x+1,y+150,z+1))
   
