#include "VertexArrayObject.h"

VertexArrayObject::VertexArrayObject(std::vector<std::shared_ptr<VertexBufferObject>>& vertex_buffer_objects, std::shared_ptr<IndexBufferObject>& index_buffer_object)
{
	_buffer_type = GL_VERTEX_ARRAY;
	_buffer_size = sizeof(vertex_buffer_objects) + sizeof(_ibo);
	_vbos = vertex_buffer_objects;
	_ibo = index_buffer_object;

	Create();
}

VertexArrayObject::~VertexArrayObject()
{
	Destroy();
}

void VertexArrayObject::Create()
{
#ifdef _DEBUG
	// Check for existing vbo(s)
	if (_vbos.empty())
		ExCore::Logger::PrintWar("NULL buffers assigned to vertex buffer array!");
#endif

	glGenVertexArrays(1, &_buffer_object);
	glBindVertexArray(_buffer_object);

#ifdef _DEBUG
	// Check if vertex array successully generated
	if (_buffer_object == NULL)
	{
		ExCore::Logger::PrintErr("Vertex array object failed to generate!");
		return;
	}
#endif

	for (auto i = 0; i < _vbos.size(); ++i)
		_vbos[i]->Create();
	
	// Only create an ibo if object is valid
	if (_ibo != nullptr)
		_ibo->Create();
}

void VertexArrayObject::Destroy()
{
	for (auto i = 0; i < _vbos.size(); ++i)
		glDisableVertexAttribArray(i);

	glDeleteVertexArrays(1, &_buffer_object);
}

void VertexArrayObject::Bind()
{
	glBindVertexArray(_buffer_object);
}

std::shared_ptr<IndexBufferObject>& VertexArrayObject::GetIndexBuffer()
{
	return _ibo;
}

std::vector< std::shared_ptr<VertexBufferObject>>& VertexArrayObject::GetVertexBuffers()
{
	return _vbos;
}
