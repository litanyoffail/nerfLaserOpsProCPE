# nerfLaserOpsProCPE

Arduino code for Adafruit Circuit Playground Express to expand Nerf Laser Ops Pro/Classic blaster Out-of-Box gameplay without modifying the blasters.

This code is for a base or single target with hit points for players to destroy during games, which encourages peeking out from cover.


**Features:**

  -supports AlphaPoint/DeltaBurst/Ion Laser Ops Pro/Classic blasters in Out-of-Box Play Mode

  -Can set the team who controls the base/target using team switch button

  -Base/target will only accept hits from the appropriate team according to stock behaviour (Purple to purple, or Red to Blue/Blue to Red)

  -10/20/30 Hit Points with mode switch to cycle between the values

  -Animation plays when base/target has 0 Hit Points and waits to be reset with either button


**Future changes/adds:**

  -use the integrated IR emitter to shoot back

  -create new game modes using the switch and inputs from extra pins from buttons or multiple IR receivers
