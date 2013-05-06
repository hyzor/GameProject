# -*- coding: utf-8 -*-
# Koden ovan låter oss använda svenska tecken i python, den måste ligga först

import PyEngine # Ladda in vår c++ modul GameEngine

class Position(object):
        x = 0.0
        y = 0.0
        z = 0.0

        def __init__(self, x, y, z):
                self.x = x
                self.y = y
                self.z = z

        def GetSelf(self):
                return float(self.x), float(self.y), float(self.z)

class Platform(object):
        id = 0
        pos = Position(0.0, 0.0, 0.0)
        bounds = Position(0.0, 0.0, 0.0)
        direction = Position(1, 1, 1)
        
        def __init__(self, id, pos):
                self.id = id
                self.pos = pos
                self.bounds = Position(self.pos.x + 100, self.pos.y + 100, self.pos.z + 100)
                PyEngine.NotifyWhen("Unchanged", GetPosition, self.id)
                
        def SwitchDirection(self):
                if self.pos.x >= self.bounds.x or self.pos.x <= -self.bounds.x:
                        self.direction.x *= -1
                if self.pos.y >= self.bounds.y or self.pos.y <= -self.bounds.y:
                        self.direction.y *= -1
                if self.pos.z >= self.bounds.z or self.pos.z <= -self.bounds.z:
                        self.direction.z *= -1

        def Move(self):
                self.SwitchDirection()
                self.pos.x += (1 * 50) * self.direction.x
                self.pos.y += (1 * 50) * self.direction.y
                self.pos.z += (1 * 50) * self.direction.z
                PyEngine.NotifyWhen("Moved", GetPosition, self.id)
                
        def GetPos(self):
                return self.pos.GetSelf()

platforms = []
def CreatePlatforms(amount):
        global platforms
        i = 0
        for i in range(0, amount):
                platforms.insert(i, Platform(i, Position(i*200, -i*200, i*200)))

def MovePlatforms():
        for Platform in platforms:
                Platform.Move()
        PyEngine.NotifyAfter(1, MovePlatforms, None)

def GetPosition(id):
        return platforms[id].GetPos()

##CreatePlatforms(10)
##i = 0
##for i in range(0, 10):
##        print(str(GetPosition(i)))
##        MovePlatforms()
##        print(str(GetPosition(i)))
##        print("Move that... platform!")

PyEngine.NotifyAfter(1, MovePlatforms, None)

