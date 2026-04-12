# SSSpace
A 3D game about space exploration and ship building.
## Main content
#### Building custom ships: 
Ships are built from grid aligned blocks, each ship can contain multiple grids with different orientations. 
#### Space exploration: 
The universe is divided into equally parts (sectors) which are procedurally generated when needed, allowing 1 to 1 scale.
#### Multiplayer: 
Each player can have their own fleet.
### Future planned content
Controlling your ships with Lua scripts.
Parallel processing of different sectors.
## Technical details
Libraries used: Vulkan, SDL3, libuv

#### Graphics:
Currently main renderer uses double buffering.
Instanced rendering is used for most objects.

#### Game Logic:
Global positions are double precision, allowing large distances.
Spatial partitioning is done via spatial hashing.


## Building and Running

`make` & `./app`
Or
`make run`

##### WIP Notes:
Currently throws an error at shutdown.
Test controls: Up&Down arrow, T key