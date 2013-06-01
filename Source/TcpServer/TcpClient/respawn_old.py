import time
import math
import random
import PyEngine

random.seed(time.time())

currentID = 0

class SpawnPoint():
    
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

    def GetSelf(self):
        return float(self.x), float(self.y), float(self.z)

class Player():
    timeOfDeath = 0
    id = 0
    
    def __init__(self, id):
        self.id = id
        
players = []

def PlayerDied(playerID):
    global players
    TimeOfDeath = time.time()
    found = False

    i = 0
    while i < len(players):
        if players[i].id == playerID:
            players[i].timeOfDeath = TimeOfDeath
            found = True
            print("wtf?")
        i = i+1

    if found is False:
        players.append(Player(playerID))
        players[len(players)-1].timeOfDeath = TimeOfDeath
    
def TimeToSpawn():
    global players
    timeToSpawn = -1
    
    i = 0
    while i < len(players):
        if players[i].id == currentID:
            timeDead = time.time()-players[i].timeOfDeath
            timeToSpawn = int(10 - timeDead)
        i = i+1    
    return timeToSpawn

def CheckSpawnTimer():
    PyEngine.NotifyWhen("derp", TimeToSpawn, None)

def RandomSpawn():
    index = random.randint(0,len(spawnpoints)-1)
    return spawnpoints[index].GetSelf()

def getSpawnPos():
    print("this is a test")
    PyEngine.NotifyWhen("Timer", RandomSpawn, None)

def setID(id):
    global currentID
    currentID = id

players = []
spawnpoints = []
spawnpoints.append(SpawnPoint(200,200,200))
   
