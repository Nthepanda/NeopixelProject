import board
from digitalio import DigitalInOut, Direction, Pull
import neopixel
from adafruit_pixel_framebuf import PixelFramebuffer, VERTICAL
import time

walkticks = 0
tempData = [0, 0, 0, 0, 0, 0, 0, 0]
pulseNumber = 1
A_BUTTON         = 0
B_BUTTON         = 1
SELECT_BUTTON    = 2
START_BUTTON     = 3
UP_BUTTON        = 4
DOWN_BUTTON      = 5
LEFT_BUTTON      = 6
RIGHT_BUTTON     = 7
levelWd = 32 

#===============================================================================
#  Pin Declarations
#===============================================================================
# Declare the pin numbers for the NES controller
nesDataPin      = board.GP4    # The data pin for the NES controller
nesClockPin     = board.GP2    # The clock pin for the NES controller
nesLatchPin     = board.GP3    # The latch pin for the NES controller

nesLatch = None
nesClock = None
nesData = None

pixel_pin = board.GP5
pixel_width = 16
pixel_height = 16

matrix = neopixel.NeoPixel(
    pixel_pin,
    pixel_width * pixel_height,
    brightness=0.05,
    auto_write=False,
)

maxtrix_buffer = PixelFramebuffer(
    matrix,
    16,
    16,
    alternating=True,
    reverse_x=True
)

builtinLED = DigitalInOut(board.LED)
builtinLED.direction = Direction.OUTPUT
builtinLED.value = True

"""
COLORS = {
  "RED": 0xff0000,
  "GREEN": 0x00ff00,
  "TRANSPARENT": 0x00,
  "ORANGE": 0xff6600,
  "CYAN": 0x00ffff,
  "BLUE": 0x0000ff
}
"""
COLORS = [
  0x0,
  0x00ff00,
  0x0000ff,
  0xff8800,
  0xffff00,
  0xff
]


# TYPE data key: {interfere with player physics}
END_OF_LEVEL_BLOCK = 2
types = [
    [1],
    [1],
    [-1]
]

#Object data key: {color, x, y, width, height, typex}
env = [
[1, 0, 14, 16, 2, 1],
[2, 3, 11, 5, 1, 1],
[2, 7, 10, 5, 1, 1],
[2, 11, 9, 5, 1, 1],
[2, 19, 14, 16, 2, 1],
[5, 31, 8, 1, 8, 2]
]


#Player variables
#{color, x, y, vertical speed, direction}  #Direction - 1: Right 0: Left
player = [3, 5, 5, 0, 1]

gravity = 1

enemies = [
  [0, 5, 14, 1]
]

MAX_PROJ_ALLOWED = 8
PROJ_FIRE_DELAY = 500
lastProjFireTime = 0
visibleProjCount = 0
#{active, color, x, y, type, speed}
proj = [
  [0, 4, 0, 0, 0, 1],
  [0, 4, 0, 0, 0, 1],
  [0, 4, 0, 0, 0, 1],
  [0, 4, 0, 0, 0, 1],
  [0, 4, 0, 0, 0, 1],
  [0, 4, 0, 0, 0, 1],
  [0, 4, 0, 0, 0, 1],
  [0, 4, 0, 0, 0, 1]
]


leftXCoord = 0

#GAME CLOCK
nextScreenRefresh = 0
GAME_REFRESH_DELAY = .07

def setup():
  global nesLatch, nesData, nesClock, maxtrix_buffer
  
# Set appropriate pins to inputs and outputs
nesData = DigitalInOut(nesDataPin)
nesData.direction = Direction.INPUT

nesClock = DigitalInOut(nesClockPin)
nesClock.direction = Direction.OUTPUT

nesLatch = DigitalInOut(nesLatchPin)
nesLatch.direction = Direction.OUTPUT

time.sleep(1)

# Set initial states for clock and latch
nesClock.value = False
nesLatch.value = False


def checkNESController(LatchN):
#while True:
  global nesLatch, nesClock, nesData, pulseNumber
  tempData = [0,0,0,0,0,0,0,0]

  for i in range(8):
    tempData[i] = 0 # Set each element to 0

  # Quickly pulse the nesLatch pin so that the register grabs the current state of the buttons
  nesLatch.value = True
  nesLatch.value = False

  # Shift in the button states for each button
  if nesData.value == False:
    tempData[8-pulseNumber] = 1 
  nesClock.value = True
  nesClock.value = False
  pulseNumber += 1
  
  if nesData.value == False:
    tempData[8-pulseNumber] = 1 
  nesClock.value = True
  nesClock.value = False
  pulseNumber += 1
  
  if nesData.value == False:
    tempData[8-pulseNumber] = 1 
  nesClock.value = True
  nesClock.value = False
  pulseNumber += 1
  
  if nesData.value == False:
    tempData[8-pulseNumber] = 1 
  nesClock.value = True
  nesClock.value = False
  pulseNumber += 1
  
  if nesData.value == False:
    tempData[8-pulseNumber] = 1 
  nesClock.value = True
  nesClock.value = False
  pulseNumber += 1
  
  if nesData.value == False:
    tempData[8-pulseNumber] = 1 
  nesClock.value = True
  nesClock.value = False
  pulseNumber += 1
  
  if nesData.value == False:
    tempData[8-pulseNumber] = 1 
  nesClock.value = True
  nesClock.value = False
  pulseNumber += 1
  
  if nesData.value == False:
    tempData[8-pulseNumber] = 1 


  returnV = 0
  returnV = tempData[LatchN]
  pulseNumber = 1
  nesClock.value = True
  nesClock.value = False
  return returnV



def addProj(direction):

  #print(visibleProjCount)
  lastProjFireTime = time.monotonic()
  
  if checkPixelType(player[1] + 1,player[2]) == -1 and visibleProjCount < MAX_PROJ_ALLOWED:

    #Loop over the proj array to find a proj that is inactive proj[x][0] = 0
    for i in range(MAX_PROJ_ALLOWED):
      
      if proj[i][0] == 0:
        #fire Projectile
        proj[i][0] = 1
        proj[i][1] = 4
        proj[i][2] = player[1]+direction
        proj[i][3] = player[2]
        proj[i][4] = 0
        proj[i][5] = direction
        #proj[visibleProjCount] = {1, 4, player[1]+1, player[2], 0, 1}
        visibleProjCount+= 1

        return

#define INTRO 1

game_state = 0

def checkPixelType(checkX, checkY):

  for i in range(len(env)):
    for x in range(env[i][1], env[i][3] + env[i][1]):
      for y in range(env[i][2], env[i][4] + env[i][2]):
        #matrix.drawPixel(x-leftXCoord, y, COLORS[env[i][0]])
        if checkX == x and checkY == y:
          return env[i][5]


  return -1


def updateLEDPanel():
  global maxtrix_buffer

  maxtrix_buffer.fill(0)
  #Draw the ENV
  for i in range(len(env)):
    for x in range(env[i][1], env[i][3] + env[i][1]):
      for y in range(env[i][2], env[i][4] + env[i][2]):
        maxtrix_buffer.pixel(x-leftXCoord, y, COLORS[env[i][0]])

    #remember Noah:
    #{active, color, x, y, type, speed}
  for i in range(MAX_PROJ_ALLOWED):
    if proj[i][0] == 1:
        maxtrix_buffer.pixel((proj[i][2]-leftXCoord), proj[i][3], COLORS[proj[i][1]])




  #leftXCoord += 1  

  #Draw the player
  maxtrix_buffer.pixel((player[1]-leftXCoord), player[2], 0xff0000)

  maxtrix_buffer.display()

#setup()


#pixel_test = neopixel.NeoPixel(board.GP5, 16*16, brightness=0.07, auto_write=False)

#pixel_test[4] = (255, 0, 0)
#pixel_test.show()
FrameN = 0
while True:
  
  

  #Update LCD Panel

  if time.monotonic() >= nextScreenRefresh:
    FrameN += 1
    #print(FrameN)
    for i in range(MAX_PROJ_ALLOWED):
      if proj[i][0] == 1:

        #Check to see if the next block to the left or right (depending on direction) is -1, true move, false set the proj to inactive
        if checkPixelType(proj[i][2] + proj[i][5], proj[i][3]) == -1 and proj[i][2] >= 0 and proj[i][2] < levelWd:
          #print(String("Proj can move") + String(i))
          proj[i][2]+=proj[i][5]
        else:
          proj[i][0] = 0
          visibleProjCount -= 1
        


    if tempData[6] == 1 and time.monotonic() >= lastProjFireTime + PROJ_FIRE_DELAY:
      
      
      addProj(player[4])

      
  
    
    
    #Vertical movement physics
    if checkNESController(7) == 1 and checkPixelType(player[1],player[2]+1) != -1:
      player[3] = 4
    

    #if player speed is negative, 
      #check the block below, if we can move into it, update player y value, otherwise set player vertical speed to 1  speeds of 0 and 1 mean player does not move
    if player[3] < 0 and checkPixelType(player[1],player[2]+1) == -1:
      player[2]+= 1
    
    #If player speed is 2 or greater
      #check the block above, if we can move into it, update player y value, otherwise set the player vertical speed to 1
    if player[3] >= 2 and checkPixelType(player[1],player[2]-1) == -1:
      player[2]-= 1
    

    #Subtract 1 from player vertical speed
    player[3]-= 1
    
    walkticks += 1
    if walkticks > 1:
      walkticks = 0

      #Player movement
      if checkNESController(0) == 1: #Right
        if player[1] < levelWd - 1 and checkPixelType(player[1]+1,player[2]) == -1:
         player[1] = player[1] + 1
         player[4] = 1
        
        if player[1] - leftXCoord > 8 and leftXCoord + 16 < levelWd:
          leftXCoord+= 1
        
      
      if checkNESController(1) == 1: #Left
        player[4] = -1
        if player[1] > 0 and checkPixelType(player[1]-1,player[2]) == -1:
            player[1] = player[1] - 1
           
        
        
       
        if player[1] - leftXCoord < 9 and leftXCoord > 0:
          
          leftXCoord-= 1
        
      """
      if tempData[3] == 1: #Up
        if player[2] > 0 and checkPixelType(player[1],player[2]-1) == -1:
            player[2] = player[2] - 1
       
      
      
      if tempData[2] == 1: #Down
        if player[2] < 15 and checkPixelType(player[1],player[2]+1) == -1:
            player[2] = player[2] + 1
      """
    
    #Update the LCD Panel
    updateLEDPanel()
    #Set the time of the next LCD panel update
    nextScreenRefresh = time.monotonic() + GAME_REFRESH_DELAY
  

  
  """
  #update camera again
  if(player[1] > 7:
  if(player[1] < (levelWd-8):
  leftXCoord = player[1] - 7
  } else {
    leftXCoord = levelWd-8
  }
  }else {
    leftXCoord = 0
  }


  #update camera again again
  if(player[1] > 7:
  if(player[1] < (levelWd-16):
  leftXCoord = player[1] - 7
  } else {
    leftXCoord = levelWd-24
  }
  }else {
    leftXCoord = 0
  }
  """
  time.sleep(1/1000)


# type: ignore

