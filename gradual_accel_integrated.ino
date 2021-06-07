#include <AccelStepper.h>

//definitions
#define sixteenth_steps_per_cm 533
#define eighth_steps_per_cm  267
#define quarter_steps_per_cm  133
#define half_steps_per_cm  67
#define full_steps_per_cm  33

long int steps_per_cm = quarter_steps_per_cm; //num of steps to cover a cm
long int max_distance_allowed = 100; //dist in cm

long int max_position1_allowed = 70; //dist in cm
//long int max_position2_allowed = 60; //dist in cm

long int max_vel_allowed = 30; //velocity in cm/sec
long int max_vel_in_steps = max_vel_allowed * steps_per_cm; //convert to steps/sec
//long int max_accel_allowed = 30; //accel in cm/sq. sec
//long int max_accel_in_steps = max_accel_allowed * steps_per_cm; //convert to steps/sq sec

//pin assignments
int switch0 = 2; //high normally, goes low when limit switch is hit
int enable = 9; //enable pin for the stepper output. 
int M1 = 6;
int M2 = 7;
int M3 = 8;

// Define a stepper and the pins it will use
AccelStepper stepper(AccelStepper::DRIVER, 4, 5);

long int pos = 100*steps_per_cm/2; //100cm * 533steps/cm (division by 2 because it goes from +val to -val)


float target_vel = 20.0; 
float target_rampdown_spot = 40;
float target_accel;

void setup()
{  
  Serial.begin(115200);
  pinMode(switch0, INPUT);
  pinMode(enable, OUTPUT);
  digitalWrite(enable, 0); //0 is enabled by default, 1 is disabled

  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M3, OUTPUT);
  digitalWrite(M1, 0); 
  digitalWrite(M2, 1); 
  digitalWrite(M3, 0); //0-0-0 is full step, 1-1-0 is eighth step, 1-1-1 is sixteenth step
 
  digitalWrite(LED_BUILTIN, 1);   

  pinMode(LED_BUILTIN, OUTPUT);
  //attachInterrupt(digitalPinToInterrupt(switch0), switch0_changed, CHANGE);

  stepper.setMaxSpeed(30*steps_per_cm); //
  stepper.setAcceleration(50*steps_per_cm);

  stepper.setSpeed(5*steps_per_cm);
  stepper.setAcceleration(50*steps_per_cm);

  //PUT HOMING SEQUENCE HERE
  //to start homing, set a slightly positive posn and move slowly until switch is triggered
    stepper.moveTo(150*steps_per_cm);
    stepper.setSpeed(5.0*steps_per_cm); //set homing speed to medium-slow pace
    stepper.setAcceleration(50.0 * (float)steps_per_cm);  //bring accel back up high 
        
    while(digitalRead(switch0)){  //keep moving until switch is triggered (goes low when triggered) OR we've gotten to the desired position
      stepper.runSpeedToPosition();
    }
    stepper.setCurrentPosition(stepper.currentPosition()); //zeros the position at which switch is triggered
    
    //once triggered, with the same slow vel, move forward 5cm
    stepper.moveTo(stepper.currentPosition() - 5*steps_per_cm);
    stepper.setSpeed(-5*steps_per_cm);
    //stepper.runToNewPosition((5) * steps_per_cm);
    while(stepper.distanceToGo() < 0){  //keep moving until switch is triggered (goes low when triggered) OR we've gotten to the desired position
      stepper.run();
    }

    Serial.println("HOMING COMPLETE");
}

void loop()
{
  //VELOCITY INPUT & INPUT-CHECKING STEP
  Serial.println("Enter target velocity (1-25cm/s):");
  while(Serial.available()==0){} //only continue once serial becomes available
  target_vel = Serial.parseFloat();
  if((target_vel > max_vel_allowed) || (target_vel < 1)){
    Serial.println("Entered velocity in cm/s is: ");
    Serial.println(target_vel);
    Serial.println("Entered velocity is outside the allowable value (1-25cm/s). Velocity set to 25cm/s instead.");
    target_vel = max_vel_allowed;
  }  
  else{
    Serial.println("Entered velocity value is:");
    Serial.println(target_vel);    
  }

  //POSN1 INPUT & INPUT-CHECKING STEP
  Serial.println("Enter stop location (20-60cm):");
  while(Serial.available()==0){} //only continue once serial becomes available
  target_rampdown_spot = Serial.parseFloat();
  if((target_rampdown_spot > max_position1_allowed)|| (target_rampdown_spot < 20)){
    Serial.println("Entered rampdown_spot is:");
    Serial.println(target_rampdown_spot);
    Serial.println("Entered rampdown_spot is outside the allowable range (20-70cm). rampdown_spot set to 20cm instead.");
    target_rampdown_spot = 20;
  }  
  else{
    Serial.println("Entered rampdown_spot is:");
    Serial.println(target_rampdown_spot);    
  }

  
  Serial.println("END OF USER INPUT SECTION");

  //CALC AND MOVEMENT PHASE START
  stepper.moveTo(stepper.currentPosition() - 20.0*steps_per_cm); //but we're only gonna go 10cm irl
  stepper.setSpeed(-1*(target_vel)*steps_per_cm);
  
  //accel calc
  target_accel = target_vel * target_vel / (2.0 * 10.0); //assumes secret p1 is 10cm from ramp start
  
  Serial.println("target_vel is");
  Serial.println(target_vel);
  Serial.println("target_accel is");
  Serial.println(target_accel);
  Serial.println("rampdown_spot is");
  Serial.println(target_rampdown_spot);
  
  stepper.setAcceleration(target_accel*steps_per_cm);

  while(stepper.distanceToGo() < -10.0*steps_per_cm){  //exit the loop before the run command actually stops
    stepper.run();
  }

  stepper.moveTo(stepper.currentPosition() - (target_rampdown_spot - 15.0)*steps_per_cm);
  stepper.setSpeed(-1*(target_vel)*steps_per_cm);

  while(stepper.distanceToGo() < 0){  //keep moving until switch is triggered (goes low when triggered) OR we've gotten to the desired position
    stepper.runSpeed();
  }

  //ramp down
  stepper.moveTo(stepper.currentPosition() - 15.0*steps_per_cm);
  stepper.setSpeed(-0.1*steps_per_cm);
  stepper.setAcceleration(target_accel*steps_per_cm/1.5);

  while(stepper.distanceToGo() < 0){  //exit the loop before the run command actually stops
    stepper.run();
  }
  //CALC AND MOVEMENT PHASE END
  
  Serial.println("END Position Reached!");
  
  //get user input to begin homing
  Serial.println("Enter any key to start homing process");
  while(Serial.available()==0){} //only continue once serial becomes available
  Serial.parseFloat();
  //to start homing, set a slightly positive posn and move slowly until switch is triggered
  stepper.moveTo(150.0*steps_per_cm);
  stepper.setSpeed(5.0*steps_per_cm); //set homing speed to medium-slow pace
  stepper.setAcceleration(50.0 * (float)steps_per_cm);  //bring accel back up high 


  Serial.println("stepper.currentPosition() is:");
  Serial.println(stepper.currentPosition());
  Serial.println("stepper.targetPosition() is:");
  Serial.println(stepper.targetPosition());
  Serial.println("stepper.distanceToGo() is:");
  Serial.println(stepper.distanceToGo());
  Serial.println("stepper.speed() isz:");
  Serial.println(stepper.speed());
      
  stepper.moveTo(100*steps_per_cm);
  stepper.setSpeed(5.0*steps_per_cm); //set homing speed to medium-slow pace
  stepper.setAcceleration(50.0 * (float)steps_per_cm);  //bring accel back up high 
      
  while(digitalRead(switch0)){  //keep moving until switch is triggered (goes low when triggered) OR we've gotten to the desired position
    stepper.runSpeedToPosition();
  }
  stepper.setCurrentPosition(stepper.currentPosition()); //zeros the position at which switch is triggered
  
  //once triggered, with the same slow vel, move forward 5cm
  stepper.moveTo(stepper.currentPosition() - 5*steps_per_cm);
  stepper.setSpeed(-5*steps_per_cm);
  //stepper.runToNewPosition((5) * steps_per_cm);
  while(stepper.distanceToGo() < 0){  //keep moving until switch is triggered (goes low when triggered) OR we've gotten to the desired position
    stepper.run();
  }
 
  //print: "HOMING COMPLETE" and end the loop
  Serial.println("HOMING COMPLETE");

  Serial.println("END OF LOOP");

}
