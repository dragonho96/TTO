#pragma once

#include "Component.h"
#include "HierarchyNode.h"
#include "MeshContainer.h"
#include "Shader.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	typedef struct tagRagdollBoneDesc
	{
		PxRigidDynamic* pRb;
		BONEDESC*		pParentBone;
		BONEDESC*		pChildBone;
	}RAGDOLLBONEDESC;
private:
	explicit CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	explicit CModel(const CModel& rhs);
	virtual ~CModel() = default;
public:
	virtual HRESULT InitializePrototype();
	virtual HRESULT Initialize(void* pArg);
	HRESULT Render(_uint iMaterialIndex, _uint iPassIndex);

public:
	_fmatrix Get_BoneMatrix(const char* pBoneName);
public:
	HRESULT SetUp_AnimationIndex(_uint iAnimationIndex, ANIM_TYPE eType);
	HRESULT Play_Animation(_double TimeDelta);
	HRESULT Blend_Animation(_double TimeDelta);
	_bool	IsLowerFinished() { return m_bFinished_Lower; }
	_bool	IsUpperFinished() { return m_bFinished_Upper; }
	void	SetAnimationLoop(_uint idx, _bool result);
	void	SetUpperRotationAngle(_float2 angle) { m_upperRotationAngle = angle; }
	void	SetAnimSeperate(_bool result) { m_bAnimSeperate = result; }
	_uint	GetCurrentKeyFrame(ANIM_TYPE eType);

public:
	HRESULT Bind_Buffers(_uint iPassIndex);
	HRESULT SetUp_TextureOnShader(const char* pConstantName, _uint iMaterialIndex, aiTextureType eTextureType);
private:
	HRESULT Create_MeshContainer(aiMesh* pMesh, _uint* pStartVertexIndex, _uint* pStartFaceIndex, _fmatrix PivotMatrix);
	HRESULT Create_VertexIndexBuffer(string pShaderFilePath);
	HRESULT Create_Materials(aiMaterial*	pMaterial, string pMeshFilePath);
	HRESULT Create_HierarchyNodes(aiNode* pNode, class CHierarchyNode* pParent = nullptr, _uint iDepth = 0, ANIM_TYPE eType = ANIM_TYPE::NONE, _fmatrix PivotMatrix = XMMatrixIdentity());
	HRESULT Sort_MeshesByMaterial();
	HRESULT SetUp_SkinnedInfo();
	HRESULT SetUp_AnimationInfo();
	HRESULT Update_CombinedTransformationMatrices(_double TimeDelta);
	HRESULT Update_CombinedTransformationMatrix_Ragdoll();
	void Add_ChannelToHierarchyNode(_uint iAnimationindex, class CChannel* pChannel);
public:
	CHierarchyNode* Find_HierarchyNode(const char* pBoneName);
	BONEDESC* Find_Bone(string pBoneName);
public:
	HRESULT CreateBuffer(string pMeshFilePath, string pMeshFileName, string pShaderFilePath = "../../Assets/Shader/Shader_Mesh.fx");
	void RemoveBuffer();
public:
	_bool& HasMeshCollider() { return m_bMeshCollider; }
	void SetMeshCollider(_bool value) { m_bMeshCollider = value; }
	CMeshContainer* GetMeshContainerByName(string name);

public:
	void SetRagdollBoneDesc(BONEDESC* desc);
	HRESULT CreateRagdollRbs();
	void CreateCapsuleRb(BONEDESC* parent, BONEDESC* child, string name);
	void CreateSphereRb(BONEDESC* parent, string name);
	void CreateD6Joint(string parent, string child, string jointBone);
	void ConfigD6Joint(physx::PxReal swing0, physx::PxReal swing1, physx::PxReal twistLo, physx::PxReal twistHi, physx::PxD6Joint* joint);
	void SetRagdollRbTransform(RAGDOLLBONEDESC* ragdollBoneDesc);
	void SetRagdollSimulate(_bool result);
	_bool IsSimulatingRagdoll() { return m_bSimulateRagdoll; }
	PxRigidDynamic* GetRagdollRb(string name);

private:
	void CreatePxMesh();

public:
	_uint Get_NumMaterials() const {
		return (_uint)m_ModelTextures.size();
	}
	string GetMeshFilePath() { return m_pMeshFilePath; }
	string GetMeshFileName() { return m_pMeshFileName; }
	string GetShaderFilePath() { return m_pShaderFilePath; }
	CShader*	GetShader() { return m_pShader.get(); }
	void	SetDissolve(_float value) { m_fDissolve = value; }

private:
	const aiScene*		m_pScene = nullptr;
	Assimp::Importer	m_Importer;


private:
	_uint				m_iNumVertices = 0;
	_uint				m_iNumFaces = 0;
	VTXMESH*			m_pVertices = nullptr;
	POLYGONINDICES32*	m_pPolygonIndices = nullptr;

	vector<class CMeshContainer*>			m_MeshContainers;

	vector<vector<class CMeshContainer*>>	m_SortByMaterialMesh;
	vector<MODELTEXTURES*>					m_ModelTextures;
	vector<class CHierarchyNode*>			m_HierarchyNodes;

	vector<class CAnimation*>				m_Animations;
	_uint									m_iAnimationIndex = 0;
	_uint									m_iPrevAnimationIndex = 0;
	_uint									m_iAnimationIndex_Upper = 32;
	_uint									m_iPrevAnimationIndex_Upper = 32;
	_bool									m_bFinished_Lower = false;
	_bool									m_bFinished_Upper = false;
	_float2									m_upperRotationAngle = { 0.f, 0.f };
	_bool									m_bAnimSeperate = false;

	_float									m_fBlendDuration = 0.2f;
	_float									m_fBlendTime = 0.0f;
	_float									m_fBlendTime_Upper = 0.0f;
	vector<class CChannel*>					m_vecPrevChannels;
private:
	//ComRef<ID3D11Buffer>			m_pVB = nullptr;
	//ComRef<ID3D11Buffer>			m_pIB = nullptr;

	ID3D11Buffer*					m_pVB = nullptr;
	ID3D11Buffer*					m_pIB = nullptr;

	_uint							m_iStride = 0;

protected:
	vector<EFFECTDESC>				m_EffectDescs;
	ID3DX11Effect*					m_pEffect = nullptr;

	Ref<class CShader>				m_pShader = nullptr;
	class CTransform*				m_pTransform = nullptr;

protected:
	string			m_pMeshFilePath = "";
	string			m_pMeshFileName = "";
	string			m_pShaderFilePath = "";
	CTexture*		m_pTextureCube;
	_float			m_fDissolve = 0.f;

private:
	PxVec3*				m_pxVertices = nullptr;
	PxU32*				m_pxIndices = nullptr;
	PxTriangleMesh*		m_pPxMesh = nullptr;
	_bool				m_bMeshCollider = false;

private:
	unordered_map<string, BONEDESC*>		m_RagdollBones;
	unordered_map<string, RAGDOLLBONEDESC*> m_RagdollRbs;
	_bool									m_bSimulateRagdoll = false;


public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END