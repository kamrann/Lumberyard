
#include "KVisualize_precompiled.h"

#include "PrimitiveRenderState.h"

#include <AzCore/Math/Transform.h>

#include <MathConversion.h>
#include <IStatObj.h>
#include <I3DEngine.h>
#include <IIndexedMesh.h>


namespace KVisualize
{
	namespace PrimRender
	{
		void TypeRenderStateSimpleMesh::Initialize()
		{
			IStatObj* pObject = gEnv->p3DEngine->CreateStatObj();

			//IMaterial* pMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial("EngineAssets/TextureMsg/DefaultSolids");
			//pObject->SetMaterial(pMaterial);

			IIndexedMesh* pMesh = pObject->GetIndexedMesh();

			if (pMesh == nullptr)
			{
				return;
			}

			InitializeMesh(*pMesh);

			// @TODO: Find out what these are
			pMesh->SetSubSetCount(1);
			pMesh->SetSubsetMaterialId(0, 0);
			//

			pMesh->CalcBBox();
			pMesh->Optimize();
			pMesh->RestoreFacesFromIndices();

			pObject->Invalidate(true);

			m_mesh = pObject;
		}

		void TypeRenderStateSimpleMesh::RenderSolid(const SRendParams& rParams, const SRenderingPassInfo& passInfo) const
		{
			m_mesh->Render(rParams, passInfo);
		}
	}
}

