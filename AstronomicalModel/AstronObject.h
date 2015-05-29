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

const float orbitPerInc = (M_PI*2.0)/(365.25*24.0*60.0);
const float rotPerInc = (M_PI*2.0)/(24.0*60.0);
const float twoPi = 2.0*M_PI;

class AstroObject
{
private:
    float radius;               // radius (in world-space units)
    float scaledRadius;         // radius scaled to make viewing easier
    float tiltAngle;            // tilt of the object from pos-y axis (radians)
    float rotSpeed;             // how fast it rotates on its axis (units relative to earth)
    float orbitRadius;          // radius of its orbit (units relative to earth)
    float scaledOrbitRadius;     // orbit radius scaled to make viewing easier
    float orbitSpeed;           // how fast it orbits (units relative to earth)
    glm::vec3 rotateAxis;       // the axis of rotational motion
    glm::vec3 orbitAxis;        // the axis of orbital motion
    // texture                  // not sure yet how to specify the texture
    // material                 // this will be an instance of a future material specification class
    float viewingScale(float);
    float scaleFactor;
public:
    AstroObject(std::string, float, float, float, float, float, float);
    std::string name;           // a simple name for the object
    float currentRotAngle;      // current rotation angle (radians, clamped to 0 to 2pi)
    float currentOrbitAngle;    // current orbit angle (radians, clamped to 0 to 2pi)
    glm::vec3 currentRelLocation;  // current location (in world-space coordinates)
    glm::vec3 currentRelVelocity;  // approximate velocity vector (in world-space coordinates
    glm::vec3 currentAbsLocation;
    glm::mat4 absLocationMatrix;      // current matrix of absolute location transformation in world coords
    glm::mat4 relLocation;      // current matrix of transformation relative to parent
    glm::mat4 modelScale;       // current matrix of scale transformation
    glm::mat4 modelOrientation; // current matrix of orientation transformation
    void incremObject(float);   // this function increments the object in its orbit and rotation
                                //      (1 unit = 10 earth minutes)
    AstroObject *leftmostChild; // pointer to the leftmost child of this object in the object tree
    AstroObject *rightSibling;  // pointer to the right sibling of this object in the object tree
    void report(float, float);  // print several parameters to stdout for error tracking
    void adjustScale(float);     // change the scale factor during run-time
    void updateAbsLocationModelOrientation(void); // done after the increment, and the updating of the absolute matrices.
};
float AstroObject::viewingScale(float value)
{
    return (pow(value, scaleFactor));
}
void AstroObject::adjustScale(float scaleFactorChange)
{
    scaleFactor +=scaleFactorChange;
    scaledRadius = viewingScale(radius);
    scaledOrbitRadius = viewingScale(orbitRadius);
    currentRelLocation = glm::vec3(scaledOrbitRadius,0.0,0.0);
    modelScale = glm::scale(glm::mat4(1.0f),glm::vec3(scaledRadius,scaledRadius,scaledRadius));
}

/*---  Constructor: creates an astronomical object instance             ---*/
AstroObject::AstroObject(std::string initName, float initRadius, float initTiltAngle,
                         float initRotSpeed, float initOrbitRadius, float initOrbitSpeed, float scaleFact)
{
    scaleFactor = scaleFact;
    name = initName;
    radius = initRadius;
    scaledRadius = viewingScale(radius);
    tiltAngle = initTiltAngle * DegreesToRadians;  
    rotSpeed = initRotSpeed;
    orbitRadius = initOrbitRadius;
    scaledOrbitRadius = viewingScale(orbitRadius);
    orbitSpeed = initOrbitSpeed;
    rotateAxis = euclidSpherical(1.0,twoPi/4.0,tiltAngle);
    orbitAxis = glm::vec3(0.0,1.0,0.0);
    currentRotAngle = 0.0;
    currentOrbitAngle = 0.0;
    currentRelLocation = glm::vec3(scaledOrbitRadius,0.0,0.0);
    currentAbsLocation = currentRelLocation;
    currentRelVelocity = glm::vec3(0.0);
    relLocation = glm::translate(glm::mat4(1.0f),currentRelLocation);  // initial loc is on pos x-Axis
    modelScale = glm::scale(glm::mat4(1.0f),glm::vec3(scaledRadius,scaledRadius,scaledRadius));
    modelOrientation = glm::rotate(glm::mat4(1.0f),tiltAngle,glm::vec3(0.0,0.0,1.0));
    leftmostChild = NULL;
    rightSibling = NULL;
    absLocationMatrix = glm::mat4(1.0);
}

/*---  This function increments the object in its orbit and rotation    ---*/
void AstroObject::incremObject(float inc)
{
    // turn the 'inc' variable into a rotational and orbital increment
    // a single unit of 'inc' is scaled to be one minute of earth time.
    float incOrbit = orbitPerInc * inc / orbitSpeed;
    float incRot = rotPerInc * inc / rotSpeed;

    // update currentOrbitAngle
    currentOrbitAngle += incOrbit;
    while (currentOrbitAngle > twoPi) currentOrbitAngle -= twoPi;       // keep angle in [0,2π]
    while (currentOrbitAngle < 0.0) currentOrbitAngle += twoPi;       // keep angle in [0,2π]

    // update currentRotAngle
    currentRotAngle += incRot;
    while (currentRotAngle > twoPi) currentRotAngle -= twoPi;       // keep angle in [0,2π]
    while (currentRotAngle < 0.0) currentRotAngle += twoPi;       // keep angle in [0,2π]

    // update the relLocation matrix by rotating around the orbital axis
    glm::mat4 preRotate = glm::rotate(matr4(1.0f),-1*currentOrbitAngle,orbitAxis);
    glm::mat4 transToOrbit = glm::translate(matr4(1.0f),glm::vec3(scaledOrbitRadius,0.0,0.0));
    relLocation = glm::rotate(matr4(1.0f),currentOrbitAngle,orbitAxis) * transToOrbit * preRotate;

    // update currentRelLocation
    glm::vec3 oldLocation=currentRelLocation;
    currentRelLocation = glm::vec3( relLocation * glm::vec4(0.0,0.0,0.0,1.0));

    // update currentRelVelocity
    currentRelVelocity= currentRelLocation - oldLocation;
}
void AstroObject::updateAbsLocationModelOrientation(void)
{
    // update currentAbsLocation
    currentAbsLocation = glm::vec3( absLocationMatrix * glm::vec4(0.0,0.0,0.0,1.0));    // where is the object?
    // update the modelScale matrix by rotating around the rotational axis
    modelOrientation = glm::translate(glm::mat4(1.0f),-currentAbsLocation);             // bring it to the origin
    modelOrientation = glm::rotate(glm::mat4(1.0f),currentRotAngle,orbitAxis) * modelOrientation;   // rotate it on its axis
    modelOrientation = glm::rotate(glm::mat4(1.0f),tiltAngle,glm::vec3(0.0,0.0,1.0)) * modelOrientation;    // tilt its axis
    modelOrientation= glm::translate(glm::mat4(1.0f),currentAbsLocation) * modelOrientation;    // put it back where it was
//    report(0.0,0.0);
}
void AstroObject::report(float incOrbit,float incRot)
{
    std::cout << "obect: " << name;
//    std::cout << "    incOrbit=" << incOrbit << " incRot=" << incRot << std::endl;
    std::cout << "    currentRelLocation = (" << currentRelLocation.x << "," << currentRelLocation.y
        << "," << currentRelLocation.z << ")\n";
    std::cout << "    currentAbsLocation = (" << currentAbsLocation.x << "," << currentAbsLocation.y
    << "," << currentAbsLocation.z << ")\n";
    std::cout << "    currentOrbitAngle= " << currentOrbitAngle << "\n";
}


class AstroGroup
{
private:
    float objectScaleFactor;
public:
    std::vector<AstroObject>::iterator iter;
    AstroGroup(float);                      // constructor that brings a custom scale factor
    void updateMontum(float);               // traverse the objects and increment them all
    void drawMontum(void);                  // draw the objects as instances of a sphere
    void traverseM(AstroObject& ,glm::mat4);    // traverse the tree of montum and assign absLocationMatrix
    BetterSphere s = BetterSphere(100,100,1.0);
    GLsizei numObjects;
    std::vector<AstroObject> montum;        // a collection of astronomical objects
    void adjustScale(float);                // change the scale factor during run-time
    float currentScaleFactor(void);         // reply with current scale factor for objects
};

float AstroGroup::currentScaleFactor(void)
{
    return objectScaleFactor;
}
void AstroGroup::adjustScale(float scaleFactorChange)
{
    for (int i =0;i < numObjects; i++)
        montum[i].adjustScale(scaleFactorChange);
    objectScaleFactor += scaleFactorChange;
}

AstroGroup::AstroGroup(float scaleFact)
{
    //                          initName initRadius initTiltAngle initRotSpeed initOrbitRadius initOrbitSpeed
    AstroObject sol = AstroObject(    "Sol",      1390000,    0.01,   26.0,   0.0,            9999.0, scaleFact);
    AstroObject mercury = AstroObject("Mercury",  4880.0,     0.133,  58.8,   57910000.0,     0.241, scaleFact);
    AstroObject venus = AstroObject(  "Venus",    12103.6,    177.4,  244.0,  108200000.0,    0.615, scaleFact);
    AstroObject earth = AstroObject(  "Earth",    12756.3,    23.4,   1.0,    149600000.0,    1.0, scaleFact);
    AstroObject luna = AstroObject(   "Luna",     3475.0,     6.7,    27.4,   238900.0,       0.0748, scaleFact);
    AstroObject mars = AstroObject(   "Mars",     6794.0,     25.2,   1.03,   227940000.0,    1.88, scaleFact);
    AstroObject phobos = AstroObject( "Phobos",   13.8,       0.0,    9999.0, 5287.0,         .0008738, scaleFact);
    AstroObject deimos = AstroObject( "Deimos",   7.8,        0.0,    9999.0, 14580.0,        0.003462, scaleFact);
    AstroObject jupiter = AstroObject("Jupiter",  142984.0,   3.1,    0.415,  778330000.0,    11.9, scaleFact);
    //                          initName initRadius initTiltAngle initRotSpeed initOrbitRadius initOrbitSpeed
    //    AstroObject sol = AstroObject("Sol",        13.90000,   0.01,   26.0,   0.0,    9999.0);
    //    AstroObject mercury = AstroObject("Mercury",.4880,      0.133,  58.8,   5.791,  0.241);
    //    AstroObject venus = AstroObject("Venus",    1.210,      177.4,  244.0,  10.8,   0.615);
    //    AstroObject earth = AstroObject("Earth",    1.2756,     23.4,   1.0,    14.,    1.0);
    //    AstroObject luna = AstroObject("Luna",      .3475,      6.7,    27.4,   2.38,   0.0748);
    //    AstroObject mars = AstroObject("Mars",      .6794,      25.2,   1.03,   22.79,  1.88);
    //    AstroObject phobos = AstroObject("Phobos",  .13,        0.0,    9999.0, 5,      .0008738);
    //    AstroObject deimos = AstroObject("Deimos",  .7,         0.0,    9999.0, 1.45,   0.003462);
    //    AstroObject jupiter = AstroObject("Jupiter",14.2984,    3.1,    0.415,  77,     11.9);
    
    montum.push_back(sol);
    montum.push_back(mercury);
    montum.push_back(venus);
    montum.push_back(earth);
    montum.push_back(luna);
    montum.push_back(mars);
    montum.push_back(phobos);
    montum.push_back(deimos);
    montum.push_back(jupiter);
    
    montum[0].leftmostChild = &montum[1];
    montum[1].rightSibling = &montum[2];
    montum[2].rightSibling= &montum[3];
    montum[3].leftmostChild = &montum[4];
    montum[3].rightSibling = &montum[5];
    montum[5].leftmostChild = &montum[6];
    montum[6].rightSibling = &montum[7];
    montum[5].rightSibling = &montum[8];
    
    numObjects = GLsizei(montum.size());
    objectScaleFactor = scaleFact;
}
void AstroGroup::updateMontum(float inc)
{
    std::vector<AstroObject>::iterator iterStop = montum.end();
    for(iter = montum.begin(); iter < iterStop; iter++)     // increment every object in the group
        (*iter).incremObject(inc);
//    traverseM(montum[0], montum[0].relLocation); // update every absLocationMatrix matrix with parents' location
    montum[0].absLocationMatrix = montum[0].relLocation;
    
    montum[1].absLocationMatrix = montum[0].absLocationMatrix * montum[1].relLocation;
    montum[2].absLocationMatrix = montum[0].absLocationMatrix * montum[2].relLocation;
    montum[3].absLocationMatrix = montum[0].absLocationMatrix * montum[3].relLocation;
    montum[5].absLocationMatrix = montum[0].absLocationMatrix * montum[5].relLocation;
    montum[8].absLocationMatrix = montum[0].absLocationMatrix * montum[8].relLocation;
    
    montum[4].absLocationMatrix = montum[3].absLocationMatrix * montum[4].relLocation;
    montum[6].absLocationMatrix = montum[5].absLocationMatrix * montum[6].relLocation;
    montum[7].absLocationMatrix = montum[5].absLocationMatrix * montum[7].relLocation;
    
    for(iter = montum.begin(); iter < iterStop; iter++)     // now, updated the absolute loc vector for all objects
        (*iter).updateAbsLocationModelOrientation();
}

void AstroGroup::traverseM(AstroObject& node,glm::mat4 m)
{
    node.absLocationMatrix =  node.relLocation * m;
    if (node.leftmostChild != NULL) traverseM(*(node.leftmostChild),node.absLocationMatrix);
    if (node.rightSibling!= NULL) traverseM(*(node.rightSibling),m);
}

// This function should only be called when the relevant shader buffers have been bound
void AstroGroup::drawMontum(void)
{
    glDrawElementsInstanced(GL_TRIANGLE_FAN,(s.theSphere.fans+2),GL_UNSIGNED_INT, (void*)(0 * sizeof(GLuint)), numObjects);
    for (int j = 0; j<(s.theSphere.bands-2); j++) {
        glDrawElementsInstanced(GL_TRIANGLE_STRIP,(2*s.theSphere.fans+2), GL_UNSIGNED_INT,
                       (void*)(((s.theSphere.fans+2)+j*(2*s.theSphere.fans+2)) * sizeof(GLuint)),numObjects);
    }
    glDrawElementsInstanced(GL_TRIANGLE_FAN,(s.theSphere.fans+2),GL_UNSIGNED_INT,
                   (void*)((s.theSphere.numIndices-s.theSphere.fans-2) * sizeof(GLuint)),numObjects);
}

#endif
