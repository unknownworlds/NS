#ifndef UTIL_QUAT_H
#define UTIL_QUAT_H

/**
 * Quaternion class.
 */
class Quat
{

public:

    Quat();
    Quat(float x, float y, float z, float w);
    Quat(const float angles[3]);
    Quat(const float xAxis[3], const float yAxis[3], const float zAxis[3]);
    Quat(float angle, const float axis[3]);

    /**
     * For a unit quaternion, the conjugate is the inverse.
     */
    Quat Conjugate() const;

    Quat Unit() const;

    void GetVectors(float xAxis[3], float yAxis[3], float zAxis[3]) const;

	void GetAngles(float outAngles[3]) const;

public:

    float x;
    float y;
    float z;
    float w;

};

Quat operator*(const Quat& q1, const Quat& q2);

Quat ConstantRateLerp(const Quat& src, const Quat& dst, float amount);

#endif

