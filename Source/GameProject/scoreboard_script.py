# -*- coding: utf-8 -*-
# Koden ovan låter oss använda svenska tecken i python, den måste ligga först

import PyEngine # Ladda in vår c++ modul GameEngine

class PlayerStats(object):

        def __init__(self, id, name):
                self.id = id
                self.name = name
                self.kills = 0
                self.deaths = 0

        def GetString(self):
                return str(self.id), self.name, str(self.kills), str(self.deaths)

class ScoreBoard(object):

        def __init__(self):
                self.desc = "ID", "Name", "Kills", "Deaths"
                self.ps = []
                
sb = ScoreBoard()
def CreatePlayerStats(id, name):
        global sb
        sb.ps.append(PlayerStats(id, name))

def GetDesc():
        return sb.desc;

def GetStats(id):
        return sb.ps[id].GetString();

def TabDown():
        PyEngine.NotifyWhen("Description", GetDesc, None)
        for i in range(len(sb.ps)):
                PyEngine.NotifyWhen("Stats", GetStats, i)


