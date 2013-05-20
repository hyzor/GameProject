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

        def __init__(self, index, name, pos):
                self.index = index
                self.name = name
                self.pos = pos

        def GetPos(self):
                return self.pos.GetSelf()

        def GetSelf(self):
                return self.index, self.name, self.pos.GetSelf()
                
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
        del spawnpoints[index]
        pos = Position(x, y, z)
        return Position(x, y, z)

def GetPos(index):
        return players[index].GetPos()

def CreatePlayer(index, name):
        global players
        players.append(Player(index, name, RandomizeSpawn()))
        PyEngine.NotifyWhen("Create player", GetPos, index)
