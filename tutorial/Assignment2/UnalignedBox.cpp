//
// Created by user on 22-Dec-22.
//

#include "UnalignedBox.h"
#include "Eigen/src/Eigenvalues/SelfAdjointEigenSolver.h"
#include "Eigen/Geometry"

UnalignedBox UnalignedBox::BuildBoxFromVertices(const std::vector<Eigen::Vector3d> &vertices) {
    // Create an empty Box instance
    UnalignedBox box;

    // Compute the center of the box as the mean of the vertices
    Eigen::Vector3d sum = Eigen::Vector3d::Zero();
    for (const auto& vertex : vertices) {
        sum += vertex;
    }
    box.center_ = sum / static_cast<double>(vertices.size());

    // Compute the dimensions of the box as the maximum extent in each direction
    Eigen::Vector3d min;
    Eigen::Vector3d max;

    double minX = std::numeric_limits<double>::infinity();
    double minY = std::numeric_limits<double>::infinity();
    double minZ = std::numeric_limits<double>::infinity();

    double maxX = -std::numeric_limits<double>::infinity();
    double maxY = -std::numeric_limits<double>::infinity();
    double maxZ = -std::numeric_limits<double>::infinity();
    for (const auto& vertex : vertices) {
        double x = vertex.x();
        double y = vertex.y();
        double z = vertex.z();

        minX = std::min(minX, x);
        minY = std::min(minY, y);
        minZ = std::min(minZ, z);

        maxX = std::max(maxX, x);
        maxY = std::max(maxY, y);
        maxZ = std::max(maxZ, z);
    }
    min << minX, minY, minZ;
    max << maxX, maxY, maxZ;

    box.dimensions_ = max - min;

    // Compute the principal axes of the box
    Eigen::Matrix3d covariance;
    covariance.setZero();
    for (const auto& vertex : vertices) {
        Eigen::Vector3d diff = vertex - box.center_;
        covariance(0, 0) += diff.x() * diff.x();
        covariance(1, 1) += diff.y() * diff.y();
        covariance(2, 2) += diff.z() * diff.z();
        covariance(0, 1) += diff.x() * diff.y();
        covariance(0, 2) += diff.x() * diff.z();
        covariance(1, 2) += diff.y() * diff.z();
    }
    covariance /= static_cast<float>(vertices.size());
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix3d> eigenSolver(covariance);
    Eigen::Matrix3d eigenVectors = eigenSolver.eigenvectors();
    Eigen::Quaterniond q(eigenVectors);
    box.orientation_ = q;

    return box;
}

bool CheckCollusion2(const UnalignedBox &box1, const UnalignedBox &box2) {
    Eigen::Vector3d center1 = box1.center();
    Eigen::Vector3d dimensions1 = box1.dimensions();
    Eigen::Quaterniond orientation1 = box1.orientation();

    // Define the center, dimensions, and orientation of the second box
    Eigen::Vector3d center2 = box2.center();
    Eigen::Vector3d dimensions2 = box2.center();
    Eigen::Quaterniond orientation2 = box2.orientation();
    // Compute the separation vector between the centers of the boxes
    Eigen::Vector3d separation = center2 - center1;

    // Rotate the separation vector by the inverse of the orientation of the first box
    separation = orientation1.inverse()._transformVector(separation);

    // Compute the separation between the boxes along each axis
    double sx = fabs(separation(0)) - dimensions1(0) / 2.0 - dimensions2(0) / 2.0;
    double sy = fabs(separation(1)) - dimensions1(1) / 2.0 - dimensions2(1) / 2.0;
    double sz = fabs(separation(2)) - dimensions1(2) / 2.0 - dimensions2(2) / 2.0;;

    return sx < 0 && sy < 0 && sz < 0;
}

bool UnalignedBox::CheckCollusion(const UnalignedBox &box1, const UnalignedBox &box2) {
    return CheckCollusion2(box1, box2);

    // Compute the relative orientation of box2 with respect to box1
    auto relativeOrientation = box1.orientation().inverse() * box2.orientation();
    // Compute the half-extents of box1 and box2 in the local frame of box1
    Eigen::Vector3d box1HalfExtents = box1.dimensions() / 2.0f;
    Eigen::Vector3d box2HalfExtents = relativeOrientation.norm() * box2.dimensions() / 2.0f;

    // Compute the relative position of box2 with respect to box1
    Eigen::Vector3d relativePosition = box2.center() - box1.center();
    relativePosition = box1.orientation() * relativePosition;

    // Test the overlap in each axis
    for (int i = 0; i < 3; i++) {
        // Compute the projection interval of box1 and box2 along the axis
        double relevantOrientationPart = i == 0 ? relativeOrientation.x() : i == 1 ? relativeOrientation.y() :
                i == 2 ? relativeOrientation.z() : relativeOrientation.w();

        double box1Projection = box1HalfExtents.dot(Eigen::Vector3d(relevantOrientationPart, relevantOrientationPart, relevantOrientationPart));
        double box2Projection = box2HalfExtents.dot(Eigen::Vector3d(i == 0, i == 1, i == 2));
        double intervalDistance = abs(relativePosition[i]);
        double intervalLength = box1Projection + box2Projection;
        if (intervalDistance > intervalLength) {
            return false;  // Separating axis found
        }
    }

    // No separating axis found, boxes collide
    return true;
}
