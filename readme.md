Requirements:

<PID_v1.h>
<PID_AutoTune_v0.h>


TODO:

- install mechanical battery isolator
- instead of sensor objects, use sensor pointers and construct with parameters
- make safety class mixin for sensors and eventually system class
- make system class
- rewrite enums as inheritence classes with string functions
- move enums into class definitions http://stackoverflow.com/questions/8703743/using-enum-says-invalid-conversion-from-int-to-type
- enums as integers, cast?
- implement status nodes length as static class variable, use to initialize statusTable 
- make separate status class

DONE:

- integer declaration in for loop
- write functions to get sensor val string in sensors: snprintSensorVal
- implement arm coefficient based on how far current is above reg and below max
- change field voltage based on battery voltage
- Add data logging mode, outputs as csv and reduces debug print period
- smooth current sensor voltage