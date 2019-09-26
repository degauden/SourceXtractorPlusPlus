/** Copyright © 2019 Université de Genève, LMU Munich - Faculty of Physics, IAP-CNRS/Sorbonne Université
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by the Free
 * Software Foundation; either version 3.0 of the License, or (at your option)
 * any later version.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */
/*
 * ModelFittingConfig.cpp
 *
 *  Created on: May 17, 2017
 *      Author: mschefer
 */

#include "SEImplementation/Configuration/LegacyModelFittingConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string MFIT_MAX_ITERATIONS {"model-fitting-iterations"};

LegacyModelFittingConfig::LegacyModelFittingConfig(long manager_id) : Configuration(manager_id) {
}

auto LegacyModelFittingConfig::getProgramOptions() -> std::map<std::string, OptionDescriptionList> {
  return { {"Model Fitting", {
      {MFIT_MAX_ITERATIONS.c_str(), po::value<int>()->default_value(1000), "Maximum number of iterations allowed for model fitting"},
  }}};

  return {};
}

void LegacyModelFittingConfig::initialize(const UserValues& args) {
  m_max_iterations = args.at(MFIT_MAX_ITERATIONS).as<int>();
  if (m_max_iterations <= 0) {
    throw Elements::Exception() << "Invalid " << MFIT_MAX_ITERATIONS << " value: " << m_max_iterations;
  }
}

} /* namespace SExtractor */


