// This file implements the API calls that draw particle groups in Half-life

#include "general.h"

#ifdef WIN32
// This is for something in gl.h.
#include <windows.h>
#endif

//#include <GL/gl.h>
#include "common/triangleapi.h"
#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"
#include "common/renderingconst.h"
#include "particles/papi.h"

// XXX #include <iostream.h>

// Draw as a splat texture on a quad.
void DrawGroupTriSplat(const pVector &view, const pVector &up,
					float size_scale, bool draw_tex,
					bool const_size, bool const_color)
{
	int cnt = pGetGroupCount();
	
	if(cnt < 1)
		return;
	
	pVector *ppos = new pVector[cnt];
	float *color = const_color ? NULL : new float[cnt * 4];
	pVector *size = const_size ? NULL : new pVector[cnt];
	
	pGetParticles(0, cnt, (float *)ppos, color, NULL, (float *)size);
	
	// Compute the vectors from the particle to the corners of its tri.
	// 2
	// |\      The particle is at the center of the x.
	// |-\     V0, V1, and V2 go from there to the vertices.
	// |x|\    The texcoords are (0,0), (2,0), and (0,2) respectively.
	// 0-+-1   We clamp the texture so the rest is transparent.
	
	pVector right = view ^ up;
	right.normalize();
	pVector nup = right ^ view;
	right *= size_scale;
	nup *= size_scale;

	pVector V0 = -(right + nup);
	pVector V1 = V0 + right * 4;
	pVector V2 = V0 + nup*4 + right*2;

	//cerr << "x " << view.x << " " << view.y << " " << view.z << endl;
	//cerr << "x " << nup.x << " " << nup.y << " " << nup.z << endl;
	//cerr << "x " << right.x << " " << right.y << " " << right.z << endl;
	//cerr << "x " << V0.x << " " << V0.y << " " << V0.z << endl;

	//glBegin(GL_TRIANGLES);
	gEngfuncs.pTriAPI->Begin( TRI_TRIANGLES );
	//gEngfuncs.pTriAPI->RenderMode( kRenderTransAlpha );
	
	for(int i = 0; i < cnt; i++)
	{
		pVector &p = ppos[i];

		float* theCurrentColor = &color[i*4];
		if(!const_color)
		{
			//glColor4fv((GLfloat *)&color[i*4]);
			gEngfuncs.pTriAPI->Color4f(theCurrentColor[0], theCurrentColor[1], theCurrentColor[2], theCurrentColor[3]);
		}
		//else
		//{
		//	gEngfuncs.pTriAPI->Color4f(1.0f, 1.0f, 1.0f, .5f);
		//}

		pVector sV0 = V0;
		pVector sV1 = V1;
		pVector sV2 = V2;

		if(!const_size)
		  {
		    sV0 *= size[i].x;
		    sV1 *= size[i].x;
		    sV2 *= size[i].x;
		  }

		//if(draw_tex) glTexCoord2f(0,0);
		if(draw_tex) gEngfuncs.pTriAPI->TexCoord2f(0,0);
		pVector ver = p + sV0;
		//glVertex3fv((GLfloat *)&ver);
		gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);

		//if(draw_tex) glTexCoord2f(2,0);
		if(draw_tex) gEngfuncs.pTriAPI->TexCoord2f(1,0);
		ver = p + sV1;
		//glVertex3fv((GLfloat *)&ver);
		gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);

		//if(draw_tex) glTexCoord2f(0,2);
		if(draw_tex) gEngfuncs.pTriAPI->TexCoord2f(.5,1);
		ver = p + sV2;
		//glVertex3fv((GLfloat *)&ver);
		gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
	}
	
	//glEnd();
	gEngfuncs.pTriAPI->End();

	delete [] ppos;
	if(color) delete [] color;
	if(size) delete [] size;
}



// Emit OpenGL calls to draw the particles. These are drawn with
// whatever primitive type the user specified(GL_POINTS, for
// example). The color and radius are set per primitive, by default.
// For GL_LINES, the other vertex of the line is the velocity vector.
// XXX const_size is ignored.
//PARTICLEDLL_API void pDrawGroupp(int primitive, bool const_size, bool const_color)
//{
//	_ParticleState &_ps = _GetPState();
//
//	// Get a pointer to the particles in gp memory
//	ParticleGroup *pg = _ps.pgrp;
//
//	if(pg == NULL)
//		return; // ERROR
//	
//	if(pg->p_count < 1)
//		return;
//	
//	if(primitive == GL_POINTS)
//	{
//		glPushClientAttrib(GL_CLIENT_VERTEX_ARRAY_BIT);
//		glEnableClientState(GL_VERTEX_ARRAY);
//		if(!const_color)
//		{
//			glEnableClientState(GL_COLOR_ARRAY);
//			glColorPointer(4, GL_FLOAT, sizeof(Particle), &pg->list[0].color);
//		}
//		
//		glVertexPointer(3, GL_FLOAT, sizeof(Particle), &pg->list[0].pos);
//		glDrawArrays((GLenum)primitive, 0, pg->p_count);
//		glPopClientAttrib();
//		// XXX For E&S
//		glDisableClientState(GL_COLOR_ARRAY);
//	}
//	else
//	{
//		// Assume GL_LINES
//		glBegin((GLenum)primitive);
//		
//		if(!const_color)
//		{
//			for(int i = 0; i < pg->p_count; i++)
//			{
//				Particle &m = pg->list[i];
//				
//				// Warning: this depends on alpha following color in the Particle struct.
//				glColor4fv((GLfloat *)&m.color);
//				glVertex3fv((GLfloat *)&m.pos);
//				
//				// For lines, make a tail with the velocity vector's direction and
//				// a length of radius.
//				pVector tail = m.pos - m.vel;			
//				glVertex3fv((GLfloat *)&tail);
//			}
//		}
//		else
//		{
//			for(int i = 0; i < pg->p_count; i++)
//			{
//				Particle &m = pg->list[i];
//				glVertex3fv((GLfloat *)&m.pos);
//				
//				// For lines, make a tail with the velocity vector's direction and
//				// a length of radius.
//				pVector tail = m.pos - m.vel;			
//				glVertex3fv((GLfloat *)&tail);
//			}
//		}
//		glEnd();
//	}
//}
//
//PARTICLEDLL_API void pDrawGroupl(int dlist, bool const_size, bool const_color, bool const_rotation)
//{
//	_ParticleState &_ps = _GetPState();
//
//	// Get a pointer to the particles in gp memory
//	ParticleGroup *pg = _ps.pgrp;
//	if(pg == NULL)
//		return; // ERROR
//	
//	if(pg->p_count < 1)
//		return;
//
//	//if(const_color)
//	//	glColor4fv((GLfloat *)&pg->list[0].color);
//
//	for(int i = 0; i < pg->p_count; i++)
//	{
//		Particle &m = pg->list[i];
//
//		glPushMatrix();
//		glTranslatef(m.pos.x, m.pos.y, m.pos.z);
//
//		if(!const_size)
//			glScalef(m.size.x, m.size.y, m.size.z);
//		else
//			glScalef(pg->list[i].size.x, pg->list[i].size.y, pg->list[i].size.z);
//
//		// Expensive! A sqrt, cross prod and acos. Yow.
//		if(!const_rotation)
//		{
//			pVector vN(m.vel);
//			vN.normalize();
//			pVector voN(m.velB);
//			voN.normalize();
//
//			pVector biN;
//			if(voN.x == vN.x && voN.y == vN.y && voN.z == vN.z)
//				biN = pVector(0, 1, 0);
//			else
//				biN = vN ^ voN;
//			biN.normalize();
//
//			pVector N(vN ^ biN);
//
//			double M[16];
//			M[0] = vN.x;  M[4] = biN.x;  M[8] = N.x;  M[12] = 0;
//			M[1] = vN.y;  M[5] = biN.y;  M[9] = N.y;  M[13] = 0;
//			M[2] = vN.z;  M[6] = biN.z;  M[10] = N.z; M[14] = 0;
//			M[3] = 0;     M[7] = 0;      M[11] = 0;   M[15] = 1;
//			glMultMatrixd(M);
//		}
//
//		// Warning: this depends on alpha following color in the Particle struct.
//		if(!const_color)
//			glColor4fv((GLfloat *)&m.color);
//
//		glCallList(dlist);
//
//		glPopMatrix();
//	}
//}
