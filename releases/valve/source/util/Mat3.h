#ifndef UTIL_MAT3_H
#define UTIL_MAT3_H

/**
 * 3x3 Matrix class.
 */
class Mat3
{

public:

	/**
	 * Elements are uninitialized.
	 */
	Mat3();

	/**
	 * Converts from Euler angles to a rotation matrix.
	 */
	Mat3(const float angles[3]);

	float& operator()(int r, int c);
	float  operator()(int r, int c) const;

	/**
	 * Extracts Euler angles from the matrix.	 
 	 */
	void GetEulerAngles(float angles[3]) const;
	void SetEulerAngles(const float angles[3]);
	void TransformVector(float vector[3]) const;

	/**
	 * If the columns of the matrix are orthonormal (as is the case with a
	 * rotation matrix), then the transpose is also the inverse.
	 */
	Mat3 Transpose() const;

private:

	float element[3][3];

};

Mat3 operator*(const Mat3& m1, const Mat3& m2);

#endif

