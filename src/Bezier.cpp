//
// Created by Miguel Chan on 2019-05-23.
//

#include "Bezier.h"
#include "Utils/Utils.h"
#include <cmath>
#include <iostream>
#include <GLFW/glfw3.h>

Bezier::Bezier(std::vector<glm::vec3> points) {
    this->points = std::move(points);

}


std::vector<glm::vec3> Bezier::getCurve(int steps) {
    float t_step = 1.0 / steps;
    int n = points.size() - 1;
    std::vector<glm::vec3> result;
    if (animating && glfwGetTime() - startTimeValue < animationTime) {
        steps = (glfwGetTime() - startTimeValue) / animationTime * steps;
    }
    // m+1 points for m step
    for (int k = 0; k <= steps; k++) {
        glm::vec3 current_point = {0, 0, 0};
        for (int i = 0; i <= n; i++) {
            current_point += points[i] * float(bernstein(i, n, t_step * k));
        }
        result.push_back(current_point);
    }
    return result;
}

int factorial(unsigned int n) {
    //cache factorial result
    static std::vector<int> cache = {1, 1, 2, 6};
    while (n >= cache.size()) {
        cache.push_back(cache[cache.size() - 1] * cache.size());
    }
    return cache[n];
}

double Bezier::bernstein(unsigned int i, unsigned int n, float t) {
    return double(factorial(n) / (factorial(i) * factorial(n - i))) * pow(t, i) * pow(1 - t, n - i);
}

void Bezier::addPoint(glm::vec3 point) {
    if (std::find(points.begin(), points.end(), point) == points.end()) {
        points.push_back(point);
    }
}

void Bezier::removePoint() {
    if (!points.empty()) {
        points.pop_back();
    }
}

void Bezier::animate() {
    startTimeValue = glfwGetTime();
    animating = true;
}

std::vector<glm::vec3> Bezier::getAnimationLines() {
    if (!animating) {
        return std::vector<glm::vec3>();
    }
    if (glfwGetTime() - startTimeValue > animationTime) {
        animating = false;
    }
    std::vector<std::pair<glm::vec3, glm::vec3>> toInterpolate;
    for (int i = 0; i < points.size() - 1; i++) {
        toInterpolate.emplace_back(points[i], points[i + 1]);
    }
    std::vector<glm::vec3> result;
    double animatePart = (glfwGetTime() - startTimeValue) / animationTime;
    while (toInterpolate.size() >= 2) {
        std::vector<std::pair<glm::vec3, glm::vec3>> nexts;
        for (int i = 0; i < toInterpolate.size() - 1; i++) {
            glm::vec3 first, second;
            std::tie(first, second) = toInterpolate[i];
            glm::vec3 next_a = first * float(1.0 - animatePart) + second * float(animatePart);
            std::tie(first, second) = toInterpolate[i + 1];
            glm::vec3 next_b = first * float(1.0 - animatePart) + second * float(animatePart);
            nexts.emplace_back(next_a, next_b);
            result.push_back(next_a);
            result.push_back(next_b);
        }
        toInterpolate = std::move(nexts);
    }
    if (result.size() >= 2) {
        glm::vec3 start = result[result.size()-2];
        glm::vec3 end = result[result.size()-1];
        glm::vec3 front = start * float(1.0 - animatePart) + end * float(animatePart);
        result.push_back(front);
    }

    return result;
}


Bezier::Bezier() = default;


