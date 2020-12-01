
#include "KVisualize_precompiled.h"

#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>
#include <AzCore/Math/Transform.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Component/TickBus.h>

#include <MathConversion.h>

#include <I3DEngine.h>
#include <IIndexedMesh.h>
#include <IEntityRenderState.h>
#include <IRenderAuxGeom.h>

#include "KVisualizerComponent.h"
#include "Rendering/PrimitiveRenderState.h"


namespace KVisualize
{
	/*!
	* RenderNode implementation responsible for integrating with the renderer.
	* The node owns render flags, the mesh instance, and the render transform.
	*/
	class KVisualizerComponentRenderNode
		: public IRenderNode
		, public AZ::TransformNotificationBus::Handler
		, public AZ::TickBus::Handler
	{
	public:
		using MaterialPtr = _smart_ptr < IMaterial >;
		using MeshPtr = _smart_ptr < IStatObj >;

		//AZ_TYPE_INFO(MeshComponentRenderNode, "{46FF2BC4-BEF9-4CC4-9456-36C127C310D7}");

		KVisualizerComponentRenderNode()
		{
			m_localBoundingBox.Reset();
			m_worldBoundingBox.Reset();
			m_worldTransform = AZ::Transform::CreateIdentity();
			m_renderTransform = Matrix34::CreateIdentity();

			Init();
		}
		
		~KVisualizerComponentRenderNode() override
		{
			AttachToEntity(AZ::EntityId());
			RegisterWithRenderer(false);
		}

		void Init()
		{
			boxShared.Initialize();
			sphereShared.Initialize();
			quadShared.Initialize();

			//
			PrimRender::BoxInst box;
			box.prim.extents = kantan::k_vec(1, 2, 3);
			box.prim.center = kantan::k_vec(0, 0, 0);
			box.prim.rotation = Eigen::AngleAxis< kantan::k_float >(3.14 * 0.25, kantan::k_vec(1, 0, 0));
			box.prim.color = kantan::k_color(0, 1, 0, 0.5);
			boxInstances.push_back(AZStd::move(box));

			PrimRender::SphereInst sph;
			sph.prim.position = kantan::k_vec(3.0, 0, 0);
			sph.prim.radius = 1.0;
			sph.prim.rotation = Eigen::AngleAxis< kantan::k_float >(3.14 * 0.25, kantan::k_vec(0, 1, 0));
			sph.prim.color = kantan::k_color(0, 0, 1, 0.5);
			sphereInstances.push_back(AZStd::move(sph));

			PrimRender::QuadInst quad;
			quad.prim.extents = kantan::k_vec2(2, 1);
			quad.prim.center = kantan::k_vec(-3.0, 0, 0);
			quad.prim.rotation = Eigen::AngleAxis< kantan::k_float >(3.14 * 0.17, kantan::k_vec(1, 1, 0).normalized());
			quad.prim.color = kantan::k_color(1, 1, 0, 0.5);
			quadInstances.push_back(AZStd::move(quad));
			//

			UpdateLocalBoundingBox();

			pAuxRenderer = gEnv->pRenderer->GetIRenderAuxGeom();
		}

		//! Notifies render node which entity owns it, for subscribing to transform
		//! bus, etc.
		void AttachToEntity(AZ::EntityId id)
		{
			if (AZ::TransformNotificationBus::Handler::BusIsConnectedId(m_attachedToEntityId))
			{
				AZ::TickBus::Handler::BusDisconnect();
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

				AZ::TickBus::Handler::BusConnect();
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

		//////////////////////////////////////////////////////////////////////////
		// AZ::TickBus::Handler interface implementation
		void OnTick(float deltaTime, AZ::ScriptTimePoint time) override
		{
			// @NOTE: Idea was to call aux render functions in tick, but in Render() seems to work fine.

/*			if (pAuxRenderer)
			{
				pAuxRenderer->
			}
*/		}
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// IRenderNode interface implementation
		void Render(const struct SRendParams& inRenderParams, const struct SRenderingPassInfo& passInfo) override
		{
			auto renderParams(inRenderParams);
			renderParams.pInstance = this;

			// @NOTE: Making mutable so can alter renderParams. This means that in the below loops, each iteration will inherit the lambda in the state
			// left by the previous iteration, so we need to ensure everything is always either reset or unconditionlly overwritten.
			// Alternatively, we could just duplicate the renderParams each time.
			auto RenderSolid = [this, renderParams, &passInfo](const auto& prim_type, Matrix34&& xform, const ColorF& color) mutable
			{
				renderParams.pMatrix = &xform;
				// @TODO: Will need a material cache hashed on RGB color.
				// Also, need to confirm if this is the correct alternative to UE4 material instancing.
				renderParams.pMaterial = gEnv->p3DEngine->GetMaterialManager()->CloneMaterial(m_material);

				// See https://docs.aws.amazon.com/lumberyard/latest/userguide/material-param-names.html for default param names
				const char* const emissive_param = "emissive_color";
				const char* const emissive_intensity_param = "emissive_intensity";
				const char* const opacity_param = "opacity";

				Vec4 useColor = color.toVec4();
				renderParams.pMaterial->SetGetMaterialParamVec4(emissive_param, useColor, false);
				renderParams.fAlpha = color.a;	// @TODO: what's difference between this approach, and passing to material via opacity_param?
				prim_type.RenderSolid(renderParams, passInfo);
			};


			for (const auto& box : boxInstances)
			{
				{
					// Type conversions
					const auto instSize = kantan_ly::k_to_cry(box.prim.extents);
					const auto instRotation = kantan_ly::k_to_cry(box.prim.rotation);
					const auto instPosition = kantan_ly::k_to_cry(box.prim.center);
					const auto instColor = kantan_ly::k_to_cry(box.prim.color);

					// Compose instance xform
					// @NOTE: Underlying box mesh is built as unit box, so we scale directly by total extents.
					Matrix34 instanceXform = Matrix34::Create(instSize, instRotation, instPosition);
					Matrix34 composedXform = m_renderTransform * instanceXform;

					RenderSolid(boxShared, AZStd::move(composedXform), instColor);
				}

				boxShared.RenderWireframe(box.prim, m_renderTransform);
			}

			for (const auto& sph : sphereInstances)
			{
				{
					// Type conversions
					const auto instRadius = (float)sph.prim.radius;
					const auto instRotation = kantan_ly::k_to_cry(sph.prim.rotation);
					const auto instPosition = kantan_ly::k_to_cry(sph.prim.position);
					const auto instColor = kantan_ly::k_to_cry(sph.prim.color);

					// Compose instance xform
					// @NOTE: Underlying sphere mesh is built as a sphere with unit radius, so we scale directly by radius.
					Matrix34 instanceXform = Matrix34::Create(Vec3(instRadius), instRotation, instPosition);
					Matrix34 composedXform = m_renderTransform * instanceXform;

					RenderSolid(sphereShared, AZStd::move(composedXform), instColor);
				}
				
				sphereShared.RenderWireframe(sph.prim, m_renderTransform);
			}

			for (const auto& quad : quadInstances)
			{
				{
					// Type conversions
					const auto instSize = kantan_ly::k_to_cry(quad.prim.extents);
					const auto instRotation = kantan_ly::k_to_cry(quad.prim.rotation);
					const auto instPosition = kantan_ly::k_to_cry(quad.prim.center);
					const auto instColor = kantan_ly::k_to_cry(quad.prim.color);

					// Compose instance xform
					// @NOTE: Underlying quad mesh is built as unit quad, so we scale directly by total extents.
					Matrix34 instanceXform = Matrix34::Create(Vec3(instSize.x, instSize.y, 1.0f), instRotation, instPosition);
					Matrix34 composedXform = m_renderTransform * instanceXform;

					RenderSolid(quadShared, AZStd::move(composedXform), instColor);
				}

				quadShared.RenderWireframe(quad.prim, m_renderTransform);
			}
		}

		EERType GetRenderNodeType() override
		{
			// @TODO: Look into eERType_RenderComponent and IComponentRender. Seems to be a generic implementation for hierarchical collection of renderable objects.
			// However, suspect this relates to CryEngine's entity/component system, so perhaps is going to be deprecated.
			return eERType_DynamicMeshRenderComponent;	// eERType_StaticMeshRenderComponent;
		}

		const char* GetName() const override
		{
			return "KVisualizerComponentRenderNode";
		}

		const char* GetEntityClassName() const override
		{
			return "KVisualizerComponentRenderNode";
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
			// @TODO: Currently always using an override (set in Render method).
			// May be better to set the material on the IStatObj?
			m_material = pMat;
		}

		_smart_ptr<IMaterial> GetMaterial(Vec3* pHitPos) override
		{
			return m_material;
		}

		_smart_ptr<IMaterial> GetMaterialOverride() override
		{
			return m_material;
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

	protected:
		//! Computes the entity-relative (local space) bounding box for
		//! the assigned mesh.
		virtual void UpdateLocalBoundingBox()
		{
			/* @TODO: May eventually be worth calculating this properly, or perhaps even refactoring into multiple render nodes so can cull individually.
			Note there is a size limit imposed in ObjManDrawEntity.cpp, search for 'detect bad objects'.
			*/
			m_localBoundingBox = AABB(10000.0f);

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

	protected:
		//! Should be visible.
		bool m_visible = true;

		//! The Id of the entity we're associated with, for bus subscription.
		//Moved from render mesh to this struct for serialization/reflection utility
		AZ::EntityId m_attachedToEntityId;

		IRenderAuxGeom* pAuxRenderer = nullptr;

		PrimRender::TypeRenderState< kvis::prim::kv_box > boxShared;
		AZStd::vector< PrimRender::BoxInst > boxInstances;

		PrimRender::TypeRenderState< kvis::prim::kv_sphere > sphereShared;
		AZStd::vector< PrimRender::SphereInst > sphereInstances;

		PrimRender::TypeRenderState< kvis::prim::kv_quad > quadShared;
		AZStd::vector< PrimRender::QuadInst > quadInstances;

		//! Currently-assigned material. Null if no material is manually assigned.
		MaterialPtr m_material;

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

		//! Computed LOD distance.
		float m_lodDistance = 0.0f;

		//! Identifies whether we've already registered our node with the renderer.
		bool m_isRegisteredWithRenderer = false;

		//! Tracks if the object was moved so we can notify the renderer.
		bool m_objectMoved = false;
	};



	const float KVisualizerComponent::s_renderNodeRequestBusOrder = 100.f;

    void KVisualizerComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serialize = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serialize->Class<KVisualizerComponent, AZ::Component>()
                ->Version(0)
				->Field("TempMaterial", &KVisualizerComponent::m_material)
				;

            if (AZ::EditContext* ec = serialize->GetEditContext())
            {
                ec->Class<KVisualizerComponent>("KVisualizerComponent", "Debug visualization component")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game"))
						->Attribute(AZ::Edit::Attributes::Category, "Kantan")
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
					->DataElement(AZ::Edit::UIHandlers::Default, &KVisualizerComponent::m_material, "Temp Material", "")
						->Attribute(AZ::Edit::Attributes::ChangeNotify, &KVisualizerComponent::OnMaterialChanged)
					;
            }
        }
    }

    void KVisualizerComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC("KVisualizerService"));
    }

    void KVisualizerComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC("KVisualizerService"));
    }

    void KVisualizerComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        (void)required;
    }

    void KVisualizerComponent::GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
        (void)dependent;
    }


	// @NOTE: Explicitly declared to allow unique_ptr with incomplete type.
	KVisualizerComponent::KVisualizerComponent() = default;
	KVisualizerComponent::~KVisualizerComponent() = default;

    void KVisualizerComponent::Init()
    {

	}

    void KVisualizerComponent::Activate()
    {
		// Initialize render node
		{
			auto node = AZStd::make_unique< KVisualizerComponentRenderNode >();
			node->RegisterWithRenderer(true);
			node->AttachToEntity(GetEntityId());

			const AZStd::string& materialPath = m_material.GetAssetPath();
			const auto mat = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial(materialPath.c_str());
			node->SetMaterial(mat);

			m_renderNode = AZStd::move(node);
		}

		//TestFirstGemRequestBus::Handler::BusConnect(GetEntityId());
		LmbrCentral::RenderNodeRequestBus::Handler::BusConnect(GetEntityId());
    }

    void KVisualizerComponent::Deactivate()
    {
		LmbrCentral::RenderNodeRequestBus::Handler::BusDisconnect();
		//TestFirstGemRequestBus::Handler::BusDisconnect();

		if (m_renderNode)
		{
			static_cast<KVisualizerComponentRenderNode*>(m_renderNode.get())->RegisterWithRenderer(false);
			m_renderNode = nullptr;
		}
    }
	
	IRenderNode* KVisualizerComponent::GetRenderNode()
	{
		return m_renderNode.get();
	}

	float KVisualizerComponent::GetRenderNodeRequestBusOrder() const
	{
		return s_renderNodeRequestBusOrder;
	}

	void KVisualizerComponent::OnMaterialChanged()
	{
		if (m_renderNode != nullptr)
		{
			const AZStd::string& materialPath = m_material.GetAssetPath();
			const auto mat = gEnv->p3DEngine->GetMaterialManager()->LoadMaterial(materialPath.c_str());
			m_renderNode->SetMaterial(mat);
		}
	}
}

