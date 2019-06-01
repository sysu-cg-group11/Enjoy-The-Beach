//
// Created by Miguel Chan on 2019-05-23.
//

#ifndef BEZIER_BEZIER_H
#define BEZIER_BEZIER_H

#include <glm/glm.hpp>
#include <vector>

class Bezier {
public:
    explicit Bezier(std::vector<glm::vec3> points);

    Bezier();

    void addPoint(glm::vec3 point);
    void removePoint();

    // The control points
    std::vector<glm::vec3> points;

    static double bernstein(unsigned int i, unsigned int n, float t);

    // Returns all points in the curve to draw
    std::vector<glm::vec3> getCurve(int steps);

    std::vector<glm::vec3> getAnimationLines();

    void animate();

    bool animating = false;
    double startTimeValue = 0;
    double animationTime = 2;

};


#endif //BEZIER_BEZIER_H
