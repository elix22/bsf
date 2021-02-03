# Compiling dependencies manually

Below you will find a list of dependencies that bs::framework relies on, as well as links to their source code and/or binaries. If a dependency isn't provided in binary form you will have to manually compile it (this is the case for the large majority or them). Make sure to compile the exact version of the dependency listed below. Newer versions *might* work, but haven't been tested. 

Once you have the dependency development files (headers and binaries) you will need to make sure they placed in the appropriate folders so bs::framework can find them during the build process. 

The dependencies are searched for in these locations:
- The `/Dependency` folder within bs::f's source. See below for the exact breakdown of how this folder is supposed to look. Usually you want to put all your dependencies here.
- If dependency cannot be found in the `/Dependency` folder, its default install path is searched for instead. For example `usr/local` on Linux/macOS or default install path if the dependency comes with an installer. 

Note that on Windows most dependencies do not have default install paths and should therefore be placed in the `/Dependency` folder. In order to avoid problems with dependency versions this should be the preferred behaviour on Linux/macOS as well. 

`/Dependency` folder breakdown:
- Static & shared libraries (.lib, .a, .so): 
  - Pick one of:
    - (bsfSource)/Dependencies/(DepName)/lib
	- (bsfSource)/Dependencies/(DepName)/lib/(Platform)
	- (bsfSource)/Dependencies/(DepName)/lib/(Platform)/(Configuration)
	- (bsfSource)/Dependencies/(DepName)/lib/(Configuration)
- Dynamic libraries (.dll, .dylib)
  - Place in (bsfSource)/bin/(Platform)/(Configuration)
- Includes
  - Place in (bsfSource)/Dependencies/(DepName)/include
- Tools (executables)
  - Place in (bsfSource)/Dependencies/tools/(DepName)  
  
Legend:
- (bsfSource) - root directory of bs::framework
- (DepName) - name of the dependency (title of each dependency shown below)
- (Platform) - x86 for 32-bit builds, x64 for 64-bit builds
- (Configuration) - Debug, RelWithDebInfo, MinSizeRel, or Release  
  
Each library is accompanied by a Find***.cmake CMake module that is responsible for finding the library. These modules are located under `Source/CMake/Modules`. They follow the rules described above, but if you are unsure where library outputs should be placed you can look at the source code for those modules to find their search paths.
   
Additionally, if the dependency structure still isn't clear, download one of the pre-compiled dependency packages to see an example.  
      
## Library list 
	  
**snappy**
- Google's Snappy compressor/decompressor
- https://github.com/BearishSun/snappy
- Required by bsfUtility
- Compile as a static library
	  
**nvtt**
- NVIDIA Texture Tools 2.1.0
- https://github.com/BearishSun/nvidia-texture-tools
- Required by bsfCore
- Compile as a static library
 
**LibICU**
- http://site.icu-project.org/
- Only required for Linux builds
- Required by bsfCore
- Compile as a static library
 - Make sure to provide `-fPIC` as an explicit flag
 - You *can* use system version of the ICU library, but your binaries will then only be compatible with Linux distros using the exact same API version (which changes often). It's safest to link ICU statically.

**FBXSDK**
- FBX SDK 2016.1
- http://usa.autodesk.com/fbx
- Required by bsfFBXImporter
- No compilation required, libraries are provided pre-compiled
 
**freetype**
- Freetype 2.3.5
- https://github.com/BearishSun/freetype (branch *banshee*)
- Required by bsfFontImporter
- Compile as a static library
   
**freeimg**
- FreeImage 3.17
- http://freeimage.sourceforge.net
- **macOS only**
  - Make sure to to build with `./configure —-without-zlib`
- Required by bsfFreeImgImporter
- Compile as a static library
      
**PhysX**
- PhysX 3.3
- https://github.com/NVIDIAGameWorks/PhysX-3.3
- Required by bsfPhysX
- Compile as a dynamic library on Windows, static library on Linux/macOS (default)
	
**OpenAL**
- OpenAL Soft 1.17.2
- https://github.com/kcat/openal-soft
- Required by bsfOpenAudio
- **Linux only**
  - Make sure to get audio backend libraries before compiling: PulseAudio, OSS, ALSA and JACK
  - On Debian/Ubuntu run: `apt-get install libpulse libasound2-dev libjack-dev`
- Compile as a dynamic library on Windows/Linux (default), static library on macOS
  - Use `LIBTYPE=STATIC` CMake flag to force it to compile as a static library
   
**libogg**
- libogg v1.3.2
- https://xiph.org/downloads/
- Required by bsfOpenAudio and bsfFMOD
- Compile as a static library
  - Switch runtime library to dynamic to avoid linker warnings when adding it to bs::f
  - This is also required when compiling libvorbis and libflac (below). See readme files included with those libraries.
  
**libvorbis**
- libvorbis commit:8a8f8589e19c5016f6548d877a8fda231fce4f93
- https://git.xiph.org/?p=vorbis.git
- Required by bsfOpenAudio and bsfFMOD
- Compile as a dynamic library on Windows, static library on Linux/macOS (default)
  - Requires libogg, as described in its readme file.
  - When compiling as static library on Linux, make sure to specify `-DCMAKE_POSITION_INDEPENDENT_CODE=ON` to CMake, otherwise it will fail to link
   
**libFLAC**
- libflac commit: f7cd466c24fb5d1966943f3ea36a1f4a37858597
- https://git.xiph.org/?p=flac.git
- Required by bsfOpenAudio
- Compile as a dynamic library on Windows (default), static library on Linux/macOS
  - Provide `--disable-shared --enable-static` flags to `configure` to force it to compile as a static library
  - Requires libogg, as described in its readme file.
   
**glslang**
- glslang commit: 377bccb143941ec4931e6aed9ac07752ccefb979
- https://github.com/KhronosGroup/glslang
- Required by bsfVulkanRenderAPI
- Compile as a static library

**SPIR-V Cross** (macOS only)
- Commit ID: 3cd89c0c0a2436f6ae8050a830243bf3536e4f4a
- https://github.com/KhronosGroup/SPIRV-Cross
- Required by bsfVulkanRenderAPI
- Compile as a static library

**MoltenVK** (macOS only)
- Commit ID: a684b47baab834e12da2af9f5997c867c4265b46
- https://github.com/KhronosGroup/MoltenVK
- Required by bsfVulkanRenderAPI
- Compile and install, then copy contents of `macOS/static/` folder into `lib` sub-folder
   
**XShaderCompiler**
- https://github.com/BearishSun/XShaderCompiler (branch *banshee*)
- Required by bsfSL
- Compile as a static library

**mono**
- Mono 5.4
- Only required if SCRIPT_API=C# option is specified during the build (i.e. C# scripting is enabled)
- http://www.mono-project.com/
- Required by bsfMono
- Compile as a dynamic library
 - See MonoIntegrationGuide.txt for additional notes

**bison**
- Bison 3.0.4
- Only required if BUILD_BSL option is specified during the build (off by default)
- **Windows**
  - http://sourceforge.net/projects/winflexbison/files/
- **Linux**
  - Debian/Ubuntu: `apt-get install bison`
  - Or equivalent package for your distribution
- **macOS**
  - `brew install bison`
  - Make sure old version of Bison that comes with Xcode is overriden:
    - Add this to $HOME/.bash_profile: `export PATH="/usr/local/opt/bison/bin:$PATH"`
    - `mv /usr/bin/bison /usr/bin/bison-2.3`
- Required by bsfSL
- Executable (tool)
 
**flex**
- Flex 2.6.1
- Only required if BUILD_BSL option is specified during the build (off by default)
- **Windows**
  - http://sourceforge.net/projects/winflexbison/files/
- **Linux**
  - Debian/Ubuntu: `apt-get install flex`
  - Or equivalent package for your distribution
- **macOS**
  - `brew install flex`
- Required by bsfSL
- Executable (tool)

 **BansheeSBGen**
 - Banshee Script Binding Generator 1.0
 - Only required if SCRIPT_BINDING_GENERATION option is specified during the build (off by default)
 - https://github.com/BearishSun/BansheeSBGen
 - Required for generation of C# script binding files. Not required if not using the scripting sub-system.
 - Executable (tool)
