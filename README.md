# EngineForAnimationCourse
Graphic Engine based on Libigl

For compiling:
1. Clone or download the project
2. Download Cmake from the link in the site
3. Run Cmake GUI, choose the project folder and destination folder for the cpp project files, press configure choose compiler (VS2019 for example). After configuration finished successfully press configure again and then press generate. 
4. If everything passes successfully click the "launch project" button or go to the build folder and launch the project from there. 

Notes for building and running with CLion:
1. Let CMake pick the generator (do not use Ninja generator!).
2. To run the demo edit Demo_bin configuration and put $CMakeCurrentGenerationDir$ in the working directory.
3. To share the build directory with VS choose build directory to be "build"
