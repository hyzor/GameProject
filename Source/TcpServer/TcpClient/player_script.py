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

class Player(object):

        def __init__(self, id, name, pos):
                self.id = id
                self.name = name
                self.pos = pos

        def GetPos(self):
                return self.pos.GetSelf()

        def GetSelf(self):
                return self.id, self.name, self.pos.GetSelf()

spawnpoints = []
players = []

def CreateSpawnPoint(x, y, z):
        global spawnpoints
        spawnpoints.append(Position(x, y+150, z))

def RandomizeSpawn():
        global spawnpoints
        index = random.randint(0, len(spawnpoints)-1)
        x = spawnpoints[index].x
        y = spawnpoints[index].y
        z = spawnpoints[index].z
        return x, y, z

def FindByID(id):
        for i in range(len(players)):
                if players[i].id is id:
                        return players[i]

def GetPos(id):
        return FindByID(id).GetPos()

def CreatePlayer(id, name):
        global players
        global spawnpoints
        index = random.randint(0, len(spawnpoints)-1)
        x = spawnpoints[index].x
        y = spawnpoints[index].y
        z = spawnpoints[index].z
        players.append(Player(id, name, Position(x, y, z)))
        PyEngine.NotifyWhen("Create player", GetPos, id)

def Solution():
        PyEngine.NotifyWhen("ToTheProblem", RandomizeSpawn, None)

##        192.168.0.17
