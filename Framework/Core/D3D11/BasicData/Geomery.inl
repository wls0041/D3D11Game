#include "Geomery.h"

template<class T>
inline Geomery<T>::~Geomery()
{
	Clear();
}

template<class T>
inline auto Geomery<T>::GetVertices(const uint & offset, const uint & count) -> const std::vector<T>
{
	return std::vector<T>();
}

template<class T>
inline void Geomery<T>::AddVertex(const T & vertex)
{
	vertices.emplace_back(vertex);
}

template<class T>
inline void Geomery<T>::Addvertices(const std::vector<T>& vertices)
{
	this->vertices.insert(this->end(), vertices.begin(), vertices.end());
}

template<class T>
inline void Geomery<T>::SetVertices(const std::vector<T>& vertices)
{
	this->vertices.clear();
	this->vertices.shrink_to_fit();

	//this->vertices = vertices;
	this->vertices.assign(vertices.begin(), vertices.end());
}

template<class T>
inline auto Geomery<T>::GetIndices(const uint & offset, const uint & count) -> const std::vector<uint>
{
	return std::vector<uint>();
}

template<class T>
inline void Geomery<T>::AddIndex(const T & index)
{
	vertices.emplace_back(index);
}

template<class T>
inline void Geomery<T>::AddIndices(const std::vector<uint>& indices)
{
	this->indices.insert(this->end(), indices.begin(), indices.end());
}

template<class T>
inline void Geomery<T>::SetIndices(const std::vector<uint>& indices)
{
	this->indices.clear();
	this->indices.shrink_to_fit();

	//this->vertices = vertices;
	this->indices.assign(indices.begin(), indices.end());
}

template<class T>
inline void Geomery<T>::Clear()
{
	vertices.clear();
	vertices.shrink_to_fit();

	indices.clear();
	indices.shrink_to_fit();
}
