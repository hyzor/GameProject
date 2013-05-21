import time
import math
import random

random.seed(time.time())

class SpawnPoint():
    
    def __init__(self, x, y, z, rotationIndex):
        self.x = x
        self.y = y
        self.z = z
        self.rotation = rotationIndex

    def GetSelf(self):
        return self.x, self.y, self.z, self.rotation

spawnpoints = []
spawnpoints.append(SpawnPoint(1,206,50,1))
TimeOfDeath = 0

def PlayerDied():
    global TimeOfDeath
    TimeOfDeath = 5

def TimeToSpawn():
    return int(10 - (time.time()-TimeOfDeath))

def getSpawnPos():
    index = random.randint(0,len(spawnpoints)-1)
    return spawnpoints[index].GetSelf()
