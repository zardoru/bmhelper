# Be-Music Helper (wyrmin)
This is a fork of [exclusion's](https://twitter.com/excln/) Be-Music Helper 
(found [here](https://excln.github.io/bmhelper.html)), 
with his permission. 
It is a tool to make BMS creation easier by creating MIDI files that reduce the number of keysounds needed and automatically arrange BMS data for slicing and inserting into an editor like BMSE or [uBMSC](https://github.com/zardoru/iBMSC/).
 

## Changes from beta 4
* i18n support for the following languages:
    * English
    * Spanish
* CMake support
* wxWidgets 3.x support
* Fixed zz_enabled not being saved to bmh files (breaks older versions' forwards compatibility)
* More modern C++ (17)