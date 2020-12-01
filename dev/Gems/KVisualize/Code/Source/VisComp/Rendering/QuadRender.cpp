
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
		void TypeRenderState< kvis::prim::kv_quad >::InitializeMesh(IIndexedMesh& mesh) const
		{
			const size_t numVerts = 8;
			const size_t numTriangles = 4;

			mesh.SetVertexCount(numVerts);
			mesh.SetFaceCount(numTriangles);
			mesh.SetTexCoordCount(numVerts);

			Vec3* const vertices = mesh.GetMesh()->GetStreamPtr<Vec3>(CMesh::POSITIONS);
			Vec3* const normals = mesh.GetMesh()->GetStreamPtr<Vec3>(CMesh::NORMALS);
			SMeshTexCoord* const texcoords = mesh.GetMesh()->GetStreamPtr<SMeshTexCoord>(CMesh::TEXCOORDS);
			SMeshFace* const faces = mesh.GetMesh()->GetStreamPtr<SMeshFace>(CMesh::FACES);

			const float halfDim = 0.5f;

			int v = 0;
			int t = 0;

			for (int face = -1; face <= 1; face += 2)
			{
				const int base_v = v;

				vertices[v + 0] = Vec3(-halfDim, -halfDim, 0);
				vertices[v + 1] = Vec3(-halfDim, halfDim, 0);
				vertices[v + 2] = Vec3(halfDim, halfDim, 0);
				vertices[v + 3] = Vec3(halfDim, -halfDim, 0);

				normals[v + 0] = normals[v + 1] = normals[v + 2] = normals[v + 3] = Vec3(0, 0, (float)face);
				// @TODO:
				texcoords[v + 0] = texcoords[v + 1] = texcoords[v + 2] = texcoords[v + 3] = SMeshTexCoord(0.0f, 0.0f);

				v += 4;

				const int i0 = 0;
				const int i1 = (face < 0) ? 1 : 2;
				const int i2 = (face < 0) ? 2 : 1;

				{
					SMeshFace& tri = faces[t];
					tri.v[i0] = base_v + 0;
					tri.v[i1] = base_v + 1;
					tri.v[i2] = base_v + 2;
					tri.nSubset = 0;
					++t;
				}

				{
					SMeshFace& tri = faces[t];
					tri.v[i0] = base_v + 0;
					tri.v[i1] = base_v + 2;
					tri.v[i2] = base_v + 3;
					tri.nSubset = 0;
					++t;
				}
			}
		}

		void TypeRenderState< kvis::prim::kv_quad >::RenderWireframe(const kvis::prim::kv_quad& quad, const Matrix34& xform) const
		{
			// Type conversions
			const auto instSize = kantan_ly::k_to_cry(quad.extents);
			const auto instRotation = kantan_ly::k_to_cry(quad.rotation);
			const auto instPosition = kantan_ly::k_to_cry(quad.center);
			const auto instColor = kantan_ly::k_to_cry(quad.color);

			const Vec3 points[] = {
				xform * (instPosition + instRotation * Vec3(instSize.x * -0.5f, instSize.y * -0.5f, 0)),
				xform * (instPosition + instRotation * Vec3(instSize.x * -0.5f, instSize.y * 0.5f, 0)),
				xform * (instPosition + instRotation * Vec3(instSize.x * 0.5f, instSize.y * 0.5f, 0)),
				xform * (instPosition + instRotation * Vec3(instSize.x * 0.5f, instSize.y * -0.5f, 0)),
			};

			auto pAuxRenderer = gEnv->pRenderer->GetIRenderAuxGeom();
			pAuxRenderer->DrawPolyline(points, 4, true, instColor);
		}
	}
}

