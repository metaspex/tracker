//
// Copyright Metaspex - 2026
// mailto:admin@metaspex.com
//

#ifndef HX2A_TRACKER_EXCEPTION_HPP
#define HX2A_TRACKER_EXCEPTION_HPP

#include "hx2a/exception.hpp"

namespace tracker {

  using namespace hx2a;

  using campaign_does_not_exist = application_exception<"tracking_miss", "Tracking code does not exist.">;
  using tracker_code_does_not_exist = application_exception<"campaign_miss", "Tracking code does not exist.">;
  using tracker_domain_not_defined = application_exception<"domain_miss", "Tracker domain has not been defined.">;

} // End namespace tracker.

#endif
  
