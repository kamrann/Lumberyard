
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
		void TypeRenderState< kvis::prim::kv_box >::InitializeMesh(IIndexedMesh& mesh) const
		{
			const size_t numVerts = 24;
			const size_t numTriangles = 12;

			mesh.SetVertexCount(numVerts);
			mesh.SetFaceCount(numTriangles);
			mesh.SetTexCoordCount(numVerts);

			Vec3* const vertices = mesh.GetMesh()->GetStreamPtr<Vec3>(CMesh::POSITIONS);
			Vec3* const normals = mesh.GetMesh()->GetStreamPtr<Vec3>(CMesh::NORMALS);
			SMeshTexCoord* const texcoords = mesh.GetMesh()->GetStreamPtr<SMeshTexCoord>(CMesh::TEXCOORDS);
			SMeshFace* const faces = mesh.GetMesh()->GetStreamPtr<SMeshFace>(CMesh::FACES);

			const float halfDim = 0.5f;

			const int otherComps[3][2] = {
				{ 1, 2 },
				{ 2, 0 },
				{ 0, 1 },
			};

			int v = 0;
			int t = 0;
			for (int cmp = 0; cmp < 3; ++cmp)
			{
				for (int face = -1; face <= 1; face += 2)
				{
					Vec3 pos = Vec3(ZERO);
					pos[cmp] = face * halfDim;

					const int base_v = v;

					for (int c1 = -1; c1 <= 1; c1 += 2)
					{
						for (int c2 = -1; c2 <= 1; c2 += 2)
						{
							pos[otherComps[cmp][0]] = c1 * halfDim;
							pos[otherComps[cmp][1]] = c2 * halfDim;

							Vec3 norm = Vec3(ZERO);
							norm[cmp] = (float)face;

							vertices[v] = pos;
							normals[v] = norm;
							// @TODO:
							texcoords[v] = SMeshTexCoord(0.0f, 0.0f);

							++v;
						}
					}

					const int i0 = 0;
					const int i1 = (face < 0) ? 1 : 2;
					const int i2 = (face < 0) ? 2 : 1;

					SMeshFace& tri1 = faces[t];
					tri1.v[i0] = base_v + 0;
					tri1.v[i1] = base_v + 1;
					tri1.v[i2] = base_v + 2;
					tri1.nSubset = 0;
					++t;

					SMeshFace& tri2 = faces[t];
					tri2.v[i0] = base_v + 2;
					tri2.v[i1] = base_v + 1;
					tri2.v[i2] = base_v + 3;
					tri2.nSubset = 0;
					++t;
				}
			}
		}

		void TypeRenderState< kvis::prim::kv_box >::RenderWireframe(const kvis::prim::kv_box& box, const Matrix34& xform) const
		{
			// Type conversions
			const auto instSize = kantan_ly::k_to_cry(box.extents);
			const auto instRotation = kantan_ly::k_to_cry(box.rotation);
			const auto instPosition = kantan_ly::k_to_cry(box.center);
			const auto instColor = kantan_ly::k_to_cry(box.color);

			// @TODO: Need to think again about xform member of base prim - since a box specifies a center and rotation, the xform would naturally apply after those, ie.
			// box-extents -> box_rotation -> box_position -> xform
			// But this would mean the xform's rotation and scale would be applied to the already rotated/translated box, which is not really useful.
			// Alternative is to pull out individual components, ie.
			// box-extents -> xform-scale -> box-rotation -> xform-rotation -> box_position -> xform-translation
			// But this then renders having an extra transform kind of pointless...

			// @NOTE: Scale size down by 0.5 as OBB uses half-extents.
			const auto box_obb = OBB::CreateOBB(Matrix33(instRotation), instSize * 0.5f, instPosition);

			auto pAuxRenderer = gEnv->pRenderer->GetIRenderAuxGeom();
			pAuxRenderer->DrawOBB(box_obb, xform, false, instColor, EBoundingBoxDrawStyle::eBBD_Faceted);
		}
	}
}

