#include "Quat.h"
#include "common/mathlib.h"

float WrapFloat(float, float, float);
void VectorsToAngles(const float forward[3], const float right[3], const float up[3], float angles[3]);

#include <math.h>

Quat::Quat()
{
}


Quat::Quat(float _x, float _y, float _z, float _w)
    : x(_x), y(_y), z(_z), w(_w)
{
}


Quat::Quat(const float angles[3])
{
    
    float xAxis[3];
    float yAxis[3];
    float zAxis[3];
    
    AngleVectors(angles, yAxis, xAxis, zAxis);
    
    *this = Quat(xAxis, yAxis, zAxis);

}


Quat::Quat(const float xAxis[3], const float yAxis[3], const float zAxis[3])
{

    float t = xAxis[0] + yAxis[1] + zAxis[2];
    
    if (t > 0) {

        float s = sqrtf(t + 1);
		
	    x = (zAxis[1] - yAxis[2]) * (0.5f / s);
	    y = (xAxis[2] - zAxis[0]) * (0.5f / s);
	    z = (yAxis[0] - xAxis[1]) * (0.5f / s);
        w = s * 0.5f;
    
    } else {

        if (xAxis[0] > yAxis[1] && xAxis[0] > zAxis[2]) {

            float s = sqrtf(xAxis[0] - yAxis[1] - zAxis[2] + 1);
            
            x = 0.5f * s;
            y = (yAxis[0] + xAxis[1]) * (0.5f / s);
            z = (zAxis[0] + xAxis[2]) * (0.5f / s);
            w = (zAxis[1] - yAxis[2]) * (0.5f / s);
        
        } else if (yAxis[1] > xAxis[0] && yAxis[1] > zAxis[2]) {

            float s = sqrtf(yAxis[1] - xAxis[0] - zAxis[2] + 1);
    
            x = (xAxis[1] + yAxis[0]) * (0.5f / s);
            y = 0.5f * s;
            z = (zAxis[1] + yAxis[2]) * (0.5f / s);
            w = (xAxis[2] - zAxis[0]) * (0.5f / s);

        } else {
    
            float s = sqrtf(zAxis[2] - xAxis[0] - yAxis[1] + 1);
            
            x = (xAxis[2] + zAxis[0]) * (0.5f / s);
            y = (yAxis[2] + zAxis[1]) * (0.5f / s);
            z = 0.5f * s;
            w = (yAxis[0] - xAxis[1]) * (0.5f / s);
            
        }   
    
    }

}


Quat::Quat(float angle, const float axis[3])
{

    float sa = sinf(angle / 2);
    float ca = cosf(angle / 2);

    x = axis[0] * sa;
    y = axis[1] * sa;
    z = axis[2] * sa;
    w = ca;

}


Quat Quat::Conjugate() const
{
    return Quat(-x, -y, -z, w);
}


Quat Quat::Unit() const
{
    float l = sqrtf(x * x + y * y + z * z + w * w);
    return Quat(x / l, y / l, z / l, w / l);
}


void Quat::GetVectors(float xAxis[3], float yAxis[3], float zAxis[3]) const
{
  
    float xx = x * x;
    float xy = x * y;
    float xz = x * z;
    float xw = x * w;
    float yy = y * y;
    float yz = y * z;
    float yw = y * w;
    float zz = z * z;
    float zw = z * w;
    float ww = w * w;
    
    xAxis[0] = 1 - 2 * (yy + zz);
    xAxis[1] = 2 * (xy - zw);
    xAxis[2] = 2 * (xz + yw);
    
    yAxis[0] = 2 * (xy + zw);
    yAxis[1] = 1 - 2 * (xx + zz);
    yAxis[2] = 2 * (yz - xw);

    zAxis[0] = 2 * (xz - yw);
    zAxis[1] = 2 * (yz + xw);
    zAxis[2] = 1 - 2 * (xx + yy);    

}


Quat operator*(const Quat& q1, const Quat& q2)
{

    return Quat(q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
                q1.w * q2.y + q1.y * q2.w + q1.z * q2.x - q1.x * q2.z,
                q1.w * q2.z + q1.z * q2.w + q1.x * q2.y - q1.y * q2.x,
                q1.w * q2.w - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z);

}

Quat ConstantRateLerp(const Quat& src, const Quat& dst, float amount)
{ 
	Quat rot = (dst * src.Conjugate()).Unit();
	
	// Compute the axis and angle we need to rotate about to go from src
	// to dst.
	
	float angle = acosf(rot.w) * 2;
	float sinAngle = sqrtf(1.0f - rot.w * rot.w);
	
	if (fabs(sinAngle) < 0.0005f)
	{
		sinAngle = 1;
	}
	
	vec3_t axis;
	
	axis[0] = rot.x / sinAngle;
	axis[1] = rot.y / sinAngle;
	axis[2] = rot.z / sinAngle;
	
	// Wrap the angle to the range -PI to PI
	angle = WrapFloat(angle, (float)(-M_PI), (float)(M_PI));
	
	// Amount to rotate this frame.
	float frameAngle = amount;
	
	if (fabs(angle) <= frameAngle)
	{
		// If we are very close, just jump to the goal orientation.
		return dst;
	}
	else
	{
		
		Quat final;
		
		if (angle < 0)
		{
			final = Quat(-frameAngle, axis) * src;
		}
		else
		{
			final = Quat(frameAngle, axis) * src;
		}

		return final;
		
	}
}

void Quat::GetAngles(float outAngles[3]) const
{
	vec3_t xAxis;
	vec3_t yAxis;
	vec3_t zAxis;
	
	GetVectors(xAxis, yAxis, zAxis);
	
	VectorsToAngles(yAxis, xAxis, zAxis, outAngles);
}
