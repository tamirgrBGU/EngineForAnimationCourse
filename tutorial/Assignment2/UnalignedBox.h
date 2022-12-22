//
// Created by user on 22-Dec-22.
//

#ifndef ENGINEREWORK_UNALIGNEDBOX_H
#define ENGINEREWORK_UNALIGNEDBOX_H


#include "Eigen/Core"
#include "Eigen/Geometry"

class UnalignedBox {
public:
    UnalignedBox() = default;
    UnalignedBox(const Eigen::Vector3d& center, const Eigen::Vector3d& dimensions, const Eigen::Quaterniond& orientation)
            : center_(center), dimensions_(dimensions), orientation_(orientation) {}
    const Eigen::Vector3d& center() const { return center_; }
    const Eigen::Vector3d& dimensions() const { return dimensions_; }
    const Eigen::Quaterniond& orientation() const { return orientation_; }
    static UnalignedBox BuildBoxFromVertices(const std::vector<Eigen::Vector3d>& vertices);

    static bool CheckCollusion(const UnalignedBox& box1, const UnalignedBox& box2);

private:
    Eigen::Vector3d center_;
    Eigen::Vector3d dimensions_;
    Eigen::Quaterniond orientation_;
};


#endif //ENGINEREWORK_UNALIGNEDBOX_H
