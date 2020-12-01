
#include "KVisualize_precompiled.h"

#include "PrimitiveRenderState.h"

#include <AzCore/Math/Transform.h>

#include <MathConversion.h>
#include <I3DEngine.h>
#include <IIndexedMesh.h>
#include <IRenderAuxGeom.h>


namespace KVisualize
{
	namespace PrimRender
	{
		void TypeRenderState< kvis::prim::kv_sphere >::InitializeMesh(IIndexedMesh& mesh) const
		{
			const float radius = 1.0f;
			const int rings = 16;
			const int sections = 16;
			
			// @NOTE: Taken from D3DRenderAuxGeom.cpp

			// calc required number of vertices/indices/triangles to build a sphere for the given parameters
			uint32 numVertices((rings - 1) * (sections + 1) + 2);
			uint32 numTriangles((rings - 2) * sections * 2 + 2 * sections);
			uint32 numIndices(numTriangles * 3);

			mesh.SetVertexCount(numVertices);
			mesh.SetFaceCount(numTriangles);
			mesh.SetTexCoordCount(numVertices);

			Vec3* const vertices = mesh.GetMesh()->GetStreamPtr<Vec3>(CMesh::POSITIONS);
			Vec3* const normals = mesh.GetMesh()->GetStreamPtr<Vec3>(CMesh::NORMALS);
			SMeshTexCoord* const texcoords = mesh.GetMesh()->GetStreamPtr<SMeshTexCoord>(CMesh::TEXCOORDS);
			SMeshFace* const faces = mesh.GetMesh()->GetStreamPtr<SMeshFace>(CMesh::FACES);

			int v = 0;

			// 1st pole vertex
			vertices[v] = Vec3(0.0f, 0.0f, radius);
			normals[v] = Vec3(0.0f, 0.0f, 1.0f);
			// @TODO:
			texcoords[v] = SMeshTexCoord(0.0f, 0.0f);
			++v;

			// calculate "inner" vertices
			float sectionSlice(DEG2RAD(360.0f / (float)sections));
			float ringSlice(DEG2RAD(180.0f / (float)rings));

			for (uint32 a(1); a < rings; ++a)
			{
				float w(sinf(a * ringSlice));
				for (uint32 i(0); i <= sections; ++i)
				{
					vertices[v] = Vec3(cosf(i * sectionSlice) * w, sinf(i * sectionSlice) * w, cosf(a * ringSlice)) * radius;
					normals[v] = vertices[v].GetNormalized();
					// @TODO:
					texcoords[v] = SMeshTexCoord(0.0f, 0.0f);
					++v;
				}
			}

			// 2nd vertex of pole (for end cap)
			vertices[v] = Vec3(0.0f, 0.0f, -radius);
			normals[v] = Vec3(0.0f, 0.0f, -1.0f);
			// @TODO:
			texcoords[v] = SMeshTexCoord(0.0f, 0.0f);
			++v;

			int t = 0;

			// build "inner" faces
			for (uint32 a(0); a < rings - 2; ++a)
			{
				for (uint32 i(0); i < sections; ++i)
				{
					{
						SMeshFace& tri = faces[t];
						tri.v[0] = 1 + a * (sections + 1) + i + 1;
						tri.v[1] = 1 + a * (sections + 1) + i;
						tri.v[2] = 1 + (a + 1) * (sections + 1) + i + 1;
						tri.nSubset = 0;
						++t;
					}

					{
						SMeshFace& tri = faces[t];
						tri.v[0] = 1 + (a + 1) * (sections + 1) + i;
						tri.v[1] = 1 + (a + 1) * (sections + 1) + i + 1;
						tri.v[2] = 1 + a * (sections + 1) + i;
						tri.nSubset = 0;
						++t;
					}
				}
			}

			// build faces for end caps (to connect "inner" vertices with poles)
			for (uint32 i(0); i < sections; ++i)
			{
				SMeshFace& tri1 = faces[t];
				tri1.v[0] = 1 + (0) * (sections + 1) + i;
				tri1.v[1] = 1 + (0) * (sections + 1) + i + 1;
				tri1.v[2] = 0;
				tri1.nSubset = 0;
				++t;
			}

			for (uint32 i(0); i < sections; ++i)
			{
				SMeshFace& tri1 = faces[t];
				tri1.v[0] = 1 + (rings - 2) * (sections + 1) + i + 1;
				tri1.v[1] = 1 + (rings - 2) * (sections + 1) + i;
				tri1.v[2] = (rings - 1) * (sections + 1) + 1;
				tri1.nSubset = 0;
				++t;
			}
		}

		// @NOTE: IRenderAuxGeom doesn't seem to be able to draw solid shapes with alpha.
/*		void TypeRenderState< kvis::prim::kv_sphere >::RenderSolid(const kvis::prim::kv_sphere& sphere, const Matrix34& xform) const
		{
			// Type conversions
			const auto instRadius = (float)sphere.radius;
			const auto instPosition = kantan_ly::k_to_cry(sphere.position);
			const auto instColor = kantan_ly::k_to_cry(sphere.color);

			auto pAuxRenderer = gEnv->pRenderer->GetIRenderAuxGeom();
			pAuxRenderer->DrawSphere(xform * instPosition, instRadius, instColor, false);
		}
*/
		void TypeRenderState< kvis::prim::kv_sphere >::RenderWireframe(const kvis::prim::kv_sphere& sphere, const Matrix34& xform) const
		{
			// Type conversions
			const auto instRadius = (float)sphere.radius;
			const auto instPosition = kantan_ly::k_to_cry(sphere.position);
			const auto instRotation = kantan_ly::k_to_cry(sphere.rotation);
			const auto instColor = kantan_ly::k_to_cry(sphere.color);

			// @TODO: Perhaps move this and similar stuff like the box mesh generation to engine-agnostic lib, and then create tri-mesh/line-mesh conversions.
			// We'll need it for custom mesh primitive anyway.
			const unsigned npts = 32;
			Vec3 xpoints[npts];
			Vec3 ypoints[npts];
			Vec3 zpoints[npts];

			for (unsigned i = 0; i < npts; i++)
			{
				float a = ((float)i / (float)npts) * gf_PI2;
				float rx = cosf(a) * instRadius;
				float ry = sinf(a) * instRadius;
				xpoints[i] = xform * (instPosition + instRotation * Vec3(rx, ry, 0));
				ypoints[i] = xform * (instPosition + instRotation * Vec3(0, rx, ry));
				zpoints[i] = xform * (instPosition + instRotation * Vec3(ry, 0, rx));
			}

			auto pAuxRenderer = gEnv->pRenderer->GetIRenderAuxGeom();
			// @TODO: Probs don't want to fix to alpha 1, but maybe have both fill and outline colors in base primitive struct.
			pAuxRenderer->DrawPolyline(xpoints, npts, true, ColorB(instColor, 1.0f));
			pAuxRenderer->DrawPolyline(ypoints, npts, true, ColorB(instColor, 1.0f));
			pAuxRenderer->DrawPolyline(zpoints, npts, true, ColorB(instColor, 1.0f));
		}
	}
}

