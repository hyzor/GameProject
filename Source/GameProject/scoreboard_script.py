# -*- coding: utf-8 -*-
# Koden ovan låter oss använda svenska tecken i python, den måste ligga först

import PyEngine # Ladda in vår c++ modul GameEngine

class PlayerStats(object):

        def __init__(self, id, name):
                self.id = id
                self.rank = id + 1
                self.name = name
                self.kills = 0
                self.deaths = 0

        def GetString(self):
                return str(self.rank), self.name, str(self.kills), str(self.deaths)

class ScoreBoard(object):

        def __init__(self):
                self.desc = "Rank", "Name", "Kills", "Deaths"
                self.ps = []

sb = ScoreBoard()
def CreatePlayerStats(id, name):
        global sb
        sb.ps.append(PlayerStats(id, name))
##        sp.ps.append(PlayerStats(id+1, "Gnidleif"))
##        sp.ps[id+1].kills = 4

def GetDesc():
        return sb.desc;

def GetStats(id):
        return FindByIndex(id).GetString();

def AddKill(id):
        global sb
        FindByIndex(id).kills += 1
##        Sort()

def AddDeath(id):
        global sb
        FindByIndex(id).deaths += 1

def TabDown():
        PyEngine.NotifyWhen("Description", GetDesc, None)
        for i in range(len(sb.ps)):
                PyEngine.NotifyWhen("Stats", GetStats, i)

def PrintPlayers():
        print "/////////"
        for PlayerStats in sb.ps:
                print PlayerStats.GetString()

def FindByIndex(index):
        for i in range(len(sb.ps)):
                if sb.ps[i].id is index:
                        return sb.ps[i]
                

def Swap(index):
        sb.ps[index].rank, sb.ps[index+1].rank = sb.ps[index+1].rank, sb.ps[index].rank
        sb.ps[index], sb.ps[index+1] = sb.ps[index+1], sb.ps[index]

def Sort():
        done = False
        while done is False:
                done = True
                for i in range(len(sb.ps)-1):
                        if sb.ps[i].kills < sb.ps[i+1].kills:
                                done = False
                                Swap(i)

