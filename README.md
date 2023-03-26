<div align="center">
<h3 align="center">Monitor Input Switch</h3>

  <p align="center">
    Powerful c++ script to change current monitor input programmatically.
    <br />
  </p>
</div>

<!-- GETTING STARTED -->
## Getting Started

<b>Do this at your own risk.</b> 
Not every monitor will react well to programmatically changing the current port. 
First of all, check out the possible problems depending on your monitor!

### Prerequisites

This is an example of how to list things you need to use the software and how to install them.
* cmake >= 3.23. https://cmake.org/download/

### Building

1. Clone a repository
2. Initialize build directory
   ```sh
   mkdir release && cd $_
   cmake.exe -DCMAKE_BUILD_TYPE=Release ../
   ```
3. Build project from root
   ```sh
   cmake.exe --build ./release --target monitor_input_switch -j 16
   ```


## Usage

`switch[.exe] --monitor <ID> --code <CODE>`

<p>
    <code>ID</code> - is a monitor code that can be retrieved from <code>switch[.exe] --list</code> command;
    <br/>
    <code>CODE</code> - is a hex code of your monitor input. table of possible values is given below:
</p>

The possible value is in range from 0 to 12 (in hex). if you don't know the desired value, you can always enumerate through all.

- 01 - D-SUB/VGA;
- 03 - DVI;
- 04 - HDMI;
- 11 - HDMI;
- 0F - DISPLAY PORT.


`switch[.exe] --list (displays all monitors whose can be set)`

`switch[.exe] --help`
