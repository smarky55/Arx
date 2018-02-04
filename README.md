# Arx
Arx is a C++ library that provides the ability to load resources from a bundled file. It is primarily intended to be used in applications distributed with a number of files that do not to (or should not) be visible to the end user. Key examples of this practice are the packaging methods used in the distribution of games.

## Features
* Package resources in a single file.
* Use a directory structure to organise your packaged files.
* Load resources from the bundle.

## Installation
### Requirements
* CMake
* Boost

Building Arx requires [CMake](cmake.org) to generate build files, please see https://cmake.org/install/ for detailed instructions for installing on your platform.

The system for building arx packages make use of Boost::Spirit to parse file manifests. Generating buld files requires the `BOOST_ROOT` environment variable to be set to the root directory of your boost installation.

The (deprecated) python packaging script requires:

* python 2.7
* pyparsing (>2.2)

Python 2.7 is available from https://www.python.org/.
PyParsing can then be installed using pip:
```
pip install pyparsing
```

### Building on Windows
To generate Visual Studio build files:
```
mkdir build
cd build
cmake ..
```
This will generate the solution file. You can then either build the solution using Visual Studio or following on from above run
```
cmake --build .
```
to compile the library.

#### Building on Unix
The library can be built by
```
mkdir build && cd build
cmake ..
make
```

## Usage
### Building an archive
#### The Manifest
The first stage of building an arx archive file is compiling a manifest file to list all the files that should be included by the packager.

The manifest file contains a list of the file paths for each file, with one file path per line, optionally contained within a folder tree. An example of the layout of the manifest file can be seen below, all folder names are examples.

The packaging process takes the file paths given in the manifest and strips the directory information to save the base name of the file (eg `file.txt` from `path/to/file.txt`). For this reason it is important that any files within a folder in the manifest have unique base names, even those in different directories.

```
file1.txt
file2.txt
subfolder{
    file3.txt
    sub_sub_folder{
        relative/path/to/image1.jpg
        /full/unix/style/path/to/image2.png
    }
}
```

#### Packaging
To package a set of files from the command line an executable is provided.
```
Windows:
packager.exe path/to/manifest.txt path/to/output.arx

Unix
packager path/to/manifest.txt path/to/output.arx
```
This should produce an arx file at the desired location ready to be loaded.

Alternatively the python packager can be used with similar syntax
```
python packager.py path/to/manifest.txt path/to/output.arx
```

### Reading from an archive
To load a file from an arx package the `Arx::ArxLoader` class is used. The arx file path is passed to the constructor and the package is loaded. The `ArxLoader` class provides two methods for retrieving a file from the package:
* `void getFile(const char * path, char * &data, unsigned &size)`
    * Loads the file into a `char *` buffer of length `size`
    * It is the user's responsibility the `delete` the buffer once finished.
* `void getStream(const char * path, std::istream * &stream)`
    * Loads the file into a string stream for file stream like usage

The format for the file path within the arx package uses colons (`:`) as separators.

#### Examples
These examples assume the above example manifest was used to build the file `package.arx`.
```
#include "ArxLoader.h"

...

Arx::ArxLoader loader("package.arx");

// Loading file to char * buffer
char * buffer;
unsigned bufferSize;
loader.getFile("subfolder:file3.txt", buffer, bufferSize);

...

delete buffer;

```
## Licence
Arx is available under the terms of the Mozilla Public License, v. 2.0. please see [LICENSE](LICENSE) for further details. If the licence file is unavailable for any reason, a full copy of the licence can be obtained from http://mozilla.org/MPL/2.0/.
