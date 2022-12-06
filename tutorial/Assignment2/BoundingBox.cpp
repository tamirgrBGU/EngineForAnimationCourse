//
// Created by user on 06-Dec-22.
//

#include "BoundingBox.h"
#include <limits>


BoundingBox::BoundingBox(double minX, double minY, double minZ, double maxX, double maxY, double maxZ):
    minX(minX), minY(minY), minZ(minZ), maxX(maxX), maxY(maxY), maxZ(maxZ)
    {}

std::optional<BoundingBox> BoundingBox::Collide(const BoundingBox &other) const {
    std::optional<BoundingBox> collusionBox;

    const BoundingBox* leftBox;
    const BoundingBox* rightBox;
    const BoundingBox* bottomBox;
    const BoundingBox* topBox;
    const BoundingBox* closeBox;
    const BoundingBox* farBox;

    if(minX < other.minX) {
        leftBox = this;
        rightBox = &other;
    } else {
        leftBox = &other;
        rightBox = this;
    }
    if(minY < other.minY) {
        bottomBox = this;
        topBox = &other;
    } else {
        bottomBox = &other;
        topBox = this;
    }
    if(minZ < other.minZ) {
        closeBox = this;
        farBox = &other;
    } else {
        closeBox = &other;
        farBox = this;
    }

    bool collide = rightBox->minX <= leftBox->maxX && topBox->minY <= bottomBox->maxY && farBox->minZ <= closeBox->maxZ;
    if(collide) {
        double collusionMinX = rightBox->minX;
        double collusionMaxX = std::min(leftBox->maxX, rightBox->maxX);
        double collusionMinY = topBox->minY;
        double collusionMaxY = std::min(bottomBox->maxY, topBox->maxY);
        double collusionMinZ = farBox->minZ;
        double collusionMaxZ = std::min(closeBox->maxZ, farBox->maxZ);

        collusionBox = std::make_optional<BoundingBox>(collusionMinX, collusionMinY, collusionMinZ,
                                                       collusionMaxX, collusionMaxY, collusionMaxZ);
    }

    return collusionBox;
}

BoundingBox::BoundingBox(std::vector<Eigen::Vector3d> points) {
    if(points.size() < 2 || points[0] == points[1]) {
        throw std::invalid_argument("Could not build a box with less then 2 points");
    }
    minX = std::numeric_limits<double>::infinity();
    minY = std::numeric_limits<double>::infinity();
    minZ = std::numeric_limits<double>::infinity();
    maxX = -std::numeric_limits<double>::infinity();
    maxY = -std::numeric_limits<double>::infinity();
    maxZ = -std::numeric_limits<double>::infinity();

    for(auto point : points) {
        minX = std::min(point.x(), minX);
        minY = std::min(point.y(), minY);
        minZ = std::min(point.z(), minZ);
        maxX = std::max(point.x(), maxX);
        maxY = std::max(point.y(), maxY);
        maxZ = std::max(point.z(), maxZ);
    }


}

bool BoundingBox::Contains(const Eigen::Vector3d &point) const {
    return minX <= point.x() && point.x() <= maxX &&
           minY <= point.y() && point.y() <= maxY &&
           minZ <= point.z() && point.z() <= maxZ;
}

std::vector<Eigen::Vector3d> BoundingBox::FilterIfOut(std::vector<Eigen::Vector3d> points) {
    std::vector<Eigen::Vector3d> filtered;
    std::copy_if(points.begin(), points.end(), std::back_inserter(filtered), [this] (Eigen::Vector3d point){return this->Contains(point);});
    return filtered;

}


