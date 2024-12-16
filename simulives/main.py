from numpy import random


class Contestant:
    def __init__(self,name,mean,stdev,lives,cap):
        self.name = name
        self.mean = mean
        self.stdev = stdev
        self.last = None
        self.lives = lives
        self.cap = cap
    def round(self):
        self.last = random.normal(loc = self.mean, scale = self.stdev)
    def lifeup(self):
        if(self.lives < self.cap and self.lives > 0):
            self.lives += 1
    def lifedown(self):
        if(self.lives > 0):
            self.lives -= 1
    def is_alive(self):
        if(self.lives > 0):
            return True
        else:
            return False
    def __str__(self):
        temp = self.name
        if(self.lives > 3):
            temp += " - "
            temp += str(self.lives)
            temp += "♥" # Alt Code 3
        elif(self.lives == 3):
            temp += " - ♥♥♥"
        elif(self.lives == 2):
            temp += " - ♥♥"
        elif(self.lives == 1):
            temp += " - ♥"
        else:
            temp += " - Dead"
        return temp

def myFunc(contestant):
    return contestant.last
print("i love you <3")

# Rules for S8
# Starting Lives: 2
# Life Cap: 3
# Prize Zone: 5% (x.5 rounds up, eg a 10 contestant round has 0.5 = ~1 prize)
# Life Loss Zone: 49%  (x.5 rounds up, eg a 50 contestant round has 24.5 = ~25 life losers)
starting = 3
lifecap = 5
prize_zone = .05
life_loss_zone = .49
f1 = open("names.txt")
f2 = open("means.txt")
f3 = open("stdev.txt")
names = []
means = []
stdev = []
for name in f1:
    names.append(name)
for mean in f2:
    means.append(float(mean))
for s in f3:
    stdev.append(float(s))
f1.close()
f2.close()
f3.close()
if(len(names) == len(means) and len(means) == len(stdev)):
    print("All arrays are size",len(names))
else:
    print("ERROR: Array sizes unequal")
    exit(1)

# Simulation Begins
alive = []
dead = []
for i in range(0,len(names)):
    temp = Contestant(names[i].strip("\r\n"),means[i],stdev[i],starting,lifecap)
    # print(temp)
    alive.append(temp)
loop = True
round_number = 0
while(loop):
    # Calculate debug info
    round_number += 1
    prizers = round(prize_zone * len(alive))
    life_losers = round(life_loss_zone * len(alive))
    if(len(alive) == 2):
        life_losers =1
    print("Round",round_number,"-","Alive:",len(alive))

    # Calculate new scores and sort
    for a in alive:
        a.round()
    alive.sort(reverse=True,key = myFunc)
    
    # Print leaderboard

    for i in range(0,len(alive)):
        if(i < prizers):
            alive[i].lifeup()
        if(i+life_losers >= len(alive)):
            alive[i].lifedown()
        print(alive[i],"-",alive[i].last)
    # Alive-Dead Status
    for i in range(0,len(alive)):
        if(i >= len(alive)):
            break
        if(not alive[i].is_alive()):
            temp = alive.pop(i)
            dead.append(temp)
            i-= 1
    #bababnas = input("Should we stop? (y/n) ") # This is debug code
    #if(bababnas == "Y" or bababnas == "y"):
    #    loop = False
    if(len(alive) < 2):
        loop = False

print("Season over, congrats to",alive[0].name,"for winning in",round_number,"rounds.")