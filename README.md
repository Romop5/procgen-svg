# ProcGen for SVG generation
An example of using *ProcGen* library for SVG images.

## Examples
<p align="center">

<img src="https://github.com/Romop5/procgen-svg/blob/master/build/bullet.svg.png?raw=true" width="300px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/densetree.svg.png?raw=true" width="300px">

<img src="https://github.com/Romop5/procgen-svg/blob/master/build/ker-10angle.svg.png?raw=true" width="300px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/newhash.svg.png?raw=true" width="300px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/nicehash.svg.png?raw=true" width="300px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/output.svg.png?raw=true" width="300px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/strom-30-06.svg.png?raw=true" width="300px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/strom-sixedge.svg.png?raw=true" width="300px">

</p>

## Turtle graphics
<p align="center">

<img src="https://github.com/Romop5/procgen-svg/blob/master/build/turtle.snowflake.png?raw=true" width="300px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/turtle.labyrint.procgen.png?raw=true" width="300px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/turtle.classic.procgen.png?raw=true" width="300px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/turtle.tree.procgen.png?raw=true" width="300px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/turtle.tree2.procgen.png?raw=true" width="300px">
<img src="https://github.com/Romop5/procgen-svg/blob/master/build/turtle.tree3.procgen.png?raw=true" width="300px">

</p>



## Usage

1. Write down script using ProcGen language and primitives defined in `svg.procgen`. 
2. Run `./svgprocgen <scriptName> <pictureName.svg> [<JSONparameterLiteral]` to get SVG picture.
3. Use *convert* from *ImageTricks* to convert SVG to PNG.

Note: you can change parameter's value from command-like.
For example, if you define ```parameter int iterations = 1;``` in script test.procgen,
then you can call render with overwritten value like this:

`./svgprocgen "test.procgen" "test.svg" '{"iterations": 10}'


