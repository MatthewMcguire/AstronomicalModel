## Astronomical object model

#### The purpose is to create a simple model of astronomical objects. It is intended for visual appreciation.

**Method**

- Create several components that are as independent and reusable as possible.
- Execute the project in C++ and OpenGL (v. 3.3 or so, possibly as low as 3.0)
- Have as few library dependencies as possible. Ideally, just GLFW 3.0+ and free header-only libraries such as GLM and STB image

**Things I will do**

- Make it sufficiently general that additional similar objects can be added (e.g. moons)
- Apply the best textures I can find to give some indication of what is what (e.g. Luna has its craters)
- Apply a lighting model of reasonable utility, so that the sun casts shadows pretty well
- Allow the user to position the view at only location and orientation
- Offer a 'reset' button to reorient the user when lost.
- Change the speed of the simulation
- Show the speed and current time of the simulation

**Things I'd like to do**

- Develop more accurate motion: e.g. ellipses rather than circles
- Model the objects with more accuracy: e.g. not perfect spheres
- Create a realistic 'starting point' instead of putting them all on the x-axis at t=0 like racehorses'
- Provide for the shadows that objects cast on *each other*, to show eclipses
- Show a little 'popup' guide to indicate 'birds eye' where the user is in the solar system. Quick orientation.
- Port this to JavaScript and WebGL

**Things I have no intention of doing**

- Making the distances or sizes realistic in proportion
- Taking into account gravitational interation between objects that are not in orbit around each other


*Notes*

Need a constant panel to show:

- current scale factor
- current speed
- 'hours' elasped

*Assets*

- uniformLocation[0] : program[0]  "objectTransform"
- uniformLocation[1] : program[0]  "sample01"

- uBlockIndex[0]     : program[0]  "camera"

- attribLocation[0]  : program[0]  "vPosition"
- attribLocation[1]  : program[0]  "vNormal"
- attribLocation[2]  : program[0]  "textureSTMap"

- shaderBuffer[0]    : [sphere]    GL_ELEMENT_ARRAY_BUFFER (vertex indices)
- shaderBuffer[1]    : [sphere]    GL_ARRAY_BUFFER  (vertices)
- shaderBuffer[2]    : [sphere]    GL_ARRAY_BUFFER  (st texture map)
- shaderBuffer[3]    : [camera]    GL_ARRAY_BUFFER  (camera uniform block)

- textureName[0]     : GL_TEXTURE0 LunaVenus.gif

