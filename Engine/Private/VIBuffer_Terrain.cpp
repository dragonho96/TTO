#include "..\Public\VIBuffer_Terrain.h"

USING(Engine)

CVIBuffer_Terrain::CVIBuffer_Terrain(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	: CVIBuffer(pDevice, pDeviceContext)
{
}

CVIBuffer_Terrain::CVIBuffer_Terrain(const CVIBuffer_Terrain & rhs)
	: CVIBuffer(rhs)
{
}

CVIBuffer_Terrain * CVIBuffer_Terrain::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(pDevice, pDeviceContext);

	//if (FAILED(pInstance->InitializePrototype()))
	//{
	//	MSG_BOX("Failed To Creating CVIBuffer_Terrain");
	//	SafeRelease(pInstance);
	//}
	pInstance->InitializePrototype();

	return pInstance;
}

CComponent * CVIBuffer_Terrain::Clone(void * pArg)
{
	CVIBuffer_Terrain*	pInstance = new CVIBuffer_Terrain(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed To Creating CVIBuffer_Terrain");
		SafeRelease(pInstance);
	}

	return pInstance;
}

void CVIBuffer_Terrain::Free()
{
	__super::Free();
	SafeDeleteArray(m_pCloneVertices);
	SafeRelease(m_pTexture);
}

HRESULT CVIBuffer_Terrain::InitializePrototype()
{
	//if (FAILED(__super::InitializePrototype()))
	//	return E_FAIL;
	__super::InitializePrototype();
	CreateBuffer(&m_pVertices);

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_pShader = make_unique<CShader>(L"../../Assets/Shader/Shader_Terrain.fx");
	m_pTexture = CTexture::Create(m_pDevice, m_pDeviceContext, CTexture::TYPE_TGA, "../../Assets/Texture/Grass.tga");

	return S_OK;
}

HRESULT CVIBuffer_Terrain::Render()
{
	m_pShader->SetUp_TextureOnShader("g_DiffuseTexture", m_pTexture);
	_matrix		ViewMatrix = CEngine::GetInstance()->GetTransform(CPipeline::D3DTS_VIEW);

	ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);
	m_pShader->SetUp_ValueOnShader("g_vCamPosition", &ViewMatrix.r[3], sizeof(_vector));
	__super::Render();
	return S_OK;
}

HRESULT CVIBuffer_Terrain::CreateBuffer(void ** pVertices)
{
	SafeDeleteArray(*pVertices);

	HANDLE		hFile = CreateFile(m_HeightMapPath.c_str(), GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	if (0 == hFile)
		return E_FAIL;

	_ulong					dwByte = 0;

	BITMAPFILEHEADER		fh;
	ReadFile(hFile, &fh, sizeof(BITMAPFILEHEADER), &dwByte, nullptr);

	BITMAPINFOHEADER		ih;
	ReadFile(hFile, &ih, sizeof(BITMAPINFOHEADER), &dwByte, nullptr);

	_ulong*		pPixel = new _ulong[ih.biWidth * ih.biHeight];
	ReadFile(hFile, pPixel, sizeof(_ulong) * ih.biWidth * ih.biHeight, &dwByte, nullptr);

	CloseHandle(hFile);

	m_iStride = sizeof(VTXNORTEX);
	m_iNumVerticesX = ih.biWidth;
	m_iNumVerticesZ = ih.biHeight;
	m_iNumVertices = m_iNumVerticesX * m_iNumVerticesZ;
	m_iNumVertexBuffers = 1;

	m_VBDesc.ByteWidth = m_iStride * m_iNumVertices;
	m_VBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_VBDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	m_VBDesc.CPUAccessFlags = 0;
	m_VBDesc.MiscFlags = 0;
	m_VBDesc.StructureByteStride = m_iStride;

	float fHeightScale = 10.f;

	*pVertices = new VTXNORTEX[m_iNumVertices];
	ZeroMemory(*pVertices, sizeof(VTXNORTEX) * m_iNumVertices);

	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			float height = (pPixel[iIndex] & 0x000000ff) / fHeightScale;
			//float height = 0.f;
			((VTXNORTEX*)*pVertices)[iIndex].vPosition = _float3((float)j, height, (float)i);
			((VTXNORTEX*)*pVertices)[iIndex].vNormal = _float3(0.f, 0.f, 0.f);
			((VTXNORTEX*)*pVertices)[iIndex].vTexUV = _float2(j / (m_iNumVerticesX - 1.f), 1.f - i / (m_iNumVerticesZ - 1.f));
		}
	}


	/* For.D3D11_SUBRESOURCE_DATA */
	m_VBSubResourceData.pSysMem = *pVertices;
#pragma endregion VERTEXBUFFER

#pragma region INDEXBUFFER

	m_iNumPrimitive = (m_iNumVerticesX - 1) * (m_iNumVerticesZ - 1) * 2;
	m_iNumVerticesPerPrimitive = 3;
	m_eIndexFormat = DXGI_FORMAT_R32_UINT;
	m_ePrimitive = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	/* For.D3D11_BUFFER_DESC */
	m_IBDesc.ByteWidth = sizeof(POLYGONINDICES32) * m_iNumPrimitive;
	m_IBDesc.Usage = D3D11_USAGE_IMMUTABLE;
	m_IBDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	m_IBDesc.CPUAccessFlags = 0;
	m_IBDesc.MiscFlags = 0;
	m_IBDesc.StructureByteStride = 0;

	POLYGONINDICES32*		pIndices = new POLYGONINDICES32[m_iNumPrimitive];
	ZeroMemory(pIndices, sizeof(POLYGONINDICES32) * m_iNumPrimitive);

	_uint		iNumPrimitive = 0;

	for (_uint i = 0; i < m_iNumVerticesZ - 1; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX - 1; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;

			_uint		iIndices[4] = { iIndex + m_iNumVerticesX,
				iIndex + m_iNumVerticesX + 1,
				iIndex + 1,
				iIndex
			};

			_vector			vNormal, vSour, vDest;

			pIndices[iNumPrimitive]._0 = iIndices[0];
			pIndices[iNumPrimitive]._1 = iIndices[1];
			pIndices[iNumPrimitive]._2 = iIndices[2];

			vSour = XMLoadFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._2].vPosition) -
				XMLoadFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._1].vPosition);

			vDest = XMLoadFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._1].vPosition) -
				XMLoadFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._0].vPosition);

			vNormal = XMVector3Cross(vDest, vSour);

			XMStoreFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._0].vNormal,
				(XMLoadFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._0].vNormal) += vNormal));
			XMStoreFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._1].vNormal,
				(XMLoadFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._1].vNormal) += vNormal));
			XMStoreFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._2].vNormal,
				(XMLoadFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._2].vNormal) += vNormal));

			XMStoreFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._0].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._0].vNormal)));
			XMStoreFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._1].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._1].vNormal)));
			XMStoreFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._2].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._2].vNormal)));

			++iNumPrimitive;


			pIndices[iNumPrimitive]._0 = iIndices[0];
			pIndices[iNumPrimitive]._1 = iIndices[2];
			pIndices[iNumPrimitive]._2 = iIndices[3];

			vSour = XMLoadFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._2].vPosition) -
				XMLoadFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._1].vPosition);

			vDest = XMLoadFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._1].vPosition) -
				XMLoadFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._0].vPosition);

			vNormal = XMVector3Cross(vDest, vSour);

			XMStoreFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._0].vNormal,
				(XMLoadFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._0].vNormal) += vNormal));
			XMStoreFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._1].vNormal,
				(XMLoadFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._1].vNormal) += vNormal));
			XMStoreFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._2].vNormal,
				(XMLoadFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._2].vNormal) += vNormal));

			XMStoreFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._0].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._0].vNormal)));
			XMStoreFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._1].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._1].vNormal)));
			XMStoreFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._2].vNormal,
				XMVector3Normalize(XMLoadFloat3(&((VTXNORTEX*)*pVertices)[pIndices[iNumPrimitive]._2].vNormal)));

			++iNumPrimitive;
		}
	}

	m_IBSubResourceData.pSysMem = pIndices;

#pragma endregion INDEXBUFFER

	if (FAILED(__super::Create_Buffers()))
		return E_FAIL;

	SafeDeleteArray(pPixel);

	SafeDeleteArray(pIndices);

	return S_OK;
}

void CVIBuffer_Terrain::SetHeightMapPath(string path)
{
	m_HeightMapPath = StringToWString(path);
	CreateBuffer(&m_pCloneVertices);

	// CreateHeightField(&m_pCloneVertices);
}

void CVIBuffer_Terrain::SetTexturePath(string path)
{
	m_TexturePath = StringToWString(path);
}

string CVIBuffer_Terrain::GetHeightMapPath()
{
	return WstringToString(m_HeightMapPath);
}

string CVIBuffer_Terrain::GetTexturePath()
{
	return WstringToString(m_TexturePath);
}

void CVIBuffer_Terrain::CreateHeightField(void ** pVertices)
{
	if (m_pTerrainActor)
		m_pTerrainActor->release();

	m_pTerrainActor = CEngine::GetInstance()->GetPhysics()->createRigidStatic(PxTransform(PxIdentity));
	m_pTerrainActor->setActorFlag(PxActorFlag::eVISUALIZATION, TRUE);

	const double i255 = 1.0 / 255.0;
	const PxReal heightScale = 20.f;

	PxHeightFieldSample* hfSamples = new PxHeightFieldSample[m_iNumVertices];
	//_uint vertexIndex = m_iNumVertices - 1;
	for (_uint i = 0; i < m_iNumVerticesZ; ++i)
	{
		for (_uint j = 0; j < m_iNumVerticesX; ++j)
		{
			_uint		iIndex = i * m_iNumVerticesX + j;
			_uint		vertexIndex = j * m_iNumVerticesX + i; // PhysX

			hfSamples[iIndex].height = (PxI16)(((VTXNORTEX*)*pVertices)[vertexIndex].vPosition.y * heightScale);
			hfSamples[iIndex].materialIndex0 = 0;
			hfSamples[iIndex].materialIndex1 = 0;
			hfSamples[iIndex].clearTessFlag();
		}
	}
	// Build PxHeightFieldDesc from samples
	PxHeightFieldDesc terrainDesc;
	terrainDesc.format = PxHeightFieldFormat::eS16_TM;
	terrainDesc.nbColumns = m_iNumVerticesX;
	terrainDesc.nbRows = m_iNumVerticesZ;
	terrainDesc.samples.data = hfSamples;
	terrainDesc.samples.stride = sizeof(PxHeightFieldSample); // 2x 8-bit material indices + 16-bit height
	PxHeightField* heightField = CEngine::GetInstance()->GetCooking()->createHeightField(terrainDesc, CEngine::GetInstance()->GetPhysics()->getPhysicsInsertionCallback());
	PxHeightFieldGeometry hfGeom(heightField, PxMeshGeometryFlags(), (1/20.f), 1, 1);
	PxTransform localPose;
	localPose.p = PxVec3(0, 0, 0);         // heightfield is at world (0,minHeight,0)
	localPose.q = PxQuat(PxIdentity);
	PxMaterial* mat = CEngine::GetInstance()->GetPhysics()->createMaterial(0.5f, 0.5f, 0.1f);
	PxShape* shape = PxRigidActorExt::createExclusiveShape(*m_pTerrainActor, hfGeom, *mat);
	shape->setLocalPose(localPose);
	CEngine::GetInstance()->AddActor(m_pTerrainActor);
	delete[] hfSamples;
}
