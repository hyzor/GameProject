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
        __speed = 5
        
        def __init__(self, id, type, pos):
                self.id = id
                self.type = type
                self.pos = pos
                self.mov = Position(0,0,0)
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
        
        def Move(self, dt):
                self.CheckDir()
                self.mov.y = self.__speed * self.__dir
                self.pos.y += self.mov.y*dt
				
        def GetSelfMove(self):
                return float(self.pos.x), float(self.pos.y), float(self.pos.z), float(self.mov.x), float(self.mov.y), float(self.mov.z)

        def GetSelf(self):
                return self.id, self.type, float(self.pos.x), float(self.pos.y), float(self.pos.z)

platforms = []

def GetPlatform(index):
        return platforms[index].GetSelf()

def GetPos(index):
        return platforms[index].pos.GetSelf()

def GetMove(index):
        return platforms[index].GetSelfMove()

def PrintPlatforms():
        for Platform in platforms:
                print Platform.GetSelf()

def MovePlatform(index, dt):
        global platforms
        platforms[index].Move(dt)
        PyEngine.NotifyWhen("Moved", GetMove, index)

def Solution(index):
        PyEngine.NotifyWhen("ToTheProblem", GetPos, index)

def CreatePlatforms():
        global platforms
        f = open("platform_info9.txt")
        i = 0
        for l in f:
                x, y, z = [int(v) for v in l.split()]
                type = random.randint(1, 4)
                platforms.append(Platform(i, type, Position(x, y, z)))
                PyEngine.NotifyWhen("Create", GetPlatform, i)
                i += 1
                pass
        f.close()


















  
