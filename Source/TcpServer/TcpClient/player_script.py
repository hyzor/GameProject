#-*- coding: utf-8 -*-

import random
import time
import math
import PyEngine
import platform_script

random.seed(time.time())

class Position(object):
        def __init__(self, x, y, z):
                self.x = x
                self.y = y
                self.z = z

        def GetSelf(self):
                return float(self.x), float(self.y), float(self.z)


def RandomSpawn():
        platforms = platform_script.platforms
        index = random.randint(0, len(platforms)-1)
        x = platforms[index].pos.x
        y = platforms[index].pos.y
        z = platforms[index].pos.z
        y += 100
        if(platforms[index].type == 3):
            z+=50
        x += 0.01
        y += 0.01
        z += 0.01

        print x,y,z, len(platforms), index
        return float(x), float(y), float(z)



def GetRandomSpawn():
        PyEngine.NotifyWhen("Spawnpoint", RandomSpawn, None)


random.seed(None)
