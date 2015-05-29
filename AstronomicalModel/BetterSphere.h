//
//  BetterSphere.h
//  WGA visualization
//
//  Created by Matthew McGuire on 4/30/15.
//  Copyright (c) 2015 Matthew McGuire. All rights reserved.
//


#ifndef WGA_visualization_BetterSphere_h
#define WGA_visualization_BetterSphere_h



/*---  (BEGIN) BetterSphere Class ---*/
struct sphereSpec
{
    int fans, bands;
    int numIndices;
    int numVertices;
    std::vector<point3> vertices;
    std::vector<unsigned int> indices;
    std::vector<point3> norms;
    std::vector<int> verticesCombinedForNorms;
    std::vector<point2> stMap;
};
class BetterSphere
{
private:
    int fans, bands;
    float radius;
    float thetaIncrem;
    float phiIncrem;
    void generateVertices(void);
    void generateIndices(void);
    void generateTextureMapsCoords(void);
    void generateNorms(void);
    void addNorm(vec3, int);
    void checkParams(int, int);
public:
    BetterSphere(int, int, float);
    sphereSpec theSphere;
        void changeSpec(int, int);
    int getBands(void);
    int getFans(void);
};
void BetterSphere::generateVertices(void)
{
    std::vector<point3>::iterator vertIter;
    vertIter = theSphere.vertices.begin();
    theSphere.vertices.insert(vertIter,point3(0.0f,radius,0.0f)); // top-of-sphere vertex
    vertIter++;
    for (int j = 1; j<bands; j++) {  // sequence of middle bands
        for (int i = 0; i<fans; i++) {
            theSphere.vertices.insert(vertIter,euclidSpherical(radius, float(i)*thetaIncrem, float(j)*phiIncrem));
            vertIter++;
        }
    }
    theSphere.vertices.insert(vertIter,point3(0.0f,-radius,0.0f));// bottom-of-sphere vertex
    // finished generating vertices for the sphere surface primitives
}
void BetterSphere::generateTextureMapsCoords(void)
{
    std::vector<point2>::iterator stIter;
    stIter = theSphere.stMap.begin();
    theSphere.stMap.insert(stIter,vec2(0.5,1.0));   // the equirectangular projection begins here
    stIter++;
    for (int j = 1; j<bands; j++) {  // sequence of middle bands
        for (int i = 0; i<fans; i++) {
            theSphere.stMap.insert(stIter,vec2((float(i)*thetaIncrem)/(2*M_PI),1.0-(float(j)*phiIncrem/M_PI)));
            stIter++;
        }
    }
    theSphere.stMap.insert(stIter,vec2(0.5,0.0));  // the equirectangular projection ends here
}
BetterSphere::BetterSphere(int inputFans, int inputBands, float inputRadius)
{
    fans = inputFans;
    bands = inputBands;
    radius = inputRadius;
    // establish parameters and memory reservations needed
    theSphere.fans = inputFans;
    theSphere.bands = inputBands;
    theSphere.numIndices=2*(fans*bands-fans+bands);
    theSphere.numVertices= fans*(bands-1)+2;
    theSphere.vertices.reserve(theSphere.numVertices);
    theSphere.indices.reserve(theSphere.numIndices);
    theSphere.norms.reserve(theSphere.numVertices);
    theSphere.verticesCombinedForNorms.reserve(theSphere.numVertices);
    theSphere.stMap.reserve(theSphere.numVertices);
    thetaIncrem = 2.0*M_PI/fans;     // how much to increment Theta when traversing
    phiIncrem = M_PI/bands;          // how much to increment Phi when traversing
    
    checkParams(fans, bands);
    generateVertices();
    generateTextureMapsCoords();
    generateIndices();
    generateNorms();
};
void BetterSphere::checkParams(int fans, int bands)
{
    if (bands <3 || fans < 4)
    {
        std::cerr << "that's not a proper design for a sphere!" << std::endl;
        exit(1);
    }
    std::cout << "Better Sphere:" << std::endl;
    std::cout << "Model: " << bands << " horizontal strips and "
    << fans << " vertical strips." << std::endl;
    std::cout << "Allocates: verts=" << theSphere.numVertices
    << " indices:" << theSphere.numIndices << std::endl;
}
void BetterSphere::generateIndices()
{
    std::vector<unsigned int>::iterator indexIter;
    indexIter = theSphere.indices.begin();
    
    int indexA = 0;
    int indexB = indexA + fans;
    for (int i=0; i<=fans; i++) {    // first the top fan
        theSphere.indices.insert(indexIter,i);
        indexIter++;
    }
    theSphere.indices.insert(indexIter,1);     // to close the triangle fan
    indexIter++;
    for (int j = 2; j < bands ; j++) { // then the bands of trianglestrips
        for(int i = 1; i <= fans; i++) {
            theSphere.indices.insert(indexIter, indexA+i);
            indexIter++;
            theSphere.indices.insert(indexIter,indexB+i);
            indexIter++;
        }
        theSphere.indices.insert(indexIter,indexA+1);  // knit the triangle strip to
        indexIter++;
        theSphere.indices.insert(indexIter,indexB+1);  // its first two vertices
        indexIter++;
        indexA += fans;
        indexB = indexA + fans;
    }
    for (int i=0; i<=fans; i++) {   // finally the bottom fan
        theSphere.indices.insert(indexIter,theSphere.numVertices-1-i);
        indexIter++;
    }
    theSphere.indices.insert(indexIter,theSphere.numVertices-2);     // to close the triangle fan
    // finished with the index array
    
}
void BetterSphere::generateNorms()
{
    // THIRD: the array of vertex normal vectors
    theSphere.verticesCombinedForNorms.insert(theSphere.verticesCombinedForNorms.begin(), theSphere.numVertices,0);
    theSphere.norms.insert(theSphere.norms.begin(), theSphere.numVertices, point3(0.0,0.0,0.0));
    
    // obtain norms for fanned triangles
    vec3 triangleNorm;
    point3 a,b,c,d;
    int vertIter = 0;
    a = theSphere.vertices[theSphere.indices[vertIter]];
    for (int i=1; i <= fans; i++) {
        b = theSphere.vertices[theSphere.indices[vertIter+i]];
        c = theSphere.vertices[theSphere.indices[vertIter+i+1]];
        triangleNorm = glm::normalize(glm::cross(b-a,c-a));
        addNorm(triangleNorm,theSphere.indices[vertIter]);
        addNorm(triangleNorm,theSphere.indices[vertIter+i]);
        addNorm(triangleNorm,theSphere.indices[vertIter+i+1]);
    }
    
    // obtain norms for strip triangles in each band
    vertIter = fans+2;
    for (int j = 2; j< bands; j++) {
        for (int i = 0; i<fans; i++) {
            a = theSphere.vertices[theSphere.indices[vertIter]];
            b = theSphere.vertices[theSphere.indices[vertIter+1]];
            c = theSphere.vertices[theSphere.indices[vertIter+2]];
            d = theSphere.vertices[theSphere.indices[vertIter+3]];
            triangleNorm = glm::normalize(glm::cross(b-a,c-a));
            addNorm(triangleNorm,theSphere.indices[vertIter]);
            addNorm(triangleNorm,theSphere.indices[vertIter+1]);
            addNorm(triangleNorm,theSphere.indices[vertIter+2]);
            triangleNorm = glm::normalize(glm::cross(c-d,b-d));
            addNorm(triangleNorm,theSphere.indices[vertIter+1]);
            addNorm(triangleNorm,theSphere.indices[vertIter+2]);
            addNorm(triangleNorm,theSphere.indices[vertIter+3]);
            vertIter +=2;
        }
        vertIter +=2;
    }
    // obtain norms for bottom fanned triangles
    a = theSphere.vertices[theSphere.indices[vertIter]];
    for (int i=0; i < fans; i++) {
        b = theSphere.vertices[theSphere.indices[vertIter+i+1]];
        c = theSphere.vertices[theSphere.indices[vertIter+i+2]];
        triangleNorm = glm::normalize(glm::cross(b-a,c-a));
        addNorm(triangleNorm,theSphere.indices[vertIter]);
        addNorm(triangleNorm,theSphere.indices[vertIter+i+1]);
        addNorm(triangleNorm,theSphere.indices[vertIter+i+2]);
    }
}
void BetterSphere::addNorm(vec3 newNorm, int vertexIndex)
{
    int soFar = theSphere.verticesCombinedForNorms[vertexIndex];
    theSphere.norms[vertexIndex] = (float(soFar) * theSphere.norms[vertexIndex] + newNorm)/float(soFar+1.0);
    theSphere.verticesCombinedForNorms[vertexIndex]++;
}
/*---  (END) BetterSphere Class ---*/
#endif
