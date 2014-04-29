//
//  MemoryStorage.h
//  CommonRealtimeGameAlgorithms
//
//  Created by Hoon H. on 14/4/29.
//
//

#pragma once

#include "../CommonRealtimeGameAlgorithmsCommon.h"
#include "../DebuggingTools/MemoryTracer.h"

EONIL_COMMON_REALTIME_GAME_ALGORITHMS_GENERIC_CONTAINERS_BEGIN






/*!
 A statically defined area of memory which does not perform initialization/cleanup automatically.
 You are responsible to do them.
 
 @classdesign
 It is guaranteed that the pointer to this object is same to a pointer to content value.
 
 Where,
 
	MemoryStorage<T>	v1{};
 
 the given expression
 
	uintptr_t(&v1) == uintptr_t(&v1.value());

 is always true.
 
 This is checked by runtime assertion in debug build.
 
 Anyway, the size of this object will be exactly same with `T`.
	
 
 @discussion
 Copy and move cannot be defined by definition of this class because we cannot track current object
 aliveness state. So all copy/move related stuffs are prohibited by explicit delete.
 
 @exception
 All methods guarantees strong exception safety as long as the type `T` provides strong exception 
 safety for all of these methods. (no state change if there's an exception thrown)
 
 -	constructors
 -	destructors
 -	copy assignment
 -	move assignment
 
 @warning
 Do not use this class for usualy/daily object management. This class is designed for specially
 designed classes.
 It is very easy to forget to call `terminate` at last. You MUST call the `terminate` function 
 EXPLICITLY to trigger C++ destructor correctly.
 */
template <typename T>
class
MemoryStorage
{
	friend class	MemoryStorageDebugginSupport;
	
//	using	MEM	=	typename std::aligned_storage<sizeof(T)>::type;
	using	MEM	=	std::array<uint8_t, sizeof(T)>;
	
	MEM		_mem;
	
public:
	MemoryStorage() = default;
	MemoryStorage(MemoryStorage const&) = delete;
	MemoryStorage(MemoryStorage&&) = delete;
	
	auto	operator=(MemoryStorage const&) -> MemoryStorage& = delete;
	auto	operator=(MemoryStorage&&) -> MemoryStorage& = delete;
	
	////
	
	auto	value() const -> T const&;
	auto	value() -> T&;
	
	template <typename... ARGS>
	auto	initialize(ARGS&&... args) -> void;
	auto	initialize(T const&) -> void;
	auto	initialize(T&&) -> void;
	auto	terminate() noexcept -> void;
};















template <typename T> auto
MemoryStorage<T>::value() const -> T const&
{
	static_assert(sizeof(T) == sizeof(MemoryStorage<T>), "Size of memory-storage must be equal to size of type `T`.");
	EONIL_COMMON_REALTIME_GAME_ALGORITHMS_DEBUG_ASSERT(uintptr_t(this) == uintptr_t(&_mem));
	EONIL_COMMON_REALTIME_GAME_ALGORITHMS_DEBUG_ASSERT(this != nullptr);
	
	MEM const&	ref1	=	_mem;
	T const&	ref2	=	reinterpret_cast<T const&>(ref1);
	return		ref2;
}
template <typename T> auto
MemoryStorage<T>::value() -> T&
{
	EONIL_COMMON_REALTIME_GAME_ALGORITHMS_DEBUG_ASSERT(uintptr_t(this) == uintptr_t(&_mem));
	EONIL_COMMON_REALTIME_GAME_ALGORITHMS_DEBUG_ASSERT(this != nullptr);
	
	MEM&	ref1	=	_mem;
	T&		ref2	=	reinterpret_cast<T&>(ref1);
	return	ref2;
}

template <typename T>
template <typename ...ARGS> auto
MemoryStorage<T>::
initialize(ARGS&&... args) -> void
{
	EONIL_COMMON_REALTIME_GAME_ALGORITHMS_DEBUG_ASSERT(this != nullptr);
	
	new (&_mem) T(std::forward<ARGS>(args)...);
}
template <typename T> auto
MemoryStorage<T>::
initialize(T const& o) -> void
{
	EONIL_COMMON_REALTIME_GAME_ALGORITHMS_DEBUG_ASSERT(this != nullptr);
	
	new (&_mem) T(o);
}
template <typename T> auto
MemoryStorage<T>::
initialize(T&& o) -> void
{
	EONIL_COMMON_REALTIME_GAME_ALGORITHMS_DEBUG_ASSERT(this != nullptr);
	
	new (&_mem) T(std::move(o));
}
template <typename T> auto
MemoryStorage<T>::
terminate() noexcept -> void
{
	EONIL_COMMON_REALTIME_GAME_ALGORITHMS_DEBUG_ASSERT(this != nullptr);
	
	MEM&	ref1	=	_mem;
	T&		ref2	=	reinterpret_cast<T&>(ref1);
	ref2.~T();
}































EONIL_COMMON_REALTIME_GAME_ALGORITHMS_GENERIC_CONTAINERS_END
