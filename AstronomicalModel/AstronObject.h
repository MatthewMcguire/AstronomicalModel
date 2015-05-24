//
//  AstronObject.h
//  AstronomicalModel
//
//  Created by Matthew McGuire on 5/22/15.
//  Copyright (c) 2015 Matthew McGuire. All rights reserved.
//

#ifndef AstronomicalModel_AstronObject_h
#define AstronomicalModel_AstronObject_h

#include <vector>

class AstroObject
{
private:
    std::string name;           // a simple name for the object
    float radius;               // radius (in world-space units)
    float tiltAngle;            // tilt of the object from pos-y axis (radians)
    float rotSpeed;             // how fast it rotates on its axis (units relative to earth)
    float orbitRadius;          // radius of its orbit (units relative to earth)
    float orbitSpeed;           // how fast it orbits (units relative to earth)

    glm::vec3 rotateAxis;       // the axis of rotational motion
    glm::vec3 orbitAxis;        // the axis of orbital motion
    glm::mat4 relLocation;      // current matrix of transformation relative to parent
    glm::mat4 relOrientScale;     // current matrix of transformation relative to parent
    // texture                  // not sure yet how to specify the texture
    // material                 // this will be an instance of a future material specification class
public:
    AstroObject(std::string, float, float, float, float, float );
    float currentRotAngle;      // current rotation angle (radians, clamped to 0 to 2pi)
    float currentOrbitAngle;    // current orbit angle (radians, clamped to 0 to 2pi)
    glm::vec3 currentLocation;  // current location (in world-space coordinates)
    glm::vec3 currentVelocity;  // approximate velocity vector (in world-space coordinates
    void incremObject(float);   // this function increments the object in its orbit and rotation
                                //      (1 unit = 10 earth minutes)
};

class AstroGroup
{
private:
    std::vector<AstroObject> montum;        // a collection of astronomical objects
    std::vector<glm::mat4> absTransform;    // a collation of matrices to apply to each basic sphere
public:
    void updateMontum(float);               // traverse the objects and increment them all
    void drawMontum(void);                  // draw the objects as instances of a sphere
};

/*---  Constructor: creates an astronomical object instance             ---*/
AstroObject::AstroObject(std::string initName,
                         float initRadius,
                         float initTiltAngle,
                         float initRotSpeed,
                         float initOrbitRadius,
                         float initOrbitSpeed)
{
    name = initName;
    radius = initRadius;
    tiltAngle = initTiltAngle;
    rotSpeed = initRotSpeed;
    orbitRadius = initOrbitRadius;
    orbitSpeed = initOrbitSpeed;
    currentRotAngle = 0.0;
    currentOrbitAngle = 0.0;
    currentLocation = glm::vec3(orbitRadius,0.0,0.0);
    currentVelocity = glm::vec3(0.0);
    relLocation = glm::translate(glm::mat4(1.0f),currentLocation);  // for now, initial loc is on pos x-Axis
    relOrientScale = glm::scale(glm::mat4(1.0f),glm::vec3(radius,radius,radius));
    rotateAxis = glm::vec3(0.0,1.0,0.0);
    orbitAxis = glm::vec3(0.0,1.0,0.0);
}

/*---  This function increments the object in its orbit and rotation    ---*/
void AstroObject::incremObject(float inc)
{
    // turn the 'inc' variable into a rotational and orbital increment
    
    // update the relLocation matrix by rotating around the orbital axis
    // update the relOrientScale matrix by rotating around the rotational axis
    // update currentRotAngle
    // update currentOrbitAngle
    // update currentLocation
    // update currentVelocity

}

#endif
