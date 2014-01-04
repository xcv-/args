/*
 * Copyright 2014 xcv_
 *
 * This file is part of Args.
 *
 *  Args is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Args is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Args.  If not, see <http://www.gnu.org/licenses/>
*/

#ifndef ARGS_RESULT_H
#define ARGS_RESULT_H

#include <algorithm>
#include <utility>


namespace args {

template <typename T>
struct ResultOkWrapper {
	T data;
};

template <typename T>
struct ResultErrWrapper {
	T data;
};


template <typename O, typename E>
class Result {
	const enum class Type {
		Ok, Err
	} type;

	union {
		O ok_value;
		E err_value;
	};

	struct OkTag {};
	struct ErrTag {};


	template <typename... Ts>
	Result(OkTag, Ts&&... args)
		: type(Type::Ok)
		, ok_value(std::forward<Ts>(args)...) {}

	template <typename... Ts>
	Result(ErrTag, Ts&&... args)
		: type(Type::Err)
		, err_value(std::forward<Ts>(args)...) {}

public:
	using OkType = O;
	using ErrType = E;

	template <typename T>
	Result(ResultOkWrapper<T>&& w)
		: Result(OkTag(), std::move(w.data)) {}

	template <typename T>
	Result(ResultErrWrapper<T>&& w)
		: Result(ErrTag(), std::move(w.data)) {}


	Result(const Result& other) : type(other.type) {
		if (is_ok())
			ok_value = other.ok_value;
		else
			err_value = other.err_value;
	}

	Result(Result&& other) : type(other.type) {
		if (is_ok())
			ok_value = std::move(other.ok_value);
		else
			err_value = std::move(other.err_value);
	}


	~Result() {
		if (is_ok())
			ok_value.~OkType();
		else
			err_value.~ErrType();
	}

	template <typename... Ts>
	static Result ok(Ts&&... args) {
		return Result(OkTag(), std::forward<Ts>(args)...);
	}

	template <typename... Ts>
	static Result err(Ts&&... args) {
		return Result(ErrTag(), std::forward<Ts>(args)...);
	}

	bool is_ok() const {
		return type == Type::Ok;
	}

	const OkType& get_ok() const {
		return ok_value;
	}

	const ErrType& get_err() const {
		return err_value;
	}

	OkType& get_ok() {
		return ok_value;
	}

	ErrType& get_err() {
		return err_value;
	}

	template <typename F>
	const Result& if_ok(F&& f) const& {
		if (is_ok())
			f(get_ok());

		return *this;
	}

	template <typename F>
	Result& if_ok(F&& f) & {
		if (is_ok())
			f(get_ok());

		return *this;
	}

	template <typename F>
	Result&& if_ok(F&& f) && {
		if (is_ok())
			f(std::move(get_ok()));

		return std::move(*this);
	}

	template <typename F>
	const Result& if_err(F&& f) const& {
		if (!is_ok())
			f(get_err());

		return *this;
	}

	template <typename F>
	Result& if_err(F&& f) & {
		if (!is_ok())
			f(get_err());

		return *this;
	}

	template <typename F>
	Result&& if_err(F&& f) && {
		if (!is_ok())
			f(std::move(get_err()));

		return std::move(*this);
	}
};


}

#endif
