import random
import time
import PyEngine

random.seed(time.time())

class Position(object):
    def __init__(self, x, y, z):
        self.x = x
        self.y = y
        self.z = z

class Pickup(object):
    def __init__(self, pType, pos, active):
        self.pickupType = pType
        self.pos = pos
        self.timeRemaining = 0.0
        self.active = active
        self.timeOfStart = -1.0
    
    def isActive(self):
        return self.active
    
    def getPos():
        return self.pos.x, self.pos.y, self.pos.z
        
        
pickups = []
pickupSpawnPoints = []

#make sure to call this from c++ with a position of a platform, with offset if needed
def createSpawnPoint(x, y, z):
    global pickupSpawnPoints
    pickupSpawnPoints.append(Position(x, y, z))
    
def createPickup():
    global pickupSpawnPoints
    global pickups
    i = random.randint(0, len(pickupSpawnPoints) - 1)
    
    pos = Position(pickupSpawnPoints[i].x, pickupSpawnPoints[i].y, pickupSpawnPoints[i].z) 
    pType = 1
    active = False
    
    pickups.append(Pickup(pType, pos, active))

#call when a player consumes a pickup
def consumePickup(pIndex):
    global pickups
    if ((pIndex >= 0) and (pIndex < len(pickups))):
        if (pickups[pIndex].isActive()):
            pickups[pIndex].active = False
            pickups[pIndex].timeOfStart = time.time()

#call to activate a certain pickup, activating a flag that indicates that it should be rendered and can be picked up by players
def activatePickup(pIndex): 
    global pickups
    if ((pIndex >= 0) and (pIndex < len(pickups))):
        if not(pickups[pIndex].isActive()):
            pickups[pIndex].active = True
            pickups[pIndex].timeOfStart = time.time()

#sort of an update     
def countDown(pIndex): 
    global pickups
    if ((pIndex >= 0) and (pIndex < len(pickups))):
        if pickups[pIndex].isActive(): # Once spawned, the pickup should be visible for 30, unless picked up
            pickups[pIndex].timeRemaining = time.time() - pickups[pIndex].timeOfStart
            pickups[pIndex].timeRemaining = 30 - pickups[pIndex].timeRemaining
 
            if (pickups[pIndex].timeRemaining <= 0):
                pickups[pIndex].active = False
                pickups[pIndex].timeOfStart = time.time()
        
        elif not(pickups[pIndex].isActive()): #once faded or picked up, the pickup reappears in 5 
            pickups[pIndex].timeRemaining = time.time() - pickups[pIndex].timeOfStart
            pickups[pIndex].timeRemaining = 5 - pickups[pIndex].timeRemaining        
        
            if (pickups[pIndex].timeRemaining <= 0):
                pickups[pIndex].active = True
                pickups[pIndex].timeOfStart = time.time()
    
def pickupIsActive(pIndex):
    global pickups
    if (pIndex >= 0 and pIndex < len(pickups)):
        if (pickups[pIndex].isActive()):
            return 1
        else :
            return 0

def getPos(pIndex):
    global pickups
    (pIndex >= 0) and (pIndex < len(pickups))):
       return (pickups[index].getPos())
        
def update():
    PyEngine.NotifyWhen("Stuff", countDown, None)
    
def getPickupPos(pIndex):
    PyEngine.NotifyWhen("OtherStuff", getPos, pIndex)
    
def checkActive(pIndex):
    PyEngine.NotifyWhen("EvenMoarStuff", pickupIsActive, pIndex)
    
createSpawnPoint(50.0, 50.0, 50.0)
createPickup()

gameIsActive = True
#activatePickup(0)
while gameIsActive:
    n = raw_input()
    number = int(n)
    if number == 1:
        gameIsActive = False
    elif number == 2:
        consumePickup(0)
    
    countDown(0)
    if pickupIsActive(0) == 1:
        print("Render pickup etcetc")
        print("time remaining until pickup fades: " )
        print(pickups[0].timeRemaining)
    else :
        print("Pickup 0 is now on cooldown")
        print("time remaining until pickup spawns: ")
        print(pickups[0].timeRemaining)