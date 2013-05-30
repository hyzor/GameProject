import PyEngine

class PlayerStats(object):

        def __init__(self, id, name, rank):
                self.id = id
                self.rank = rank
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
    rank = len(sb.ps)+1
    sb.ps.append(PlayerStats(id, name, rank))

def GetDesc():
    return sb.desc;

def GetStats(rank):
    return FindByRank(rank).GetString();

def AddKill(identity):
    global sb
    FindByID(identity).kills += 1
    Sort()

def AddDeath(identity):
    global sb
    FindByID(identity).deaths += 1

def TabDown():
    PyEngine.NotifyWhen("Description", GetDesc, None)
    for i in range(len(sb.ps)):
        PyEngine.NotifyWhen("Stats", GetStats, i+1)

# For debugging
def PrintPlayers():
    print "/////////"
    for PlayerStats in sb.ps:
            print PlayerStats.GetString()

def FindByRank(rank):
    global sb
    for PlayerStats in sb.ps:
        if rank == PlayerStats.rank:
            return PlayerStats

def FindByID(identity):
    global sb
    for PlayerStats in sb.ps:
        if identity == PlayerStats.id:
            return PlayerStats

def Swap(index):
    sb.ps[index].rank, sb.ps[index+1].rank = sb.ps[index+1].rank, sb.ps[index].rank
    sb.ps[index], sb.ps[index+1] = sb.ps[index+1], sb.ps[index]

def Sort():
    global sb
    done = False
    while done is False:
        done = True
        i = 0
        while i is not len(sb.ps)-1:
            if sb.ps[i].kills < sb.ps[i+1].kills:
                done = False
                Swap(i)
            i += 1

def ResetStats():
    for PlayerStats in sb.ps:
            PlayerStats.kills = 0
            PlayerStats.deaths = 0

def RemovePlayer(identity):
    global sb
    i = 0
    while i is not len(sb.ps):
        if identity == sb.ps[i].id:
            sb.ps.pop(i)
            j = i
            while j is not len(sb.ps):
                if sb.ps[j].rank > j+1:
                    sb.ps[j].rank -= 1
                j += 1
            break
        i += 1