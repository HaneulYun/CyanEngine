#include "pch.h"
#include "Mesh.h"

void Mesh::Render(UINT nInstances)
{
	RendererManager::Instance()->commandList->IASetVertexBuffers(m_nSlot, 1, &vertexBufferView);
	RendererManager::Instance()->commandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	if (indexBuffer)
	{
		RendererManager::Instance()->commandList->IASetIndexBuffer(&indexBufferView);
		RendererManager::Instance()->commandList->DrawIndexedInstanced(m_nIndices, nInstances, 0, 0, 0);
	}
	else
	{
		RendererManager::Instance()->commandList->DrawInstanced(m_nVertices, nInstances, m_nOffset, 0);
	}
}