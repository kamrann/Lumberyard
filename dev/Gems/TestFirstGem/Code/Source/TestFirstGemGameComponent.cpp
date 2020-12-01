
#include "TestFirstGem_precompiled.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Math/Transform.h>
#include <AzCore/Component/TransformBus.h>

#include <MathConversion.h>

#include <I3DEngine.h>
#include <IIndexedMesh.h>

#include "TestFirstGemGameComponent.h"

namespace TestFirstGem
{
	/*!
	* RenderNode implementation responsible for integrating with the renderer.
	* The node owns render flags, the mesh instance, and the render transform.
	*/
	class TestComponentRenderNode
		: public IRenderNode
		, public AZ::TransformNotificationBus::Handler
	{
		//friend class EditorMeshComponent;

	public:
		//using MaterialPtr = _smart_ptr < IMaterial >;
		using MeshPtr = _smart_ptr < IStatObj >;

		//AZ_TYPE_INFO(MeshComponentRenderNode, "{46FF2BC4-BEF9-4CC4-9456-36C127C310D7}");

		TestComponentRenderNode(MeshPtr meshObj)
		{
			m_localBoundingBox.Reset();
			m_worldBoundingBox.Reset();
			m_worldTransform = AZ::Transform::CreateIdentity();
			m_renderTransform = Matrix34::CreateIdentity();

			SetMesh(meshObj);
		}
		
		~TestComponentRenderNode() override
		{
			RegisterWithRenderer(false);
		}

		void SetMesh(MeshPtr mesh)
		{
			m_statObj = AZStd::move(mesh);

			UpdateLocalBoundingBox();
		}

		bool HasMesh() const
		{
			return m_statObj != nullptr;
		}

//		void CopyPropertiesTo(MeshComponentRenderNode& rhs) const;

		//! Notifies render node which entity owns it, for subscribing to transform
		//! bus, etc.
		void AttachToEntity(AZ::EntityId id)
		{
			if (AZ::TransformNotificationBus::Handler::BusIsConnectedId(m_attachedToEntityId))
			{
				AZ::TransformNotificationBus::Handler::BusDisconnect(m_attachedToEntityId);
			}

			if (id.IsValid())
			{
				if (!AZ::TransformNotificationBus::Handler::BusIsConnectedId(id))
				{
					AZ::TransformNotificationBus::Handler::BusConnect(id);
				}

				AZ::Transform entityTransform = AZ::Transform::CreateIdentity();
				EBUS_EVENT_ID_RESULT(entityTransform, id, AZ::TransformBus, GetWorldTM);
				UpdateWorldTransform(entityTransform);
			}

			m_attachedToEntityId = id;
		}

		//! Updates the render node's world transform based on the entity's.
		void UpdateWorldTransform(const AZ::Transform& entityTransform)
		{
			m_worldTransform = entityTransform;

			m_renderTransform = AZTransformToLYTransform(m_worldTransform);

			UpdateWorldBoundingBox();

			m_objectMoved = true;
		}

		//! Computes world-space AABB.
		AZ::Aabb CalculateWorldAABB() const
		{
			AZ::Aabb aabb = AZ::Aabb::CreateNull();
			if (!m_worldBoundingBox.IsReset())
			{
				aabb.AddPoint(LYVec3ToAZVec3(m_worldBoundingBox.min));
				aabb.AddPoint(LYVec3ToAZVec3(m_worldBoundingBox.max));
			}
			return aabb;
		}

		//! Computes local-space AABB.
		AZ::Aabb CalculateLocalAABB() const
		{
			AZ::Aabb aabb = AZ::Aabb::CreateNull();
			if (!m_localBoundingBox.IsReset())
			{
				aabb.AddPoint(LYVec3ToAZVec3(m_localBoundingBox.min));
				aabb.AddPoint(LYVec3ToAZVec3(m_localBoundingBox.max));
			}
			return aabb;
		}

		//////////////////////////////////////////////////////////////////////////
		// AZ::TransformNotificationBus::Handler interface implementation
		void OnTransformChanged(const AZ::Transform& local, const AZ::Transform& world) override
		{
			// The entity to which we're attached has moved.
			UpdateWorldTransform(world);
		}
		//////////////////////////////////////////////////////////////////////////


		//! Returns true after all required assets are loaded
/*		bool IsReady() const override;

		//! Instantiate mesh instance.
		void CreateMesh();

		//! Destroy mesh instance.
		void DestroyMesh();

		//! Returns true if the node has geometry assigned.
		bool HasMesh() const;

		//! Assign a new mesh asset
		void SetMeshAsset(const AZ::Data::AssetId& id);

		//! Get the mesh asset
		AZ::Data::Asset<AZ::Data::AssetData> GetMeshAsset() { return m_meshAsset; }

		//! Invoked in the editor when the user assigns a new asset.
		void OnAssetPropertyChanged();

		//! Render the mesh
		void RenderMesh(const struct SRendParams& inRenderParams, const struct SRenderingPassInfo& passInfo);

		//////////////////////////////////////////////////////////////////////////
		// AZ::Data::AssetBus::Handler
		void OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset) override;
		void OnAssetReloaded(AZ::Data::Asset<AZ::Data::AssetData> asset) override;
		//////////////////////////////////////////////////////////////////////////

*/
		//////////////////////////////////////////////////////////////////////////
		// IRenderNode interface implementation
		void Render(const struct SRendParams& inRenderParams, const struct SRenderingPassInfo& passInfo) override
		{
			if (m_statObj)
			{
				SRendParams rParams(inRenderParams);
				rParams.pInstance = this;
				rParams.pMatrix = &m_renderTransform;
				m_statObj->Render(rParams, passInfo);
			}
		}

		EERType GetRenderNodeType() override
		{
			return eERType_DynamicMeshRenderComponent;	// eERType_StaticMeshRenderComponent;
		}

		const char* GetName() const override
		{
			return "TestComponentRenderNode";
		}

		const char* GetEntityClassName() const override
		{
			return "TestComponentRenderNode";
		}

		Vec3 GetPos(bool bWorldOnly = true) const override
		{
			return m_renderTransform.GetTranslation();
		}

		// WS
		const AABB GetBBox() const override
		{
			return m_worldBoundingBox;
		}

		void SetBBox(const AABB& WSBBox) override
		{
			m_worldBoundingBox = WSBBox;
		}

		void OffsetPosition(const Vec3& delta) override
		{
			// Recalculate local transform
			AZ::Transform localTransform = AZ::Transform::CreateIdentity();
			EBUS_EVENT_ID_RESULT(localTransform, m_attachedToEntityId, AZ::TransformBus, GetLocalTM);

			localTransform.SetTranslation(localTransform.GetTranslation() + LYVec3ToAZVec3(delta));
			EBUS_EVENT_ID(m_attachedToEntityId, AZ::TransformBus, SetLocalTM, localTransform);

			m_objectMoved = true;
		}

		void SetMaterial(_smart_ptr<IMaterial> pMat) override
		{

		}

		_smart_ptr<IMaterial> GetMaterial(Vec3* pHitPos) override
		{
			return nullptr;
		}

		_smart_ptr<IMaterial> GetMaterialOverride() override
		{
			return nullptr;
		}

		void SetPhysics(IPhysicalEntity* pPhys) override
		{

		}

		IPhysicalEntity* GetPhysics() const override
		{
			return nullptr;
		}

		float GetMaxViewDist() override
		{
			return 1000000.0f;
		}

		void GetMemoryUsage(class ICrySizer* pSizer) const override
		{
			pSizer->AddObjectSize(this);
		}
		
		/*
		bool GetLodDistances(const SFrameLodInfo& frameLodInfo, float* distances) const override;
		float GetFirstLodDistance() const override { return m_lodDistance; }
		IStatObj* GetEntityStatObj(unsigned int nPartId = 0, unsigned int nSubPartId = 0, Matrix34A* pMatrix = nullptr, bool bReturnOnlyVisible = false) override;
		_smart_ptr<IMaterial> GetEntitySlotMaterial(unsigned int nPartId, bool bReturnOnlyVisible = false, bool* pbDrawNear = nullptr) override;
		ICharacterInstance* GetEntityCharacter(unsigned int nSlot = 0, Matrix34A* pMatrix = nullptr, bool bReturnOnlyVisible = false) override;
		float GetUniformScale() override;
		float GetColumnScale(int column) override;
*/		//////////////////////////////////////////////////////////////////////////

		AZ::EntityId GetEntityId() override { return m_attachedToEntityId; }

		//! Invoked in the editor when a property requiring render state refresh
		//! has changed.
/*		void RefreshRenderState();

		//! Set/get auxiliary render flags.
		void SetAuxiliaryRenderFlags(uint32 flags);
		uint32 GetAuxiliaryRenderFlags() const { return m_auxiliaryRenderFlags; }
		void UpdateAuxiliaryRenderFlags(bool on, uint32 mask);

		void SetVisible(bool isVisible);
		bool GetVisible();
*/
//		static void Reflect(AZ::ReflectContext* context);

//		static float GetDefaultMaxViewDist();
//		static AZ::Uuid GetRenderOptionsUuid() { return AZ::AzTypeInfo<MeshRenderOptions>::Uuid(); }

		//! Registers or unregisters our render node with the render.
		void RegisterWithRenderer(bool registerWithRenderer)
		{
			if (gEnv && gEnv->p3DEngine)
			{
				if (registerWithRenderer)
				{
					if (!m_isRegisteredWithRenderer)
					{
						//ApplyRenderOptions();

						gEnv->p3DEngine->RegisterEntity(this);

						m_isRegisteredWithRenderer = true;
					}
				}
				else
				{
					if (m_isRegisteredWithRenderer)
					{
						gEnv->p3DEngine->FreeRenderNodeState(this);
						m_isRegisteredWithRenderer = false;
					}
				}
			}
		}

		bool IsRegisteredWithRenderer() const { return m_isRegisteredWithRenderer; }

		//! This function caches off the static flag stat of the transform;
//		void SetTransformStaticState(bool isStatic);

	protected:

		//! Computes the entity-relative (local space) bounding box for
		//! the assigned mesh.
		virtual void UpdateLocalBoundingBox()
		{
			m_localBoundingBox.Reset();

			if (HasMesh())
			{
				m_localBoundingBox.Add(m_statObj->GetAABB());
			}

			UpdateWorldBoundingBox();
		}

		//! Updates the world-space bounding box and world space transform
		//! for the assigned mesh.
		void UpdateWorldBoundingBox()
		{
			m_worldBoundingBox.SetTransformedAABB(m_renderTransform, m_localBoundingBox);

			if (m_isRegisteredWithRenderer)
			{
				// Re-register with the renderer to update culling info
				gEnv->p3DEngine->RegisterEntity(this);
			}
		}

		//! Applies configured render options to the render node.
//		void ApplyRenderOptions();

/*		class MeshRenderOptions
		{
		public:

			AZ_TYPE_INFO(MeshRenderOptions, "{EFF77BEB-CB99-44A3-8F15-111B0200F50D}")

				MeshRenderOptions();

			float m_opacity; //!< Alpha/opacity value for rendering.
			float m_maxViewDist; //!< Maximum draw distance.
			float m_viewDistMultiplier; //!< Adjusts max view distance. If 1.0 then default max view distance is used.
			AZ::u32 m_lodRatio; //!< Controls LOD distance ratio.
			bool m_useVisAreas; //!< Allow VisAreas to control this component's visibility.
			bool m_castShadows; //!< Casts shadows.
			bool m_rainOccluder; //!< Occludes raindrops.
			bool m_affectNavmesh; //!< Cuts out of the navmesh.
			bool m_affectDynamicWater; //!< Affects dynamic water (ripples).
			bool m_acceptDecals; //!< Accepts decals.
			bool m_receiveWind; //!< Receives wind.
			bool m_visibilityOccluder; //!< Appropriate for visibility occluding.
			bool m_dynamicMesh; // Mesh can change or deform independent of transform
			bool m_hasStaticTransform;

			//! The Id of the entity we're associated with, for bus subscription.
			//Moved from render mesh to this struct for serialization/reflection utility
			AZ::EntityId m_attachedToEntityId;

			AZStd::function<void()> m_changeCallback;

			//Due to the fact that some ui elements like sliders don't seem to work properly with
			//entire tree refreshes we needed to have two on change functions.
			AZ::u32 OnMinorChanged()
			{
				if (m_changeCallback)
				{
					m_changeCallback();
				}
				return AZ::Edit::PropertyRefreshLevels::None;
			}

			AZ::u32 OnMajorChanged()
			{
				if (m_changeCallback)
				{
					m_changeCallback();
				}
				return AZ::Edit::PropertyRefreshLevels::EntireTree;
			}

			//Returns true if the transform is static and the mesh is not deformable.
			bool IsStatic() const;
			AZ::Crc32 StaticPropertyVisibility() const;
			static void Reflect(AZ::ReflectContext* context);

		private:
			static bool VersionConverter(AZ::SerializeContext& context,
				AZ::SerializeContext::DataElementNode& classElement);
		};
*/
		//! Should be visible.
		bool m_visible = true;

		//! The Id of the entity we're associated with, for bus subscription.
		//Moved from render mesh to this struct for serialization/reflection utility
		AZ::EntityId m_attachedToEntityId;


		//! User-specified material override.
//		AzFramework::SimpleAssetReference<MaterialAsset> m_material;

		//! Render flags/options.
//		MeshRenderOptions m_renderOptions;

		//! Currently-assigned material. Null if no material is manually assigned.
//		MaterialPtr m_materialOverride;

		//! World and render transforms.
		//! These are equivalent, but for different math libraries.
		AZ::Transform m_worldTransform;
		Matrix34 m_renderTransform;

		//! Local and world bounding boxes.
		AABB m_localBoundingBox;
		AABB m_worldBoundingBox;

		//! Additional render flags -- for special editor behavior, etc.
		uint32 m_auxiliaryRenderFlags = 0;

		//! Remember which flags have ever been toggled externally so that we can shut them off
		uint32 m_auxiliaryRenderFlagsHistory = 0;

		//! Reference to current asset
//		AZ::Data::Asset<MeshAsset> m_meshAsset;
		MeshPtr m_statObj = nullptr;
//		IDeformableNode* m_deformNode;

		//! Computed LOD distance.
		float m_lodDistance = 0.0f;

		//! Identifies whether we've already registered our node with the renderer.
		bool m_isRegisteredWithRenderer = false;

		//! Tracks if the object was moved so we can notify the renderer.
		bool m_objectMoved = false;
	};



	const float TestFirstGemGameComponent::s_renderNodeRequestBusOrder = 100.f;

    void TestFirstGemGameComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<TestFirstGemGameComponent, AZ::Component>()
                ->Version(0)
				->Field("AutoFoo", &TestFirstGemGameComponent::m_autoFoo)
				;

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<TestFirstGemGameComponent>("TestFirstGemComponent", "Deals mostly with bars and foos")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
						->Attribute(AZ::Edit::Attributes::Category, "Kantan")
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
					->DataElement(AZ::Edit::UIHandlers::Default, &TestFirstGemGameComponent::m_autoFoo, "Auto Foo", "This is a foo for auto use.")
						->Attribute(AZ::Edit::Attributes::Min, 0)
						->Attribute(AZ::Edit::Attributes::Max, 10)
					;
            }
        }
    }

    void TestFirstGemGameComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("TestFirstGemService"));
    }

    void TestFirstGemGameComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("TestFirstGemService"));
    }

    void TestFirstGemGameComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        (void)required;
    }

    void TestFirstGemGameComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        (void)dependent;
    }

    void TestFirstGemGameComponent::Init()
    {
		CryLog("BinFolderName: %s", BINFOLDER_NAME);

	}

    void TestFirstGemGameComponent::Activate()
    {
		if (InitMesh())
		{
			{
				auto node = AZStd::make_unique< TestComponentRenderNode >(m_mesh);
				node->RegisterWithRenderer(true);
				node->AttachToEntity(GetEntityId());
				m_renderNode = AZStd::move(node);
			}

			TestFirstGemRequestBus::Handler::BusConnect(GetEntityId());
			LmbrCentral::RenderNodeRequestBus::Handler::BusConnect(GetEntityId());
		}
    }

    void TestFirstGemGameComponent::Deactivate()
    {
		LmbrCentral::RenderNodeRequestBus::Handler::BusDisconnect();
		TestFirstGemRequestBus::Handler::BusDisconnect();

		if (m_renderNode)
		{
			static_cast<TestComponentRenderNode*>(m_renderNode.get())->RegisterWithRenderer(false);
			m_renderNode = nullptr;
		}
		m_mesh = nullptr;
    }


	AZStd::string TestFirstGemGameComponent::BarMeAFoo(int foo)
	{
		return AZStd::string::format("Bar_%i", foo);
	}

	AZStd::string TestFirstGemGameComponent::BarMeAnAutoFoo()
	{
		return BarMeAFoo(m_autoFoo);
	}


	IRenderNode* TestFirstGemGameComponent::GetRenderNode()
	{
		return m_renderNode.get();
	}

	float TestFirstGemGameComponent::GetRenderNodeRequestBusOrder() const
	{
		return s_renderNodeRequestBusOrder;
	}


	bool TestFirstGemGameComponent::InitMesh()
	{
		IStatObj *pObject = gEnv->p3DEngine->CreateStatObj();
		//pObject->AddRef();

		// Init stuff
		//IMaterial* pMaterial = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial("EngineAssets/TextureMsg/DefaultSolids");
		//pEntity->SetMaterial(pMaterial);
		//pObject->SetMaterial(pMaterial);

		IIndexedMesh *pMesh = pObject->GetIndexedMesh();

		if (pMesh == NULL)
			return false;

		//Assign vert counts and stuff
		const size_t numVerts = 3;
		const size_t numTriangles = 2;

		pMesh->SetVertexCount(numVerts);
		pMesh->SetFaceCount(numTriangles);
		pMesh->SetTexCoordCount(numVerts);


		//Get the different mesh streams
		Vec3* const vertices = pMesh->GetMesh()->GetStreamPtr<Vec3>(CMesh::POSITIONS);
		Vec3* const normals = pMesh->GetMesh()->GetStreamPtr<Vec3>(CMesh::NORMALS);
		SMeshTexCoord* const texcoords = pMesh->GetMesh()->GetStreamPtr<SMeshTexCoord>(CMesh::TEXCOORDS);
		SMeshFace* const faces = pMesh->GetMesh()->GetStreamPtr<SMeshFace>(CMesh::FACES);


		//Assign stuff
		vertices[0] = Vec3(0.0f, 0.0f, 0.0f);

		texcoords[0] = SMeshTexCoord(0.0f, 0.0f);

		vertices[1] = Vec3(0.0f, 10.0f, 0.0f);

		texcoords[1] = SMeshTexCoord(0.0f, 1.0f);

		vertices[2] = Vec3(10.0f, 0.0f, 0.0f);

		texcoords[2] = SMeshTexCoord(1.0f, 0.0f);

		SMeshFace* const meshface1 = &faces[0];
		meshface1->v[0] = 2;
		meshface1->v[1] = 1;
		meshface1->v[2] = 0;
		meshface1->nSubset = 0;

		SMeshFace* const meshface2 = &faces[1];
		meshface2->v[0] = 2;
		meshface2->v[1] = 0;
		meshface2->v[2] = 1;
		meshface2->nSubset = 0;

		normals[0] = Vec3(1, 0, 0);
		normals[1] = Vec3(1, 0, 0);
		normals[2] = Vec3(1, 0, 0);

		//Probably also does something. So yep.
		pMesh->SetSubSetCount(1);
		pMesh->SetSubsetMaterialId(0, 0);

		//Calc bounding box. then optimize stuff! Whatever that does...
		pMesh->CalcBBox();

		pMesh->Optimize();
		pMesh->RestoreFacesFromIndices();

		pObject->Invalidate(true);

		//pEntity->Activate(true);
		//int slotId = pEntity->SetStatObj(pObject, -1, true);
		//pObject->Release();

		m_mesh = pObject;
		return true;
	}

}
