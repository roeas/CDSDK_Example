#pragma once

#include "Math/Matrix.hpp"
#include "Math/Quaternion.hpp"

namespace cd
{

template<typename T>
class TTransform final
{
public:
	using ValueType = T;
	static constexpr std::size_t Size = 10;
	using Iterator = T*;
	using ConstIterator = const T*;

public:
	TTransform() = default;

	TTransform(TVector<T, 3> translation, TQuaternion<T> rotation, TVector<T, 3> scale) :
		m_translation(cd::MoveTemp(translation)),
		m_rotation(cd::MoveTemp(rotation)),
		m_scale(cd::MoveTemp(scale))
	{
	}

	TTransform(const TTransform&) = default;
	TTransform& operator=(const TTransform&) = default;
	TTransform(TTransform&&) = default;
	TTransform& operator=(TTransform&&) = default;
	~TTransform() = default;

	void Clear()
	{
		m_translation.Clear();
		m_rotation.Clear();
		m_scale.Clear();
	}

	// Get
	CD_FORCEINLINE Iterator Begin() { return &m_translation[0]; }
	CD_FORCEINLINE Iterator End() { return &m_translation[0] + Size; }
	CD_FORCEINLINE ConstIterator Begin() const { return &m_translation[0]; }
	CD_FORCEINLINE ConstIterator End() const { return &m_translation[0] + Size; }

	CD_FORCEINLINE void SetTranslation(TVector<T, 3> translation) { m_translation = cd::MoveTemp(translation); }
	CD_FORCEINLINE TVector<T, 3>& GetTranslation() { return m_translation; }
	CD_FORCEINLINE const TVector<T, 3>& GetTranslation() const { return m_translation; }

	CD_FORCEINLINE void SetRotation(TQuaternion<T> rotation) { m_rotation = cd::MoveTemp(rotation); }
	CD_FORCEINLINE TQuaternion<T>& GetRotation() { return m_rotation; }
	CD_FORCEINLINE const TQuaternion<T>& GetRotation() const { return m_rotation; }

	CD_FORCEINLINE void SetScale(TVector<T, 3> scale) { m_scale = cd::MoveTemp(scale); }
	CD_FORCEINLINE TVector<T, 3>& GetScale() { return m_scale; }
	CD_FORCEINLINE const TVector<T, 3>& GetScale() const { return m_scale; }

	TMatrix<T, 4, 4> GetMatrix() const
	{
		constexpr T zero = static_cast<T>(0);
		constexpr T one = static_cast<T>(1);
		constexpr T two = static_cast<T>(2);

		T w = m_rotation.w();
		T ww = w * w;
		T angle = two * std::acos(w);
		T ax = m_rotation.x() / std::sqrt(one - ww);
		T ay = m_rotation.y() / std::sqrt(one - ww);
		T az = m_rotation.z() / std::sqrt(one - ww);

		T sinax = std::sin(ax);
		T cosax = std::cos(ax);
		T sinay = std::sin(ay);
		T cosay = std::cos(ay);
		T sinaz = std::sin(az);
		T cosaz = std::cos(az);
		T sinaxz = sinax * sinaz;
		T cosayz = cosay * cosaz;

		T tx = m_translation.x();
		T ty = m_translation.y();
		T tz = m_translation.z();
		T sx = m_scale.x();
		T sy = m_scale.y();
		T sz = m_scale.z();

		return TMatrix<T, 4, 4>(sx * (cosayz - sinaxz * sy), sx * -cosax * sz, sx * (cosaz * sy + cosay * sinaxz), zero,
			                    sy * (cosaz * sx * sy + cosay * sz), sy * cosax * cosaz, sy * (sy * sz - cosayz * sx), zero,
			                    sz * -cosax * sy, sz * sx, sz * cosax * cosay, zero,
			                    tx, ty, tz, one);
	}

private:
	TVector<T, 3> m_translation;
	TQuaternion<T> m_rotation;
	TVector<T, 3> m_scale;
};

using Transform = TTransform<float>;

static_assert(10 * sizeof(float) == sizeof(Transform));
static_assert(std::is_standard_layout_v<Transform>&& std::is_trivial_v<Transform>);

}