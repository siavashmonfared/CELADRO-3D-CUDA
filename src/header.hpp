/*
 * This file is part of CELADRO-3D-CUDA, Copyright (C) 2024, Siavash Monfared
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */


#ifndef HEADER_HPP_
#define HEADER_HPP_

// defines project-wide header list to be precompiled
// this allows us to reduce the compile time considerably

#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <exception>
#include <sstream>
#include <utility>
#include <limits>
#include <random>
#include <map>
#include <vector>
#include <complex>
#include <array>
#include <stack>
#include <functional>
#include <memory>
#include <type_traits>
#include <chrono>

#include "cuda.h"
#include <cuComplex.h>
#include <curand.h>
#include <curand_kernel.h>
#include "vec_cuda.h"
#include "error_msg.hpp"
//#include "threads.hpp"
#include "tools.hpp"

#include "error_msg.hpp"
//#include "threads.hpp"
#include "tools.hpp"

// boost program_options
#include <boost/program_options.hpp>
namespace opt = boost::program_options;

// =============================================================================
// Constants

/** display width (change it directly here) */
constexpr unsigned width = 70;
/** An infmaous constant */
constexpr double Pi = 3.14159265358979323846;

#endif//HEADER_HPP_
