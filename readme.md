## MDLJazz
MDLJazz is just a test program I made to get comfortable with the OpenGL basics.

Someone asked me for the code though so now that leaves us with MDLJazz on github. Enjoy!

## Compilation

Make sure you have CMake and A C compiler installed (and git to clone the repo I guess).
This should work on all platforms provided you have an even semi-modern GPU.

open the terminal or powershell to whatever folder you do development stuff in

Clone the repo with: `git clone https://github.com/jarcheroelund/MDLJazz.git`

Change into the repo directory with: `cd MDLJazz`

Create the build directory with: `mkdir build`

Run cmake in the project directory: `cmake ..`

Build with `make`

The executable should be in `build/src`

## Running

MDLJazz accepts two command line arguments one for the `.pak` path and one for the `.mdl` path within the pak archive

The Quake Shareware pak `PAK0.PAK` resides within the `assets` folder in the root of the repo.

As an exaple run (from the `build` directory) `src/mdljazz ../assets/PAK0.PAK progs/ogre.mdl` on linux and `src/mdljazz.exe ../assets/PAK0.PAK progs/ogre.mdl` on windows. you should get this result:

![](/assets/output-sample.png)
