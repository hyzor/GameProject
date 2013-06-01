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

        def __init__(self, id, type, pos, limit, speed):
                self.id = id
                self.type = type
                self.pos = pos
                self.mov = Position(0,0,0)
                self.__start = Position(pos.x, pos.y, pos.z)
                self.__speed = speed

                if random.randint(0, 1) is 0:
                        self.__dir = -1
                else:
                        self.__dir = 1

                self.__lenght = math.sqrt(limit.x*limit.x+limit.y*limit.y+limit.z*limit.z)
                self.__nLimit = limit
                if self.__lenght != 0:
                    self.__nLimit.x /= self.__lenght
                    self.__nLimit.y /= self.__lenght
                    self.__nLimit.z /= self.__lenght


        def CheckDir(self):
                moved = math.sqrt(pow(self.pos.x-self.__start.x)+pow(self.pos.y-self.__start.y)+pow(self.pos.z-self.__start.z))
                if moved >= self.__lenght:
                        self.pos.x = self.__start.x+self.__nLimit.x*self.__lenght*self.__dir
                        self.pos.y = self.__start.y+self.__nLimit.y*self.__lenght*self.__dir
                        self.pos.z = self.__start.z+self.__nLimit.z*self.__lenght*self.__dir
                        self.__dir *= -1

        def Move(self, dt):
                self.CheckDir()
                self.mov.x = self.__nLimit.x*self.__speed*self.__dir
                self.mov.y = self.__nLimit.y*self.__speed*self.__dir
                self.mov.z = self.__nLimit.z*self.__speed*self.__dir
                self.pos.x += self.mov.x*dt
                self.pos.y += self.mov.y*dt
                self.pos.z += self.mov.z*dt

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

def CreatePlatforms():
        global platforms
        f = open("platform_info9.txt")
        i = 0
        for l in f:
                lsplit = l.split();
                t = 0
                x = 0
                y = 0
                z = 0
                lx = 0
                ly = 0
                lz = 0
                s = 0
                if len(lsplit) == 4:
                    t, x, y, z = [int(v) for v in lsplit]
                elif len(lsplit) == 8:
                    t, x, y, z, lx, ly, lz, s = [int(v) for v in lsplit]
                else:
                    continue
                platforms.append(Platform(i, t, Position(x, y, z), Position(lx,ly,lz), s))
                PyEngine.NotifyWhen("Create", GetPlatform, i)
                i += 1
                pass
        f.close()


def pow(a):
    return a*a


















