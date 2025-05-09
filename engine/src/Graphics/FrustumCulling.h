#pragma once

#include <array>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

namespace math
{
struct Sphere;
}
class Camera;

struct Frustum {
    struct Plane {
        Plane() = default;
        Plane(const glm::vec3& p1, const glm::vec3& norm) :
            normal(glm::normalize(norm)), distance(glm::dot(normal, p1))
        {}

        glm::vec3 normal{0.f, 1.f, 0.f};

        // distance from the origin to the nearest point in the plane
        float distance{0.f};

        float getSignedDistanceToPlane(const glm::vec3& point) const
        {
            return glm::dot(normal, point) - distance;
        }
    };

    Plane farFace;
    Plane nearFace;

    Plane leftFace;
    Plane rightFace;

    Plane topFace;
    Plane bottomFace;
};

namespace edge
{
    std::array<glm::vec3, 8> calculateFrustumCornersWorldSpace(const glm::mat4& viewProj);
    Frustum createFrustumFromCamera(const Camera& camera);
    bool isInFrustum(const Frustum& frustum, const math::Sphere& s);

    math::Sphere calculateBoundingSphereWorld(
        const glm::mat4& transform,
        const math::Sphere& s,
        bool hasSkeleton
    );
}
