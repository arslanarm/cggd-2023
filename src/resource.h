#pragma once

#include "utils/error_handler.h"

#include <algorithm>
#include <linalg.h>
#include <vector>


using namespace linalg::aliases;

namespace cg
{
	template<typename T>
	class resource
	{
	public:
		resource(size_t size);
		resource(size_t x_size, size_t y_size);
		~resource();

		const T* get_data();
		T& item(size_t item);
		T& item(size_t x, size_t y);

		size_t get_size_in_bytes() const;
		size_t get_number_of_elements() const;
		size_t get_stride() const;

	private:
		std::vector<T> data;
		size_t item_size = sizeof(T);
		size_t stride;
	};

	template<typename T>
	inline resource<T>::resource(size_t size)
	{
		data.resize(size);
		stride = size;
	}
	template<typename T>
	inline resource<T>::resource(size_t x_size, size_t y_size)
	{
		data.resize(x_size * y_size);
		stride = x_size;
	}
	template<typename T>
	inline resource<T>::~resource()
	{
		std::destroy(data.begin(), data.end());
	}
	template<typename T>
	inline const T* resource<T>::get_data()
	{
		return data.data();
	}
	template<typename T>
	inline T& resource<T>::item(size_t item)
	{
		return data.at(item);
	}
	template<typename T>
	inline T& resource<T>::item(size_t x, size_t y)
	{
		return data.at(y * stride + x);
	}
	template<typename T>
	inline size_t resource<T>::get_size_in_bytes() const
	{
		return data.size() * item_size;
	}
	template<typename T>
	inline size_t resource<T>::get_number_of_elements() const
	{
		return data.size();
	}

	template<typename T>
	inline size_t resource<T>::get_stride() const
	{
		return stride;
	}

	struct color
	{
		static color from_float3(const float3& in)
		{
			return color{ in.x, in.y, in.z };
		};
		float3 to_float3() const
		{
			return float3{ r, g, b };
		}
		float r;
		float g;
		float b;
	};

	struct unsigned_color
	{
		static unsigned_color from_color(const color& color)
		{
			return unsigned_color{
					(uint8_t)std::clamp((color.r * 255), 0.f, 255.f),
					(uint8_t)std::clamp((color.g * 255), 0.f, 255.f),
					(uint8_t)std::clamp((color.b * 255), 0.f, 255.f)
			};
		};
		static unsigned_color from_float3(const float3& color)
		{
			return unsigned_color{
					(uint8_t)std::clamp((color.x * 255), 0.f, 255.f),
					(uint8_t)std::clamp((color.y * 255), 0.f, 255.f),
					(uint8_t)std::clamp((color.z * 255), 0.f, 255.f)
			};
		};
		float3 to_float3() const
		{
			return float3{
					(float) r,
					(float) g,
					(float) b
			} / 255.f;
		};
		uint8_t r;
		uint8_t g;
		uint8_t b;
	};


	struct vertex
	{
		float x;
		float y;
		float z;
		float nx;
		float ny;
		float nz;
		float u;
		float v;
		float ambient_r;
		float ambient_g;
		float ambient_b;
		float diffuse_r;
		float diffuse_g;
		float diffuse_b;
		float emissive_r;
		float emissive_g;
		float emissive_b;
	};

}// namespace cg