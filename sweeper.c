#include <stdlib.h>
#include <SDL/SDL.h>
#include <math.h> 
#include <time.h>
#include "tick.h"
#include <wiringPi.h>
#include "BrickPi.h"
#include <linux/i2c-dev.h>  
#include <fcntl.h>

#undef DEBUG

/* Sweeper: A remote controlled sweeper bot.
 *
 * Qasim Dove (emailqasim@gmail.com)
 *
 * Written: 2014/12/12
 *
 * These files have been made available online through a Creative Commons
 * Attribution-ShareAlike 3.0  license.
 * (http://creativecommons.org/licenses/by-sa/3.0/)
 */

// Integer Variables //
int result;
int encoderPosition;
int encoderTarget = 359;
int rotations = 0;
int motorASpeed;
int motorBSpeed;
int motorCSpeed;
int motionSpeed;

int sweeper_on;
int i;
int l_joystick_speed;
int r_joystick_speed;
int joystick_count;
int quit_status = 0;

SDL_Joystick *joystick;

int joystickSetup(){
  if (SDL_Init( SDL_INIT_JOYSTICK | SDL_INIT_VIDEO) < 0){
    fprintf(stderr, "Couldn't initilize SDL: %s\n", SDL_GetError());
    exit(1);
  }

  SDL_JoystickEventState(SDL_ENABLE);
  joystick = SDL_JoystickOpen(0);
  joystick_count = SDL_NumJoysticks();

  if (joystick_count > 0){
    printf("%i Joysticks were found.\n\n", joystick_count);
    printf("The names of the joysticks are:\n\n");
    printf("\tName:       %s\n", SDL_JoystickName(0));
    printf("\tAxes:       %i\n", SDL_JoystickNumAxes(joystick));
    printf("\tTrackballs: %i\n", SDL_JoystickNumBalls(joystick));
    printf("\tButtons:    %i\n", SDL_JoystickNumButtons(joystick));
  }else{
    printf("No joysticks found. Exiting\n\n");
    exit(1);
  }

  for(i=0; i < joystick_count; i++ ){
    printf("%s\n", SDL_JoystickName(i));
  }

}

/*
  Name: robotSetup
  Arguments: Nothing.
  Returns:    
    1 - When a setup function is not successful.
    0 - When a setup function is successful.
*/
int robotSetup(){
  // Resets the timer in "tick.h" //
  ClearTick();
   
  // Checks to see if the BrickPi is connected. //
  result = BrickPiSetup();
  printf("BrickPiSetup: %d\n", result);
  if(result){
    printf("Error: Could not setup BrickPi. Check to see if it's connected");
    return 1;
  };

  // Sets the first two digits of the BrickPi's Address  to 1.2...
  BrickPi.Address[0] = 1;
  BrickPi.Address[1] = 2;

  // Enables the motor ports 
  BrickPi.MotorEnable[PORT_A] = 1;
  BrickPi.MotorEnable[PORT_B] = 1;
  BrickPi.MotorEnable[PORT_C] = 1;
  
  // Checks BrickPi sensors to ensure they were setup correctly.
  // If they aren't it exits.
  result = BrickPiSetupSensors();
  printf("BrickPiSetupSensors: %d\n", result); 
  if(result){
    printf("Error: Could not setup BrickPi Sensors. Are the right sensors connected?");
    return 1;
  };
  return 0;
}

/*
  Name: motorATurn
  Arguments: motorASpeed (integer)
  Returns: Nothing.
*/
void motorATurn(motorASpeed){
  BrickPi.MotorSpeed[PORT_A] = motorASpeed;
  BrickPiUpdateValues(); // Updates the sensors connected to the BrickPi
}

/*
  Name: motorBTurn
  Arguments: motorBSpeed (integer)
  Returns: Nothing.
*/
void motorBTurn(motorBSpeed, time){
  BrickPi.MotorSpeed[PORT_B] = motorBSpeed;
  BrickPiUpdateValues(); // Updates the sensors connected to the BrickPi

}

/*
  Name: motorCTurn
  Arguments: motorCSpeed (integer)
  Returns: Nothing.
*/
void motorCTurn(motorCSpeed){
  BrickPi.MotorSpeed[PORT_C] = motorCSpeed;
  BrickPiUpdateValues(); // Updates the sensors connected to the BrickPi
}

/*
  Name: move
  Arguments: motionSpeed (integer)
  Returns: Nothing.
*/
void moveForward(move_power, move_time){
  //start counter.
  time_t start_time;
  start_time = time(NULL);
  move_power = move_power * -1;
  
  //Tracking
  time_t current_time;
  current_time = start_time;
  
  int end_time = start_time + move_time;
  
  while(current_time <= end_time){
    current_time = time(NULL);
    motorBTurn(move_power);
    motorCTurn(move_power);
    
    //To be safe
    if (current_time > end_time){
      break;
    }
  }
}

void moveBackward(move_power, move_time){
  //start counter.
  time_t start_time;
  start_time = time(NULL);
  
  //Tracking
  time_t current_time;
  current_time = start_time;
  
  int end_time = start_time + move_time;
  
  while(current_time <= end_time){
    current_time = time(NULL);
    motorBTurn(move_power);
    motorCTurn(move_power);
    
    //To be safe
    if (current_time > end_time){
      break;
    }
  }
}

void turnLeft(move_power, move_time){
  //start counter.
  time_t start_time;
  start_time = time(NULL);
  
  //Tracking
  time_t current_time;
  current_time = start_time;
  
  int end_time = start_time + move_time;
  
  while(current_time <= end_time){
    current_time = time(NULL);
    motorBTurn(move_power);
    motorCTurn(move_power * -1);
    
    //To be safe
    if (current_time > end_time){
      break;
    }
  }
}

void turnRight(move_power, move_time){
  //start counter.
  time_t start_time;
  start_time = time(NULL);
  
  //Tracking
  time_t current_time;
  current_time = start_time;
  
  int end_time = start_time + move_time;
  
  while(current_time <= end_time){
    current_time = time(NULL);
    motorBTurn(move_power * -1);
    motorCTurn(move_power);
    
    //To be safe
    if (current_time > end_time){
      break;
    }
  }
}

void autonomousMode(){

  moveForward(1000, 2);
  moveBackward(1000, 2);
  turnRight(1000, 4);
  turnLeft(1000, 4);
  moveBackward(1000, 2);
  moveForward(1000, 2);
}

/*
  Name: main
  Arguments: Nothing.
  Returns:
    0 - When main is complete.
*/

int main(){
  result = robotSetup(); 
  if(!result){ 
    result = BrickPiUpdateValues();
    if(!result){
      
      joystickSetup();
      SDL_Event event;
      
      while(quit_status == 0){
        while(SDL_PollEvent(&event) !=0 && quit_status == 0){
          switch(event.type){
            case SDL_JOYBUTTONDOWN:
              printf("Joystick: %i\n Button: %i\n", event.jbutton.which, event.jbutton.button);
                if (event.jbutton.button == 3){
                  printf(" \nExiting... \n");
                  quit_status = 1;
                }
                if (event.jbutton.button == 0){
                  autonomousMode();
                }
                if (event.jbutton.button == 14){
                  sweeper_on = 0;
                }
                else{
                  sweeper_on = -1;
                }
                break;
            case SDL_JOYAXISMOTION:
              if(event.jaxis.axis == 1){
                l_joystick_speed = (event.jaxis.value/3276) * 25;
                printf("Left: %i\n", l_joystick_speed);
              }
              if(event.jaxis.axis == 3){
                r_joystick_speed = (event.jaxis.value/3276) * 25;
                printf("Right: %i\n", r_joystick_speed);
              }
              break;
          }
        }
        motorATurn(sweeper_on * 250);
        motorCTurn(r_joystick_speed);
        motorBTurn(l_joystick_speed);
      }
      SDL_JoystickClose(joystick);
      return 0;
    }
    usleep(10000);
  }
  return 0;
}  
