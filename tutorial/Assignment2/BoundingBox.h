//
// Created by user on 06-Dec-22.
//

#ifndef ENGINEREWORK_BOUNDINGBOX_H
#define ENGINEREWORK_BOUNDINGBOX_H

#include <optional>
#include "Eigen/Core"
#include "Model.h"

class BoundingBox;

class BoundingBox {
public:
    explicit BoundingBox(double minX, double minY, double minZ,
                         double maxX, double maxY, double maxZ);
    explicit BoundingBox(std::vector<Eigen::Vector3d> points);
    std::vector<Eigen::Vector3d> FilterIfOut(std::vector<Eigen::Vector3d> points);
    bool Contains(const Eigen::Vector3d &point) const;
    std::optional<BoundingBox> Collide(const BoundingBox &other) const;
private:
    double minX;
    double minY;
    double minZ;
    double maxX;
    double maxY;
    double maxZ;
};

#endif //ENGINEREWORK_BOUNDINGBOX_H
