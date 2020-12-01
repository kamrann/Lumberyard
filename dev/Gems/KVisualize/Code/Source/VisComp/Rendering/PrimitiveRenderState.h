
#pragma once

#include <smartptr.h>

#include "LY_kv_api.h"


struct IStatObj;
struct IIndexedMesh;
struct SRendParams;
struct SRenderingPassInfo;

namespace KVisualize
{
	namespace PrimRender
	{
		using MeshPtr = _smart_ptr< IStatObj >;


		template < typename PrimType >
		struct TypeRenderState;

		struct TypeRenderStateSimpleMesh
		{
			void Initialize();
			void RenderSolid(const SRendParams& inRenderParams, const SRenderingPassInfo& passInfo) const;

		protected:
			virtual void InitializeMesh(IIndexedMesh& mesh) const = 0;

		private:
			MeshPtr m_mesh;
		};

		template <>
		struct TypeRenderState< kvis::prim::kv_box > : TypeRenderStateSimpleMesh
		{
			void InitializeMesh(IIndexedMesh& mesh) const override;
			void RenderWireframe(const kvis::prim::kv_box& box, const Matrix34& xform) const;
		};

		template <>
		struct TypeRenderState< kvis::prim::kv_sphere > : TypeRenderStateSimpleMesh
		{
			void InitializeMesh(IIndexedMesh& mesh) const override;
			void RenderWireframe(const kvis::prim::kv_sphere& sphere, const Matrix34& xform) const;
		};

		template <>
		struct TypeRenderState< kvis::prim::kv_quad > : TypeRenderStateSimpleMesh
		{
			void InitializeMesh(IIndexedMesh& mesh) const override;
			void RenderWireframe(const kvis::prim::kv_quad& quad, const Matrix34& xform) const;
		};


		// todo: may not be necessary if we simply use the raw prim data, but might sense to precompute/cache some LY-specific per-primitive instance data...

		template < typename PrimType >
		struct InstanceRenderState;

		template < typename PrimType >
		struct InstanceRenderStateBase
		{
			PrimType prim;
		};

		template <>
		struct InstanceRenderState< kvis::prim::kv_box >: InstanceRenderStateBase< kvis::prim::kv_box >
		{

		};

		template <>
		struct InstanceRenderState< kvis::prim::kv_sphere > : InstanceRenderStateBase< kvis::prim::kv_sphere >
		{

		};

		template <>
		struct InstanceRenderState< kvis::prim::kv_quad > : InstanceRenderStateBase< kvis::prim::kv_quad >
		{

		};

		using BoxInst = InstanceRenderState< kvis::prim::kv_box >;
		using SphereInst = InstanceRenderState< kvis::prim::kv_sphere >;
		using QuadInst = InstanceRenderState< kvis::prim::kv_quad >;
	}
}
