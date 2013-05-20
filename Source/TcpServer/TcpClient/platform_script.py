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

class Platform(object):
        __speed = 0.5
        
        def __init__(self, id, type, pos):
                self.id = id
                self.type = type
                self.pos = pos
                self.__limit = random.randint(10, 100)
                if random.randint(0, 1) is 0:
                        self.__dir = -1
                else:
                        self.__dir = 1
                self.__upper = self.pos.y + self.__limit
                self.__lower = self.pos.y - self.__limit
                
        def CheckDir(self):
                if self.pos.y >= self.__upper or self.pos.y <= self.__lower:
                        self.__dir *= -1
        
        def Move(self):
                self.CheckDir()
                self.pos.y += self.__speed * self.__dir

        def GetSelf(self):
                return self.id, self.type, self.pos.x, self.pos.y, self.pos.z

platforms = []

def GetPlatform(index):
        return platforms[index].GetSelf()

def GetPos(index):
        return platforms[index].pos.GetSelf()

def CreatePlatforms():
        global platforms
        amount = 2
        index = 0
        offset = 400
        for i in range(amount):
                for j in range(amount):
                        for k in range(amount):
                                pos = Position(i*offset, j*offset, k*offset)
                                type = random.randint(1, 4)
                                platforms.append(Platform(index, type, pos))
                                PyEngine.NotifyWhen("Create", GetPlatform, index)
                                index += 1

def MovePlatform(index):
        global platforms
        platforms[index].Move()
        PyEngine.NotifyWhen("Moved", GetPos, index)
        PyEngine.NotifyAfter(0.1, MovePlatform, index)
                
