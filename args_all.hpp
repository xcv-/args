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

#include "args/flag.hpp"
#include "args/lambda_arg.hpp"
#include "args/map_lookup_arg.hpp"

#include "args/flag/param_parsers.hpp"

#include "common/arg.hpp"
#include "common/parse_error.hpp"
#include "common/parser.hpp"
#include "common/parser_state.hpp"
#include "common/types.hpp"

#include "display/error.hpp"
#include "display/help.hpp"

#include "functors/condition.hpp"

#include "utils/result.hpp"
#include "utils/string_view.hpp"
