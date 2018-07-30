/*
 * GroupingConfig.cpp
 *
 *  Created on: Jul 3, 2018
 *      Author: mschefer
 */

#include <boost/algorithm/string.hpp>

#include "ElementsKernel/Exception.h"
#include "Configuration/ConfigManager.h"

#include "SEImplementation/Configuration/GroupingConfig.h"

using namespace Euclid::Configuration;
namespace po = boost::program_options;

namespace SExtractor {

static const std::string GROUPING_ALGORITHM {"grouping-algorithm" };

static const std::string GROUPING_ALGORITHM_NONE {"NONE" };
static const std::string GROUPING_ALGORITHM_OVERLAP {"OVERLAP" };
static const std::string GROUPING_ALGORITHM_SPLIT {"SPLIT" };

GroupingConfig::GroupingConfig(long manager_id)
    : Configuration(manager_id), m_selected_algorithm(Algorithm::SPLIT_SOURCES) {
}

std::map<std::string, Configuration::OptionDescriptionList> GroupingConfig::getProgramOptions() {
  return { {"Grouping", {
      {GROUPING_ALGORITHM.c_str(), po::value<std::string>()->default_value(GROUPING_ALGORITHM_SPLIT),
          "Grouping algorithm to be used."},
  }}};
}

void GroupingConfig::initialize(const UserValues& args) {
  auto algorithm_name = boost::to_upper_copy(args.at(GROUPING_ALGORITHM).as<std::string>());
  if (algorithm_name == GROUPING_ALGORITHM_NONE) {
    m_selected_algorithm = Algorithm::NO_GROUPING;
  } else if (algorithm_name == GROUPING_ALGORITHM_OVERLAP) {
    m_selected_algorithm = Algorithm::OVERLAPPING;
  } else if (algorithm_name == GROUPING_ALGORITHM_SPLIT) {
    m_selected_algorithm = Algorithm::SPLIT_SOURCES;
  } else {
    throw Elements::Exception() << "Unknown grouping algorithm : " << algorithm_name;
  }
}

} // SExtractor namespace



