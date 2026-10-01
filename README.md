# Training - Design Patterns in C++ #

## Documentation + slides

* https://cpp-dp.infotraining.pl
* https://cpp-dp.infotraining.pl/slides

## Environment setup

Please choose one of the options below:

### Local

Before the training, please install:

#### A C++ compiler supporting C++17 - choose one:
  * Visual Studio 2022
    * during installation, select the following options:
      * Desktop development with C++
      * C++ CMake tools for Windows
      * vcpkg package manager

  * GCC - Linux or WSL
    * gcc (version >= 12)
    * [CMake > 3.25](https://cmake.org/)
      * please check the version on the command line        
  
        ```
        cmake --version
        ```
    * vcpkg
      * installation - https://learn.microsoft.com/en-us/vcpkg/get_started/get-started?pivots=shell-bash
        * clone the vcpkg repository
          ```
          git clone https://github.com/microsoft/vcpkg.git
          ```
        * run the bootstrap-vcpkg.sh script
          ```
          cd vcpkg && ./bootstrap-vcpkg.sh
          ``` 
        * add the VCPKG_ROOT environment variable
          * add the following entry to your `.bashrc` file
          ```
          export VCPKG_ROOT=/path/to/vcpkg
          export PATH=$VCPKG_ROOT:$PATH
          ```
    * IDE: Visual Studio Code
      * [Visual Studio Code](https://code.visualstudio.com/)
      * install the extensions
        * C/C++ Extension Pack
        * Live Share

### Docker + Visual Studio Code

If training participants use Docker at work, please install:

#### Docker Desktop (Windows)

* https://www.docker.com/products/docker-desktop/

#### Visual Studio Code

* [Visual Studio Code](https://code.visualstudio.com/)
* Install the extensions
  * Live Share
  * Dev Containers ([requirements](https://code.visualstudio.com/docs/devcontainers/containers#_system-requirements))
    * after installing the extension - open the folder containing the cloned repository in VS Code and
      from the command palette (Ctrl+Shift+P) select **Dev Containers: Rebuild and Reopen in Container**