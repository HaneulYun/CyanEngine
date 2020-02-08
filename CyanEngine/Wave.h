#pragma once
#include "framework.h"

class Wave : public MonoBehavior<Wave>
{
private /*이 영역에 private 변수를 선언하세요.*/:
	// Simulation constants we can precompute.
	float mK1 = 0.0f;
	float mK2 = 0.0f;
	float mK3 = 0.0f;

	Mesh* mesh;
	CTexturedVertex* pVertices;

public  /*이 영역에 public 변수를 선언하세요.*/:
	int mNumRows = 0;
	int mNumCols = 0;

	int mVertexCount = 0;
	int mTriangleCount = 0;

	float mTimeStep = 0.0f;
	float mSpatialStep = 0.0f;

	std::vector<DirectX::XMFLOAT3> mPrevSolution;
	std::vector<DirectX::XMFLOAT3> mCurrSolution;
	std::vector<DirectX::XMFLOAT3> mNormals;
	std::vector<DirectX::XMFLOAT3> mTangentX;

private:
	friend class GameObject;
	friend class MonoBehavior<Wave>;
	Wave() = default;
	Wave(Wave&) = default;

public:
	~Wave() {}

	void Start(/*초기화 코드를 작성하세요.*/)
	{
		for(int k = 0; k < 30; ++k)
		{
			int i = Random::Range(4, mNumRows - 5);
			int j = Random::Range(4, mNumCols - 5);

			float r = Random::Range(0.2f, 0.5f);

			Disturb(i, j, r);
		}
	}

	void Update(/*업데이트 코드를 작성하세요.*/)
	{
		static float time{};
		time -= Time::deltaTime;

		static float t_base = 0.0f;
		if (time < 0.0f)
		{
			time += 0.2f;
			t_base += 0.03f;

			int i = Random::Range(4, mNumRows - 5);
			int j = Random::Range(4, mNumCols - 5);

			float r = Random::Range(0.2f, 0.5f);

			Disturb(i, j, r);
		}

		Update(Time::deltaTime);

		RendererManager::Instance()->commandList->Reset(RendererManager::Instance()->commandAllocator.Get(), NULL);

		pVertices = new CTexturedVertex[mVertexCount];
		for (int i = 0; i < mVertexCount; ++i)
		{
			float x = (mPrevSolution[i].x + 500) / 1000;
			float y = (mPrevSolution[i].z + 500) / 1000;
			pVertices[i] = CTexturedVertex(mPrevSolution[i], XMFLOAT2(x, y));
		}
		mesh->vertexBuffer = CreateBufferResource(pVertices, mesh->m_nStride * mesh->m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &mesh->vertexUploadBuffer);
		mesh->vertexBufferView.BufferLocation = mesh->vertexBuffer->GetGPUVirtualAddress();
		mesh->vertexBufferView.StrideInBytes = mesh->m_nStride;
		mesh->vertexBufferView.SizeInBytes = mesh->m_nStride * mesh->m_nVertices;

		RendererManager::Instance()->commandList->Close();

		ID3D12CommandList* ppd3dCommandLists[] = { RendererManager::Instance()->commandList.Get() };
		RendererManager::Instance()->commandQueue->ExecuteCommandLists(_countof(ppd3dCommandLists), ppd3dCommandLists);

		RendererManager::Instance()->WaitForPreviousFrame();
	}

	// 필요한 경우 함수를 선언 및 정의 하셔도 됩니다.
	void Set(int m, int n, float dx, float dt, float speed, float damping)
	{
		mNumRows = m;
		mNumCols = n;

		mVertexCount = m * n;
		mTriangleCount = (m - 1) * (n - 1) * 2;

		mTimeStep = dt;
		mSpatialStep = dx;

		float d = damping * dt + 2.0f;
		float e = (speed * speed) * (dt * dt) / (dx * dx);
		mK1 = (damping * dt - 2.0f) / d;
		mK2 = (4.0f - 8.0f * e) / d;
		mK3 = (2.0f * e) / d;

		mPrevSolution.resize(m * n);
		mCurrSolution.resize(m * n);
		mNormals.resize(m * n);
		mTangentX.resize(m * n);


		float halfWidth = (n - 1) * dx * 0.5f;
		float halfDepth = (m - 1) * dx * 0.5f;
		for (int i = 0; i < m; ++i)
		{
			float z = halfDepth - i * dx;
			for (int j = 0; j < n; ++j)
			{
				float x = -halfWidth + j * dx;

				mPrevSolution[i * n + j] = XMFLOAT3(x, 0.0f, z);
				mCurrSolution[i * n + j] = XMFLOAT3(x, 0.0f, z);
				mNormals[i * n + j] = XMFLOAT3(0.0f, 1.0f, 0.0f);
				mTangentX[i * n + j] = XMFLOAT3(1.0f, 0.0f, 0.0f);
			}
		}
	}

	void Update(float dt)
	{
		static float t = 0;

		t += dt;

		if (t >= mTimeStep)
		{
			for (int i = 1; i < mNumRows - 1; ++i)
			{
				for (int j = 1; j < mNumCols - 1; ++j)
				{
					mPrevSolution[i * mNumCols + j].y =
						mK1 * mPrevSolution[i * mNumCols + j].y +
						mK2 * mCurrSolution[i * mNumCols + j].y +
						mK3 * (mCurrSolution[(i + 1) * mNumCols + j].y +
							mCurrSolution[(i - 1) * mNumCols + j].y +
							mCurrSolution[i * mNumCols + j + 1].y +
							mCurrSolution[i * mNumCols + j - 1].y);
				}
			}
			std::swap(mPrevSolution, mCurrSolution);

			t = 0.0f;

			for (int i = 1; i < mNumRows - 1; ++i)
			{
				for (int j = 1; j < mNumCols - 1; ++j)
				{
					float l = mCurrSolution[i * mNumCols + j - 1].y;
					float r = mCurrSolution[i * mNumCols + j + 1].y;
					float t = mCurrSolution[(i - 1) * mNumCols + j].y;
					float b = mCurrSolution[(i + 1) * mNumCols + j].y;
					mNormals[i * mNumCols + j].x = -r + l;
					mNormals[i * mNumCols + j].y = 2.0f * mSpatialStep;
					mNormals[i * mNumCols + j].z = b - t;

					XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&mNormals[i * mNumCols + j]));
					XMStoreFloat3(&mNormals[i * mNumCols + j], n);

					mTangentX[i * mNumCols + j] = XMFLOAT3(2.0f * mSpatialStep, r - l, 0.0f);
					XMVECTOR T = XMVector3Normalize(XMLoadFloat3(&mTangentX[i * mNumCols + j]));
					XMStoreFloat3(&mTangentX[i * mNumCols + j], T);
				}
			}
		}
	}

	void Disturb(int i, int j, float magnitude)
	{
		assert(i > 1 && i < mNumRows - 2);
		assert(j > 1 && j < mNumCols - 2);

		float halfMag = 0.5f * magnitude;

		pVertices[i * mNumCols + j].position.y = mCurrSolution[i * mNumCols + j].y += magnitude;
		pVertices[i * mNumCols + j + 1].position.y = mCurrSolution[i * mNumCols + j + 1].y += halfMag;
		pVertices[i * mNumCols + j - 1].position.y = mCurrSolution[i * mNumCols + j - 1].y += halfMag;
		pVertices[(i + 1) * mNumCols + j].position.y = mCurrSolution[(i + 1) * mNumCols + j].y += halfMag;
		pVertices[(i - 1) * mNumCols + j].position.y = mCurrSolution[(i - 1) * mNumCols + j].y += halfMag;
	}

	float Width()const
	{
		return mNumCols * mSpatialStep;
	}

	float Depth()const
	{
		return mNumRows * mSpatialStep;
	}

	Mesh* MakeMesh()
	{
		mesh = new Mesh;
		mesh->m_nVertices = mVertexCount;
		mesh->m_nStride = sizeof(CTexturedVertex);
		mesh->m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

		pVertices = new CTexturedVertex[mVertexCount];

		for (int i = 0; i < mVertexCount; ++i)
		{
			float x = (mPrevSolution[i].x + 500) / 1000;
			float y = (mPrevSolution[i].z + 500) / 1000;
			pVertices[i] = CTexturedVertex(mPrevSolution[i], XMFLOAT2(x, y));
		}

		mesh->vertexBuffer = CreateBufferResource(pVertices, mesh->m_nStride * mesh->m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &mesh->vertexUploadBuffer);

		mesh->vertexBufferView.BufferLocation = mesh->vertexBuffer->GetGPUVirtualAddress();
		mesh->vertexBufferView.StrideInBytes = mesh->m_nStride;
		mesh->vertexBufferView.SizeInBytes = mesh->m_nStride * mesh->m_nVertices;

		mesh->m_nIndices = 3 * mTriangleCount;
		UINT* pnIndices = new UINT[mesh->m_nIndices];
		int m = mNumRows;
		int n = mNumCols;
		int k = 0;
		for (int i = 0; i < m - 1; ++i)
		{
			for (int j = 0; j < n - 1; ++j)
			{
				pnIndices[k] = i * n + j;
				pnIndices[k + 1] = i * n + j + 1;
				pnIndices[k + 2] = (i + 1) * n + j;

				pnIndices[k + 3] = (i + 1) * n + j;
				pnIndices[k + 4] = i * n + j + 1;
				pnIndices[k + 5] = (i + 1) * n + j + 1;

				k += 6;
			}
		}

		mesh->indexBuffer = CreateBufferResource(pnIndices, sizeof(UINT) * mesh->m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &mesh->indexUploadBuffer);

		mesh->indexBufferView.BufferLocation = mesh->indexBuffer->GetGPUVirtualAddress();
		mesh->indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		mesh->indexBufferView.SizeInBytes = sizeof(UINT) * mesh->m_nIndices;

		return mesh;
	}
};